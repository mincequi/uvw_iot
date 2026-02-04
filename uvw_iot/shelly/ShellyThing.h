#pragma once

#include <uvw_iot/http/HttpThing.h>

namespace uvw_iot {
namespace shelly {

class ShellyThing : public http::HttpThing {
public:
    static ThingPtr from(const std::string& host, uint16_t port);

private:
    ShellyThing(const std::string& host, uint16_t port, bool isPm);

    // Thing overrides
    void fetchProperties() override;
    void onSetProperties(const ThingPropertyMap& properties) override;

    // HttpThing overrides
    void onBody(const std::string& body) override;

    bool _isPm = false;

    int _currentDay = 0;
    int _initialEnergy = 0;
};

} // namespace shelly
} // namespace uvw_iot
