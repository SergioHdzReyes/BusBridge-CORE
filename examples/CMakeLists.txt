add_executable(read_bme280 read_bme280.cpp)
target_link_libraries(read_bme280 PRIVATE busbridge-core)

install(TARGETS read_bme280
        RUNTIME DESTINATION bin/
)
