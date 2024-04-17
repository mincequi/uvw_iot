#include "ThingRepository.h"

#include <rpp/observables/dynamic_observable.hpp>
#include <uvw/loop.h>
#include <uvw/timer.h>

namespace uvw_iot {
namespace common {

ThingRepository::ThingRepository() {
}

void ThingRepository::addThing(ThingPtr thing) {
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

void ThingRepository::getProperties() {
    // TODO: clear errornous things here
    for (const auto& t : _things) {
        t.second->getProperties();
    }
}

dynamic_observable<std::pair<std::string, ThingPropertyMap>> ThingRepository::propertiesObservable() const {
    return _properties.get_observable();
}

} // namespace common
} // namespace uvw_iot
