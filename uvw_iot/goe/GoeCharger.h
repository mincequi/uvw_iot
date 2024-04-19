#pragma once

#include <uvw_iot/common/ThingStatus.h>
#include <uvw_iot/common/ThingType.h>

#include <uvw_iot/http/HttpThing.h>

namespace uvw_iot {
namespace goe {

using uvw_iot::common::ThingType;

class GoeCharger : public http::HttpThing {
public:
    static common::ThingPtr from(const std::string& host, uint16_t port);

private:
    using HttpThing::HttpThing;

    inline ThingType type() const override { return ThingType::EvStation; }

    void getProperties() override;
    void onSetProperty(common::ThingPropertyKey key, const common::ThingPropertyValue& value) override;

    // HttpThing overrides
    void onBody(const std::string& body) override;

    common::ThingStatus _status = common::ThingStatus::unknown;
};

} // namespace goe
} // namespace uvw_iot
