/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "FileOper.h"

// 根据块号, 读取文件数据GDataBlock
int readDataByBlkId(short int blk_id,struct GDataBlock *data_blk)
{
	
	// 读取文件
    FILE *fp = NULL;
    fp = fopen(DISK_PATH, "r+");//打开文件
    if(fp == NULL){
        printError("Open disk file failed! The file may don't exits.");
        return 0;
    }
    // printSuccess("Open disk file success!");

	// 文件打开后，就用blk_id * FS_BLOCK_SIZE作为偏移量
	fseek(fp, blk_id * FS_BLOCK_SIZE, SEEK_SET);
	// 清空
	memset(data_blk, 0, sizeof(struct GDataBlock));
	if(fread(data_blk, sizeof(struct GDataBlock), 1 , fp) == 0){
        printSuccess("Read data block success!");
    }else{
        printError("Read data block failed!");
    }
	
	
    // 判断是否正确读取
    if(ferror(fp))
	{
		printError("Read disk file failed!");
        return -1;
	}

	fclose(fp);
    return 0;
}
// 根据块号, 读取Inode
int readInodeByBlkId(short int blk_id,struct GInode *inode_blk)
{
	struct GDataBlock gblk;
	int ret = readDataByBlkId(blk_id, &gblk);
	if(ret != 0){
		return ret;
	}
	// 复制内存 
	memcpy(inode_blk, &gblk, sizeof(struct GInode));
	return 0;
}




// 给定根据hash_num和cur_i, 返回对应的inode块号
int getInodeBlkByHash(const int hash_num, const int cur_i, int *target_i)
{

}

// 根据hash_num和cur_i, 返回对应的FileDir todo: 解决哈希冲突
int getFileDirByHash(const int hash_num, const int cur_i, int *target_i, struct GFileDir * p_filedir)
{
	struct  GInode * temp_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// 获取当前inode号的inode
	readInodeByBlkId(cur_i, temp_inode);
	struct GDataBlock *data_blk = malloc(sizeof(struct GDataBlock));
	

	// todo: 解决哈希冲突
	if(0 <= hash_num < FD_ZEROTH_INDIR){
		int i = hash_num / FD_PER_BLK;
		short int addr = temp_inode->addr[i];
		if(addr < 0) goto error;
		readDataByBlkId(addr, data_blk);
		int offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromData(data_blk->data, offset, p_filedir);
	
	// todo: 优化成循环函数
	}else if(hash_num < FD_FIRST_INDIR){
		// 一次间接块  4
		short int addr = temp_inode->addr[4];
		if(addr < 0) goto error;
		int offset = (hash_num - FD_ZEROTH_INDIR) * sizeof(short int) / FD_PER_BLK;
		readDataByBlkId(addr, data_blk);
		addr = retShortIntFromData(data_blk->data, offset);
		readDataByBlkId(addr, data_blk);
		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromData(data_blk->data, offset, p_filedir);
	}else if(hash_num < FD_SECOND_INDIR){
		// 二次间接块  5
		short int addr = temp_inode->addr[5];
		if(addr < 0) goto error;
		readDataByBlkId(addr, data_blk);
		int offset = (hash_num - FD_FIRST_INDIR) * sizeof(short int) / (FD_PER_BLK * FD_PER_BLK);	
		addr = retShortIntFromData(data_blk->data, offset);
		readDataByBlkId(addr, data_blk);
		
		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		readDataByBlkId(addr, data_blk);
		
		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromData(data_blk->data, offset, p_filedir);

	}else if(hash_num < MAX_HASH_SIZE){
		// 三次间接块  6
		short int addr = temp_inode->addr[6];
		if(addr < 0) goto error;
		readDataByBlkId(addr, data_blk);
		int offset = (hash_num - FD_SECOND_INDIR) * sizeof(short int) / (FD_PER_BLK * FD_PER_BLK * FD_PER_BLK);	
		addr = retShortIntFromData(data_blk->data, offset);
		readDataByBlkId(addr, data_blk);

		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		readDataByBlkId(addr, data_blk);
		
		offset = offset * FD_PER_BLK;
		addr = retShortIntFromData(data_blk->data, offset);
		readDataByBlkId(addr, data_blk);
		
		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		getFileDirFromData(data_blk->data, offset, p_filedir);

	}else{
		error:
		printError("Get file dir but addr is <0!");
		free(data_blk);
		free(temp_inode);
		return -1;
	}
	free(data_blk);
	free(temp_inode);
	return 0;
}

// Inode相关函数
// 根据路径, 获取inode, 
int getInodeBlkByPath(const char * path, long *file_inode)
{
	
	
	return 0;
}

// 检查路径
int checkFilePath(const char * path)
{
	char *base_name;
	base_name = strrchr(path, '/');
	if (base_name == NULL) {
		printError("checkFilePath: file base name is Null!");
		printf("Path: %s\n", path);
		return -ENOENT;
	}
	base_name++;
	if (strlen(base_name) > 255) {
		printError("checkFilePath: basename exceeds 255 characters");
		printf("Path: %s\n", path);
		return -ENAMETOOLONG;
	}
	return 0;
}

// 根据路径，到相应的目录寻找文件的GFileDir，并赋值给attr
int getFileDirByPath(const char * path,struct GFileDir *attr)
{
	// 获取磁盘根目录块的位置
	printSuccess("Get File Dir To Attr Start!");
	printf("getFileDirToAttr: get file dir by: %s\n", path);
	
    struct GDataBlock *data_blk;
	data_blk = malloc(sizeof(struct GDataBlock));

	// 读出超级块
	if (readDataByBlkId(0, data_blk) == -1) 
	{
		printError("getFileDirToAttr: read super block failed!");
		free(data_blk);	
        return -1;
	}else{
        printSuccess("getFileDirToAttr: read super block success!");
    }


	struct GSuperBlock* sp_blk;
    // 将原本GDataBlock的数据转换成GSuperBlock
	sp_blk = (struct GSuperBlock*) data_blk;
	long start_blk;
	start_blk = sp_blk->first_blk;

    printf("Super Block:\nfs_size=%ld, first_blk=%ld, datasize=%ld, first_inode=%ld, inode_area_size=%ld, fisrt_blk_of_inodebitmap=%ld, inodebitmap_size=%ld, first_blk_of_databitmap=%ld, databitmap_size=%ld\n",
       sp_blk->fs_size, sp_blk->first_blk, sp_blk->datasize, sp_blk->first_inode, sp_blk->inode_area_size, sp_blk->fisrt_blk_of_inodebitmap,
       sp_blk->inodebitmap_size, sp_blk->first_blk_of_databitmap, sp_blk->databitmap_size);
	printf("start_blk:%ld\n",start_blk);

	// 解析path
	char *tmp_path;  
	tmp_path=strdup(path);
	
	// 检查路径
	int ret = checkFilePath(tmp_path);
	// 如果路径为空，则出错返回-1
	if (ret != 0) 
	{
		printError("getFileDirToAttr:check file path failed!");
		free(sp_blk);
		return -1;
	}
	
	// 如果路径为根目录路径
	if (strcmp(tmp_path, "/") == 0) 
	{
		attr->flag = 2; // 2 menu
		attr->nMenuBlock = start_blk;
		attr->nInodeBlock = start_blk;
		free(sp_blk);
		printSuccess("getFileDirToAttr: this is a root menu");
		return 0;
	}

	char * base_name;
	int name_len = strlen(tmp_path);
	// 文件或目录的长度
	int len = 0;
	// 当前的目录或文件的inode
	long cur_i = start_blk;
	// 类型标志 -1表示都有可能, 2表示路径
	// int flag = -1;

	tmp_path++;
	name_len--;
	// 取路径的第一个字符, 跳过了根目录 /
	char c = *tmp_path;

	while(1)
	{
		base_name = tmp_path;
		// 循环得到每一级的目录
		for(len = 0; --name_len >= 0; len++ )
		{
			c = *(tmp_path ++);
			if(c == '/')
				break;
		}
		if(name_len < 0) break;
		char* menu_flag = strchr(base_name,'/');
		// flag = -1;  // 重置标志
		
		// 计算斜杠后的部分的长度
        size_t length =  menu_flag - base_name;
        // 创建一个新字符串来存储
        char *const result = (char*)malloc(sizeof(char) * (length + 1));
		if(menu_flag != NULL){
			// flag = 2;  // 是个目录
			
        	strncpy(result, path, length);
        	result[length] = '\0'; // 添加字符串结束符
			base_name = result;
		} 
		
		// 下面根据base_name和cur_i 查找 RD 并更新 cur_i
		// 采用哈希表的方式查找
		int hash_num = hash(base_name);
		// 根据hash_num和cur_i, 返回对应的块号
		// if (strcmp(current->name, fileName) != 0) {
        //     goto error;
        // }
		
		free(result);
	}
	
	
    free(tmp_path);
    free(data_blk);
	
	return -1;
}

// 将char的data 转化为short int形式, 注意采用小端序编码
short int retShortIntFromData(const char* data,const int offset) {
    // 使用位运算将两个字节的数据合并成一个 short int
    short int result = ((uint16_t)data[offset + 1] << 8) | (uint16_t)data[offset];
    return result;
}

// 给定GDataBlock中char* 将data 解读成GFileDir
void getFileDirFromData(const char* data,const int offset, struct GFileDir * p_fd){
	memcpy(p_fd, &data[offset], sizeof(struct GFileDir));
}

// 根据inode, 获取该文件占据了多少块
void getFileBlkNum(struct GInode *inode, int *blk_num)
{
	if(inode == NULL){
		printError("getFileBlkNum: inode is NULL!");
		return;
	}
	int ret_num = 0;
	// 直接地址
	for(int i = 0; i < 4; i++){
		if(inode->addr[i] >= 0){
			ret_num++;
		}
	}

	// int ADDR_PER_BLK = MAX_DATA_IN_BLOCK / sizeof(short int);
	struct GDataBlock *first_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *second_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *third_data_blk = malloc(sizeof(struct GDataBlock));

	// 一次间接
	short int first_Indir = inode->addr[4];
	if(first_Indir >= 0){
		readDataByBlkId(first_Indir, first_data_blk);
		ret_num += first_data_blk->size / sizeof(short int);
	}

	// 二次间接 
	short int second_Indir = inode->addr[5];
	if(second_Indir >= 0){
		readDataByBlkId(second_Indir, second_data_blk);
		for(int i = 0; i < second_data_blk->size; i += sizeof(short int)){
			readDataByBlkId(retShortIntFromData(second_data_blk->data, i), first_data_blk);
			ret_num += first_data_blk->size / sizeof(short int);
		}
	}

	// 三次间接
	short int third_Indir = inode->addr[6];
	if(third_Indir >= 0){
		readDataByBlkId(third_Indir, third_data_blk);
		for(int i = 0; i < third_data_blk->size; i += sizeof(short int)){
			readDataByBlkId(retShortIntFromData(third_data_blk->data, i), second_data_blk);
			for(int i = 0; i < second_data_blk->size; i += sizeof(short int)){
				readDataByBlkId(retShortIntFromData(second_data_blk->data, i), first_data_blk);
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