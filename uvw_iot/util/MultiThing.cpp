#include "MultiThing.h"

namespace uvw_iot {

MultiThing::MultiThing() {}

const std::string& MultiThing::id() const {
    return _id;
}

void MultiThing::publish(const std::string& id, const ThingPropertyMap& properties) {
    _id = id;
    Thing::publish(properties);
}

} // namespace uvw_iot
