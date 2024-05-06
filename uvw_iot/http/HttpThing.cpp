#include "HttpThing.h"

#include <uvw_net/http/HttpClient.h>
#include <uvw_net/http/HttpRequest.h>
#include <uvw_net/http/HttpResponse.h>

namespace uvw_iot {
namespace http {

using uvw_net::http::HttpClient;
using uvw_net::http::HttpResponse;

class HttpThingPrivate {
public:
    HttpThingPrivate(const std::string& host, uint16_t port) :
        getClient(host, port),
        setClient(host, port) {
    }
    HttpClient getClient;
    HttpClient setClient;
};

HttpThing::HttpThing(const std::string& host, uint16_t port) :
    d(std::make_unique<HttpThingPrivate>(host, port)),
    _host(host),
    _port(port) {
    d->getClient.on<uvw::error_event>([this](const uvw::error_event&, const HttpClient&) {
        ++_errorCount;
        if (_errorCount > 5) close();
    });
    d->getClient.on<HttpResponse>([this](const HttpResponse& response, const HttpClient&) {
        _errorCount = 0;
        if (!response.body.empty()) onBody(response.body);
    });

    d->setClient.on<uvw::error_event>([this](const uvw::error_event&, const HttpClient&) {
    });
    d->setClient.on<HttpResponse>([this](const HttpResponse&, const HttpClient&) {
    });
}

HttpThing::~HttpThing() {
}

const std::string& HttpThing::id() const {
    return host();
}

const std::string& HttpThing::host() const {
    return _host;
}

void HttpThing::get(const std::string& path) {
    uvw_net::http::HttpRequest request;
    request.host = _host;
    request.path = path;
    d->getClient.get(request);
}

void HttpThing::set(const std::string& path) {
    uvw_net::http::HttpRequest request;
    request.host = _host;
    request.path = path;
    d->setClient.get(request, true);
}

void HttpThing::onBody(const std::string& body) {
}

} // namespace http
} // namespace uvw_iot
