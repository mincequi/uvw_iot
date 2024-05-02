#pragma once

#include <uvw_iot/Thing.h>

namespace uvw_iot {

class ThingFactory {
public:
    static ThingPtr from(const std::string& host);

private:
    ThingFactory();
};

} // namespace uvw_iot

