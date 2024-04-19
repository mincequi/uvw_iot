#pragma once

#include <uvw_iot/common/ThingStatus.h>
#include <uvw_iot/http/HttpThing.h>

namespace uvw_iot {
namespace goe {

class GoeCharger : public http::HttpThing {
public:
    static common::ThingPtr from(const std::string& host, uint16_t port);

private:
    using HttpThing::HttpThing;

    void getProperties() override;
    void onSetProperty(common::ThingPropertyKey key, const common::ThingPropertyValue& value) override;

    // HttpThing overrides
    void onBody(const std::string& body) override;

    common::ThingStatus _status = common::ThingStatus::unknown;
};

} // namespace goe
} // namespace uvw_iot
