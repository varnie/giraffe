cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -pedantic -Werror -Wall -Wextra -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=sign-compare")
add_definitions(${GCC_CPP11_LINK_FLAGS})

add_library(ecs SHARED Storage.cpp ../include/Storage.h)
