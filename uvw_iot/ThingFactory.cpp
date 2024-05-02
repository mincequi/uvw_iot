#include "ThingFactory.h"

#include <uvw_iot/goe/GoeCharger.h>
#include <uvw_iot/shelly/ShellyThing.h>

namespace uvw_iot {

ThingPtr ThingFactory::from(const std::string& host) {
    ThingPtr thing;
    thing = shelly::ShellyThing::from(host, 80);
    if (thing) return thing;

    thing = goe::GoeCharger::from(host, 80);
    if (thing) return thing;

    return nullptr;
}

ThingFactory::ThingFactory() {
}

} // namespace uvw_iot
