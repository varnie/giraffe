set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME giraffe)
project(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
foreach(testsourcefile ${files})
    get_filename_component(FILE_WITHOUT_EXT ${testsourcefile} NAME_WE)
    add_test(NAME "${FILE_WITHOUT_EXT}" COMMAND "${FILE_WITHOUT_EXT}" )
endforeach()