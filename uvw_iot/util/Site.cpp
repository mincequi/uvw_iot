#include "Site.h"

#include <numeric>

#include <uvw_iot/RppUvw.h>
#include <uvw_iot/ThingRepository.h>
#include <uvw_iot/util/Filter.h>

namespace uvw_iot::util {

Site::Site(const ThingRepository& repo, const SiteConfig& cfg) : _repo(repo), _cfg(cfg) {
    _repo.thingAdded().subscribe([this](ThingPtr thing) {
        // If we configured this meter for site (or there is no explicit config)
        if (thing->type() == ThingType::SmartMeter && (_cfg.gridMeter == thing->id() || _cfg.gridMeter.empty())) {
            thing->propertiesObservable()
                | filter([&](const auto& p) { return p.template contains<ThingPropertyKey::power>(); })
                | map([&](const auto& p) { return *(p.template get<ThingPropertyKey::power>()); })
                | subscribe([this](const auto& power) {
                      _gridPower.get_observer().on_next(power);
                  });
        } else if (thing->type() == ThingType::SolarInverter && (_cfg.pvMeters.empty() || _cfg.pvMeters.contains(thing->id()))) {
            thing->propertiesObservable()
                | filter([thing](const auto& p) { return p.template contains<ThingPropertyKey::power>(); })
                | map([thing](const auto& p) { return std::make_pair(thing->id(), *(p.template get<ThingPropertyKey::power>())); })
                | subscribe([this](const auto& power) {
                      _pvPowers.get_observer().on_next(power);
                  });
        }
    });

    _pvPowers.get_observable()
        | scan(std::map<std::string, int>{}, [](auto&& result, const auto& value) {
              result[value.first] = value.second;
              return std::move(result);
          })
        | map([](const auto& kv) { return std::accumulate(kv.cbegin(), kv.cend(), 0, [](int result, const auto& value) { return result + value.second; }); })
        | subscribe(_pvPower.get_observer());

    _pvPower.get_observable()
        | combine_latest([this](int pvPower, int gridPower) {
              const auto now = std::chrono::system_clock::now();
              const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
              return Site::Properties { (int)seconds, pvPower, gridPower };
          }, _gridPower.get_observable())
        | debounce(_cfg.debounceTime, rpp_uvw::schedulers::main_thread_scheduler{})
        | scan(Site::Properties(), [this](Site::Properties&& prev, const Site::Properties& curr) {
              ema(prev.shortTermGridPower, curr.gridPower, std::chrono::milliseconds((curr.ts - prev.ts) * 1000), _cfg.shortTermTau);
              ema(prev.longTermGridPower, curr.gridPower, std::chrono::milliseconds((curr.ts - prev.ts) * 1000),  _cfg.longTermTau);
              return std::move(Site::Properties{curr.ts, curr.pvPower, curr.gridPower, prev.shortTermGridPower, prev.longTermGridPower});
          })
        | subscribe(_properties.get_observer());

    // Init with zero power
    _pvPower.get_observer().on_next(0);
    _gridPower.get_observer().on_next(0);

    _properties.get_observable().subscribe([this](const Site::Properties& data){
        while (_history.size() > 3600) {
            _history.pop_front();
        }
        _history.push_back(data);
    });
}

dynamic_observable<int> Site::gridPower() const {
    return _gridPower.get_observable();
}

dynamic_observable<Site::Properties> Site::properties() const {
    return _properties.get_observable();
}

const std::list<Site::Properties>& Site::history() const {
    return _history;
}

} // namespace uvw_iot::util
