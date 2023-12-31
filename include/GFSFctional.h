/*
Author: keeser
Email: 1326658417@qq.com
Liense: MPL 2.0
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
int GFS_open(const char *path, struct fuse_file_info *fi);
int GFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int GFS_release(const char *path, struct fuse_file_info *fi);
int GFS_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int GFS_mkdir(const char *path, mode_t mode);
int GFS_rmdir(const char *path);
int GFS_access(const char *path, int flag);
int GFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);
int GFS_utimes(const char *path, const struct timespec tv[2], struct fuse_file_info *fi);

#endif