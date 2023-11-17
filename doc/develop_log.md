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
- [x] GFS_init初始化
- [x] GFS_getattr获取文件属性
- [x] GFS_mknod创建文件
- [ ] 优化disk的地址逻辑, 弄成不用手动设置
- [ ] 将所有的步骤写成脚本
- [ ] 解决哈希冲突
- [ ] 支持文件和文件夹同名
- [ ] 优化函数参数名

<br>

## fuse框架的使用
### hello world例子
挂载文件系统并查看
```shell
cd ~/libfuse/build/example
mkdir testmount
ls -al testmount
./hello -f testmount
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
 
<br>

### 测试文件
在test文件夹下  
由于要用到自己写的c库, 所以测试的代码要写上extern C  
所以先把c库编译成静态库, 再链接到测试的cpp  
 
<br>

### 编写测试
可以在原本的GanshinTest.cpp添加测试代码  
如果需要添加别的测试文件, 则需要重新写cmakelist.txt

<br>


### 运行所有测试案例
```shell
cd build/
./GanshinTest 
```
 
<br>

### 单独运行某个测试样例
```shell
cd build/
./GanshinTest --gtest_filter=GFS_init.init
```

<br>

## LLDB
### 安装
直接在vscode插件市场安装即可

<br>

### 控制程序的版本debug or release
修改CMakeList.txt中的
```c
option(USE_DEBUG "Build with debug flags" ON)
```
debug版本--ON  
release版本--OFF  

<br>

### 查看程序的版本debug or release
```shell
readelf -S ./GanshinTest 
```
有debug相关信息的就是debug版本

<br>

### launch.json 
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "lldb",
            "request": "launch",
            "name": "Debug",
            "program": "${workspaceFolder}/build/GanshinTest",
            "args": [],
            "cwd": "${workspaceFolder}"
        }
    ]
}
```

<br>

## 文件权限(st_mode)
### 数字表示权限
用数字表示文件权限的八进制表示法，通常用于表示文件或目录的权限。

- 0755: 这个权限表示对于一个文件或目录而言，所有者（Owner）具有读、写、执行权限（7），而组（Group）和其他用户（Others）具有读和执行权限（5）。具体解释是：
  - 第一个数字 0 表示特殊权限位，一般不被使用。
  - 第二个数字 7 表示所有者权限，分别是 4（读取）、2（写入）和 1（执行），因此所有者有 4+2+1=7 的权限。
  - 后面的数字 5 表示组权限和其他用户权限，分别具有 4（读取）和 1（执行），因此组和其他用户都有 4+1=5 的权限。

- 0444: 这个权限表示对于一个文件或目录而言，所有用户（包括所有者、组和其他用户）都具有只读权限。具体解释是：
  - 第一个数字 0 表示特殊权限位。
  - 后面的数字 4 表示所有用户（所有者、组和其他用户）都有读取权限。

在权限设置中，每个数字表示一组权限（针对所有者、组和其他用户），每个权限的数值对应读、写、执行权限的组合。

<br>

### 文件类型
- 普通文件：S_IFREG，通常表示为 0100000。
- 目录文件：S_IFDIR，通常表示为 0040000。
- 符号链接文件：S_IFLNK，通常表示为 0120000。
- 字符设备文件：S_IFCHR，通常表示为 0020000。
- 块设备文件：S_IFBLK，通常表示为 0060000。
- 套接字文件：S_IFSOCK，通常表示为 0140000。
- FIFO（命名管道）文件：S_IFIFO，通常表示为 0010000。
可以与上面的权限进行或操作

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

<br>

### 获取文件属性GFS_getattr
要利用到Config.h定义的GFileData数据结构

<br>

#### 根据文件名查找对应的文件的DR和inode块号
采取哈希表的方式查找

<br>

### 创建文件GFS_mknod
先确定菜单的inode  
然后确定菜单中没有该文件  
计算哈希,插入  

#### createFileDirByHash

<br>

## 常见问题
Q: 为什么我设了断点没用  
A: 因为编译出来的不是debug版本, 更改cmakelist.txt  
---
Q: str的memcpy, 字符串不对?  
A: 注意len()要加一, 把'\0'也复制   
--- 
Q: 挂载后出现"由软件导致的连接断开"  
A: 试着不要在sudo环境下运行
---
Q: 出现" 输入/输出错误"  
A: inode中的st_mode不对  

<br>


## 参考资料

- [inode 详解](https://www.cnblogs.com/llife/p/11470668.html)
- [fuse结合buildroot](https://cloud.tencent.com/developer/article/1006138?areaSource=106002.3)


## 进度
该addr的时候把目录的addr也给改了, 出问题  
一个inode就占了一个块了,太过于浪费, 重构代码  
bitmap初始化问题  
写一个判断该块是否free的函数  
根目录的st_size不太对  
removeFileByPath把超级块给弄成0了
写入文件或者读取不全  
writeFileDataByInodeId按照size来遍历, 但是之前已经初始化为零了, 不会写不进去吗?  
