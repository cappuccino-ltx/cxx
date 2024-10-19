
#VERSION ?
cmake_minimum_required(VERSION 3.10)

project(tcp)

file(GLOB server_src tcpserver.cc)
file(GLOB client_src tcpclient.cc)


add_executable(tcpclient ${client_src})
add_executable(tcpserver ${server_src})

find_library(thread pthread)

target_link_libraries(tcpserver ${thread})