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
	struct GDataBlock *data_blk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));

	// 根据路径读取file_dir
	if (getFileDirByPath(path, file_dir) == -1)
	{
		ret = -ENOENT;
		printError("GFS_read: read file failed!");
		goto error;
	}

	// 如果读取到的对象是目录，那么返回错误（只有文件会用到read这个函数）
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
	free(data_blk);
	return ret;
}

// 此函数用来关闭文件
int GFS_release(const char *path, struct fuse_file_info *fi)
{
	(void)fi;
	(void)path;
	return 0;
}

// 此函数用来写入文件
int GFS_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	(void)fi;
	int ret = size;

	struct file_directory *attr = malloc(sizeof(struct file_directory));
	// 打开path所指的对象，将其file_directory读到attr中
	get_fd_to_attr(path, attr);

	// 然后检查要写入数据的位置是否越界
	if (offset > attr->fsize)
	{
		free(attr);
		printf("MFS_write：offset越界，函数结束返回\n\n");
		return -EFBIG;
	}
	long start_blk = attr->nStartBlock;
	if (start_blk == -1)
	{
		printf("MFS_write：该文件为空（无起始块），函数结束返回\n\n");
		free(attr);
		return -errno;
	}
	int res, num, p_offset = offset; // p_offset用来记录修改前最后一个文件块的位置
	struct data_block *data_blk = malloc(sizeof(struct data_block));
	// 找到offset指定的块位置和块内的偏移量位置，注意，offset有可能很大，大于一个块的数据容量
	// 而通过find_off_blk可以找到相应的文件的块位置start_blk和块内偏移位置（用offset记录）
	if ((res = find_off_blk(&start_blk, &offset, data_blk)))
		return res;

	// 创建一个指针管理数据
	char *pt = data_blk->data;
	// 找到offset所在块中offset位置
	pt += offset;
	int towrite = 0;
	int writen = 0;
	// 磁盘块剩余空间小于文件大小，则写满该磁盘块剩余的数据空间，否则，说明磁盘足够大，可以写入整个文件
	if (MAX_DATA_IN_BLOCK - offset < size)
		towrite = MAX_DATA_IN_BLOCK - offset;
	else
		towrite = size;
	strncpy(pt, buf, towrite); // 写入长度为towrite的内容
	buf += towrite;			   // 移到字符串待写处
	data_blk->size += towrite; // 该数据块的size增加已写数据量towrite
	writen += towrite;		   // buf中已写的数据量
	size -= towrite;		   // buf中待写的数据量
	// 如果size>0，说明数据还没被写完，要构造空闲块作为待写入文件的新块
	long *next_blk = malloc(sizeof(long));
	if (size > 0)
	{
		// 还有数据未写入，因此要找到num个连续的空闲块写入
		num = get_empty_blk(size / MAX_DATA_IN_BLOCK + 1, next_blk); // 注意返回的是一片连续的存储空间
		// num可能小于size/MAX_DATA_IN_BLOCK+1，先写入一部分
		if (num == -1)
		{
			free(attr);
			free(data_blk);
			free(next_blk);
			printf("MFS_write：文件没有写完，申请空闲块失败，函数结束返回\n\n");
			return -errno;
		}
		data_blk->nNextBlock = *next_blk;
		// start_blk记录的是原文件的最后一个文件块，现在更新为扩大了的块
		write_data_block(start_blk, data_blk);

		printf("MFS_write：开始把没写完的数据写到申请到的空闲块中\n\n");
		// 下面开始不断循环，把没写完的数据全部写到申请到的空闲块里面
		while (1)
		{
			for (int i = 0; i < num; i++) // 重复写文件的操作（因为没写完）
			{
				// 在新块写入数据，如果需要写入的剩余数据size已经不足一块的容量，那么towrite为size
				if (MAX_DATA_IN_BLOCK < size)
					towrite = MAX_DATA_IN_BLOCK;
				else
					towrite = size;
				data_blk->size = towrite;
				strncpy(data_blk->data, buf, towrite);
				buf += towrite;	   // buf指针移动
				size -= towrite;   // 待写入数据量减少
				writen += towrite; // 已写入数据量增加
				// 注意，每次写完都要检查是不是已经把整个字符串写完了，因为该文件的最后一个块的nNextBlock为-1
				if (size == 0)
					data_blk->nNextBlock = -1;
				else
					data_blk->nNextBlock = *next_blk + 1; // 未写完增加后续块
				// 更新块为扩容后的块
				write_data_block(*next_blk, data_blk);
				*next_blk = *next_blk + 1;
			}
			if (size == 0)
				break; // 已写完
			// num小于size/504+1,找到的num不够，继续找
			num = get_empty_blk(size / MAX_DATA_IN_BLOCK + 1, next_blk);
			if (num == -1)
			{
				free(attr);
				free(data_blk);
				free(next_blk);
				return -errno;
			}
		}
	}
	else if (size == 0) // 块空间小于剩余空间
	{
		// 缓存nextblock
		long next_blok = data_blk->nNextBlock;
		// 终点块
		data_blk->nNextBlock = -1;
		write_data_block(start_blk, data_blk);
		// 清理nextblock（之前的nextblock不再需要）
		ClearBlocks(next_blok, data_blk);
	}
	size = writen;
	// 修改被写文件file_directory的文件大小信息为:写入起始位置+写入内容大小
	attr->fsize = p_offset + size;
	if (setattr(path, attr, 1) == -1)
		size = -errno;

error:
	free(attr);
	free(data_blk);
	free(next_blk);
	return ret;
}