cmake_minimum_required (VERSION 2.6)
project (hIRC_bot)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Boost)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif (Boost_FOUND)

add_executable(ircHistoryBot
  main.cc
  client.cc
  libiconv_wrapper.cc
  logger.cc
  message.cc
  xml_config.cc)

target_link_libraries(ircHistoryBot ${Boost_LIBRARIES})
