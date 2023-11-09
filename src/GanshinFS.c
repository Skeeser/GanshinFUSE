/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "GanshinFS.h"

// 文件相关操作, fuse会在相应的linux操作中执行这些操作
static const struct fuse_operations GFS_oper = {
    .init = GFS_init,       // 初始化
    .getattr = GFS_getattr, // 获取文件属性（包括目录的）
    .mknod = GFS_mknod,     // 创建文件
    // .unlink     = MFS_unlink,//删除文件
    // .open		= MFS_open,//无论是read还是write文件，都要用到打开文件
    // .read		= MFS_read,//读取文件内容
    // .write      = MFS_write,//修改文件内容
    // //.release    = MFS_release,//和open相对，关闭文件
    // .mkdir = MFS_mkdir, // 创建目录
    // .rmdir      = MFS_rmdir,// 删除目录
    // .access		= MFS_access,// 进入目录
    // .readdir	= MFS_readdir,  // 读取目录
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