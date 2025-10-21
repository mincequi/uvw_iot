#include "GoeCharger.h"

#include <cmath>
#include <regex>
#include <nlohmann/json.hpp>

#include "GoeUtil.h"

namespace uvw_iot {
namespace goe {

using json = nlohmann::json;

ThingPtr GoeCharger::from(const std::string& host, uint16_t port) {
    if (std::regex_match(host, std::regex("go-echarger_\\d{6}"))) {
        return ThingPtr(new GoeCharger(host, port));
    }
    return nullptr;
}

void GoeCharger::onSetProperties(const ThingPropertyMap& properties) {
    if (_status != ThingStatus::waiting && _status != ThingStatus::charging) return;

    std::optional<std::string> command = std::nullopt;
    for (const auto& p : properties) {
        switch (p.first) {
        case ThingPropertyKey::current:
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    if (arg == 0) {
                        command = "/api/set?psm=1&frc=1&amp=6";
                    } else {
                        command = "/api/set?psm=1&frc=2&amp=" + std::to_string(std::clamp(arg, 6, 32));
                    }
                } else if constexpr (std::is_same_v<T, std::array<int, 3>>) {
                    command = "/api/set?psm=2&frc=2&amp=" + std::to_string(std::clamp(arg.front(), 6, 32));
                }
            }, p.second);
            break;
        default:
            break;
        }
    }

    if (command.has_value()) {
        set(command.value());
    }
}

void GoeCharger::getProperties() {
    // alw,car,eto,nrg,wh,trx,cards"
    get("/api/status?filter=nrg,car"); // psm (for phases) and amp (for amps)
}

void GoeCharger::onBody(const std::string& body) {
    const auto doc = json::parse(body);
    if (!doc.contains("nrg")) return;

    const auto nrg = doc["nrg"].get<std::vector<double>>();
    if (nrg.empty()) return;

    std::array<int, 3> voltage;
    voltage.at(0) = ceil(nrg[0]);
    voltage.at(1) = ceil(nrg[1]);
    voltage.at(2) = ceil(nrg[2]);

    _status = goe::toStatus(doc["car"].get<int>());

    publish({
        { ThingPropertyKey::status, (int)_status },
        { ThingPropertyKey::power, (int)round(nrg[11]) },
        { ThingPropertyKey::voltage, voltage }
    });
}

} // namespace goe
} // namespace uvw_iot
