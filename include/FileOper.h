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

int readDataByBlkId(long blk_id,struct GDataBlock *data_blk);

int getInodeBlkByPath(const char * path, long *file_inode);
int getFileDirByPath(const char * path,struct GFileDir *attr);

void fillStatByInode(struct GInode *inode, struct stat *st);


short int retShortIntFromData(const char* data, int offset);
void getFileBlkNum(struct GInode *inode, int *blk_num);

#endif