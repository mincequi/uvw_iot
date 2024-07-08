#include <chrono>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <rpp/operators/combine_latest.hpp>
#include <rpp/operators/debounce.hpp>
#include <rpp/subjects/publish_subject.hpp>
#include <uvw/loop.h>
#include <uvw/timer.h>
#include <uvw_iot/RppUvw.h>

using namespace std::chrono_literals;
using namespace rpp::operators;
using rpp::subjects::publish_subject;

TEST_CASE("Example test case") {
    int debounceCount = 0;
    publish_subject<int> _gridPower;
    publish_subject<int> _pvPower;

    auto _gridPowerTimer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    _gridPowerTimer->on<uvw::timer_event>([&_gridPower](const auto&, auto&) {
        _gridPower.get_observer().on_next(rand()%100);
    });
    _gridPowerTimer->start(0ms, 2ms);

    auto _pvPowerTimer = uvw::loop::get_default()->resource<uvw::timer_handle>();
    _pvPowerTimer->on<uvw::timer_event>([&_pvPower](const auto&, auto&) {
        _pvPower.get_observer().on_next(rand()%100);
    });
    _pvPowerTimer->start(0ms, 3ms);

    _pvPower.get_observable()
        | combine_latest([](int pvPower, int gridPower) {
              return std::make_pair(pvPower, gridPower);
          }, _gridPower.get_observable())
        | debounce(1ms, rpp_uvw::schedulers::main_thread_scheduler{})
        | subscribe([&](const std::pair<int,int>& data) {
            ++debounceCount;
              if (debounceCount%1000 == 0) {
                  std::cout << "debounceCount: " << debounceCount << std::endl;
              }
          });

    uvw::loop::get_default()->run();
}
