# CMakeList.txt : CMake project for ComUdpProxy, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.26)

#boost
project(p)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.82.0
)
FetchContent_MakeAvailable(Boost)


# Add source to this project's executable.
add_executable (ComUdpProxy "ComUdpProxy.cpp" "ComUdpProxy.h")


target_link_libraries(ComUdpProxy
  PRIVATE Boost::filesystem
  Boost::program_options
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ComUdpProxy PROPERTY CXX_STANDARD 20)
endif()




# TODO: Add tests and install targets if needed.
