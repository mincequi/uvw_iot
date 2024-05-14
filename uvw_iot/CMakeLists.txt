#add_subdirectory(goe)

add_library(uvw_iot STATIC
  MultiThing.cpp
  Site.cpp
  Thing.cpp
  ThingFactory.cpp
  ThingRepository.cpp
  Util.cpp
  goe/GoeCharger.cpp
  goe/GoeUtil.cpp
  http/HttpThing.cpp
  shelly/ShellyThing.cpp
  sunspec/SunSpecThing.cpp
  MultiThing.h MultiThing.cpp
)

target_link_libraries(uvw_iot
PUBLIC
  RPP::rpp
  uvw_net::http
  uvw_net::sunspec
  nlohmann_json::nlohmann_json
)

target_include_directories(uvw_iot
  PUBLIC
    ../
)
