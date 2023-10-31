# 开发日志

<br>

## 开发计划(ToDos)
- [x] 了解fuse怎么使用  
- [x] 运行hello world例子
- [x] 尝试读懂hello.c的源码
- [x] 写CMakeList.txt
- [x] 根据hello.c的源码, 试着写一下GanshinFUSE的类似代码
- [x] 实现格式化程序(GanshinInit)
- [ ] 文件系统实现(GanshinFS)
- [ ] 将所有的步骤写成脚本
- [ ] 解决哈希冲突
- [ ] 支持文件和文件夹同名


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

## gtest
### 安装
```shell
cd ~
git clone https://github.com/google/googletest.git
sudo apt-get install cmake
cd ~/googletest/
mkdir build
cd build/
cmake ..
make
sudo make install
```

### 测试文件
在test文件夹下
由于要用到自己写的c库, 所以测试的代码要写上extern C
所以先把c库编译成静态库, 再链接到测试的cpp

### 单独运行某个测试样例
```shell
./GanshinTest --gtest_filter=GFS_init.init
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
#### 需要把InodeBitmap的初始化的对应块
1. 根目录

需要把第一个Byte的第一个bit置1

<br>

#### 需要把Bitmap的初始化的对应块
1. SuperBlock
2. InodeBitmap
3. DataBitmap
4. 根目录

<br>


### 初始化Inode
#### 将根目录的相关信息填写到inode区的第一个inode。
**索引表结构**
i节点, 根目录指向data区的一个块地址

<br>

## 文件系统实现(GanshinFS)
### 初始化GFS_init
暂时没有什么要初始化操作的

### 获取文件属性GFS_getattr
要利用到Config.h定义的GFileData数据结构
#### 根据文件名查找对应的文件的DR和inode块号
采取哈希表的方式查找

<br>

## 文件操作功能封装(FileOper)
### int readDataByBlkId(long blk_id,struct GDataBlock *data_blk);
读取超级块, 然后把数据放在data_blk里

### int getFileDirToAttr(const char * path,struct GFileDir *attr);
根据路径，到相应的目录寻找文件的GFileDir，并赋值给attr
#### 实现思路
1. 读出超级块, 拿到相关数据
2. 将超级块类型转换为GSuperBlock
3. 解析path
<br>

## 参考资料

- [inode 详解](https://www.cnblogs.com/llife/p/11470668.html)
- [fuse结合buildroot](https://cloud.tencent.com/developer/article/1006138?areaSource=106002.3)