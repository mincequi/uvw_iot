#include <iostream>

#include <magic_enum_iostream.hpp>

#include <uvw/timer.h>
#include <uvw_iot/common/ThingFactory.h>
#include <uvw_iot/common/ThingRepository.h>
#include <uvw_net/dns_sd/DnsServiceDiscovery.h>

using namespace uvw_iot::common;
using namespace uvw_net::dns_sd;

using magic_enum::iostream_operators::operator<<;
std::ostream& operator<<(std::ostream& s, uvw_iot::common::ThingPropertyValue const& v){
    std::visit([&](auto& arg) {
        s << arg;
    }, v); return s;
}

int main() {
    ThingRepository thingRepository;
    thingRepository.thingAdded().subscribe([](const ThingPtr& t) {
        std::cout << "thing added> id: " << t->id() << std::endl;
    });
    thingRepository.thingRemoved().subscribe([](const std::string& id) {
        std::cout << "thing removed> id: " << id << std::endl;
    });
    thingRepository.propertiesObservable().subscribe([](const auto& t) {
        std::cout << t.first << "> properties updated> ";
        for (const auto& p : t.second) {
            std::cout << p.first << ": " << p.second << ", ";
        }
        std::cout << std::endl;
    });

    DnsServiceDiscovery dnsDiscovery;
    //dnsDiscovery.discover("_http._tcp.local");
    dnsDiscovery.on<DnsRecordDataSrv>([&](const DnsRecordDataSrv& data, const DnsServiceDiscovery&) {
        const auto host = data.target.substr(0, data.target.find("."));
        auto thing = ThingFactory::from(host);
        if (thing) thingRepository.addThing(thing);
    });

    // Start a timer
    auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([&](const auto&, auto&) {
        dnsDiscovery.discover("_http._tcp.local");
        thingRepository.getProperties();
    });
    timer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{3000});

    // Run the event loop
    return uvw::loop::get_default()->run();
}
