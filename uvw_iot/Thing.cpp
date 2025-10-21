#include "Thing.h"

namespace uvw_iot {

Thing::Thing() {
}

void Thing::getProperties() {
}

void Thing::setProperty(ThingPropertyKey key, const ThingPropertyValue& value) {
    publish({{key, value}});
    onSetProperties({{key, value}});
}

void Thing::setProperties(const ThingPropertyMap& properties) {
    publish(properties);
    onSetProperties(properties);
}

dynamic_observable<ThingPropertyMap> Thing::propertiesObservable() const {
    return _propertiesSubject.get_observable();
}

void Thing::onSetProperties(const ThingPropertyMap& properties) {
}

void Thing::publish(const ThingPropertyMap& properties) {
    for (const auto& p : properties)
        _properties[p.first] = p.second;
    _propertiesSubject.get_observer().on_next(properties);
}

void Thing::close() {
     _propertiesSubject.get_observer().on_completed();
}

} // namespace uvw_iot
