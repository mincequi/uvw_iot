#include "GoeCharger.h"

#include <cmath>
#include <regex>
#include <nlohmann/json.hpp>

#include "GoeUtil.h"

namespace uvw_iot {
namespace goe {

using json = nlohmann::json;
using namespace uvw_iot::common;

ThingPtr GoeCharger::from(const std::string& host, uint16_t port) {
    if (std::regex_match(host, std::regex("go-echarger_\\d{6}"))) {
        return ThingPtr(new GoeCharger(host, port));
    }
    return nullptr;
}

void GoeCharger::onSetProperty(ThingPropertyKey key, const ThingPropertyValue& value) {
    if (_status != ThingStatus::waiting && _status != ThingStatus::charging) return;

    switch (key) {
    case ThingPropertyKey::current:
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                if (arg == 0) {
                    set("/api/set?frc=1");
                } else {
                    set("/api/set?psm=1"); // force 1 phase
                    set("/api/set?amp=" + std::to_string(std::clamp(arg, 6, 32)));
                    set("/api/set?frc=2"); // switch on
                }
            } else if constexpr (std::is_same_v<T, std::array<int, 3>>) {
                set("/api/set?psm=2"); // force 3 phase
                set("/api/set?amp=" + std::to_string(std::clamp(arg.front(), 6, 32)));
                set("/api/set?frc=2");
            }
        }, value);
        break;
    default:
        break;
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
