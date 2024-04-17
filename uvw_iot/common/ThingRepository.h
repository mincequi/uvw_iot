#pragma once

#include <uvw_iot/common/Thing.h>

namespace uvw_iot {
namespace common {

class ThingRepository {
public:
    ThingRepository();

    void addThing(ThingPtr thing);
    dynamic_observable<ThingPtr> thingAdded() const;
    dynamic_observable<std::string> thingRemoved() const;

    void getProperties();
    dynamic_observable<std::pair<std::string, ThingPropertyMap>> propertiesObservable() const;

private:
    std::map<std::string, ThingPtr> _things;
    publish_subject<ThingPtr> _thingAdded;
    publish_subject<std::string> _thingRemoved;
    publish_subject<std::pair<std::string, ThingPropertyMap>> _properties;
};

} // namespace common
} // namespace uvw_iot
