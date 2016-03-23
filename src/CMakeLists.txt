set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -pedantic -Werror -Wall -Wextra -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=sign-compare -DNDEBUG")

include_directories(include)

add_library(giraffe SHARED Component.cpp ComponentsPool.cpp System.cpp FilterIterator.cpp Predicates.cpp Storage.cpp Giraffe.cpp)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
set(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

install(
      DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
      DESTINATION include/giraffe
)

INSTALL(TARGETS giraffe LIBRARY DESTINATION "${libdir}" ARCHIVE DESTINATION "${libdir}" )
