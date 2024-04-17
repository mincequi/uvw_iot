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
    HttpClient client;
};

HttpThing::HttpThing(const std::string& host) :
    d(std::make_unique<HttpThingPrivate>()),
    _host(host) {
    d->client.on<uvw::error_event>([this](const uvw::error_event&, const HttpClient&) {
        ++_errorCount;
        if (_errorCount > 2) close();
    });
    d->client.on<HttpResponse>([this](const HttpResponse& response, const HttpClient&) {
        _errorCount = 0;
        onBody(response.body);
    });
}

HttpThing::~HttpThing() {
}

const std::string& HttpThing::host() const {
    return _host;
}

void HttpThing::get(const std::string& path) {
    uvw_net::http::HttpRequest request;
    request.host = _host;
    request.path = path;
    d->client.get(request);
}

void HttpThing::set(const std::string& path) {
    uvw_net::http::HttpRequest request;
    request.host = _host;
    request.path = path;
    d->client.get(request, true);
}

void HttpThing::onBody(const std::string& body) {
}

} // namespace http
} // namespace uvw_iot
