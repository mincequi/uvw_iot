#pragma once

#include <uvw_iot/http/HttpThing.h>

namespace uvw_iot {
namespace shelly {

using uvw_iot::common::ThingPtr;
using uvw_iot::common::ThingPropertyKey;
using uvw_iot::common::ThingPropertyValue;
using uvw_iot::http::HttpThing;

class ShellyThing : public HttpThing {
public:
    static ThingPtr from(const std::string& host, uint16_t port);

private:
    ShellyThing(const std::string& host, uint16_t port, bool isPm);

    // Thing overrides
    const std::string& id() const override;
    void getProperties() override;
    void onSetProperty(ThingPropertyKey key, const ThingPropertyValue& value) override;

    // HttpThing overrides
    void onBody(const std::string& body) override;

    uint16_t _port;
    bool _isPm = false;
};

} // namespace shelly
} // namespace uvw_iot
