#include "Thing.h"

namespace uvw_iot {

Thing::Thing() {
}

void Thing::setProperties(const ThingPropertyMap& properties) {
    publish(properties);
    onSetProperties(properties);
}

void Thing::publish(const ThingPropertyMap& properties) {
    _lastSeen = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    _properties.assignFrom(properties);
    _propertiesSubject.get_observer().on_next(properties);
}

void Thing::close() {
     _propertiesSubject.get_observer().on_completed();
}

} // namespace uvw_iot
