cmake_minimum_required (VERSION 2.6)
project (hIRC_bot)


set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

add_library(client client.cc)
add_library(libiconv_wrapper libiconv_wrapper.cc)
add_library(logger logger.cc)
add_library(libmessage message.cc)
add_library(xml_config xml_config.cc)

find_package(Boost)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries (xml_config ${Boost_LIBRARIES})
endif (Boost_FOUND)

target_link_libraries(client
  libiconv_wrapper
  libmessage
  logger
  xml_config)

add_executable(ircHistoryBot main.cc)

target_link_libraries(ircHistoryBot client)
