cmake_minimum_required(VERSION 2.8)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG -std=c++11 -pedantic -Werror -Wall -Wextra -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=sign-compare")
#add_definitions(${GCC_CPP11_LINK_FLAGS})

set(PROJECT_NAME ecs)
project(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
foreach(testsourcefile ${files})
    get_filename_component(FILE_WITHOUT_EXT ${testsourcefile} NAME_WE)
    add_test(NAME "${FILE_WITHOUT_EXT}" COMMAND "${FILE_WITHOUT_EXT}" )
endforeach()
