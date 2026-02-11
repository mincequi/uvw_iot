#include "ShellyThing.h"

#include <cmath>
#include <regex>

#include <nlohmann/json.hpp>

namespace uvw_iot {
namespace shelly {

using json = nlohmann::json;

ShellyThing::ShellyThing(const std::string& host, uint16_t port, bool isPm) :
    HttpThing(host),
    _isPm(isPm) {
}

ThingPtr ShellyThing::from(const std::string& host, uint16_t port) {
    std::string str = host;
    std::regex rgx("shelly[1-4](pm)*-[0-9A-F]{12}");
    std::smatch matches;
    auto match = std::regex_match(str, matches, rgx);
    if (match) {
        return ThingPtr(new ShellyThing(host, port, !matches[1].str().empty()));
    }
    return nullptr;
}

void ShellyThing::fetchProperties() {
    get("/status");
}

void ShellyThing::onSetProperties(const ThingPropertyMap& properties) {
    properties.on<ThingPropertyKey::power_control>([&](const auto& isOn) {
        const std::string strValue = isOn ? "on" : "off";
        set("/relay/0?turn=" + strValue);
    });
}

void ShellyThing::onBody(const std::string &body) {
    const auto doc = json::parse(body);
    ThingPropertyMap properties;
    if (doc.contains("relays")) {
        properties.set<ThingPropertyKey::power_control>(doc.at("relays").at(0).at("ison").get<bool>());
    } else if (doc.contains("ison")) {
        properties.set<ThingPropertyKey::power_control>(doc.at("ison").get<bool>());
    }

    // Only Shelly PM reports power and energy
    if (_isPm && doc.contains("meters")) {
        properties.set<ThingPropertyKey::power>((int)round(doc["meters"][0]["power"].get<double>()));
        // Shelly PM reports energy in Watt-minute, but we want it in 0.1 kWh
        const auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        const int day = now / (24 * 3600);
        const int energy = doc["meters"][0]["total"].get<int>();

        if (day != _currentDay) {
            _currentDay = day;
            _initialEnergy = energy;
        }

        properties.set<ThingPropertyKey::energy>((energy - _initialEnergy) / 1000); // Wm to kWm
    }

    if (doc.contains("ext_temperature") && !doc.at("ext_temperature").empty()) {
        properties.set<ThingPropertyKey::temperature>((int)round(doc.at("ext_temperature").at("0").at("tC").get<double>()*10.0));
    }

    if (!properties.empty()) {
        publish(properties);
    }
}

} // namespace shelly
} // namespace uvw_iot
