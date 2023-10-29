/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FUSE_USE_VERSION 31
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fuse3/fuse.h>


// logo
// 存储ASCII艺术文本的字符串
const char *const LOGO = 
        "\033[35m\n"
        "   _____                 _     _       ______ _____ \n"
        "  / ____|               | |   (_)     |  ____/ ____|\n"
        " | |  __  __ _ _ __  ___| |__  _ _ __ | |__ | (___  \n"
        " | | |_ |/ _` | '_ \\/ __| '_ \\| | '_ \\|  __| \\___ \\ \n"
        " | |__| | (_| | | | \\__ \\ | | | | | | | |    ____) |\n"
        "  \\_____|\\__,_|_| |_|___/_| |_|_|_| |_|_|   |_____/ \n"
        "\033[0m\n";


// 用作文件系统的磁盘文件
const char *const DISK_PATH = "../out/diskimg";
// 一些文件系统的设置, 不用#define是因为define的名称编译器看不见
const int FS_BLOCK_SIZE = 512;  // Byte
const int SUPER_BLOCK = 1;  // Block
const int INODE_BITMAP = 1;  // Block
const int DATA_BITMAP = 4;  // Block
const int INODE_BLOCK = 512;  // Block
const int INODE_SIZE = 64;  // Byte

const int MAX_FILE_NUM = FS_BLOCK_SIZE * INODE_BLOCK / INODE_SIZE;
const int DISK_SIZE = 8 * 1024 * 1024; // 8MB Byte
const int TOTAL_BLOCK_NUM = DISK_SIZE / FS_BLOCK_SIZE;

// const int MAX_DATA_IN_BLOCK = 504; //size_t和long nNextBlock各占4byte
const int MAX_DIR_IN_BLOCK = 8;
const int MAX_FILENAME = 8;
const int MAX_EXTENSION = 3;
const long FILE_SIZE = 4096;  // Byte

// 超级块结构体
// 9 * 8 = 72 Byte
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
// 磁盘地址有7个, addr[0]-addr[3]是直接地址，addr[4]是一次间接，
// addr[5]是二次间接，addr[6]是三次间接。
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


// 记录文件信息的数据结构, 统一存放在目录文件里面
struct GFileData {
    char fname[MAX_FILENAME + 1]; //文件名 (plus space for nul)
    char fext[MAX_EXTENSION + 1]; //扩展名 (plus space for nul)
    size_t fsize; //文件大小（file size）
    long nStartBlock; //目录开始块位置（where the first block is on disk）
    int flag; //indicate type of file. 0:for unused; 1:for file; 2:for directory
};

// //文件内容存放用到的数据结构，大小为 512 bytes，占用1块磁盘块
// struct data_block {
//     size_t size; //文件的数据部分使用了这个块里面的多少Bytes
//     long nNextBlock; //（该文件太大了，一块装不下，所以要有下一块的地址）   long的大小为4Byte
//     char data[MAX_DATA_IN_BLOCK];// And all the rest of the space in the block can be used for actual data storage.
// };


#endif