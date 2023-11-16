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
	return 0;
}

// 此函数用来读取文件内容
int MFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	printf("MFS_read：函数开始\n\n");
	struct file_directory *attr = malloc(sizeof(struct file_directory));
	// 读取该path所指对象的file_directory
	if (get_fd_to_attr(path, attr) == -1)
	{
		free(attr);
		printf("错误：MFS_read：get_fd_to_attr失败，函数结束返回\n\n");
		return -ENOENT;
	}
	// 如果读取到的对象是目录，那么返回错误（只有文件会用到read这个函数）
	if (attr->flag == 2)
	{
		free(attr);
		printf("错误：MFS_read：对象为目录不是文件，读取失败，函数结束返回\n\n");
		return -EISDIR;
	}
	struct data_block *data_blk = malloc(sizeof(struct data_block));
	// 根据文件信息读取文件内容
	if (read_cpy_data_block(attr->nStartBlock, data_blk) == -1)
	{
		free(attr);
		free(data_blk);
		printf("错误：MFS_read：读取文件起始块内容失败，函数结束返回\n\n");
		return -1;
	}
	// 查找文件数据块,读取并读入buf中
	// 要保证 读取的位置 和 加上size后的位置 在文件范围内
	if (offset < attr->fsize)
	{
		if (offset + size > attr->fsize)
			size = attr->fsize - offset;
	}
	else
		size = 0;
	int i;
	long check_blk = data_blk->nNextBlock;		  // 由于该文件可能就只有一块，所以要加以记录，后面来判断是否需要跳过block
	int blk_num = offset / MAX_DATA_IN_BLOCK;	  // 到达offset处要跳过的块的数目
	int real_offset = offset % MAX_DATA_IN_BLOCK; // offset所在块的偏移量
	// 跳过offset之前的block块,找到offset所指的开始位置的块
	for (i = 0; i < blk_num; i++)
	{
		if (read_cpy_data_block(data_blk->nNextBlock, data_blk) == -1 || check_blk == -1)
		{
			printf("错误：MFS_read：跳到offset偏移量所在数据块时发生错误，函数结束返回\n\n");
			free(attr);
			free(data_blk);
			return -1;
		}
	}
	// 文件内容可能跨多个block块，所以要用一个变量temp记录size的值，stb_size是offset所在块中需要读取的数据量
	int temp = size, stb_size = 0;
	char *pt = data_blk->data; // 先读出offset所在块的数据
	pt += real_offset;		   // 将数据指针移动到offset所指的位置
	// 这里判断在offset这个文件块中要读取的byte数目
	if (MAX_DATA_IN_BLOCK - real_offset < size)
		stb_size = MAX_DATA_IN_BLOCK - real_offset;
	else
		stb_size = size;

	strncpy(buf, pt, stb_size);
	temp -= stb_size;
	// 把剩余未读完的内容读出来
	while (temp > 0)
	{
		if (read_cpy_data_block(data_blk->nNextBlock, data_blk) == -1)
			break;					  // 读到文件最后一个块就跳出
		if (temp > MAX_DATA_IN_BLOCK) // 如果剩余的数据量仍大于一个块的数据量
		{
			memcpy(buf + size - temp, data_blk->data, MAX_DATA_IN_BLOCK); // 从buf上一次结束的位置开始继续记录数据
			temp -= MAX_DATA_IN_BLOCK;
		}
		else // 剩余的数据量少于一个块的量，说明size大小的数据读完了
		{
			memcpy(buf + size - temp, data_blk->data, temp);
			break;
		}
	}
	printf("MFS_read：文件读取成功，函数结束返回\n\n");
	free(attr);
	free(data_blk);
	return size;
	/*
	size_t len;
	(void) fi;
	if(strcmp(path+1, options.filename) != 0)//先检查这个文件存不存在
		return -ENOENT;
	len = strlen(options.contents);//读取文件的大小
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, options.contents + offset, size);
	} else
		size = 0;
	return size;*/
}