

cmake_minimum_required(VERSION 3.10)

project(calculate)

file(GLOB server_src ./server.cc)
add_executable(server ${server_src})

find_library(thread pthread)
target_link_libraries(server ${thread})