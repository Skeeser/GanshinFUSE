#!/bin/bash

echo "<----------------start build GanshinFUSE!---------------->"

# 安装libfuse
echo "<----------------install pkg---------------->"
cd ~ && sudo apt-get install git gcc vim lrzsz openssh-server meson pkg-config make unity-tweak-tool libtool m4 autoconf pkg-config meson cmake

if [ $? -eq 0 ]; then
    echo "<----------------clone repositories---------------->"
    git clone https://github.com/libfuse/libfuse.git
else
    echo -e "\e[31m<----------------install pkg false---------------->\e[0m"
    exit
fi

if [ $? -eq 0 ]; then
    echo "<----------------start build---------------->"
    cd libfuse && mkdir build && cd build && meson .. && ninja && sudo ninja install
else
    echo -e "\e[31m<----------------clone repositories false---------------->\e[0m"
    exit
fi


# 将编译出的libfuse复制到本项目
if [ $? -eq 0 ]; then
    echo "<----------------copy libfuse to GanshinFUSE---------------->"
    cp -r ./lib ~/GanshinFUSE
else
    echo -e "\e[31m<----------------build false---------------->\e[0m"
    exit
fi

if [ $? -ne 0 ]; then
    echo -e "\e[31m<----------------copy libfuse false---------------->\e[0m"
    exit
fi
