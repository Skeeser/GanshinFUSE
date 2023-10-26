# cmake verson，指定cmake版本 
 cmake_minimum_required(VERSION 3.2)
 
 # project name，指定项目的名称，一般和项目的文件夹名称对应
 PROJECT(GanshinFUSE)
 
 # head file path，头文件目录
 INCLUDE_DIRECTORIES(
 include
 )
 
 # source directory，源文件目录
 AUX_SOURCE_DIRECTORY(src DIR_SRCS)
 
 # set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行 的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
 SET(
 ${DIR_SRCS}
 )
 
 # add executable file，添加要编译的可执行文件
 ADD_EXECUTABLE(${PROJECT_NAME})
 
 