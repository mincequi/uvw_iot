#include <iostream>

#include <magic_enum_iostream.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/fmt/ostr.h>

#include <uvw/timer.h>
#include <uvw_iot/common/ThingFactory.h>
#include <uvw_iot/common/ThingProperty.h>
#include <uvw_iot/common/ThingRepository.h>
#include <uvw_iot/sunspec/SunSpecThing.h>
#include <uvw_net/dns_sd/DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

using namespace spdlog;
using namespace uvw_iot::common;
using namespace uvw_iot::sunspec;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;
using namespace uvw_net::sunspec;

using magic_enum::iostream_operators::operator<<;
std::ostream& operator<<(std::ostream& s, const uvw_iot::common::ThingPropertyMap& map) {
    for (const auto& [k, v] : map) {
        s << k << ": ";
        std::visit([&](auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::array<int, 3>>) {
                s << "[" << arg[0] << ", " << arg[1] << ", " << arg[2] << "]";
            } else {
                s << arg;
            }
        }, v);
        s << ", ";
    }

    return s;
}

int main() {
    ThingRepository thingRepository;
    thingRepository.thingAdded().subscribe([&](const ThingPtr& t) {
        info("thing added> id: {}, things count: {}", t->id(), thingRepository.things().size());
    });
    thingRepository.thingRemoved().subscribe([&](const std::string& id) {
        info("thing removed> id: {}, things count: {}", id, thingRepository.things().size());
    });
    thingRepository.propertiesObservable().subscribe([&](const auto& t) {
        std::stringstream ss;
        ss << t.second;
        info("{}> properties updated> {}", t.first, ss.str());
    });

    DnsServiceDiscovery dnsDiscovery;
    dnsDiscovery.on<DnsRecordDataSrv>([&](const DnsRecordDataSrv& data, const DnsServiceDiscovery&) {
        const auto host = data.target.substr(0, data.target.find("."));
        auto thing = ThingFactory::from(host);
        if (thing) thingRepository.addThing(thing);
    });

    SunSpecDiscovery sunspecDiscovery;
    sunspecDiscovery.on<SunSpecClientPtr>([&](SunSpecClientPtr client, const SunSpecDiscovery&) {
        auto thing = std::make_shared<SunSpecThing>(client);
        thingRepository.addThing(thing);
    });

    ModbusDiscovery modbusDiscovery;
    modbusDiscovery.on<ModbusClientPtr>([&](ModbusClientPtr client, const ModbusDiscovery&) {
        sunspecDiscovery.discover(client);
    });

    // Start a discovery timer
    auto discoveryTimer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    discoveryTimer->on<uvw::timer_event>([&](const auto&, auto&) {
        info("start discoveries>");
        dnsDiscovery.discover("_http._tcp.local");
        modbusDiscovery.discover();
    });
    discoveryTimer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{30000});

    // Start a read timer
    auto readTimer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    readTimer->on<uvw::timer_event>([&](const auto&, auto&) {
        thingRepository.getProperties();
    });
    readTimer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{3000});

    // Run the event loop
    return uvw::loop::get_default()->run();
}
