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

// 规范函数名称, 作用和返回值, 默认返回0为成功
// path
int checkFilePath(const char *path);

// Data
int getDataByBlkId(short int blk_id, struct GDataBlock *data_blk);
int getFreeDataBlk(int num, long *start_blk);

// Inode
int getInodeByBlkId(short int blk_id, struct GInode *inode_blk);
int getInodeBlkByHash(const int hash_num, const int cur_i, int *target_i);
int getInodeBlkByPath(const char *path, short int *file_inode);
int getFreeInodeBlk(int num, long *start_blk);

// Utils
void fillStatByInode(struct GInode *inode, struct stat *st);
short int retShortIntFromData(const char *data, const int offset);

// FileDir
int getFileDirByHash(const int hash_num, const int cur_i, struct GFileDir *p_filedir);
int createFileDirByHash(const int hash_num, const int cur_i, struct GFileDir *p_filedir);
int getFileDirByPath(const char *path, struct GFileDir *attr);
void getFileDirFromDataBlk(const struct GDataBlock *data_blk, const int offset, struct GFileDir *p_fd);
int writeFileDirToDataBlk(const struct GFileDir *p_fd, const int offset, struct GDataBlock *data_blk);

// File
void getFileBlkNum(struct GInode *inode, int *blk_num);
int createFileByPath(const char *path, enum GTYPE file_type);
int checkFileFname(const char *fname);
int checkFileFext(const char *fext);
int divideFileNameByPath(const char *path, char *fname, char *fext, char *fall_name, char *remain_path, enum GTYPE file_type);

#endif