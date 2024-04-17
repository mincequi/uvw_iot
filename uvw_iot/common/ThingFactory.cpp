#include "ThingFactory.h"

#include <uvw_iot/shelly/ShellyThing.h>

namespace uvw_iot {
namespace common {

using uvw_iot::shelly::ShellyThing;

ThingPtr ThingFactory::from(const std::string& host) {
    return ShellyThing::from(host, 80);
}

ThingFactory::ThingFactory() {
}

} // namespace common
} // namespace uvw_iot
