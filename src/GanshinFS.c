/*
Author: keeser
Email: 1326658417@qq.com
Liense: MPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "GanshinFS.h"

// 开发者编写的用户态程序实现了FUSE提供的回调函数，用于处理文件系统的各种操作
static const struct fuse_operations GFS_oper = {
    .init = GFS_init,       // 初始化
    .getattr = GFS_getattr, // 获取文件属性
    .mknod = GFS_mknod,     // 创建文件
    .unlink = GFS_unlink,   // 删除文件
    .open = GFS_open,       // 打开文件
    .read = GFS_read,       // 读取文件内容
    .write = GFS_write,     // 写入文件内容
    .release = GFS_release, // 关闭文件
    .mkdir = GFS_mkdir,     // 创建目录
    .rmdir = GFS_rmdir,     // 删除目录
    .access = GFS_access,   // 进入目录
    .readdir = GFS_readdir, // 读取目录
    .utimens = GFS_utimes,  // 修改时间
};

int main(int argc, char *argv[])
{
    // 打印LOGO
    printf("%s", LOGO);

    // 给出最大权限
    umask(0);

    // 返回时调用fuse的main函数
    return fuse_main(argc, argv, &GFS_oper, NULL);
}