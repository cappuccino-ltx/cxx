cmake_minimum_required(VERSION 3.10)
project(udp)

#添加一些编译选项
#add_compile_options(std=c++11)

#指定头文件路径
include_directories(.)

#指定源文件所在目录 ,因为一个文件下有多个main函数的文件，所以不能直接用这个SRC_DIR去add_executable
aux_source_directory(. SRC_DIR)

#查找具体的源文件
#file(GLOB client_source ${CMAKE_CURRENT_LIST_DIR}/udpclient.cc)
#file(GLOB server_source ${CMAKE_CURRENT_LIST_DIR}/udpserver.cc)

file(GLOB client_source ./udpclient.cc)
file(GLOB server_source ./udpserver.cc)

#指定要生成的可执行程序
add_executable(udpserver ${server_source})
add_executable(udpcleint ${client_source})

