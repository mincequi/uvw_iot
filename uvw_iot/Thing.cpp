#include "Thing.h"

namespace uvw_iot {

Thing::Thing() {
}

void Thing::setProperties(const ThingPropertyMap& properties) {
    publish(properties);
    onSetProperties(properties);
}

dynamic_observable<ThingPropertyMap> Thing::propertiesObservable() const {
    return _propertiesSubject.get_observable();
}


void Thing::publish(const ThingPropertyMap& properties) {
    _properties.assignFrom(properties);
    _propertiesSubject.get_observer().on_next(properties);
}

void Thing::close() {
     _propertiesSubject.get_observer().on_completed();
}

} // namespace uvw_iot
