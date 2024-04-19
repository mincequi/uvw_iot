#pragma once

namespace uvw_iot {
namespace common {

enum class ThingType {
    Unknown = 0,
    SmartMeter,
    SolarInverter,
    EvStation,
    Relay,

    Site,
    Battery,
    WeatherStation,
};

} // namespace common
} // namespace uvw_iot
