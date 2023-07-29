# CMakeList.txt : CMake project for ComUdpProxy, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.26)

#boost
project(p)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options asio date_time)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_PROGRESS TRUE
  GIT_TAG boost-1.82.0
)
FetchContent_MakeAvailable(Boost)

#message(STATUS "Boost src directories: ${Boost_SOURCE_DIR}")

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#  message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#INCLUDE_DIRECTORIES(${BOOST_LIBRARY_INCLUDES})
#include_directories(asio INTERFACE ${boost_asio_SOURCE_DIR}/include)

# Add source to this project's executable.
add_executable (ComUdpProxy "ComUdpProxy.cpp" "ComUdpProxy.h" "Sender.h"  "Receiver.h" "Message.h")

#target_include_directories(ComUdpProxy PRIVATE ${BOOST_LIBRARY_INCLUDES})

target_link_libraries(ComUdpProxy
  PRIVATE 
  Boost::asio
  Boost::filesystem
  Boost::thread
  Boost::program_options
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ComUdpProxy PROPERTY CXX_STANDARD 20)
endif()




# TODO: Add tests and install targets if needed.
