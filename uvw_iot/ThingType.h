#pragma once

namespace uvw_iot {

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

} // namespace uvw_iot
