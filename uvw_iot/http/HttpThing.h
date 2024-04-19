#pragma once

#include <uvw_iot/common/Thing.h>

namespace uvw_iot {
namespace http {

class HttpThing : public common::Thing {
public:
    HttpThing(const std::string& host, uint16_t port = 80);
    // Note: this has to be defined for unique_ptr member.
    ~HttpThing();

protected:
    const std::string& id() const override;

    const std::string& host() const;

    void get(const std::string& path);
    void set(const std::string& path);

    virtual void onBody(const std::string& body);

private: 
    std::unique_ptr<class HttpThingPrivate> const d;

    std::string _host;
    uint16_t _port = 80;
    size_t _errorCount = 0;
};

} // namespace http
} // namespace uvw_iot
