<div align="center">

<img alt="LOGO" src="assets/sys.jpg" width="256" height="346" />

# GanshinFUSE

<br>

<div>
    <img alt="C" src="https://img.shields.io/badge/C-gcc-%2300599C">
</div>
<div>
    <img alt="platform" src="https://img.shields.io/badge/platform-Windows%20-blueviolet">
</div>
<div>
    <img alt="license" src="https://img.shields.io/github/license/Skeeser/GanshinFUSE">
    <img alt="commit" src="https://img.shields.io/github/commit-activity/m/Skeeser/GanshinFUSE?color=%23ff69b4">
    <img alt="stars" src="https://img.shields.io/github/stars/Skeeser/GanshinFUSE?style=social">
</div>
<br>

[简体中文](README_ZH.md) | [English](README_EN.md)

OS File System Based on FUSE.

<br>

~~**要是 SCUTer 看见了, 都是一个班, 抄了很容易被老师认为抄袭哦**~~

<br>

</div>

<br>

## 功能特性
- 开创性用了shell脚本安装依赖, 一行命令轻松构建
- 利用 FUSE 框架创建一个 SFS 文件系统，这个文件系统采用 inode 方式管理文件系统
- 类似于 UFS, 空闲块和空闲 inode 均采用位图的方式管理，文件数据块采用直接和间接索引的方式，支持多级目录。
- 为了方便实现，文件名格式为 8.3，即文件名为 8 个字节，扩展名为 3 个字节。之后想办法实现长文件名。

<br>

## 文件结构

- doc => 存放开发文档
  - develop_log.md => 开发日志
- include => 头文件
- src => 源代码
- assets => 资源文件
- build => 编译文件
- CMakeLists.txt => CMake 配置文件

<br>

## 系统架构

Linux

<br>

## 快速开始

### 依赖检查

- cmake
- gcc
- libfuse

<br>

### 构建
#### 下载本项目
```shell
cd ~
git clone https://github.com/Skeeser/GanshinFUSE.git
```

#### 脚本一行构建
```shell
sudo chmod +x ~/GanshinFUSE/build.sh && ~/GanshinFUSE/build.sh
```

<br>

#### 编译

```shell
mkdir build
cd build
cmake ..
make
```

<br>

### 运行


<br>

## 使用指南
- 注意依赖有没有装上

<br>

## 如何贡献
如果你见到了这个项目并且想要完善它  
强烈建议阅读下面文档  
- [如何用github参与开源项目](doc/github参与开源项目流程.md)
- [开发文档](doc/develop_log.md)

## 关于作者

keeser

<br>

## TODO

- [ ] 先搞明白制订开发计划
- [ ] 开发功能
- [ ] 看能不能搞单元测试
- [ ] 测试通过后整理开源仓库
- [ ] 写实验报告

<br>

## 许可证

MPL 2.0
