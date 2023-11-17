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
	*inode_id = -1;

	// 根据路径获取对应文件的inode
	if (getInodeBlkByPath(path, inode_id) != 0)
	{
		free(file_inode);
		printError("GFS_getattr: error!");
		return -1;
	}
	getInodeByInodeId(*inode_id, file_inode);
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
	return createFileByPath(path, GFILE);
}

// 此函数用来删除文件
int GFS_unlink(const char *path)
{
	return removeFileByPath(path, GFILE);
}

// 此函数用来打开文件
int GFS_open(const char *path, struct fuse_file_info *fi)
{
	(void)fi;
	(void)path;
	return 0;
}

// 此函数用来读取文件内容
int GFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	(void)fi;
	int ret = 0;
	struct GFileDir *file_dir = (struct GFileDir *)malloc(sizeof(struct GFileDir));
	// struct GDataBlock *data_blk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));

	// 根据路径读取file_dir
	if (getFileDirByPath(path, file_dir) == -1)
	{
		ret = -ENOENT;
		printError("GFS_read: read file failed!");
		goto error;
	}

	// 如果读取到的对象是目录，返回错误
	if (file_dir->flag == (int)GDIRECTORY)
	{
		ret = -EISDIR;
		printError("GFS_read: read dir not file!");
		goto error;
	}

	const long inode_id = file_dir->nInodeBlock;
	// 根据文件信息读取文件内容
	if (getFileDataByInodeId(inode_id, size, offset, buf) == -1)
	{
		ret = -1;
		printf("GFS_read: read file failed!");
		goto error;
	}

	printSuccess("GFS_read: file read success!");

error:
	free(file_dir);
	// free(data_blk);
	return ret;
}

// 此函数用来关闭文件
int GFS_release(const char *path, struct fuse_file_info *fi)
{
	(void)fi;
	(void)path;
	return 0;
}

// 此函数用来写入文件, 注意offset是数据块内的偏移, 而不是buf的偏移
int GFS_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	(void)fi;
	int ret = size;

	struct GFileDir *file_dir = (struct GFileDir *)malloc(sizeof(struct GFileDir));

	// 根据路径读取file_dir
	if (getFileDirByPath(path, file_dir) == -1)
	{
		ret = -ENOENT;
		printError("GFS_write: get file dir failed!");
		goto error;
	}

	// 如果读取到的对象是目录，返回错误
	if (file_dir->flag == (int)GDIRECTORY)
	{
		ret = -EISDIR;
		printError("GFS_write: read dir not file!");
		goto error;
	}

	const long file_inode_id = file_dir->nInodeBlock;

	// 根据inode_id来写入文件
	// 根据文件信息读取文件内容
	if ((ret = writeFileDataByInodeId(file_inode_id, size, offset, buf)) != 0)
	{
		printf("GFS_write: write file failed!");
		goto error;
	}

	printSuccess("GFS_write: write file success!");
error:
	free(file_dir);
	return ret;
}

// 此函数用来创建目录
int GFS_mkdir(const char *path, mode_t mode)
{
	(void)mode;
	return createFileByPath(path, GDIRECTORY);
}

// 此函数用来删除目录
int GFS_rmdir(const char *path)
{
	return removeFileByPath(path, GDIRECTORY);
}