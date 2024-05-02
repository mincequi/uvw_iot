#pragma once

#include <uvw_iot/Rpp.h>
#include <uvw_iot/ThingProperty.h>
#include <uvw_iot/ThingType.h>

namespace uvw_iot {

class Thing {
public:
    virtual const std::string& id() const = 0;
    inline virtual ThingType type() const { return ThingType::Unknown; }

    virtual void getProperties();
    void setProperty(ThingPropertyKey key, const ThingPropertyValue& value);
    void setProperties(const ThingPropertyMap& properties);
    dynamic_observable<ThingPropertyMap> propertiesObservable() const;
    inline const ThingPropertyMap& properties() const { return _properties; }

protected:
    Thing();
    virtual ~Thing() = default;

    void publish(const ThingPropertyMap& properties);
    void close();

    virtual void onSetProperty(ThingPropertyKey key, const ThingPropertyValue& value);

private:
    ThingPropertyMap _properties;
    // Note: we do not use here a behaviour_subject since we only want to
    //       publish changed properties.
    publish_subject<ThingPropertyMap> _propertiesSubject;
};

using ThingPtr = std::shared_ptr<Thing>;

} // namespace uvw_iot
