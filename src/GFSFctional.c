/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/

#include "GFSFctional.h"

// 文件系统初始化函数
void *GFS_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
	printSuccess("GanshinFS init start!");
	(void)conn;
	(void)cfg;
	// 在此处存放初始化的操作

	printSuccess("GanshinFS init finished!");
	return NULL;
}

// 这个函数用于读取文件属性, 通过对象的路径获取文件的属性, 并赋值给stbuf
int GFS_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
	(void)fi;
	int res = 0;
	struct GInode *file_inode = (struct GInode *)malloc(sizeof(struct GInode));
	short int *inode_id;
	*inode_id = 0;

	// 根据路径获取对应文件的inode
	if (getInodeBlkByPath(path, inode_id) != 0)
	{
		free(file_inode);
		printError("GFS_getattr: error!");
		return -1;
	}
	getInodeByBlkId(*inode_id, file_inode);
	// 根据inode赋值给stbuf
	fillStatByInode(file_inode, stbuf);

	free(file_inode);
	return res;
}

// 此函数用来创建文件
int GFS_mknod(const char *path, mode_t mode, dev_t dev)
{
	mode = (mode_t)0;
	dev = (dev_t)0;
	createFileByPath(path, GFILE);
}