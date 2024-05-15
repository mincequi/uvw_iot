#pragma once

#include <uvw_iot/Thing.h>

namespace uvw_iot {

class MultiThing : public Thing {
public:
    MultiThing();

    void publish(const std::string& id, const ThingPropertyMap& properties);

protected:
    const std::string& id() const override;

private:
    std::string _id;
};

} // namespace uvw_iot
