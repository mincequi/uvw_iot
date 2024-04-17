#pragma once

#include <uvw_iot/common/Thing.h>

namespace uvw_iot {
namespace common {

class ThingFactory {
public:
    static ThingPtr from(const std::string& host);

private:
    ThingFactory();
};

} // namespace common
} // namespace uvw_iot

