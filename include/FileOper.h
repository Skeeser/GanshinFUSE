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
// superblock
int getSuperBlock(struct GSuperBlock *sp_blk);

// path
int checkFilePath(const char *path);

// Data
int getDataByBlkId(const short int blk_id, struct GDataBlock *data_blk);
int writeDataByBlkId(const short int blk_id, const struct GDataBlock *data_blk);
int getFreeDataBlk(const int need_num, short int *start_blk);
void getAddrDataDirectIndex(short int *addr, struct GDataBlock *data_blk);
void getAddrDataIndirectIndex(const int offset, short int *addr, struct GDataBlock *data_blk);
void initShortIntToData(char *data);
int writeShortIntToData(const short int addr, const int offset, char *data);

// Inode
int getInodeByBlkId(const short int blk_id, struct GInode *inode_blk);
int writeInodeByBlkId(const short int blk_id, const struct GInode *inode_blk);
int getInodeBlkByHash(const int hash_num, const int cur_i, int *target_i);
int getInodeBlkByPath(const char *path, short int *file_inode);
int getFreeInodeBlk(const int need_num, short int *start_blk);
int initInode(struct GInode *inode);
int updateMenuInode(const short int cur_i, struct GInode *menu_inode);

// Bitmap
int setBitmapUsed(const long start_bitmap_blk, const long offset_bit, const int num);

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