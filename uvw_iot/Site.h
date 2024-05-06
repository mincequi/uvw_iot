#pragma once

#include <chrono>
#include <list>
#include <set>
#include <string>

#include <uvw_iot/Rpp.h>

namespace uvw_iot {

class ThingRepository;
using namespace std::chrono_literals;

struct SiteConfig {
    std::string gridMeter;
    std::set<std::string> pvMeters;
    std::chrono::milliseconds debounceTime = 400ms;
};

class Site {
public:
    struct Properties {
        int ts = 0;
        int pvPower = 0;
        int gridPower = 0;
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

} // namespace uvw_iot
