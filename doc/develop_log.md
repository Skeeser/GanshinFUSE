# 开发日志

<br>

## 开发计划(ToDos)
- [x] 了解fuse怎么使用  
- [x] 运行hello world例子
- [x] 尝试读懂hello.c的源码
- [x] 写CMakeList.txt
- [ ] 根据hello.c的源码, 试着写一下GanshinFUSE的类似代码


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
dd bs=1K count=5K if=/dev/zero of=diskimg
``` 



<br>

## 参考资料

- [inode 详解](https://www.cnblogs.com/llife/p/11470668.html)
