cmake_minimum_required (VERSION 2.6)
project (hIRC_bot)
add_executable(client client.cc)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
endif()


find_package(Boost)
if (BOOST_FOUND)
  include_directories(${BOOST_INCLUDE_DIRS})
  target_link_libraries (helloworld ${BOOST_LIBRARIES})
endif (BOOST_FOUND)
