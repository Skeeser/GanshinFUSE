/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#ifndef __GFSFCTIONAL_H__
#define __GFSFCTIONAL_H__

#include "Config.h"
#include "Utils.h"
#include "FileOper.h"

void *GFS_init(struct fuse_conn_info *conn, struct fuse_config *cfg);
int GFS_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi);
int GFS_mknod(const char *path, mode_t mode, dev_t dev);
int GFS_unlink(const char *path);

#endif