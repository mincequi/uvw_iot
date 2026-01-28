#include <iostream>

#include <magic_enum_iostream.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/fmt/ostr.h>

#include <uvw/timer.h>
#include <uvw_iot/ThingFactory.h>
#include <uvw_iot/ThingProperty.h>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/sunspec/SunSpecThing.h>
#include <uvw_iot/util/Filter.h>
#include <uvw_iot/util/MultiThing.h>
#include <uvw_iot/util/Site.h>
#include <uvw_net/dns_sd/DnsServiceDiscovery.h>
#include <uvw_net/modbus/ModbusDiscovery.h>
#include <uvw_net/sunspec/SunSpecDiscovery.h>

using namespace spdlog;
using namespace uvw_iot;
using namespace uvw_iot::sunspec;
using namespace uvw_iot::util;
using namespace uvw_net::dns_sd;
using namespace uvw_net::modbus;
using namespace uvw_net::sunspec;

using magic_enum::iostream_operators::operator<<;
std::ostream& operator<<(std::ostream& s, const uvw_iot::ThingPropertyMap& map) {
    //for (const auto& [k, v] : map) {
    map.forEach([&](const ThingPropertyKey k, const auto& v) {
        s << k << ": ";
        // Check for type of v and then format accordingly
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::array<int, 3>>) {
            s << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
        } else {
            s << v;
        }

        s << ", ";
    });

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
        info("{}> {}", t.first, ss.str());
    });

    Site site(thingRepository);
    site.properties().subscribe([&](const Site::Properties& p) {
        info("site> pvPower: {}, gridPower: {}", p.pvPower, p.gridPower);
        info("site> shortTermGridPower: {}, longTermGridPower: {}", p.shortTermGridPower, p.longTermGridPower);
    });

    DnsServiceDiscovery dnsDiscovery("_http._tcp.local");
    dnsDiscovery.on<MdnsResponse>([&](const MdnsResponse& data, const DnsServiceDiscovery&) {
        if (!data.srvData) return;

        const auto host = data.srvData->target;
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
        dnsDiscovery.discover();
        modbusDiscovery.discover();
    });
    discoveryTimer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{30000});

    // Start a read timer
    auto readTimer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    readTimer->on<uvw::timer_event>([&](const auto&, auto&) {
        thingRepository.fetchProperties();
    });
    readTimer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{3000});

    // Run the event loop
    return uvw::loop::get_default()->run();
}
