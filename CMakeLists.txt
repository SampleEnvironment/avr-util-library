cmake_minimum_required(VERSION 3.20)

set(This Gaszaehler)

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


add_compile_definitions(UNIT_TEST)

enable_testing()

add_subdirectory(googletest)





set(Headers
    DS3231M.h
    TestMocks/Mock_main.h
    TestMocks/Mock_I2C_utilities.h
    TestMocks/Mock_LCD.h
)

set(Sources 
    DS3231M.c
    TestMocks/Mock_main.c
    TestMocks/Mock_I2C_utilities.c
    TestMocks/Mock_LCD.c
)

add_library(${This} STATIC ${Sources} ${Headers})


add_subdirectory(unitTests)