cmake_minimum_required (VERSION 2.6)
project (hIRC_bot)

add_library(client client.cc)
add_library(libiconv_wrapper libiconv_wrapper.cc)
add_library(logger logger.cc)
add_library(libmessage message.cc)
add_library(xml_config xml_config.cc)

target_link_libraries(client
  libiconv_wrapper
  libmessage
  logger
  xml_config)


add_executable(ircHistoryBot main.cc)

target_link_libraries(ircHistoryBot client)


if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
endif()


find_package(Boost)
if (BOOST_FOUND)
  include_directories(${BOOST_INCLUDE_DIRS})
  target_link_libraries (helloworld ${BOOST_LIBRARIES})
endif (BOOST_FOUND)
