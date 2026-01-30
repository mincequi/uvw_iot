#add_subdirectory(goe)

add_library(uvw_iot STATIC
  Thing.cpp
  ThingFactory.cpp
  ThingRepository.cpp
  goe/GoeCharger.cpp
  goe/GoeUtil.cpp
  http/HttpThing.cpp
  shelly/ShellyThing.cpp
  sunspec/SunSpecThing.cpp
  util/Filter.cpp
  util/MultiThing.cpp
  util/Site.cpp
  util/Util.cpp
)

target_link_libraries(uvw_iot
PUBLIC
  RPP::rpp
  uvw_net::http
  uvw_net::sunspec
  nlohmann_json
  reflectcpp
)

target_include_directories(uvw_iot
  PUBLIC
    ../
)
