set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2 -DNDEBUG")

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(testsourcefile ${files})
    get_filename_component(FILE_WITHOUT_EXT ${testsourcefile} NAME_WE)
    add_executable("${FILE_WITHOUT_EXT}" ${testsourcefile})
    target_link_libraries("${FILE_WITHOUT_EXT}" ${GTEST_LIBRARIES} pthread giraffe)
endforeach()
