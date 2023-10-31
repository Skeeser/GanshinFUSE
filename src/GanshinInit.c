/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "../include/GanshinInit.h"

// 将文件系统的相关信息写入超级块
static void initSuperBlock(FILE *const fp)
{
    //动态内存分配，申请super_blk
    struct GSuperBlock *const super_blk = malloc(sizeof(struct GSuperBlock));
    super_blk->fs_size = TOTAL_BLOCK_NUM;
    // 从0开始, 超级块数 + InodeBitMap块数 + DataBitMap块数 + Inode块数
    super_blk->first_blk = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP + INODE_BLOCK;
    super_blk->datasize = DATA_BITMAP * FS_BLOCK_SIZE * 8;
    super_blk->first_inode = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP;
    super_blk->inode_area_size = INODE_BLOCK;
    super_blk->fisrt_blk_of_inodebitmap = SUPER_BLOCK;
    super_blk->inodebitmap_size = INODE_BITMAP;
    super_blk->first_blk_of_databitmap = SUPER_BLOCK + INODE_BITMAP;
    super_blk->databitmap_size = DATA_BITMAP;
    if(fwrite(super_blk, sizeof(struct GSuperBlock), 1, fp) == 0){
        printSuccess("Init super block success!");
    }else{
        printError("Init super block failed!");
    }
    free(super_blk);
} 

// Bitmap 总大小 (INODE_BITMAP + DATA_BITMAP) * FS_BLOCK_SIZE * 8  bit
// 初始化InodeBitmap
static void initInodeBitmap(FILE *const fp)
{
    // 移动指针到文件的InodeBitmap块
    if (fseek(fp, FS_BLOCK_SIZE * (SUPER_BLOCK), SEEK_SET) == 0){
        printSuccess("InodeBitmap fseek success!");
    }else{
        printError("InodeBitmap fseek failed!");
    }

    /* 
    需要把InodeBitmap的初始化的对应块
    1. 根目录

    需要把第一个Byte的第一个bit置1
    */
    const int inode_bitmap_bit = INODE_BITMAP * FS_BLOCK_SIZE * 8;
  
    // 第一个Byte的第一个bit置1
    int temp_data=0;
    int mask = 1;
	mask <<= 31;  
    temp_data |= mask;

    fwrite(&temp_data, sizeof(int), 1, fp);
    
    //接着是剩余的部分置0, 
    int rest_of_bitmap= (inode_bitmap_bit - 32) / 32;
    int rest_data[rest_of_bitmap];
    memset(rest_data, 0, sizeof(rest_data));
    fwrite(rest_data, sizeof(rest_data), 1, fp);
    
    printSuccess("Initial InodeBitmap success!");
}

// 初始化DataBitmap
static void initDataBitmap(FILE *const fp)
{
    // 移动指针到文件的DataBitmap块
    if (fseek(fp, FS_BLOCK_SIZE * (SUPER_BLOCK + INODE_BITMAP), SEEK_SET) == 0){
        printSuccess("DataBitmap fseek success!");
    }else{
        printError("DataBitmap fseek failed!");
    }

    /* 
    需要把DataBitmap的初始化的对应块
    1. SuperBlock  1
    2. InodeBitmap  1
    3. DataBitmap  4 
    4. 根目录   1
    */
    const int data_bitmap_bit = DATA_BITMAP * FS_BLOCK_SIZE * 8;
    const int used_block = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP + 1;

    const int int_block_num = used_block / 32;
    if(int_block_num){
        int a[int_block_num];
        memset(a,0,sizeof(a));
        fwrite(a,sizeof(a),1,fp);
    }
    
    int temp_data=0;
    const int rest_used_block = used_block % 32;
    for(int i = 0 ; i < rest_used_block; i++){
        int mask = 1;
	    mask <<= (31 - i);    //1281
	    temp_data |= mask;
    }
    fwrite(&temp_data, sizeof(int), 1, fp);
    
    //接着是剩余的部分置0, 
    const int rest_of_bitmap= (data_bitmap_bit - int_block_num * 32 - 32) / 32;
    int rest_data[rest_of_bitmap];
    memset(rest_data, 0, sizeof(rest_data));
    fwrite(rest_data, sizeof(rest_data), 1, fp);

    printSuccess("Initial DataBitmap success!");
}

// 初始化Inode
static void initInode(FILE *const fp)
{
    // 移动指针到文件的Inode块
    if (fseek(fp, FS_BLOCK_SIZE * (SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP), SEEK_SET) == 0){
        printSuccess("Inode fseek success!");
    }else{
        printError("Inode fseek failed!");
    }

    struct GInode *const root = malloc(sizeof(struct GInode));
    // 为根目录的 struct GInode 对象赋值
    root->st_mode = 0755; // 权限，例如 rwxr-xr-x
    root->st_ino = 0;     // i-node号为0
    root->st_nlink = 1;   // 连接数，通常为1
    root->st_uid = 0;     // 根目录的用户 ID，通常为0（超级用户）
    root->st_gid = 0;     // 根目录的组 ID，通常为0（超级用户组）
    root->st_size = FILE_SIZE; // 文件大小，为4KB
    // 设置 st_atim 和其他字段
    // struct timespec last_access_time;
    // // 使用 clock_gettime 函数获取当前时间戳
    // if (clock_gettime(0, &last_access_time) == 0) {
    //     printSuccess("Inode get time success!");
    // } else {
    //     printError("Inode get time failed!");
    // }
    // root->st_atim = last_access_time;
    time_t currentTime;
    currentTime = time(NULL); // 获取当前时间

    if (currentTime == (time_t)-1) {
        printError("Inode get time failed!");
    }else {
        printSuccess("Inode get time success!");
    }
    root->st_atim = currentTime;

    // 设置磁盘地址
    // 磁盘地址有7个, addr[0]-addr[3]是直接地址，addr[4]是一次间接，
    // addr[5]是二次间接，addr[6]是三次间接。
    memset(root->addr, -1, sizeof(root->addr));
     const int data_first_block = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP + INODE_BLOCK;
    // 根目录指向data区的一个块地址
    root->addr[0] = data_first_block;
    fwrite(root, sizeof(struct GInode), 1, fp); //写入磁盘，初始化完成
    free(root);
    printSuccess("Initial Inode success!");
}

int main(int argc, char *argv[])
{
    // 打印LOGO
    printf("%s", LOGO);

    // 读取文件
    FILE *fp = NULL;
    fp = fopen(DISK_PATH, "r+");//打开文件
    if(fp == NULL){
        printError("Open disk file failed! The file may don't exits.");
        printf("disk_path: %s", DISK_PATH);
        return 0;
    }
    printSuccess("Open disk file success!");
    
    // 初始化InodeBitmap
    initInodeBitmap(fp);

    // 初始化DataBitmap
    initDataBitmap(fp);

    // 初始化Inode
    initInode(fp);

    // 释放fp
    fclose(fp);

    // 开香槟
    printSuccess("Super_Bitmap_Inode_Data_Blocks init success!");
    return 0;
}