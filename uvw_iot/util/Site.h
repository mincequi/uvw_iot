#pragma once

#include <chrono>
#include <list>
#include <set>
#include <string>

#include <uvw_iot/Rpp.h>

namespace uvw_iot {
class ThingRepository;

namespace util {
using namespace std::chrono_literals;

struct SiteConfig {
    std::string gridMeter;
    std::set<std::string> pvMeters;
    /// Debounce time in milliseconds for site properties gridPower and pvPower.
    std::chrono::milliseconds debounceTime = 400ms;
    std::chrono::milliseconds shortTermTau = 10000ms;
    std::chrono::milliseconds longTermTau = 60000ms;
};

class Site {
public:
    struct Properties {
        int ts = 0;
        int pvPower = 0;
        int gridPower = 0;
        double shortTermGridPower = 0.0;
        double longTermGridPower = 0.0;
    };

    Site(const ThingRepository& repo, const SiteConfig& cfg = {});

    dynamic_observable<int> gridPower() const;
    dynamic_observable<Site::Properties> properties() const;

    const std::list<Properties>& history() const;

private:
    const ThingRepository& _repo;
    const SiteConfig _cfg;

    publish_subject<int> _gridPower;
    publish_subject<std::pair<std::string, int>> _pvPowers;
    publish_subject<int> _pvPower;
    publish_subject<Properties> _properties;

    std::list<Properties> _history;
};

} // namespace util
} // namespace uvw_iot
