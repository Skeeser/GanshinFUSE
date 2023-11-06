/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "FileOper.h"

// 根据块号, 读取文件数据GDataBlock
int getDataByBlkId(short int blk_id, struct GDataBlock *data_blk)
{

	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+"); // 打开文件
	if (fp == NULL)
	{
		printError("getDataByBlkId: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		return -1;
	}
	// printSuccess("Open disk file success!");

	// 文件打开后，就用blk_id * FS_BLOCK_SIZE作为偏移量
	fseek(fp, blk_id * FS_BLOCK_SIZE, SEEK_SET);
	// 清空
	memset(data_blk, 0, sizeof(struct GDataBlock));
	if (fread(data_blk, sizeof(struct GDataBlock), 1, fp) > 0)
	{
		printSuccess("getDataByBlkId: Read data block success!");
	}
	else
	{
		printError("getDataByBlkId: Read data block failed!");
		fclose(fp);
		return -1;
	}

	// 判断是否正确读取
	if (ferror(fp))
	{
		printError("getDataByBlkId: Read disk file failed!");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}
// 根据块号, 读取Inode
int getInodeByBlkId(short int blk_id, struct GInode *inode_blk)
{
	struct GDataBlock *gblk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));
	// struct GInode *temp_inode = malloc(sizeof(struct GInode));
	int ret = getDataByBlkId(blk_id, gblk);
	if (ret != 0)
	{
		free(gblk);
		return ret;
	}
	// 复制内存
	// error
	// temp_inode = (struct GInode *)gblk;

	memcpy(inode_blk, (struct GInode *)gblk, sizeof(struct GInode));
	// free(temp_inode);
	free(gblk);
	return 0;
}

// 给定根据hash_num和cur_i, 返回对应的inode块号
int getInodeBlkByHash(const int hash_num, const int cur_i, int *target_i)
{
	struct GFileDir *p_fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
	int ret = getFileDirByHash(hash_num, cur_i, p_fd);
	if (ret != 0)
	{
		free(p_fd);
		printError("getInodeBlkByHash : failed!");
		return ret;
	}
	*target_i = p_fd->nInodeBlock;
	free(p_fd);
	return 0;
}

// 根据hash_num和cur_i, 返回对应的FileDir  todo: 解决哈希冲突
int getFileDirByHash(const int hash_num, const int cur_i, struct GFileDir *p_filedir)
{
	struct GInode *temp_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// 获取当前inode号的inode
	getInodeByBlkId(cur_i, temp_inode);
	struct GDataBlock *data_blk = malloc(sizeof(struct GDataBlock));

	if (0 <= hash_num < FD_ZEROTH_INDIR)
	{
		int i = hash_num / FD_PER_BLK;
		short int addr = temp_inode->addr[i];
		if (addr < 0)
			goto error;
		getDataByBlkId(addr, data_blk);
		int offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);

		// todo: 优化成循环函数
	}
	else if (hash_num < FD_FIRST_INDIR)
	{
		// 一次间接块  4
		short int addr = temp_inode->addr[4];
		if (addr < 0)
			goto error;
		int offset = (hash_num - FD_ZEROTH_INDIR) * sizeof(short int) / FD_PER_BLK;
		getDataByBlkId(addr, data_blk);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);
		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
	else if (hash_num < FD_SECOND_INDIR)
	{
		// 二次间接块  5
		short int addr = temp_inode->addr[5];
		if (addr < 0)
			goto error;
		getDataByBlkId(addr, data_blk);
		int offset = (hash_num - FD_FIRST_INDIR) * sizeof(short int) / (FD_PER_BLK * FD_PER_BLK);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
	else if (hash_num < MAX_HASH_SIZE)
	{
		// 三次间接块  6
		short int addr = temp_inode->addr[6];
		if (addr < 0)
			goto error;
		getDataByBlkId(addr, data_blk);
		int offset = (hash_num - FD_SECOND_INDIR) * sizeof(short int) / (FD_PER_BLK * FD_PER_BLK * FD_PER_BLK);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
	else
	{
	error:
		printf("getFileDirByHash: the file may be not exited.\n");
		free(data_blk);
		free(temp_inode);
		return -1;
	}
	free(data_blk);
	free(temp_inode);
	return 0;
}

// 将filedir写入到GDataBlock的data中
int writeFileDirToDataBlk(const struct GFileDir *p_fd, const int offset, struct GDataBlock *data_blk)
{
	// 增加大小
	data_blk->size += sizeof(struct GFileDir);
	// 写入
	memcpy(&(data_blk->data[offset]), p_fd, sizeof(struct GFileDir));
}

// 根据哈希值在menu中创建file dir
int createFileDirByHash(const int hash_num, const int cur_i, struct GFileDir *p_filedir)
{
	int ret = 0;

	struct GInode *temp_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// 获取当前inode号的inode
	getInodeByBlkId(cur_i, temp_inode);
	struct GDataBlock *data_blk = malloc(sizeof(struct GDataBlock));

	// 地址未被创建

	// 地址已被创建

	if (0 <= hash_num < FD_ZEROTH_INDIR)
	{
		int i = hash_num / FD_PER_BLK;
		short int addr = temp_inode->addr[i];
		if (addr < 0)
			goto error;
		getDataByBlkId(addr, data_blk);
		int offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);

		getFileDirFromDataBlk(data_blk, offset, p_filedir);

		// todo: 优化成循环函数
	}
	else if (hash_num < FD_FIRST_INDIR)
	{
		// 一次间接块  4
		short int addr = temp_inode->addr[4];
		if (addr < 0)
			goto error;
		int offset = (hash_num - FD_ZEROTH_INDIR) * sizeof(short int) / FD_PER_BLK;
		getDataByBlkId(addr, data_blk);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);
		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
	else if (hash_num < FD_SECOND_INDIR)
	{
		// 二次间接块  5
		short int addr = temp_inode->addr[5];
		if (addr < 0)
			goto error;
		getDataByBlkId(addr, data_blk);
		int offset = (hash_num - FD_FIRST_INDIR) * sizeof(short int) / (FD_PER_BLK * FD_PER_BLK);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
	else if (hash_num < MAX_HASH_SIZE)
	{
		// 三次间接块  6
		short int addr = temp_inode->addr[6];
		if (addr < 0)
			goto error;
		getDataByBlkId(addr, data_blk);
		int offset = (hash_num - FD_SECOND_INDIR) * sizeof(short int) / (FD_PER_BLK * FD_PER_BLK * FD_PER_BLK);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
	else
	{
	error:
		printf("getFileDirByHash: the file may be not exited.\n");
		free(data_blk);
		free(temp_inode);
		return -1;
	}
	free(data_blk);
	free(temp_inode);
	return ret;
}

// Inode相关函数
// 根据路径, 获取inode,
int getInodeBlkByPath(const char *path, short int *file_inode)
{
	struct GFileDir *p_fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
	int ret = getFileDirByPath(path, p_fd);
	if (ret != 0)
	{
		free(p_fd);
		printError("getInodeBlkByPath : failed!");
		return ret;
	}
	*file_inode = p_fd->nInodeBlock;
	free(p_fd);
	return 0;
}

// 检查路径
int checkFilePath(const char *path)
{
	char *base_name;
	base_name = strrchr(path, '/');
	if (base_name == NULL)
	{
		printError("checkFilePath: file base name is Null!");
		printf("Path: %s\n", path);
		return -1;
	}
	base_name++;
	if (strlen(base_name) > MAX_PATH_LENGTH)
	{
		printError("checkFilePath: basename exceeds 255 characters");
		printf("Path: %s\n", path);
		return -1;
	}
	return 0;
}

// 根据路径，到相应的目录寻找文件的GFileDir，并赋值给attr
int getFileDirByPath(const char *path, struct GFileDir *attr)
{
	// 获取磁盘根目录块的位置
	printSuccess("Get File Dir To Attr Start!");
	printf("getFileDirToAttr: get file dir by: %s\n", path);

	struct GDataBlock *data_blk;
	data_blk = malloc(sizeof(struct GDataBlock));

	// 读出超级块
	if (getDataByBlkId(0, data_blk) == -1)
	{
		printError("getFileDirToAttr: read super block failed!");
		free(data_blk);
		return -1;
	}
	else
		printSuccess("getFileDirToAttr: read super block success!");

	struct GSuperBlock *sp_blk;
	// 将原本GDataBlock的数据转换成GSuperBlock
	sp_blk = (struct GSuperBlock *)data_blk;
	long start_inode;
	start_inode = sp_blk->first_inode;

	printf("Super Block:\nfs_size=%ld, first_blk=%ld, datasize=%ld, first_inode=%ld, inode_area_size=%ld, fisrt_blk_of_inodebitmap=%ld, inodebitmap_size=%ld, first_blk_of_databitmap=%ld, databitmap_size=%ld\n",
		   sp_blk->fs_size, sp_blk->first_blk, sp_blk->datasize, sp_blk->first_inode, sp_blk->inode_area_size, sp_blk->fisrt_blk_of_inodebitmap,
		   sp_blk->inodebitmap_size, sp_blk->first_blk_of_databitmap, sp_blk->databitmap_size);
	printf("start_inode:%ld\n", start_inode);

	int ret = 0;
	// 解析path
	char *tmp_path;
	char *free_tmp_path;
	tmp_path = strdup(path);
	// 复制指针, 方便释放
	free_tmp_path = tmp_path;

	// 检查路径
	ret = checkFilePath(tmp_path);
	// 如果路径为空，则出错返回-1
	if (ret != 0)
	{
		printError("getFileDirToAttr:check file path failed!");
		free(data_blk);
		return ret;
	}

	// 如果路径为根目录路径
	if (strcmp(tmp_path, "/") == 0)
	{
		ret = 0;
		attr->flag = 2; // 2 menu
		attr->nMenuInode = start_inode;
		attr->nInodeBlock = start_inode;
		free(data_blk);
		printSuccess("getFileDirToAttr: this is a root menu");
		return ret;
	}

	// 如果给的是目录, 排除例如 /etc/a/的情况
	if (tmp_path[strlen(tmp_path) - 1] == '/')
	{
		// 删除最后的'/'
		tmp_path[strlen(tmp_path) - 1] = '\0';
	}

	char *base_name;
	int name_len = strlen(tmp_path);
	// 文件或目录的长度
	int len = 0;
	// 当前的目录或文件的inode
	long cur_i = start_inode;
	// 类型标志 -1表示都有可能, 2表示路径
	// int flag = -1;

	tmp_path++;
	name_len--;
	// 取路径的第一个字符, 跳过了根目录 /
	char c = *tmp_path;
	int if_final = 0;
	while (1)
	{
		if (name_len < 0)
			break;
		base_name = tmp_path;
		// 循环得到每一级的目录
		for (len = 0; --name_len >= 0; len++)
		{
			c = *(tmp_path++);
			if (c == '/')
				break;
		}
		char *menu_flag = strchr(base_name, '/');
		size_t length = 0;
		if (menu_flag != NULL)
			*menu_flag = '\0';
		else
			if_final = 1;

		// 下面根据base_name和cur_i 查找 RD 并更新 cur_i
		// 采用哈希表的方式查找
		int hash_num = hash(base_name);
		// 根据hash_num和cur_i, 返回对应的GFileDir
		struct GFileDir *p_fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
		if (getFileDirByHash(hash_num, cur_i, p_fd) != 0)
		{
			printError("getFileDirByPath: getFileDirByHash failed!");
			goto error;
		}

		cur_i = p_fd->nInodeBlock;

		if (if_final)
		{
			// 分割扩展名
			char *ret = strchr(base_name, '.');
			// 如果没有 "."
			if (ret == NULL)
			{
				if (strcmp(p_fd->fname, base_name) != 0 || p_fd->flag != 0)
					goto error;
				else
					memcpy(attr, p_fd, sizeof(struct GFileDir));
			}
			else
			{
				// 如果有 . 分割
				*ret = '\0';
				char *fname = base_name;
				char *fext = ++ret;
				if (strcmp(p_fd->fname, fname) != 0 || strcmp(p_fd->fext, fext) != 0 || p_fd->flag != 0)
				{
				error:
					free(p_fd);
					free(data_blk);
					free(free_tmp_path);
					return -1;
				}
				else
				{
					cur_i = p_fd->nInodeBlock;
					memcpy(attr, p_fd, sizeof(struct GFileDir));
				}
			}
		}

		free(p_fd);
	}

	free(free_tmp_path);
	free(data_blk);
	printSuccess("getFileDirToAttr: success!");
	return 0;
}

// 将char的data 转化为short int形式, 注意采用小端序编码
short int retShortIntFromData(const char *data, const int offset)
{
	// 使用位运算将两个字节的数据合并成一个 short int
	short int result = ((uint16_t)data[offset + 1] << 8) | (uint16_t)data[offset];
	return result;
}

// 给定GDataBlock中char* 将data 解读成GFileDir
void getFileDirFromDataBlk(const struct GDataBlock *data_blk, const int offset, struct GFileDir *p_fd)
{
	memcpy(p_fd, &(data_blk->data[offset]), sizeof(struct GFileDir));
}

// 根据inode, 获取该文件占据了多少块
void getFileBlkNum(struct GInode *inode, int *blk_num)
{
	if (inode == NULL)
	{
		printError("getFileBlkNum: inode is NULL!");
		return;
	}
	int ret_num = 0;
	// 直接地址
	for (int i = 0; i < 4; i++)
	{
		if (inode->addr[i] >= 0)
		{
			ret_num++;
		}
	}

	// int ADDR_PER_BLK = MAX_DATA_IN_BLOCK / sizeof(short int);
	struct GDataBlock *first_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *second_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *third_data_blk = malloc(sizeof(struct GDataBlock));

	// 一次间接
	short int first_Indir = inode->addr[4];
	if (first_Indir >= 0)
	{
		getDataByBlkId(first_Indir, first_data_blk);
		ret_num += first_data_blk->size / sizeof(short int);
	}

	// 二次间接
	short int second_Indir = inode->addr[5];
	if (second_Indir >= 0)
	{
		getDataByBlkId(second_Indir, second_data_blk);
		for (int i = 0; i < second_data_blk->size; i += sizeof(short int))
		{
			getDataByBlkId(retShortIntFromData(second_data_blk->data, i), first_data_blk);
			ret_num += first_data_blk->size / sizeof(short int);
		}
	}

	// 三次间接
	short int third_Indir = inode->addr[6];
	if (third_Indir >= 0)
	{
		getDataByBlkId(third_Indir, third_data_blk);
		for (int i = 0; i < third_data_blk->size; i += sizeof(short int))
		{
			getDataByBlkId(retShortIntFromData(third_data_blk->data, i), second_data_blk);
			for (int i = 0; i < second_data_blk->size; i += sizeof(short int))
			{
				getDataByBlkId(retShortIntFromData(second_data_blk->data, i), first_data_blk);
				ret_num += first_data_blk->size / sizeof(short int);
			}
		}
	}

	*blk_num = ret_num;

	free(first_data_blk);
	free(second_data_blk);
	free(third_data_blk);
}

// 将GInode赋值给stat
/*
struct stat {
		mode_t     st_mode;       //文件对应的模式，文件，目录等
		ino_t      st_ino;       //inode节点号
		dev_t      st_dev;        //设备号码
		dev_t      st_rdev;       //特殊设备号码
		nlink_t    st_nlink;      //文件的连接数
		uid_t      st_uid;        //文件所有者
		gid_t      st_gid;        //文件所有者对应的组
		off_t      st_size;       //普通文件，对应的文件字节数
		time_t     st_atime;      //文件最后被访问的时间
		time_t     st_mtime;      //文件内容最后被修改的时间
		time_t     st_ctime;      //文件状态改变时间
		blksize_t st_blksize;    //文件内容对应的块大小
		blkcnt_t   st_blocks;     //文件内容对应的块数量
	};
*/
void fillStatByInode(struct GInode *inode, struct stat *st)
{
	int *st_blocks;
	getFileBlkNum(inode, st_blocks);
	// stbuf初始化为0
	memset(st, 0, sizeof(struct stat));
	st->st_mode = inode->st_mode;
	st->st_ino = inode->st_ino;
	st->st_nlink = inode->st_nlink;
	st->st_uid = inode->st_uid;
	st->st_gid = inode->st_gid;
	st->st_size = inode->st_size;
	st->st_atime = inode->st_atim;
	st->st_blksize = FS_BLOCK_SIZE;
	st->st_blocks = *st_blocks;
}

/*
// 拆分路径
int dividePath(char *name, char *ext, const char *path, enum GTYPE file_type)
{
	char *tmp_path, *m, *n;
	tmp_path = strdup(path); // 用来记录最原始的路径
	struct file_directory *attr = malloc(sizeof(struct file_directory));
	m = tmp_path;
	if (!m)
		return -errno; // 路径为空
	m++;			   // 跳过第一个'/'

	n = strchr(m, '/'); // 看是否有二级路径
	// 如果找到二级路径的'/'，并且要创建的是目录，那么不允许，返回-1
	if (n != NULL && flag == 2)
	{
		printf("错误：divide_path:二级路径下不能再创建目录，函数结束返回-EPERM\n\n");
		return -EPERM;
	}
	else if (n != NULL)
	{
		printf("divide_path:要创建的是二级路径下的文件\n\n");
		*n = '\0';
		n++; // 此时n指向要创建的文件名的第一个字母
		m = n;
		if (get_fd_to_attr(tmp_path, attr) == -1)
		{ // 读取该path的父目录，确认这个父目录是存在的
			printf("错误：divide_path：找不到二级路径的父目录，函数结束返回-ENOENT\n\n");
			free(attr);
			return -ENOENT;
		}
	}
	printf("divide_path检查：tmp_path=%s\nm=%s\nn=%s\n\n", tmp_path, m, n);

	// 如果找不到二级路径'/'，说明要创建的对象是： /目录 或 /文件
	// 那么这个路径的父目录为根目录，直接读出来
	if (n == NULL)
	{
		printf("divide_path:要创建的是根目录下的对象\n\n");
		if (get_fd_to_attr("/", attr) == -1)
		{
			printf("错误：divide_path：找不到根目录，函数结束返回-ENOENT\n\n");
			free(attr);
			return -ENOENT;
		}
	}
	// 记录完要创建的对象的名字，如果该对象是文件，还要记录后缀名（有的文件没有后缀名）
	if (flag == 1)
	{
		printf("divide_path:这是文件，有后缀名\n\n");
		n = strchr(m, '.');
		if (n != NULL)
		{
			*n = '\0'; // 截断tmp_path
			n++;	   // 此时n指针指向后缀名的第一位
		}
	}
	// 要创建对象，还要检查：文件名（目录名），后缀名的长度是否超长
	if (flag == 1) // 如果创建的是文件
	{
		if (strlen(m) > MAX_FILENAME + 1)
		{
			free(attr);
			return -ENAMETOOLONG;
		}
		else if (strlen(m) > MAX_FILENAME)
		{
			if (*(m + MAX_FILENAME) != '~')
			{
				free(attr);
				return -ENAMETOOLONG;
			}
		}
		else if (n != NULL) // 如果有后缀名
		{
			if (strlen(n) > MAX_EXTENSION + 1)
			{
				free(attr);
				return -ENAMETOOLONG;
			}
			else if (strlen(n) > MAX_EXTENSION)
			{
				if (*(n + MAX_EXTENSION) != '~')
				{
					free(attr);
					return -ENAMETOOLONG;
				}
			}
		}
	}
	else if (flag == 2) // 如果创建的是目录
	{
		if (strlen(m) > MAX_DIR_IN_BLOCK)
		{
			free(attr);
			return -ENAMETOOLONG;
		}
	}
	*name = '\0';
	*ext = '\0';
	if (m != NULL)
		strcpy(name, m);
	if (n != NULL)
		strcpy(ext, n);

	printf("已经获取到父目录的file_directory（attr），检查一下：\n\n");
	printf("attr:fname=%s，fext=%s，fsize=%ld，nstartblock=%ld，flag=%d\n\n", attr->fname,
		   attr->fext, attr->fsize, attr->nStartBlock, attr->flag);
	// 把开始块信息赋值给par_dir_stblk
	*par_dir_stblk = attr->nStartBlock;
	// 这里要获取父目录文件的大小
	*par_size = attr->fsize;
	printf("divide_path：检查过要创建对象的文件（目录）名，并没有问题\n\n");
	printf("divide_path：分割后的父目录名：%s\n文件名：%s\n后缀名：%s\npar_dir_stblk=%ld\n\n", tmp_path, name, ext, *par_dir_stblk);
	printf("divide_path：函数结束返回\n\n");
	free(attr);
	free(tmp_path);
	if (*par_dir_stblk == -1)
		return -ENOENT;

	return 0;
}

*/

// 检查文件名
int checkFileFname(const char *fname)
{
	int ret = 0;
	if (strlen(fname) > MAX_FILENAME)
	{
		ret = -1;
		printError("checkFileFname: fname is too long!");
	}

	return ret;
}

// 检查拓展名
int checkFileFext(const char *fext)
{
	int ret = 0;
	if (strlen(fext) > MAX_EXTENSION)
	{
		ret = -1;
		printError("checkFileFext: fext is too long!");
	}

	return ret;
}

// 根据路径和文件类型, 划分文件名, 拓展名和剩下的路径
int divideFileNameByPath(const char *path, char *fname, char *fext, char *fall_name, char *remain_path, enum GTYPE file_type)
{

	int ret = 0;
	// 解析path
	char *tmp_path;
	char *free_tmp_path;
	tmp_path = strdup(path);
	free_tmp_path = tmp_path;
	// 清空字符串
	memset(fname, '\0', strlen(fname));
	memset(fext, '\0', strlen(fext));
	memset(fall_name, '\0', strlen(fall_name));
	memset(remain_path, '\0', strlen(remain_path));

	// 检查路径
	ret = checkFilePath(tmp_path);
	// 如果路径为空，则出错返回-1
	if (ret != 0)
	{
		printError("divideFileNameByPath:check file path failed!");
		goto error;
	}

	// 如果路径为根目录路径
	if (strcmp(tmp_path, "/") == 0)
	{
		ret = -1;
		printError("divideFileNameByPath: this is a root menu!");
		goto error;
	}

	// 如果给的是目录, 排除例如 /etc/a/的情况
	if (file_type == GDIRECTORY && tmp_path[strlen(tmp_path) - 1] == '/')
	{
		// 删除最后的'/'
		tmp_path[strlen(tmp_path) - 1] = '\0';
	}

	char *base_name;
	int name_len = strlen(tmp_path);
	// 文件或目录的长度
	int len = 0;

	tmp_path++;
	name_len--;
	// 取路径的第一个字符, 跳过了根目录 /
	char c = *tmp_path;
	int if_final = 0;

	while (1)
	{
		if (name_len < 0)
			break;
		base_name = tmp_path;
		// 循环得到每一级的目录
		for (len = 0; --name_len >= 0; len++)
		{
			c = *(tmp_path++);
			if (c == '/')
				break;
		}
		char *menu_flag = strchr(base_name, '/');
		size_t length = 0;
		if (menu_flag != NULL)
			*menu_flag = '\0';
		else
			if_final = 1;

		length = strlen(base_name);
		// printf("basename: %s long : %ld\n", base_name, length);

		if (if_final == 1)
		{
			strncpy(fall_name, base_name, strlen(base_name));
			// 取出剩余路径
			strncpy(remain_path, path, base_name - free_tmp_path - 1);
			// 如果剩余路径为空, 说明为根目录
			if (strcmp(remain_path, "") == 0)
			{
				strncpy(remain_path, path, base_name - free_tmp_path);
			}

			// 分割扩展名
			char *dot_ret = strchr(base_name, '.');
			// 如果没有 "."
			if (dot_ret == NULL && file_type == GDIRECTORY)
			{
				if (checkFileFname(base_name) == 0)
				{
					memcpy(fname, base_name, length + 1);
				}
				else
				{
					ret = -1;
					goto error;
				}
			}
			else if (dot_ret != NULL && file_type == GFILE)
			{
				// 如果有 .
				// 分割
				*dot_ret = '\0';
				char *tmp_name = base_name;
				char *tmp_ext = ++dot_ret;
				if (checkFileFname(tmp_name) == 0 && checkFileFext(tmp_ext) == 0)
				{
					memcpy(fname, tmp_name, strlen(tmp_name));
					memcpy(fext, tmp_ext, strlen(tmp_ext));
				}
				else
				{
					ret = -1;
					goto error;
				}
			}
			else
			{
				ret = -1;
				printError("divideFileNameByPath: final char is wrong!");
				goto error;
			}
		}
	}

	printSuccess("divideFileNameByPath: success!");

error:
	free(free_tmp_path);
	return ret;
}

// 根据路径新建文件
int createFileByPath(const char *path, enum GTYPE file_type)
{

	int res, par_size;
	// 建立文件的目录的inode块
	long *par_menu_blk;

	int ret = 0;
	short int menu_inode_num = 0;
	// 文件名和扩展名
	char *fname = (char *)malloc(MAX_FILENAME * sizeof(char));
	char *fext = (char *)malloc(MAX_EXTENSION * sizeof(char));
	char *fall_name = (char *)malloc((MAX_EXTENSION + MAX_FILENAME) * sizeof(char));
	char *remain_path = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
	struct GInode *menu_inode = (struct GInode *)malloc(sizeof(struct GInode));
	struct GDataBlock *data_blk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));
	struct GFileDir *file_dir = (struct GFileDir *)malloc(sizeof(struct GFileDir));

	// 获取文件名, 扩展名, 全名, 剩余路径
	if ((ret = divideFileNameByPath(path, fname, fext, fall_name, remain_path, file_type)) != 0)
	{
		printError("createFileByPath: divide file name failed!");
		goto error;
	}

	// 获取parent menu的inode
	if ((ret = getInodeBlkByPath(remain_path, &menu_inode_num)) != 0)
	{
		printError("createFileByPath: get Menu Inode Blk failed!");
		goto error;
	}

	// 检查该文件或目录是否已经存在
	if ((ret = getFileDirByPath(path, file_dir)) == 0)
	{
		ret = -1;
		printError("createFileByPath: the file already exit.");
		goto error;
	}

	// 不存在, 则创建新的
	int hash_num = hash(fall_name);

	// 赋值file_dir
	memcpy(file_dir->fname, fname, strlen(fname));
	memcpy(file_dir->fext, fext, strlen(fext));
	// 文件大小初始化为0
	file_dir->fsize = 0;
	file_dir->nMenuInode = menu_inode_num;
	file_dir->nInodeBlock = -1;
	file_dir->flag = file_type;

	// 根据哈希值在目录创建filedir
	if ((ret = createFileDirByHash(hash_num, menu_inode_num, file_dir)) != 0)
	{
		printError("createFileByPath: create file dir failed!");
		goto error;
	}

	// 当前块放不下目录内容

	// 为父目录文件新增一个块

	// 块容量足够，直接加size

	// 给新建的file_directory赋值

	// 找到空闲块作为起始块

	// 为新建的文件申请一个空闲块

	// 将要创建的文件或目录信息写入上层目录中

	// 文件起始块内容为空

	printSuccess("createFileByPath: success!");

error:
	free(fname);
	free(fext);
	free(remain_path);
	free(data_blk);
	free(file_dir);
	free(menu_inode);
	return ret;
}
