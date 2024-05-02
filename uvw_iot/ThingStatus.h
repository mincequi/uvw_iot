#pragma once

#include <cstdint>

namespace uvw_iot {

enum class ThingStatus : int16_t {
    unknown         = 0, //invalid
    off             = 1,
    idle            = 2, //sleeping
    waiting         = 3, //starting
    charging        = 4, //mpp//on//active
    throttled       = 5,
    complete        = 6, //shuttingDown
    error           = 7,
    service         = 8
};

} // namespace uvw_iot
