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

int readDataByBlkId(long blk_id,struct GDataBlock *data_blk);

int getFileDirToAttr(const char * path,struct GFileDir *attr);

#endif