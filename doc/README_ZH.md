<div align="center">

<img alt="LOGO" src="https://github.com/Skeeser/GanshinFUSE/blob/main/assets/sys.jpg" width="256" height="346" />

# GanshinFUSE

<br>

<div>
    <img alt="C" src="https://img.shields.io/badge/C-gcc-%2300599C">
</div>
<div>
    <img alt="platform" src="https://img.shields.io/badge/platform-Linux%20-blueviolet">
</div>
<div>
    <img alt="license" src="https://img.shields.io/github/license/Skeeser/GanshinFUSE">
    <img alt="commit" src="https://img.shields.io/github/commit-activity/m/Skeeser/GanshinFUSE?color=%23ff69b4">
    <img alt="stars" src="https://img.shields.io/github/stars/Skeeser/GanshinFUSE?style=social">
</div>
<br>

[简体中文](doc/README_ZH.md) | [English](doc/README_EN.md)

OS File System Based on libfuse.  
基于libfuse实现的类UFS文件系统  
如果觉得不错的话, 麻烦点个star吧 :wink:​  

<br>

</div>

<br>

## 功能特性
- 利用 libfuse 框架创建了一个类 UFS 文件系统，采用 inode 方式管理文件系统  
- 空闲块和空闲 inode 均采用位图的方式管理，文件数据块采用直接和间接索引的方式，支持多级目录。
- 有意减少文件IO操作，缓存进内存中，等更改到最后才写入文件，使文件操作用时更短，效率更高。
- 文件和目录的增删改查均用了哈希表来进行, 比起常规的顺序存储, 效率更高。 
- 清晰统一的函数名和变量名，好的代码不用多少注释，光看名字就能清楚作用。规范函数名称, 作用和返回值, 默认返回0为成功。示例可以看(FileOper.h)  
- 代码风格仿照c++的异常机制, 运行出错容易回溯。严格控制内存管理，防止内存泄漏。  
- 用了shell脚本安装依赖, 一行命令轻松安装依赖, 告别复杂的环境配置  
- 写了CMakeList.txt, 用cmake生成makefile构建, 简单高效, 跨平台性强  
- 使用gtest框架进行单元测试, 使代码鲁棒性比较强, 功能可靠 
<br>

## 文件结构

- doc => 存放开发文档
  - develop_log.md => 开发日志
- include => 头文件
- src => 源代码
- assets => 资源文件
- build => 编译文件
- example => 存放示例文件
- out => 存放磁盘文件
- test => 测试代码
- CMakeLists.txt => CMake 配置文件
- build.sh => 构建脚本
- README.md => 说明文档  

<br>

## 系统架构

Linux

<br>


## 依赖检查
- cmake
- gcc
- libfuse
- gtest (单元测试需要)
- lldb (调试需要)

<br>

## 构建
### 下载本项目
```shell
cd ~
git clone https://github.com/Skeeser/GanshinFUSE.git
```

<br>

### 脚本一行构建
```shell
sudo chmod +x ~/GanshinFUSE/build.sh && ~/GanshinFUSE/build.sh
```

<br>

### 创建disk文件
8MB大小
```shell
dd bs=1M count=8 if=/dev/zero of=~/GanshinFUSE/out/diskimg
```
  
<br>

### 设置disk地址
需要修改`~/GanshinFUSE/include/config.h` 中的  
```c
#define DISK_PATH "/home/keeser/GanshinFUSE/out/diskimg"
```
设置你的disk的绝对路径

<br>

## 编译
### 设置debug 或者 release版本
修改CMakeList.txt中的  
```c
option(USE_DEBUG "Build with debug flags" ON)
```
debug版本--ON  
release版本--OFF  
特别说明: 如果设置了release版本就不会编译gtest代码了  
如果需要安装gtest，可以见[开发文档](doc/develop_log.md)  

<br>

### 执行编译
```shell
cd ~/GanshinFUSE/
mkdir build
cd build
cmake .. 
make
```

<br>

## 运行
### 在build文件夹下
```shell
cd build/
```
<br>

### 初始化

```shell
./GanshinInit
```

能够看到本项目的logo

<div align="left" >
<img alt="logo2" src="https://github.com/Skeeser/GanshinFUSE/blob/main/assets/logo.png" />
</div>

<br>

### 新建挂载的文件夹
新建要将文件系统挂载的文件夹, 任意位置文件夹都可  
此处将文件夹建在build文件夹内  
```shell
cd build/
mkdir mountdir
```

<br>

### 将文件系统挂载
GanshinFS 启动  
```shell
# 默认文件系统在后台运行
./GanshinFS ./mountdir

# 强制文件系统在前台运行
./GanshinFS -f ./mountdir

# 将文件系统以调试模式运行
./GanshinFS -d ./mountdir
```

<br>

### 卸载文件系统
```shell
fusermount -u mountdir
```

<br>

### 运行测试
```shell
./GanshinTest
```

<br>

## 调试工具
LLDB
详细使用见[开发文档](doc/develop_log.md)

<br>

## 使用指南
- 注意依赖有没有装上

<br>

## 如何贡献
如果你见到了这个项目并且想要完善它  
强烈建议阅读下面文档  

- [如何用github参与开源项目](doc/github参与开源项目流程.md)
- [开发文档](doc/develop_log.md)

<br>

## 关于作者

keeser

<br>

## TODO

- [x] 先搞明白制订开发计划
- [x] 开发GanshinInit
- [x] 开发GanshinFS
- [x] 看能不能搞单元测试
- [ ] 试着将写的GanshinFuse结合buildroot烧写在嵌入式板上
- [x] 测试通过后整理开源仓库
- [x] 写实验报告

<br>

## 许可证

MPL 2.0
