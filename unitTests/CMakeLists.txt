cmake_minimum_required(VERSION 3.20)

set(This DS3231mTests)

set(Sources 
    DS3231M_Test.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} 
PUBLIC gtest_main
Gaszaehler
)

add_test(
    NAME ${This}
    COMMAND ${This}
)