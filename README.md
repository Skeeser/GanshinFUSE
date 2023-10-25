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

[简体中文](README_ZH.md)  | [English](README_EN.md) 

OS File System Based on FUSE.

<br>

~~**要是SCUTer看见了, 都是一个班, 抄了很容易被老师认为抄袭哦**~~

<br>

</div>

<br>

## 功能特性

- 利用FUSE框架创建一个SFS文件系统，这个文件系统采用inode方式管理文件系统
- 类似于UFS, 空闲块和空闲inode均采用位图的方式管理，文件数据块采用直接和间接索引的方式，支持多级目录。
- 为了方便实现，文件名格式为8.3，即文件名为8个字节，扩展名为3个字节。之后想办法实现长文件名。

<br>

## 文件结构(可选)

- doc => 存放开发文档



<br>

## 系统架构

Linux

<br>

## 快速开始

### 依赖检查

<!-- 描述该项目的依赖，比如依赖的包、工具或者其他任何依赖项 -->
<br>

### 构建

<!-- 描述如何构建该项目 -->
<br>

### 运行

<!-- 描述如何运行该项目 -->
<br>

## 使用指南

<!-- 描述如何使用该项目 -->
<br>

## 关于作者
keeser

<br>

## TODO（可选）

<br>
## 许可证

MPL 2.0
