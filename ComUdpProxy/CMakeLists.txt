# CMakeList.txt : CMake project for ComUdpProxy, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.26)

#boost
project(p)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options asio date_time)
set(BOOST_ENABLE_CMAKE ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_PROGRESS TRUE
  GIT_TAG boost-1.82.0
)
FetchContent_MakeAvailable(Boost)


message("boost asio downloaded at " ${Boost_SOURCE_DIR}/libs/asio/include)

file(GLOB files "${Boost_SOURCE_DIR}/libs/*/include")
foreach(filename ${files})
    include_directories(${filename})
endforeach()

file(GLOB files "${Boost_SOURCE_DIR}/libs/*/*/include")
foreach(filename ${files})
    include_directories(${filename})
endforeach()


include_directories (SYSTEM ${Boost_INCLUDE_DIR})
message(STATUS "Boost version: ${Boost_VERSION}")

# Add source to this project's executable.
add_executable (ComUdpProxy "ComUdpProxy.cpp" "ComUdpProxy.h" "Sender.h"  "Receiver.h")


target_link_libraries(ComUdpProxy
  PUBLIC Boost::filesystem
  Boost::thread
  Boost::program_options
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ComUdpProxy PROPERTY CXX_STANDARD 20)
endif()




# TODO: Add tests and install targets if needed.
