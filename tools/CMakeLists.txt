add_executable(collect
  collect.cpp
)
target_link_libraries(collect
  uvw_iot
  uvw_net::dns_sd
)
