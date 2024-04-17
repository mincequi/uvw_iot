#include "Thing.h"

namespace uvw_iot {
namespace common {

Thing::Thing() {
}

void Thing::getProperties() {
}

void Thing::setProperty(ThingPropertyKey key, const ThingPropertyValue& value) {
    publish({{key, value}});
    onSetProperty(key, value);
}

dynamic_observable<ThingPropertyMap> Thing::propertiesObservable() const {
    return _propertiesSubject.get_observable();
}

void Thing::onSetProperty(ThingPropertyKey, const ThingPropertyValue&) {
}

void Thing::publish(const ThingPropertyMap& properties) {
    for (const auto& p : properties)
        _properties[p.first] = p.second;
    _propertiesSubject.get_observer().on_next(properties);
}

void Thing::close() {
     _propertiesSubject.get_observer().on_completed();
}

} // namespace common
} // namespace uvw_iot
