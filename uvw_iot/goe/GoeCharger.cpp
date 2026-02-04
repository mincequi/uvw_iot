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
    const auto current = properties.get<ThingPropertyKey::current>();
    const auto phases = properties.get<ThingPropertyKey::phases>();

    if (!current || !phases) return;

    std::optional<std::string> command = std::nullopt;
    if (*current == 0) {
        command = "/api/set?psm=1&frc=1&amp=6";
    } else {
        if (*phases == 1) {
            command = "/api/set?psm=1&frc=2&amp=" + std::to_string(std::clamp(*current, 6, 32));
        } else if (*phases == 3) {
            command = "/api/set?psm=2&frc=2&amp=" + std::to_string(std::clamp(*current, 6, 32));
        }
    }

    if (command.has_value()) {
        set(command.value());
    }
}

void GoeCharger::fetchProperties() {
    // alw,car,eto,nrg,wh,trx,cards"
    get("/api/status?filter=nrg,car,eto"); // psm (for phases) and amp (for amps)
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

    ThingPropertyMap properties;
    properties.set<ThingPropertyKey::status>((int)_status);
    properties.set<ThingPropertyKey::power>((int)round(nrg[11]));
    properties.set<ThingPropertyKey::voltage>(voltage);

    if (doc.contains("eto")) {
        const auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        const int day = now / (24 * 3600);
        const int energy = doc["eto"].get<int>()/100;

        if (day != _currentDay) {
            _currentDay = day;
            _initialEnergy = energy;
        }

        properties.set<ThingPropertyKey::energy>(energy - _initialEnergy);
    }

    publish(properties);
}

} // namespace goe
} // namespace uvw_iot
