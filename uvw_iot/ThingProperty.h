#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <variant>

namespace uvw_iot {

enum class ThingPropertyKey : int8_t {
    min_value = -24,

    // custom properties - generic
    timestamp = -1,

    // custom properties - things
    name = -2,
    pinned = -3,
    status = -4,
    events = -5,
    type = -6,
    icon = -7,

    // custom properties - strategies
    offset = -16,
    thing_interval = -17,
    time_constant = -18,
    countdown = -19,

    // custom properties - site
    pv_power = -20,
    grid_power = -21,
    short_term_grid_power = -22,
    long_term_grid_power = -23,

    // ipso properties
    generic_sensor = 0,
    temperature = 3,
    power = 5,
    actuation = 6,
    power_control = 12,

    phases = 13,
    next_phases = 14,

    // ipso properties - not meant for export
    voltage = 16, // Only object, no reusable object
    current = 17, // Only object, no reusable object

    max_value = 24
};

using ThingPropertyValue = std::variant<bool, int, std::string, std::array<int, 3>>;
using ThingProperty = std::pair<ThingPropertyKey, ThingPropertyValue>;
using ThingPropertyMap = std::map<ThingPropertyKey, ThingPropertyValue>;

} // namespace uvw_iot
