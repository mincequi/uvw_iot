CPMAddPackage("gh:mincequi/uvw_net#d0cc276")
#CPMAddPackage("gh:victimsnino/ReactivePlusPlus@2.1.1")
CPMAddPackage("gh:victimsnino/ReactivePlusPlus#bebc5b8")
CPMAddPackage("gh:nlohmann/json@3.11.3")

add_library(uvw_iot STATIC
  common/Thing.cpp
  common/ThingFactory.cpp
  common/ThingRepository.cpp
  http/HttpThing.cpp
  shelly/ShellyThing.cpp
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

#add_executable(test_iot
#   ../main.cpp
#)
#target_link_libraries(test_iot
#PRIVATE
#  uvw_iot
#)
