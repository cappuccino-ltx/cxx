

cmake_minimum_required(VERSION 3.10)

project(calculate)

file(GLOB server_src ./server.cc)
file(GLOB client_src ./client.cc)
add_executable(server ${server_src})
add_executable(client ${client_src})

find_library(thread pthread)
find_library(json jsoncpp)
target_link_libraries(server ${thread} ${json}) 
target_link_libraries(client ${json}) 