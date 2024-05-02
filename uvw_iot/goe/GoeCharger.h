#pragma once

#include <uvw_iot/ThingStatus.h>
#include <uvw_iot/ThingType.h>

#include <uvw_iot/http/HttpThing.h>

namespace uvw_iot {
namespace goe {

class GoeCharger : public http::HttpThing {
public:
    static ThingPtr from(const std::string& host, uint16_t port);

private:
    using HttpThing::HttpThing;

    inline ThingType type() const override { return ThingType::EvStation; }

    void getProperties() override;
    void onSetProperty(ThingPropertyKey key, const ThingPropertyValue& value) override;

    // HttpThing overrides
    void onBody(const std::string& body) override;

    ThingStatus _status = ThingStatus::unknown;
};

} // namespace goe
} // namespace uvw_iot
