/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#ifndef __FILEOPER_H__
#define __FILEOPER_H__

#include "Utils.h"
#include "Config.h"

// path
int checkFilePath(const char * path);

int readDataByBlkId(short int blk_id,struct GDataBlock *data_blk);
int readInodeByBlkId(short int blk_id,struct GInode *inode_blk);

int getInodeBlkByHash(const int hash_num, const int cur_i, int *target_i);
int getFileDirByHash(const int hash_num, const int cur_i, int *target_i, struct GFileDir * p_filedir);


int getInodeBlkByPath(const char * path, long *file_inode);
int getFileDirByPath(const char * path,struct GFileDir *attr);

void fillStatByInode(struct GInode *inode, struct stat *st);


short int retShortIntFromData(const char* data,const int offset);
void getFileDirFromData(const char* data,const int offset, struct GFileDir * p_fd);

void getFileBlkNum(struct GInode *inode, int *blk_num);

#endif