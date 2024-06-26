#include "ThingRepository.h"

#include <rpp/observables/dynamic_observable.hpp>
#include <uvw/loop.h>
#include <uvw/timer.h>

namespace uvw_iot {

ThingRepository::ThingRepository() {
}

void ThingRepository::addThing(ThingPtr thing) const {
    if (_things.contains(thing->id())) return;

    auto it = _things[thing->id()] = thing;
    _thingAdded.get_observer().on_next(it);

    const auto id = thing->id();
    it->propertiesObservable().subscribe(
        [this, id](const auto& properties) {
            _properties.get_observer().on_next({id, properties});
        },
        [this, id]() {
            auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
            timer->on<uvw::timer_event>([this, id](auto&, auto&) {
                _thingRemoved.get_observer().on_next(id);
                if (_things.contains(id)) _things.at(id)->disconnect();
                _things.erase(id);
            });
            timer->start(uvw::timer_handle::time{0}, uvw::timer_handle::time{0});
        }
    );
}

dynamic_observable<ThingPtr> ThingRepository::thingAdded() const {
    return _thingAdded.get_observable();
}

dynamic_observable<std::string> ThingRepository::thingRemoved() const {
    return _thingRemoved.get_observable();
}

void ThingRepository::getProperties() const {
    for (const auto& t : _things) {
        t.second->getProperties();
    }
}

dynamic_observable<std::pair<std::string, ThingPropertyMap>> ThingRepository::propertiesObservable() const {
    return _properties.get_observable();
}

void ThingRepository::setThingProperties(const std::string& id, const ThingPropertyMap& properties) const {
    if (!_things.contains(id)) return;

    _things.at(id)->setProperties(properties);
}

void ThingRepository::setThingProperty(const std::string& id, ThingPropertyKey property, const ThingPropertyValue& value) const {
    if (!_things.contains(id)) return;

    _things.at(id)->setProperty(property, value);
}

} // namespace uvw_iot
