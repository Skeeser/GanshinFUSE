# cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.2)
 
# project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(GanshinFUSE)
 
# SET(CMAKE_CXX_STANDARD 11) # 设置c++版本为c11 
SET(CMAKE_C_STANDARD 11)  # 设置c标准

# 定义一个选项，用于控制生成何种类型的Makefile
option(USE_DEBUG "Build with debug flags" ON)

# 在Debug模式下添加调试标志，优化标志，和gtest
if(USE_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_BUILD_TYPE Release)
endif()



# head file path，头文件目录
INCLUDE_DIRECTORIES(
 include
)
 
# source directory，源文件目录
# AUX_SOURCE_DIRECTORY(src DIR_SRCS)
 
# set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行 的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(SRC_INIT src/GanshinInit.c src/Utils.c)
SET(EXE_INIT GanshinInit)

SET(SRC_FS src/GanshinFS.c)
SET(EXE_FS GanshinFS)

SET(SRC_TEST test/GanshinTest.cpp)
SET(EXE_TEST GanshinTest)

SET(SRC_FUC src/Utils.c src/FileOper.c src/GFSFctional.c  src/Utils.c )
SET(LIB_FUC GFSFuctional)

# add library
ADD_LIBRARY(${LIB_FUC} ${SRC_FUC})

# add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(${EXE_INIT} ${SRC_INIT})
ADD_EXECUTABLE(${EXE_FS} ${SRC_FS})
ADD_EXECUTABLE(${EXE_TEST} ${SRC_TEST})


# 链接libfuse3库和pthread库
find_package(PkgConfig)
# pkg_search_module(自定义名  必需项  查找库名)
pkg_search_module(FUSE REQUIRED fuse3)

target_include_directories(${EXE_INIT} PUBLIC ${FUSE_INCLUDE_DIRS})
target_link_libraries(${EXE_INIT} PUBLIC ${FUSE_LIBRARIES})

# target_include_directories(${EXE_FS} PUBLIC ${FUSE_INCLUDE_DIRS})
# target_link_libraries(${EXE_FS} PUBLIC ${FUSE_LIBRARIES})

# target_include_directories(${EXE_TEST} PUBLIC ${FUSE_INCLUDE_DIRS})
# target_link_libraries(${EXE_TEST} PUBLIC ${FUSE_LIBRARIES})

target_include_directories(${LIB_FUC} PUBLIC ${FUSE_INCLUDE_DIRS})
target_link_libraries(${LIB_FUC} PUBLIC ${FUSE_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(${EXE_INIT} PUBLIC Threads::Threads)
target_link_libraries(${LIB_FUC} PUBLIC Threads::Threads)
# target_link_libraries(${EXE_FS} PUBLIC Threads::Threads)
# target_link_libraries(${EXE_TEST} PUBLIC Threads::Threads)

# target_link_libraries(${EXE_TEST} ${LIB_FUC} )
target_link_libraries(${EXE_FS} ${LIB_FUC} )

find_package(GTest CONFIG REQUIRED)
target_link_libraries(${EXE_TEST} PUBLIC GTest::gtest GTest::gtest_main ${LIB_FUC} )

