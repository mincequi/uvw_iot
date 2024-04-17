#pragma once

#include <uvw_iot/common/Rpp.h>
#include <uvw_iot/common/ThingProperty.h>

namespace uvw_iot {
namespace common {

class Thing {
public:
    virtual const std::string& id() const = 0;

    virtual void getProperties();
    void setProperty(ThingPropertyKey key, const ThingPropertyValue& value);
    dynamic_observable<ThingPropertyMap> propertiesObservable() const;

protected:
    Thing();

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

} // namespace common
} // namespace uvw_iot
