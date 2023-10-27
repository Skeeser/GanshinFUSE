#include "../include/GanshinInit.h"



// 将文件系统的相关信息写入超级块
static void initSuperBlock(const FILE *fp)
{
    struct GSuperBlock *const super_blk = malloc(sizeof(struct GSuperBlock));//动态内存分配，申请super_blk
    super_blk->fs_size = DISK_SIZE / FS_BLOCK_SIZE;
    // 从0开始, 超级块数 + InodeBitMap块数 + DataBitMap块数 + Inode块数
    super_blk->first_blk = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP + INODE_BLOCK;
    super_blk->datasize = DATA_BITMAP * FS_BLOCK_SIZE * 8;
    super_blk->first_inode = SUPER_BLOCK + INODE_BITMAP + DATA_BITMAP;
    super_blk->inode_area_size = 
    printSuccess("Init super block success.");
    free(super_blk);
} 



int main(int argc, char *argv[])
{
    // 打印LOGO
    printf("%s", LOGO);

    // 读取文件
    FILE *fp = NULL;
    fp = fopen(DISK_PATH, "r+");//打开文件
    if(fp == NULL){
        printError("Open fisk file failed! The file may don't exits.");
        return 0;
    }
    printSuccess("Open fisk file success!");
    
    // 初始化超级块


    fclose(fp);
    return 0;
}