
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>
#include <fuse3/fuse.h>



// 用作文件系统的磁盘文件
const char *const DISK_PATH = "../out/diskimg";



// 超级块结构体
struct GSuperBlock
{
    long fs_size;                  // 文件系统的大小，以块为单位
    long first_blk;                // 数据区的第一块块号，根目录也放在此
    long datasize;                 // 数据区大小，以块为单位
    long first_inode;              // inode区起始块号
    long inode_area_size;          // inode区大小，以块为单位
    long fisrt_blk_of_inodebitmap; // inode位图区起始块号
    long inodebitmap_size;         // inode位图区大小，以块为单位
    long first_blk_of_databitmap;  // 数据块位图起始块号
    long databitmap_size;          // 数据块位图大小，以块为单位
};

// Inode结构体
struct GInode
{
    short int st_mode;       /* 权限，2字节 */
    short int st_ino;        /* i-node号，2字节 */
    char st_nlink;           /* 连接数，1字节 */
    uid_t st_uid;            /* 拥有者的用户 ID ，4字节 */
    gid_t st_gid;            /* 拥有者的组 ID，4字节  */
    off_t st_size;           /*文件大小，4字节 */
    struct timespec st_atim; /* 16个字节time of last access */
    short int addr[7];       /*磁盘地址，14字节*/
};

#endif