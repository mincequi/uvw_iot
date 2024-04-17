#pragma once

#include <uvw_iot/common/Thing.h>

namespace uvw_iot {
namespace http {

using common::Thing;

class HttpThing : public Thing {
public:
    HttpThing(const std::string& host);
    // Note: this has to be defined for unique_ptr member.
    ~HttpThing();

protected:
    const std::string& host() const;

    void get(const std::string& path);
    void set(const std::string& path);

    virtual void onBody(const std::string& body);

private: 
    std::unique_ptr<class HttpThingPrivate> const d;

    std::string _host;
    size_t _errorCount = 0;
};

} // namespace http
} // namespace uvw_iot
