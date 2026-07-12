cmake_minimum_required(VERSION 3.15)
project(Embedded_Sensor_Data_Processor)

set(CMAKE_CXX_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(Embedded_Sensor_Data_Processor
        main.c
        adc.c
        cmake-build-debug/
        adc.h
        stats.c
        stats.h
        IO.c
        IO.h
        )

target_link_libraries(Embedded_Sensor_Data_Processor m)
