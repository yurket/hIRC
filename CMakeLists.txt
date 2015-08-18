cmake_minimum_required (VERSION 2.6)
project (hIRC_bot)

add_library(libiconv_wrapper libiconv_wrapper.cc)
add_executable(client client.cc)
target_link_libraries(client LINK_PUBLIC libiconv_wrapper)


if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
endif()


find_package(Boost)
if (BOOST_FOUND)
  include_directories(${BOOST_INCLUDE_DIRS})
  target_link_libraries (helloworld ${BOOST_LIBRARIES})
endif (BOOST_FOUND)
