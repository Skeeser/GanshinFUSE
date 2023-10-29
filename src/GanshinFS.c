/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "GanshinFS.h"

// 文件系统初始化函数
static void* GFS_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
	printSuccess("GanshinFS init start!");
	(void) conn;
    (void) cfg;
	// 在此处存放初始化的操作

	printSuccess("GanshinFS init finished!");
    return NULL;
}

// 这个函数用于读取文件属性, 通过对象的路径获取文件的属性, 并赋值给stbuf
static int GFS_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    (void) fi;
	int res = 0;

	struct GFileDir *const attr = malloc(sizeof(struct GFileDir));

	if (getFileDirToAttr(path, attr) == -1) 
	{
		free(attr);	
        printError("GFS_getattr: get FileDir To Attr failed!");
        return -ENOENT;
	}
	
    // memset(stbuf, 0, sizeof(struct stat));//将stat结构中成员的值全部置0
	
    // if (attr->flag==2)
	// {//从path判断这个文件是		一个目录	还是	一般文件
	// 	printf("MFS_getattr：这个file_directory是一个目录\n\n");
	// 	stbuf->st_mode = S_IFDIR | 0666;//设置成目录,S_IFDIR和0666（8进制的文件权限掩码），这里进行或运算
	// 	//stbuf->st_nlink = 2;//st_nlink是连到该文件的硬连接数目,即一个文件的一个或多个文件名。说白点，所谓链接无非是把文件名和计算机文件系统使用的节点号链接起来。因此我们可以用多个文件名与同一个文件进行链接，这些文件名可以在同一目录或不同目录。
	// } 
	// else if (attr->flag==1) 
	// {
	// 	printf("MFS_getattr：这个file_directory是一个文件\n\n");
	// 	stbuf->st_mode = S_IFREG | 0666;//该文件是	一般文件
	// 	stbuf->st_size = attr->fsize;
	// 	//stbuf->st_nlink = 1;
	// } 
	// else {printf("MFS_getattr：这个文件（目录）不存在，函数结束返回\n\n");;res = -ENOENT;}//文件不存在
	
	// printf("MFS_getattr：getattr成功，函数结束返回\n\n");
	free(attr);
	return res;
}

// 文件相关操作, fuse会在相应的linux操作中执行这些操作
static const struct fuse_operations GFS_oper = {
	.init       = GFS_init,//初始化
	.getattr	= GFS_getattr,//获取文件属性（包括目录的）
	// .mknod      = MFS_mknod,//创建文件
    // .unlink     = MFS_unlink,//删除文件
	// .open		= MFS_open,//无论是read还是write文件，都要用到打开文件
	// .read		= MFS_read,//读取文件内容
    // .write      = MFS_write,//修改文件内容
    // //.release    = MFS_release,//和open相对，关闭文件
    // .mkdir      = MFS_mkdir,//创建目录
    // .rmdir      = MFS_rmdir,//删除目录
	// .access		= MFS_access,//进入目录
	// .readdir	= MFS_readdir,//读取目录
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