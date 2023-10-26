#include "../include/GanshinFS.h"





//所有文件的操作都要放到这里，fuse会帮我们在相应的linux操作中执行这些我们写好的函数
// static struct fuse_operations MFS_oper = {
// 	.init       = MFS_init,//初始化
// 	.getattr	= MFS_getattr,//获取文件属性（包括目录的）
// 	.mknod      = MFS_mknod,//创建文件
//     .unlink     = MFS_unlink,//删除文件
// 	.open		= MFS_open,//无论是read还是write文件，都要用到打开文件
// 	.read		= MFS_read,//读取文件内容
//     .write      = MFS_write,//修改文件内容
//     //.release    = MFS_release,//和open相对，关闭文件
//     .mkdir      = MFS_mkdir,//创建目录
//     .rmdir      = MFS_rmdir,//删除目录
// 	.access		= MFS_access,//进入目录
// 	.readdir	= MFS_readdir,//读取目录
// };


int main(int argc, char *argv[])
{
    return 0;
}