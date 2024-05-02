CPMAddPackage("gh:mincequi/uvw_net@0.1.1")
CPMAddPackage("gh:victimsnino/ReactivePlusPlus@2.1.1")
#CPMAddPackage("gh:victimsnino/ReactivePlusPlus#bebc5b8")
CPMAddPackage("gh:nlohmann/json@3.11.3")

#add_subdirectory(goe)

add_library(uvw_iot STATIC
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
