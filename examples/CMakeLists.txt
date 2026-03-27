add_executable(read_bme280 read_bme280.cpp)
target_link_libraries(read_bme280 PRIVATE busbridge-core)

add_executable(read_can_sniffer can_sniffer.cpp)
target_link_libraries(read_can_sniffer PRIVATE busbridge-core)

install(TARGETS read_bme280 read_can_sniffer
        RUNTIME DESTINATION bin/
)
