/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "FileOper.h"

// 根据块号, 读取文件数据GDataBlock
int getDataByBlkId(const short int blk_id, struct GDataBlock *data_blk)
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
	// getDebugByteData(fp);

	// 文件打开后，就用blk_id * FS_BLOCK_SIZE作为偏移量
	fseek(fp, blk_id * FS_BLOCK_SIZE, SEEK_SET);
	// 清空
	memset(data_blk, 0, sizeof(struct GDataBlock));
	if (fread(data_blk, sizeof(struct GDataBlock), 1, fp) > 0)
	{
		// printSuccess("getDataByBlkId: Read data block success!");
		;
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

// 根据块号, 将文件数据GDataBlock写入文件
int writeDataByBlkId(short int blk_id, const struct GDataBlock *data_blk)
{

	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+"); // 打开文件
	if (fp == NULL)
	{
		printError("writeDataByBlkId: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		return -1;
	}
	// printSuccess("Open disk file success!");

	// 文件打开后，就用blk_id * FS_BLOCK_SIZE作为偏移量
	fseek(fp, blk_id * FS_BLOCK_SIZE, SEEK_SET);

	if (fwrite(data_blk, sizeof(struct GDataBlock), 1, fp) > 0)
	{
		printSuccess("writeDataByBlkId: write data block success!");
	}
	else
	{
		printError("writeDataByBlkId: write data block failed!");
		fclose(fp);
		return -1;
	}

	// 判断是否正确读取
	if (ferror(fp))
	{
		printError("writeDataByBlkId: write disk file failed!");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

// 根据inode号, 读取Inode
// inode号, 即规定以根目录的inode的id为0
int getInodeByInodeId(const short int inode_id, struct GInode *inode_blk)
{
	// inode号转换成块号
	const int inode_start = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP;
	const int blk_offset = inode_start + inode_id / MAX_INODE_IN_BLOCK;
	const int inode_offset = inode_id % MAX_INODE_IN_BLOCK;
	struct GDataBlock *gblk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));

	int ret = getDataByBlkId(blk_offset, gblk);
	if (ret != 0)
	{
		free(gblk);
		return ret;
	}

	memcpy(inode_blk, gblk->data + inode_offset * sizeof(struct GInode), sizeof(struct GInode));
	free(gblk);
	return 0;
}

// 根据块号, 写入Inode, 一个块多个inode
int writeInodeByInodeId(const short int inode_id, const struct GInode *inode_blk)
{
	// inode号转换成块号
	const int inode_start = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP;
	const int blk_offset = inode_start + inode_id / MAX_INODE_IN_BLOCK;
	const int inode_offset = inode_id % MAX_INODE_IN_BLOCK;
	struct GDataBlock *gblk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));
	// 读入原来的数据
	getDataByBlkId(blk_offset, gblk);
	if (gblk->size < 0)
		gblk->size = 0;

	// memcpy(gblk->data + gblk->size, inode_blk, sizeof(struct GInode));
	memcpy(gblk->data + inode_offset * sizeof(struct GInode), inode_blk, sizeof(struct GInode));

	gblk->size += sizeof(struct GInode);
	int ret = writeDataByBlkId(blk_offset, gblk);

	if (ret != 0)
	{
		free(gblk);
		return ret;
	}

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
	getInodeByInodeId(cur_i, temp_inode);
	struct GDataBlock *data_blk = malloc(sizeof(struct GDataBlock));

	if (0 <= hash_num && hash_num < FD_ZEROTH_INDIR)
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
		int offset = (((hash_num - FD_ZEROTH_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
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
		int offset = (((hash_num - FD_FIRST_INDIR) / (ADDR_PER_BLK * FD_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);
		offset = (((hash_num - FD_FIRST_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
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
		int offset = (((hash_num - FD_SECOND_INDIR) / (ADDR_PER_BLK * ADDR_PER_BLK * FD_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		offset = (((hash_num - FD_SECOND_INDIR) / (FD_PER_BLK * ADDR_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		addr = retShortIntFromData(data_blk->data, offset);
		getDataByBlkId(addr, data_blk);

		// 断了?
		offset = (((hash_num - FD_SECOND_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
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
	int ret = 0;

	// 增加大小
	data_blk->size += sizeof(struct GFileDir);
	// 写入
	memcpy(&(data_blk->data[offset]), p_fd, sizeof(struct GFileDir));

	return ret;
}

// 将filedir从GDataBlock的data中删除
int removeFileDirFromDataBlk(const int offset, struct GDataBlock *data_blk)
{
	int ret = 0;
	// 减少大小
	data_blk->size -= sizeof(struct GFileDir);
	// 写入
	memset(&(data_blk->data[offset]), -1, sizeof(struct GFileDir));
	return ret;
}

// 设置bitmap已经被使用
int setBitmapUsed(const long start_bitmap_blk, const long offset_bit, const int num)
{
	int ret = 0;
	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+");
	if (fp == NULL)
	{
		ret = -1;
		printError("setBitmapUsed: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		goto error;
	}

	const long offset_temp_byte = offset_bit / 8;
	const long offset_temp_bit = offset_bit % 8;

	// 移动指针到文件的DataBitmap块
	if (fseek(fp, FS_BLOCK_SIZE * start_bitmap_blk + offset_temp_byte, SEEK_SET) != 0)
	{
		ret = -1;
		printError("setBitmapUsed: DataBitmap fseek failed!");
		goto error;
	}

	unsigned char temp_byte = 0x00;
	fread(&temp_byte, sizeof(unsigned char), 1, fp);
	// 返回之前的指针
	fseek(fp, -1 * sizeof(temp_byte), SEEK_CUR);
	// 处理开头的offset的bit情况
	for (int i = 0; i < min(num, (8 - offset_temp_bit)); i++)
	{
		unsigned char mask = 0x80;
		mask >>= (offset_temp_bit + i);
		temp_byte |= mask;
	}
	fwrite(&temp_byte, sizeof(unsigned char), 1, fp);

	// 处理一整个Byte都是num区间内的情况
	const int set_byte_num = (num + offset_temp_bit) / 8 - 1;
	if (set_byte_num > 0)
	{
		unsigned char a[set_byte_num];
		memset(a, 0xFF, sizeof(a));
		fwrite(a, sizeof(a), 1, fp);
	}

	if ((num + offset_temp_bit) > 8)
	{
		// 处理一整个Byte只有一部分在num区间内的情况
		temp_byte = 0x00;
		fread(&temp_byte, sizeof(unsigned char), 1, fp);
		// 返回之前的指针
		fseek(fp, -1, SEEK_CUR);
		const int rest_used_bit = (num + offset_temp_bit) % 8;
		// 利用循环置1
		for (int i = 0; i < rest_used_bit; i++)
		{
			unsigned char mask = 0x01;
			mask <<= (7 - i);
			temp_byte |= mask;
		}
		fwrite(&temp_byte, sizeof(int), 1, fp);
	}

	printSuccess("setBitmapUsed: set free bit done!");

error:
	fclose(fp);
	return ret;
}

// 将使用过的块重新变为空闲块
int unsetBitmapUsed(const long start_bitmap_blk, const long offset_bit, const int num)
{
	int ret = 0;
	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+");
	if (fp == NULL)
	{
		ret = -1;
		printError("unsetBitmapUsed: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		goto error;
	}

	const long offset_temp_byte = offset_bit / 8;
	const long offset_temp_bit = offset_bit % 8;

	// 移动指针到文件的DataBitmap块
	if (fseek(fp, FS_BLOCK_SIZE * start_bitmap_blk + offset_temp_byte, SEEK_SET) != 0)
	{
		ret = -1;
		printError("unsetBitmapUsed: DataBitmap fseek failed!");
		goto error;
	}

	unsigned char temp_byte = 0x00;
	fread(&temp_byte, sizeof(unsigned char), 1, fp);
	// 返回之前的指针
	fseek(fp, -1 * sizeof(temp_byte), SEEK_CUR);
	// 处理开头的offset的bit情况
	for (int i = 0; i < min(num, (8 - offset_temp_bit)); i++)
	{
		unsigned char mask = 0x80;
		mask >>= (offset_temp_bit + i);
		// 判断对应位是否为1
		if ((mask & temp_byte) == mask)
		{
			// 若为1,用异或操作置零
			temp_byte ^= mask;
		}
	}
	fwrite(&temp_byte, sizeof(unsigned char), 1, fp);

	// 处理一整个Byte都是num区间内的情况
	const int set_byte_num = (num + offset_temp_bit) / 8 - 1;
	if (set_byte_num > 0)
	{
		unsigned char a[set_byte_num];
		memset(a, 0x00, sizeof(a));
		fwrite(a, sizeof(a), 1, fp);
	}

	if ((num + offset_temp_bit) > 8)
	{
		// 处理一整个Byte只有一部分在num区间内的情况
		temp_byte = 0x00;
		fread(&temp_byte, sizeof(unsigned char), 1, fp);
		// 返回之前的指针
		fseek(fp, -1, SEEK_CUR);
		const int rest_used_bit = (num + offset_temp_bit) % 8;
		// 利用循环置1
		for (int i = 0; i < rest_used_bit; i++)
		{
			unsigned char mask = 0x01;
			mask <<= (7 - i);
			// 判断对应位是否为1
			if ((mask & temp_byte) == mask)
			{
				// 若为1,用异或操作置零
				temp_byte ^= mask;
			}
		}
		fwrite(&temp_byte, sizeof(int), 1, fp);
	}

	printSuccess("unsetBitmapUsed: set free bit done!");

error:
	fclose(fp);
	return ret;
}

// 找到data空闲块
int getFreeDataBlk(const int need_num, short int *start_blk)
{
	int ret = 0;

	// 读入超级块
	// todo: 优化成直接使用config中的数据?
	struct GSuperBlock *sp_blk = (struct GSuperBlock *)malloc(sizeof(struct GSuperBlock));
	unsigned char *temp_unit = malloc(sizeof(unsigned char)); // 8bits
	getSuperBlock(sp_blk);
	const long start_data_bitmap = sp_blk->first_blk_of_databitmap;
	const long max_data_size = sp_blk->databitmap_size;
	const long max_data_bitmap = start_data_bitmap + max_data_size;
	const int max_num_perblk = FS_BLOCK_SIZE;

	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+");
	if (fp == NULL)
	{
		ret = -1;
		printError("getFreeDataBlk: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		goto error;
	}

	// 移动指针到文件的DataBitmap块
	if (fseek(fp, FS_BLOCK_SIZE * start_data_bitmap, SEEK_SET) != 0)
	{
		ret = -1;
		printError("getFreeDataBlk: DataBitmap fseek failed!");
		goto error;
	}

	long cur_blk = start_data_bitmap;
	// 已经遍历的块数量
	long iter_blk_num = 0;
	// 已经遍历的byte数量
	long iter_byte_num = 0;
	// 已经遍历的bit数量
	long iter_bit_num = 0;
	// 当前已经满足的bit数量
	int cur_neededBit_num = 0;
	// 检查结束标志
	int done_flag = 0;

	// 检查数据区bitmap, 不断循环直到找到符合条件的连续n块空闲块
	while (cur_blk < max_data_bitmap)
	{
		unsigned char cur_mask = 0x80;
		// 遍历块中的每一个Byte
		for (iter_byte_num = 0; iter_byte_num < max_num_perblk; iter_byte_num++)
		{
			// getDebugByteData(fp);
			// 读出8个bit
			fread(temp_unit, sizeof(unsigned char), 1, fp);
			const unsigned char cur_byte = *temp_unit;
			// 遍历一个Byte
			for (iter_bit_num = 0; iter_bit_num < 8; iter_bit_num++)
			{
				// 与操作, 等于mask说明当前指向的bit被占用
				if ((cur_byte & cur_mask) != cur_mask)
					cur_neededBit_num++;
				else
					// 被占用,清零
					cur_neededBit_num = 0;

				// 空闲bit数符合要求
				if (cur_neededBit_num == need_num)
				{
					done_flag = 1;
				}

				if (done_flag)
					break;
				// mask最低位为1, 说明8个bit已读完
				if ((cur_mask & 0x01) == 0x01)
				{
					// 重新初始化为高位
					cur_mask = 0x80;
				}
				// 位为1,右移1位，检查下一位是否可用
				else
					cur_mask >>= 1;
			}
			if (done_flag)
				break;
		}
		if (done_flag)
			break;

		// 增加已经遍历的块号
		iter_blk_num++;
	}

	// 检查是否到了最后还是不满足
	if (cur_blk == max_data_bitmap - 1 && iter_byte_num == max_num_perblk - 1 && iter_bit_num == 7)
	{
		ret = -1;
		printError("getFreeDataBlk: data bitmap has no blk.");
		goto error;
	}

	// 计算开始的块号
	*start_blk = iter_blk_num * FS_BLOCK_SIZE * 8 + iter_byte_num * 8 + iter_bit_num - need_num + 1;

	// 标记已经使用的块号
	if ((ret = setBitmapUsed(start_data_bitmap, *start_blk, need_num)) != 0)
	{

		printError("getFreeDataBlk: set bitmap failed!");
		goto error;
	}
	printSuccess("getFreeDataBlk: malloc data blk success!");

error:
	fclose(fp);
	free(sp_blk);
	free(temp_unit);
	return ret;
}

// 找到inode空闲块
int getFreeInodeBlk(const int need_num, short int *start_blk)
{
	int ret = 0;
	// 读入超级块
	struct GSuperBlock *sp_blk = (struct GSuperBlock *)malloc(sizeof(struct GSuperBlock));
	unsigned char *temp_unit = malloc(sizeof(unsigned char)); // 8bits
	getSuperBlock(sp_blk);
	const long start_inode_bitmap = sp_blk->first_blk_of_inodebitmap;
	const long max_inode_size = sp_blk->inodebitmap_size;
	const long max_inode_bitmap = start_inode_bitmap + max_inode_size;
	const int max_num_perblk = FS_BLOCK_SIZE;

	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+");
	if (fp == NULL)
	{
		ret = -1;
		printError("getFreeInodeBlk: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		goto error;
	}

	// 移动指针到文件的inodeBitmap块
	if (fseek(fp, FS_BLOCK_SIZE * start_inode_bitmap, SEEK_SET) != 0)
	{
		ret = -1;
		printError("getFreeInodeBlk: inodeBitmap fseek failed!");
		goto error;
	}

	long cur_blk = start_inode_bitmap;
	// 已经遍历的块数量
	long iter_blk_num = 0;
	// 已经遍历的byte数量
	long iter_byte_num = 0;
	// 已经遍历的bit数量
	long iter_bit_num = 0;
	// 当前已经满足的bit数量
	int cur_neededBit_num = 0;
	// 检查结束标志
	int done_flag = 0;

	// 检查Inode区bitmap, 不断循环直到找到符合条件的连续n块空闲块
	while (cur_blk < max_inode_bitmap)
	{
		unsigned char cur_mask = 0x80;
		// 遍历块中的每一个Byte
		for (iter_byte_num = 0; iter_byte_num < max_num_perblk; iter_byte_num++)
		{
			// 读出8个bit
			fread(temp_unit, sizeof(unsigned char), 1, fp);
			const unsigned char cur_byte = *temp_unit;
			// 遍历一个Byte
			for (iter_bit_num = 0; iter_bit_num < 8; iter_bit_num++)
			{
				// 与操作, 等于mask说明当前指向的bit被占用
				if ((cur_byte & cur_mask) != cur_mask)
					cur_neededBit_num++;
				else
					// 被占用,清零
					cur_neededBit_num = 0;

				// 空闲bit数符合要求
				if (cur_neededBit_num == need_num)
				{
					done_flag = 1;
				}

				if (done_flag)
					break;
				// mask最低位为1, 说明8个bit已读完
				if ((cur_mask & 0x01) == 0x01)
				{
					// 重新初始化为高位
					cur_mask = 0x80;
				}
				// 位为1,右移1位，检查下一位是否可用
				else
					cur_mask >>= 1;
			}
			if (done_flag)
				break;
		}
		if (done_flag)
			break;

		// 增加已经遍历的块号
		iter_blk_num++;
	}

	// 检查是否到了最后还是不满足
	if (cur_blk == max_inode_bitmap - 1 && iter_byte_num == max_num_perblk - 1 && iter_bit_num == 7)
	{
		ret = -1;
		printError("getFreeInodeBlk: inode bitmap has no blk.");
		goto error;
	}

	// 计算开始的块号
	*start_blk = iter_blk_num * FS_BLOCK_SIZE * 8 + iter_byte_num * 8 + iter_bit_num - need_num + 1;

	// 标记已经使用的块号
	if ((ret = setBitmapUsed(start_inode_bitmap, *start_blk, need_num)) != 0)
	{

		printError("getFreeInodeBlk: set bitmap failed!");
		goto error;
	}
	printSuccess("getFreeInodeBlk: malloc inode blk success!");

error:
	fclose(fp);
	free(sp_blk);
	free(temp_unit);
	return ret;
}

// 初始化inode
int initInode(struct GInode *inode)
{
	int ret = 0;
	// 为根目录的 struct GInode 对象赋值
	inode->st_mode = 0755;		// 权限，例如 rwxr-xr-x
	inode->st_ino = 0;			// i-node号为0
	inode->st_nlink = 1;		// 连接数，通常为1
	inode->st_uid = 0;			// 根目录的用户 ID，通常为0（超级用户）
	inode->st_gid = 0;			// 根目录的组 ID，通常为0（超级用户组）
	inode->st_size = FILE_SIZE; // 文件大小，为4KB
	time_t currentTime;
	currentTime = time(NULL); // 获取当前时间

	if (currentTime == (time_t)-1)
	{
		ret = -1;
		printError("Inode get time failed!");
	}
	else
	{
		printSuccess("Inode get time success!");
	}
	inode->st_atim = currentTime;

	// 设置磁盘地址
	// 磁盘地址有7个, addr[0]-addr[3]是直接地址，addr[4]是一次间接，
	// addr[5]是二次间接，addr[6]是三次间接。
	memset(inode->addr, -1, sizeof(inode->addr));
	printSuccess("initInode: success!");
	return ret;
}

// 根据传入的addr, 直接修改menu的addr并且获取data blk, 第0层索引, 即addr是从inode中得到
void getAddrDataDirectIndex(short int *addr, struct GDataBlock *data_blk)
{

	if (*addr < 0 || checkBitmapUsed((SUPER_BLOCK + INODE_BITMAP), *addr) == 0) // 地址未被创建
	{
		// 根据data bitmap, 找到空闲块
		getFreeDataBlk(1, addr);
		// 将该数据块的GDataBlock中的size全部初始化为0
		getDataByBlkId(*addr, data_blk);
		data_blk->size = 0;
		// 将data中的short int 置-1, 可能init到了存储file dir的块
		initShortIntToData(data_blk->data);
		// 将data_block写进文件
		writeDataByBlkId(*addr, data_blk);
	}
	else // 地址已被创建
	{
		getDataByBlkId(*addr, data_blk);
		// getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
}

// 根据传入的addr, 修改addr并且获取data blk, 适用于0层以上索引, 即addr不是从inode中得到
void getAddrDataIndirectIndex(short int *addr, const int offset, struct GDataBlock *data_blk)
{
	const short int old_addr = *addr;
	const short int new_addr = retShortIntFromData(data_blk->data, offset);
	if (new_addr < 0 || checkBitmapUsed((SUPER_BLOCK + INODE_BITMAP), new_addr) == 0) // 地址未被创建
	{
		// 赋值原来的addr
		// short int old_addr = *addr;
		// 根据data bitmap, 找到空闲块
		*addr = -1;
		getFreeDataBlk(1, addr);
		// 将获得的addr写入原来的位置
		writeShortIntToData(*addr, offset, data_blk->data);
		data_blk->size += sizeof(short int);
		// 将data写进文件
		writeDataByBlkId(old_addr, data_blk);
		// 将该数据块的GDataBlock中的size全部初始化为0
		getDataByBlkId(*addr, data_blk);
		data_blk->size = 0;
		// 将data中的short int 置-1, 可能init到了存储file dir的块
		initShortIntToData(data_blk->data);
		// 将data_block写进文件
		writeDataByBlkId(*addr, data_blk);
	}
	else // 地址已被创建
	{
		getDataByBlkId(*addr, data_blk);
		// getFileDirFromDataBlk(data_blk, offset, p_filedir);
	}
}

// 因为写进了file dir, 更新menu的inode
int updateMenuInode(const short int cur_i, struct GInode *menu_inode)
{
	int ret = 0;
	// 目录的inode中增加st_size
	menu_inode->st_size += sizeof(struct GFileDir);
	// 更新目录的inode st_atim
	time_t currentTime;
	currentTime = time(NULL); // 获取当前时间
	if (currentTime == (time_t)-1)
		printError("Inode get time failed!");
	menu_inode->st_atim = currentTime;
	// 写进menu_inode
	ret = writeInodeByInodeId(cur_i, menu_inode);
	return ret;
}

// 因为删除更新menu的inode
int rmUpdateMenuInode(const short int cur_i, struct GInode *menu_inode)
{
	int ret = 0;
	// 目录的inode中减少st_size
	menu_inode->st_size -= sizeof(struct GFileDir);
	if (menu_inode->st_size <= 0)
	{
		memset(menu_inode->addr, -1, sizeof(menu_inode->addr));
	}

	// 更新目录的inode st_atim
	time_t currentTime;
	currentTime = time(NULL); // 获取当前时间
	if (currentTime == (time_t)-1)
		printError("Inode get time failed!");
	menu_inode->st_atim = currentTime;
	// 写进menu_inode
	ret = writeInodeByInodeId(cur_i, menu_inode);
	return ret;
}

// 根据哈希值在menu中创建file dir, cur_i是当前菜单的inode号
int createFileDirByHash(const int hash_num, const int cur_i, struct GFileDir *p_filedir)
{
	int ret = 0;

	struct GInode *menu_inode = (struct GInode *)malloc(sizeof(struct GInode));
	struct GInode *temp_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// 获取当前inode号的inode
	getInodeByInodeId(cur_i, menu_inode);
	struct GDataBlock *data_blk = malloc(sizeof(struct GDataBlock));

	if (0 <= hash_num && hash_num < FD_ZEROTH_INDIR)
	{
		int i = hash_num / FD_PER_BLK;
		short int *addr = &menu_inode->addr[i];
		int offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);

		// 获取地址和data blk
		getAddrDataDirectIndex(addr, data_blk);
		// 更新menu inode
		updateMenuInode(cur_i, menu_inode);

		// 根据inode bitmap, 找到空闲块, 作为创建文件的inode, 并将inode号赋值给传入的p_filedir
		short int temp_free_inode = -1;
		getFreeInodeBlk(1, &temp_free_inode);
		// 初始化新的inode
		initInode(temp_inode);
		// 将新inode写进文件
		writeInodeByInodeId(temp_free_inode, temp_inode);

		// 更新传进来的file dir
		p_filedir->nInodeBlock = temp_free_inode;
		// 将file dir写入到data blk中
		writeFileDirToDataBlk(p_filedir, offset, data_blk);
		// 将data blk写入到文件
		writeDataByBlkId(*addr, data_blk);
	}
	else if (hash_num < FD_FIRST_INDIR)
	{
		// 一次间接块  4
		short int *addr = &menu_inode->addr[4];
		// 获取地址和data blk
		getAddrDataDirectIndex(addr, data_blk);

		// 下面不能传入addr,要不然会把菜单的addr给改了
		short int indir_addr = *addr;
		int offset = (((hash_num - FD_ZEROTH_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
		getAddrDataIndirectIndex(&indir_addr, offset, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		// 更新menu inode
		updateMenuInode(cur_i, menu_inode);

		// 根据inode bitmap, 找到空闲块, 作为创建文件的inode, 并将inode号赋值给传入的p_filedir
		short int temp_free_inode = -1;
		getFreeInodeBlk(1, &temp_free_inode);
		// 初始化新的inode
		initInode(temp_inode);
		// 将新inode写进文件
		writeInodeByInodeId(temp_free_inode, temp_inode);

		// 更新传进来的file dir
		p_filedir->nInodeBlock = temp_free_inode;
		// 将file dir写入到data blk中
		writeFileDirToDataBlk(p_filedir, offset, data_blk);
		// 将data blk写入到文件
		writeDataByBlkId(indir_addr, data_blk);
	}
	else if (hash_num < FD_SECOND_INDIR)
	{
		// 二次间接块  5
		short int *addr = &menu_inode->addr[5];
		// 获取地址和data blk
		getAddrDataDirectIndex(addr, data_blk);

		// 下面不能传入addr,要不然会把菜单的addr给改了
		short int indir_addr = *addr;
		int offset = (((hash_num - FD_FIRST_INDIR) / (ADDR_PER_BLK * FD_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		getAddrDataIndirectIndex(&indir_addr, offset, data_blk);

		offset = (((hash_num - FD_FIRST_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
		getAddrDataIndirectIndex(&indir_addr, offset, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		// 更新menu inode
		updateMenuInode(cur_i, menu_inode);

		// 根据inode bitmap, 找到空闲块, 作为创建文件的inode, 并将inode号赋值给传入的p_filedir
		short int temp_free_inode = -1;
		getFreeInodeBlk(1, &temp_free_inode);
		// 初始化新的inode
		initInode(temp_inode);
		// 将新inode写进文件
		writeInodeByInodeId(temp_free_inode, temp_inode);

		// 更新传进来的file dir
		p_filedir->nInodeBlock = temp_free_inode;
		// 将file dir写入到data blk中
		writeFileDirToDataBlk(p_filedir, offset, data_blk);
		// 将data blk写入到文件
		writeDataByBlkId(indir_addr, data_blk);
	}
	else if (hash_num < MAX_HASH_SIZE)
	{
		// 三次间接块  6
		short int *addr = &menu_inode->addr[6];
		// 获取地址和data blk
		getAddrDataDirectIndex(addr, data_blk);

		short int indir_addr = *addr;
		int offset = (((hash_num - FD_SECOND_INDIR) / (ADDR_PER_BLK * ADDR_PER_BLK * FD_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		getAddrDataIndirectIndex(&indir_addr, offset, data_blk);

		offset = (((hash_num - FD_SECOND_INDIR) / (FD_PER_BLK * ADDR_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		getAddrDataIndirectIndex(&indir_addr, offset, data_blk);

		offset = (((hash_num - FD_SECOND_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
		getAddrDataIndirectIndex(&indir_addr, offset, data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		// 更新menu inode
		updateMenuInode(cur_i, menu_inode);

		// 根据inode bitmap, 找到空闲块, 作为创建文件的inode, 并将inode号赋值给传入的p_filedir
		short int temp_free_inode = -1;
		getFreeInodeBlk(1, &temp_free_inode);
		// 初始化新的inode
		initInode(temp_inode);
		// 将新inode写进文件
		writeInodeByInodeId(temp_free_inode, temp_inode);

		// 更新传进来的file dir
		p_filedir->nInodeBlock = temp_free_inode;
		// 将file dir写入到data blk中
		writeFileDirToDataBlk(p_filedir, offset, data_blk);
		// 将data blk写入到文件
		writeDataByBlkId(indir_addr, data_blk);
	}
	else
	{
		ret = -1;
		printf("createFileDirByHash: the file may be not exited.\n");
		goto error;
	}

error:
	free(temp_inode);
	free(data_blk);
	free(menu_inode);
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

// 读超级块
int getSuperBlock(struct GSuperBlock *sp_blk)
{

	struct GDataBlock *data_blk;
	data_blk = malloc(sizeof(struct GDataBlock));

	// 读出超级块
	if (getDataByBlkId(0, data_blk) == -1)
	{
		printError("readSuperBlock: read super block failed!");
		free(data_blk);
		return -1;
	}
	else
		printSuccess("readSuperBlock: read super block success!");
	memcpy(sp_blk, data_blk->data, sizeof(struct GSuperBlock));

	free(data_blk);
	return 0;
}

// 根据路径，到相应的目录寻找文件的GFileDir，并赋值给attr
int getFileDirByPath(const char *path, struct GFileDir *attr)
{
	// 获取磁盘根目录块的位置
	// printSuccess("Get File Dir To Attr Start!");
	printf("getFileDirToAttr: get file dir by: %s\n", path);

	struct GSuperBlock *sp_blk;
	// 将原本GDataBlock的数据转换成GSuperBlock
	sp_blk = (struct GSuperBlock *)malloc(sizeof(struct GSuperBlock));
	getSuperBlock(sp_blk);
	long start_inode = -1;
	start_inode = sp_blk->first_inode;
	printf("Super Block:\nfs_size=%ld, first_blk=%ld, datasize=%ld, first_inode=%ld, inode_area_size=%ld, first_blk_of_inodebitmap=%ld, inodebitmap_size=%ld, first_blk_of_databitmap=%ld, databitmap_size=%ld\n",
		   sp_blk->fs_size, sp_blk->first_blk, sp_blk->datasize, sp_blk->first_inode, sp_blk->inode_area_size, sp_blk->first_blk_of_inodebitmap,
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
		return ret;
	}

	// 如果路径为根目录路径
	if (strcmp(tmp_path, "/") == 0)
	{
		ret = 0;
		attr->flag = 2; // 2 menu
		attr->nMenuInode = start_inode;
		attr->nInodeBlock = start_inode;
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
				// todo: 此处注意文件类型的判断,默认没有点为目录, 有点为文件
				if (strcmp(p_fd->fname, base_name) != 0) // || p_fd->flag != GFileDir
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
				if (strcmp(p_fd->fname, fname) != 0 || strcmp(p_fd->fext, fext) != 0) //  || p_fd->flag == 0
				{
				error:
					free(p_fd);
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

// 将short int写入到data中, 采用小端序编码
int writeShortIntToData(const short int addr, const int offset, char *data)
{
	int ret = 0;

	uint16_t init_num = addr;
	data[offset] = (init_num & 0x00FF);
	data[offset + 1] = (init_num & 0xFF00) >> 8;

	printSuccess("writeShortIntToData: success!");
	return ret;
}

// 初始化目录间接索引的short int全部为-1
void initShortIntToData(char *data)
{
	for (int i = 0; i < MAX_DATA_IN_BLOCK; i += sizeof(short int))
	{
		uint16_t init_num = -1;
		data[i] = (init_num & 0x00FF);
		data[i + 1] = (init_num & 0xFF00) >> 8;
	}
	printSuccess("initShortIntToData: success!");
}

// 给定GDataBlock中char* 将data 解读成GFileDir offset以Byte为单位
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
	// int *st_blocks;
	// getFileBlkNum(inode, st_blocks);
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
	st->st_blocks = inode->st_size / FS_BLOCK_SIZE + 1;
}

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
	memset(fname, '\0', sizeof(fname) + 1);
	memset(fext, '\0', sizeof(fext) + 1);
	memset(fall_name, '\0', sizeof(fall_name) + 1);
	memset(remain_path, '\0', sizeof(remain_path) + 1);

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
			remain_path[base_name - free_tmp_path - 1] = '\0';
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
					strncpy(fname, base_name, length + 1);
					strcpy(fext, "");
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
					strcpy(fname, tmp_name);
					strcpy(fext, tmp_ext);
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

// 初始化file_dir
int initFileDir(struct GFileDir *file_dir)
{
	int ret = 0;
	memset(file_dir->fname, '\0', MAX_FILENAME + 1);
	memset(file_dir->fext, '\0', MAX_EXTENSION + 1);
	file_dir->flag = 0;
	file_dir->fsize = 0;
	file_dir->nInodeBlock = -1;
	file_dir->nMenuInode = -1;
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
	// struct GInode *menu_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// struct GDataBlock *data_blk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));
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
	if (getFileDirByPath(path, file_dir) == 0)
	{
		ret = -1;
		printError("createFileByPath: the file already exit.");
		goto error;
	}
	else
	{
		printSuccess("createFileByPath: getFileDirByHash failed is right! not exit already.");
	}

	// 不存在, 则创建新的
	int hash_num = hash(fall_name);
	// 初始化file_dir
	initFileDir(file_dir);

	// 赋值file_dir
	strcpy(file_dir->fname, fname);
	strcpy(file_dir->fext, fext);

	file_dir->nMenuInode = menu_inode_num;
	file_dir->flag = file_type;

	// 根据哈希值在目录创建filedir
	if ((ret = createFileDirByHash(hash_num, menu_inode_num, file_dir)) != 0)
	{
		printError("createFileByPath: create file dir failed!");
		goto error;
	}

	printSuccess("createFileByPath: success!");

error:
	free(fname);
	free(fext);
	free(remain_path);
	// free(data_blk);
	free(file_dir);
	// free(menu_inode);
	return ret;
}

// 根据路径,删除文件
int removeFileByPath(const char *path, enum GTYPE file_type)
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
	// struct GInode *menu_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// struct GDataBlock *data_blk = (struct GDataBlock *)malloc(sizeof(struct GDataBlock));
	struct GFileDir *file_dir = (struct GFileDir *)malloc(sizeof(struct GFileDir));

	// 获取文件名, 扩展名, 全名, 剩余路径
	if ((ret = divideFileNameByPath(path, fname, fext, fall_name, remain_path, file_type)) != 0)
	{
		printError("removeFileByPath: divide file name failed!");
		goto error;
	}

	// 获取parent menu的inode
	if ((ret = getInodeBlkByPath(remain_path, &menu_inode_num)) != 0)
	{
		printError("removeFileByPath: get Menu Inode Blk failed!");
		goto error;
	}

	// 检查该文件或目录是否已经存在
	if (getFileDirByPath(path, file_dir) != 0)
	{
		ret = -1;
		printError("removeFileByPath: the file not exit.");
		goto error;
	}

	// 文件存在, 要进行删除
	// 计算哈希
	int hash_num = hash(fall_name);

	// 根据哈希进行删除
	if ((ret = removeFileByHash(hash_num, menu_inode_num)) != 0)
	{
		printError("removeFileByPath: remove file failed!");
		goto error;
	}

	printSuccess("removeFileByPath: success!");
error:
	free(fname);
	free(fext);
	free(remain_path);
	// free(data_blk);
	free(file_dir);
	// free(menu_inode);
	return ret;
}

// 根据inode id 删除inode中的文件数据
int removeFileDataByInodeId(const short int inode_id)
{
	int ret = 0;
	const int start_data_bitmap = SUPER_BLOCK + INODE_BITMAP;
	struct GInode *temp_inode = (struct GInode *)malloc(sizeof(struct GInode));
	// int ADDR_PER_BLK = MAX_DATA_IN_BLOCK / sizeof(short int);
	struct GDataBlock *first_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *second_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *third_data_blk = malloc(sizeof(struct GDataBlock));

	if (getInodeByInodeId(inode_id, temp_inode) != 0)
	{
		ret = -1;
		goto error;
	}

	// 将原有占了bitmap的块置0
	// 默认文件是顺序存储, 遍历
	const int blk_num = temp_inode->st_size / FS_BLOCK_SIZE;
	int cur_num = 0;

	// 直接地址
	for (int i = 0; i < 4; i++)
	{
		const short int addr = temp_inode->addr[i];
		if (addr >= 0)
			unsetBitmapUsed(start_data_bitmap, addr, 1);
		else
			goto error;

		if (cur_num == blk_num)
			goto error;
		else
			cur_num++;
	}

	// 一次间接
	const short int first_Indir = temp_inode->addr[4];
	if (first_Indir >= 0)
	{
		unsetBitmapUsed(start_data_bitmap, first_Indir, 1);
		getDataByBlkId(first_Indir, first_data_blk);
		for (int i = 0; i < first_data_blk->size; i += sizeof(short int))
		{
			const short int addr = retShortIntFromData(first_data_blk->data, i);

			if (addr >= 0)
				unsetBitmapUsed(start_data_bitmap, addr, 1);
			else
				goto error;

			if (cur_num == blk_num)
				goto error;
			else
				cur_num++;
		}
	}
	else
		goto error;

	// 二次间接
	const short int second_Indir = temp_inode->addr[5];
	if (second_Indir >= 0)
	{
		unsetBitmapUsed(start_data_bitmap, second_Indir, 1);
		getDataByBlkId(second_Indir, second_data_blk);
		for (int i = 0; i < second_data_blk->size; i += sizeof(short int))
		{
			const short int first_Indir = retShortIntFromData(second_data_blk->data, i);
			unsetBitmapUsed(start_data_bitmap, first_Indir, 1);
			getDataByBlkId(first_Indir, first_data_blk);
			for (int i = 0; i < first_data_blk->size; i += sizeof(short int))
			{
				const short int addr = retShortIntFromData(first_data_blk->data, i);
				if (addr >= 0)
					unsetBitmapUsed(start_data_bitmap, addr, 1);
				else
					goto error;

				if (cur_num == blk_num)
					goto error;
				else
					cur_num++;
			}
		}
	}
	else
		goto error;

	// 三次间接
	short int third_Indir = temp_inode->addr[6];
	if (third_Indir >= 0)
	{
		unsetBitmapUsed(start_data_bitmap, third_Indir, 1);
		getDataByBlkId(third_Indir, third_data_blk);
		for (int i = 0; i < third_data_blk->size; i += sizeof(short int))
		{
			const short int second_Indir = retShortIntFromData(third_data_blk->data, i);
			unsetBitmapUsed(start_data_bitmap, second_Indir, 1);
			getDataByBlkId(retShortIntFromData(third_data_blk->data, i), second_data_blk);
			for (int i = 0; i < second_data_blk->size; i += sizeof(short int))
			{
				const short int first_Indir = retShortIntFromData(second_data_blk->data, i);
				unsetBitmapUsed(start_data_bitmap, first_Indir, 1);
				getDataByBlkId(first_Indir, first_data_blk);
				for (int i = 0; i < first_data_blk->size; i += sizeof(short int))
				{
					const short int addr = retShortIntFromData(first_data_blk->data, i);
					if (addr >= 0)
						unsetBitmapUsed(start_data_bitmap, addr, 1);
					else
						goto error;

					if (cur_num == blk_num)
						goto error;
					else
						cur_num++;
				}
			}
		}
	}
	else
		goto error;

	printSuccess("removeFileDataByInodeId: success");
error:
	free(first_data_blk);
	free(second_data_blk);
	free(third_data_blk);
	free(temp_inode);
	return ret;
}

// 检查某个块是否被用了 0表示没有, 1表示被用了, -1表示错误
int checkBitmapUsed(const long start_bitmap_blk, const long offset_bit)
{
	int ret = 1;
	// 读取文件
	FILE *fp = NULL;
	fp = fopen(DISK_PATH, "r+");
	if (fp == NULL)
	{
		ret = -1;
		printError("setBitmapUsed: Open disk file failed! The file may don't exits.");
		printf("disk_path: %s\n", DISK_PATH);
		goto error;
	}

	const long offset_temp_byte = offset_bit / 8;
	const long offset_temp_bit = offset_bit % 8;

	// 移动指针到文件的DataBitmap块
	if (fseek(fp, FS_BLOCK_SIZE * start_bitmap_blk + offset_temp_byte, SEEK_SET) != 0)
	{
		ret = -1;
		printError("setBitmapUsed: DataBitmap fseek failed!");
		goto error;
	}

	unsigned char temp_byte = 0x00;
	fread(&temp_byte, sizeof(unsigned char), 1, fp);

	unsigned char mask = 0x80;
	mask >>= offset_temp_bit;
	// 对应位为0
	if (temp_byte & mask != mask)
	{
		ret = 0;
	}
error:
	fclose(fp);
	return ret;
}

// 根据哈希值, 删除文件
/*
要删除的东西:
1. 对应文件的inode_bitmap的bit置0
2. 对应文件的inode所有的addr用的data块对应的data_bitmap的bit置0
3. 在目录的对应的file_dir位置中写个空的file_dir进去
4. 回收目录块
*/
int removeFileByHash(const int hash_num, const int menu_cur_i)
{
	int ret = 0;
	const int start_inodebitmap_blk = SUPER_BLOCK;
	const int start_databitmap_blk = SUPER_BLOCK + INODE_BITMAP;
	struct GInode *menu_inode = (struct GInode *)malloc(sizeof(struct GInode));
	struct GInode *temp_inode = (struct GInode *)malloc(sizeof(struct GInode));
	struct GFileDir *file_dir = (struct GFileDir *)malloc(sizeof(struct GFileDir));
	// 获取当前inode号的inode
	getInodeByInodeId(menu_cur_i, menu_inode);
	struct GDataBlock *first_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *second_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *third_data_blk = malloc(sizeof(struct GDataBlock));
	struct GDataBlock *fourth_data_blk = malloc(sizeof(struct GDataBlock));
	// struct GDataBlock *data_blk = malloc(sizeof(struct GDataBlock));

	if (0 <= hash_num && hash_num < FD_ZEROTH_INDIR)
	{
		int i = hash_num / FD_PER_BLK;
		short int *first_addr = &menu_inode->addr[i];
		int offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);

		// 获取地址和data blk
		getDataByBlkId(*first_addr, first_data_blk);

		// 更新menu inode
		rmUpdateMenuInode(menu_cur_i, menu_inode);

		// 回收inode
		getFileDirFromDataBlk(first_data_blk, offset, file_dir);
		const short int inode_id = file_dir->nInodeBlock;
		unsetBitmapUsed(start_inodebitmap_blk, inode_id, 1);
		// 回收inode中的data块
		removeFileDataByInodeId(inode_id);

		// 从data blk中删除file dir
		removeFileDirFromDataBlk(offset, first_data_blk);
		// 将data blk写入到文件
		writeDataByBlkId(*first_addr, first_data_blk);

		// 回收块
		if (first_data_blk->size <= 0)
		{
			unsetBitmapUsed(start_databitmap_blk, *first_addr, 1);
			*first_addr = -1;
		}
	}
	else if (hash_num < FD_FIRST_INDIR)
	{
		// 一次间接块  4
		short int *first_addr = &menu_inode->addr[4];
		// 获取地址和data blk
		getDataByBlkId(*first_addr, first_data_blk);
		// 更新menu inode
		rmUpdateMenuInode(menu_cur_i, menu_inode);

		// 计算偏移
		int offset = (((hash_num - FD_ZEROTH_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
		// 根据偏移计算出第一块间接索引块的地址
		const short int second_addr = retShortIntFromData(first_data_blk->data, offset);
		// 获取第一块间接索引块指向的data  blk
		getDataByBlkId(second_addr, second_data_blk);

		// 重新计算偏移
		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);

		// 回收最终指向的文件的inode
		getFileDirFromDataBlk(second_data_blk, offset, file_dir);
		const short int inode_id = file_dir->nInodeBlock;
		unsetBitmapUsed(start_inodebitmap_blk, inode_id, 1);
		// 回收inode中的data块
		removeFileDataByInodeId(inode_id);

		// 从data blk中删除file dir
		removeFileDirFromDataBlk(offset, second_data_blk);
		// 将data blk写回到文件
		writeDataByBlkId(second_addr, second_data_blk);

		// 回收size为零的间接索引块和数据块
		// 回收数据块
		if (second_data_blk->size <= 0)
		{
			first_data_blk->size -= sizeof(short int);
			unsetBitmapUsed(start_databitmap_blk, second_addr, 1);
		}

		if (first_data_blk->size <= 0)
		{
			unsetBitmapUsed(start_databitmap_blk, *first_addr, 1);
			// 将菜单上面的地址写回去
			*first_addr = -1;
		}
	}
	else if (hash_num < FD_SECOND_INDIR)
	{
		// 二次间接块  5
		short int *first_addr = &menu_inode->addr[5];
		// 获取地址和data blk
		getDataByBlkId(*first_addr, first_data_blk);
		// 更新menu inode
		rmUpdateMenuInode(menu_cur_i, menu_inode);

		int offset = (((hash_num - FD_FIRST_INDIR) / (ADDR_PER_BLK * FD_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		// 根据偏移计算出第一块间接索引块的地址
		const short int second_addr = retShortIntFromData(first_data_blk->data, offset);
		// 获取第一块间接索引块指向的data  blk
		getDataByBlkId(second_addr, second_data_blk);

		offset = (((hash_num - FD_FIRST_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
		// 根据偏移计算出第一块间接索引块的地址
		const short int third_addr = retShortIntFromData(second_data_blk->data, offset);
		// 获取第一块间接索引块指向的data  blk
		getDataByBlkId(third_addr, third_data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		// 回收最终指向的文件的inode
		getFileDirFromDataBlk(third_data_blk, offset, file_dir);
		const short int inode_id = file_dir->nInodeBlock;
		unsetBitmapUsed(start_inodebitmap_blk, inode_id, 1);
		// 回收inode中的data块
		removeFileDataByInodeId(inode_id);

		// 从data blk中删除file dir
		removeFileDirFromDataBlk(offset, third_data_blk);
		// 将data blk写回到文件
		writeDataByBlkId(third_addr, third_data_blk);

		// 回收size为零的间接索引块和数据块
		// 回收数据块
		if (third_data_blk->size <= 0)
		{
			second_data_blk->size -= sizeof(short int);
			unsetBitmapUsed(start_databitmap_blk, third_addr, 1);
		}

		if (second_data_blk->size <= 0)
		{
			first_data_blk->size -= sizeof(short int);
			unsetBitmapUsed(start_databitmap_blk, second_addr, 1);
		}

		if (first_data_blk->size <= 0)
		{
			unsetBitmapUsed(start_databitmap_blk, *first_addr, 1);
			// 将菜单上面的地址写回去
			*first_addr = -1;
		}
	}
	else if (hash_num < MAX_HASH_SIZE)
	{
		// 三次间接块  6
		short int *first_addr = &menu_inode->addr[6];
		// 获取地址和data blk
		// 获取地址和data blk
		getDataByBlkId(*first_addr, first_data_blk);
		// 更新menu inode
		rmUpdateMenuInode(menu_cur_i, menu_inode);

		int offset = (((hash_num - FD_SECOND_INDIR) / (ADDR_PER_BLK * ADDR_PER_BLK * FD_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		// 根据偏移计算出第一块间接索引块的地址
		const short int second_addr = retShortIntFromData(first_data_blk->data, offset);
		// 获取第一块间接索引块指向的data  blk
		getDataByBlkId(second_addr, second_data_blk);

		offset = (((hash_num - FD_SECOND_INDIR) / (FD_PER_BLK * ADDR_PER_BLK)) % ADDR_PER_BLK) * sizeof(short int);
		// 根据偏移计算出第一块间接索引块的地址
		const short int third_addr = retShortIntFromData(second_data_blk->data, offset);
		// 获取第一块间接索引块指向的data  blk
		getDataByBlkId(third_addr, third_data_blk);

		offset = (((hash_num - FD_SECOND_INDIR) / FD_PER_BLK) % ADDR_PER_BLK) * sizeof(short int);
		// 根据偏移计算出第一块间接索引块的地址
		const short int fourth_addr = retShortIntFromData(second_data_blk->data, offset);
		// 获取第一块间接索引块指向的data  blk
		getDataByBlkId(fourth_addr, fourth_data_blk);

		offset = (hash_num % FD_PER_BLK) * sizeof(struct GFileDir);
		// 回收最终指向的文件的inode
		getFileDirFromDataBlk(fourth_data_blk, offset, file_dir);
		const short int inode_id = file_dir->nInodeBlock;
		unsetBitmapUsed(start_inodebitmap_blk, inode_id, 1);
		// 回收inode中的data块
		removeFileDataByInodeId(inode_id);

		// 从data blk中删除file dir
		removeFileDirFromDataBlk(offset, fourth_data_blk);
		// 将data blk写回到文件
		writeDataByBlkId(fourth_addr, fourth_data_blk);

		// 回收size为零的间接索引块和数据块
		// 回收数据块
		if (fourth_data_blk->size <= 0)
		{
			third_data_blk->size -= sizeof(short int);
			unsetBitmapUsed(start_databitmap_blk, fourth_addr, 1);
		}

		if (third_data_blk->size <= 0)
		{
			second_data_blk->size -= sizeof(short int);
			unsetBitmapUsed(start_databitmap_blk, third_addr, 1);
		}

		if (second_data_blk->size <= 0)
		{
			first_data_blk->size -= sizeof(short int);
			unsetBitmapUsed(start_databitmap_blk, second_addr, 1);
		}

		if (first_data_blk->size <= 0)
		{
			unsetBitmapUsed(start_databitmap_blk, *first_addr, 1);
			// 将菜单上面的地址写回去
			*first_addr = -1;
		}
	}
	else
	{
		ret = -1;
		printf("removeFileByHash: the file may be not exited.\n");
		goto error;
	}

error:
	free(temp_inode);
	free(menu_inode);
	free(file_dir);
	free(first_data_blk);
	free(second_data_blk);
	free(third_data_blk);
	free(fourth_data_blk);
	return ret;
}
