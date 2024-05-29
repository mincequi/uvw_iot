#pragma once

#include <uvw_iot/Thing.h>
#include <uvw_net/sunspec/SunSpecClient.h>

namespace uvw_iot {
namespace sunspec {

using namespace uvw_net::sunspec;

class SunSpecThing : public Thing {
public:
    SunSpecThing(SunSpecClientPtr client);
    ~SunSpecThing() override;

private:
    const std::string& id() const override;
    ThingType type() const override;
    void disconnect() override;

    void getProperties() override;

    SunSpecClientPtr _client;
    ThingType _type = ThingType::Unknown;
    size_t _errorCount = 0;
};

} // namespace sunspec
} // namespace uvw_iot
