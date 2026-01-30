#pragma once

#include <map>

#include <uvw_iot/Thing.h>

namespace uvw_iot {

class ThingRepository {
public:
    ThingRepository();

    inline const std::map<std::string, ThingPtr>& things() const { return _things; }
    void addThing(ThingPtr thing) const;
    dynamic_observable<ThingPtr> thingAdded() const;
    dynamic_observable<std::string> thingRemoved() const;

    void fetchProperties() const;
    dynamic_observable<std::pair<std::string, ThingPropertyMap>> propertiesObservable() const;
    void setThingProperties(const std::string& id, const ThingPropertyMap& properties) const;
    //void setThingProperty(const std::string& id, ThingPropertyKey property, const ThingPropertyValue& value) const;

private:
    mutable std::map<std::string, ThingPtr> _things;
    publish_subject<ThingPtr> _thingAdded;
    publish_subject<std::string> _thingRemoved;
    publish_subject<std::pair<std::string, ThingPropertyMap>> _properties;
};

} // namespace uvw_iot
