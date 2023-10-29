# 开发日志

<br>

## 开发计划(ToDos)
- [x] 了解fuse怎么使用  
- [x] 运行hello world例子
- [x] 尝试读懂hello.c的源码
- [x] 写CMakeList.txt
- [x] 根据hello.c的源码, 试着写一下GanshinFUSE的类似代码
- [ ] 实现格式化程序(GanshinInit)

- [ ] 将所有的步骤写成脚本


<br>

## fuse框架的使用
### hello world例子
挂载文件系统并查看
```shell
cd ~/libfuse/build/example
mkdir testmount
ls -al testmount
./hello testmount
ls -al testmount
cat testmount/hello
```

卸载文件系统
```shell
fusermount -u testmount
```

<br>

## 创建过程
### 创建文件系统所在的文件 
```shell
dd bs=1M count=8K if=/dev/zero of=~/GanshinFUSE/out/diskimg
```

<br>

## 格式化程序(GanshinInit)
### 生成一个8M大小的文件作为文件系统的载体；
```shell
dd bs=1M count=8 if=/dev/zero of=~/GanshinFUSE/out/diskimg
```
- dd：这是一个用于复制文件和数据的命令行工具。
- bs=1M：bs 表示块大小（block size），这里设置为 11M。这意味着 dd 命令将每次读取和写入 11M 的数据。
- count=8K：count 表示要复制的块数，这里设置为 8，表示要复制 8 个块，每个块大小为 1MB。
- if=/dev/zero：if 表示输入文件（input file）。在这里，/dev/zero 是一个特殊的设备文件，它会产生连续的零字节数据。
- of=~/GanshinFUSE/out/diskimg：of 表示输出文件（output file）。这里指定的文件路径是 ~/GanshinFUSE/out/diskimg，它将用来存储生成的磁盘镜像。
[dd结果](assets/dd.png)

<br>

### 将文件系统的相关信息写入超级块
这部分根据之前设置的文件系统相关设置进行计算初始化

<br>

### 初始化Bitmap


<br>


### 根目录作为文件系统中的第一个文件，需要做的事情
#### inode位图的第一个字节的第一位置为1，表示第一个inode已分配
初始化inode位图

<br>

#### 将根目录的相关信息填写到inode区的第一个inode。


<br>

## 参考资料

- [inode 详解](https://www.cnblogs.com/llife/p/11470668.html)
