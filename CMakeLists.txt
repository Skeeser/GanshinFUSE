# cmake verson，指定cmake版本 
 cmake_minimum_required(VERSION 3.2)
 
 # project name，指定项目的名称，一般和项目的文件夹名称对应
 PROJECT(test_sqrt)
 
 # head file path，头文件目录
 INCLUDE_DIRECTORIES(
 include
 )
 
 # source directory，源文件目录
 AUX_SOURCE_DIRECTORY(src DIR_SRCS)
 
 # set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行 的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
 SET(TEST_MATH
 ${DIR_SRCS}
 )
 set (CMAKE_CXX_STANDARD 11) # 设置c++版本为c11 
 
 # add executable file，添加要编译的可执行文件
 ADD_EXECUTABLE(${PROJECT_NAME} ${TEST_MATH})
 
 # 引入dylib 或者 dll  so文件环境目录，自动去搜索该目录下的相对于的dylib  so  dll文件
link_directories(/Users/zhoumeixu/Documents/cmakedemoa/build)
 
 # add link library，添加可执行文件所需要的库，比如我们用到了libm.so（命名规则：lib+name+.so），就添加该库的名称
 TARGET_LINK_LIBRARIES(${PROJECT_NAME} m)