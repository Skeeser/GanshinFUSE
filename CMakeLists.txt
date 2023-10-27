# cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.2)
 
# project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(GanshinFUSE)
 
# SET(CMAKE_CXX_STANDARD 11) # 设置c++版本为c11 
SET(CMAKE_C_STANDARD 11)  # 设置c标准
 
# head file path，头文件目录
INCLUDE_DIRECTORIES(
 include
)
 
# source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
 
# set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行 的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(SRC_INIT src/GanshinInit.c)
SET(EXE_INIT GanshinInit)
 
# add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(${EXE_INIT} ${SRC_INIT})

# 链接libfuse3库和pthread库
find_package(PkgConfig)
# pkg_search_module(自定义名  必需项  查找库名)
pkg_search_module(FUSE REQUIRED fuse3)
target_include_directories(${EXE_INIT} PUBLIC ${FUSE_INCLUDE_DIRS})
target_link_libraries(${EXE_INIT} PUBLIC ${FUSE_LIBRARIES})


find_package(Threads REQUIRED)
target_link_libraries(${EXE_INIT} PUBLIC Threads::Threads)