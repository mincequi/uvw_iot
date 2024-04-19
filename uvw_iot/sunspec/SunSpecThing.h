#pragma once

#include <uvw_iot/common/Thing.h>
#include <uvw_net/sunspec/SunSpecClient.h>

namespace uvw_iot {
namespace sunspec {

using namespace uvw_net::sunspec;

class SunSpecThing : public common::Thing {
public:
    SunSpecThing(SunSpecClientPtr client);

private:
    const std::string& id() const override;
    void getProperties() override;

    SunSpecClientPtr _client;
    size_t _errorCount = 0;
};

} // namespace sunspec
} // namespace uvw_iot
