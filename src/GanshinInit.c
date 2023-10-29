#include "../include/GanshinInit.h"



// 将文件系统的相关信息写入超级块
static void initSuperBlock(FILE *const fp)
{
    //动态内存分配，申请super_blk
    struct GSuperBlock *const super_blk = malloc(sizeof(struct GSuperBlock));
    super_blk->fs_size = DISK_SIZE / FS_BLOCK_SIZE;
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

// 初始化Bitmap
static void initBitmap(FILE *const fp)
{
    // 移动指针到文件的第二块
    if (fseek(fp, FS_BLOCK_SIZE * 1, SEEK_SET) == 0){
        printSuccess("Bitmap fseek success!");
    }else{
        printError("Bitmap fseek failed!");
    }

    // Bitmap 总大小 (INODE_BITMAP + DATA_BITMAP) * FS_BLOCK_SIZE * 8  bit
    int a[40];//刚好大小为1280bit，可以用来初始化bitmap_block的前1280bit
    memset(a,-1,sizeof(a));
    fwrite(a,sizeof(a),1,fp);
    
    //然后我们还有2bit需要置1
    int b=0;
    int mask = 1;
	mask <<= 30;    //1281
	b |= mask;
	mask <<= 1;     //1280
	b |= mask;
    fwrite(&b, sizeof(int), 1, fp);
        //接着是包含这1282bit的块剩余的部分置0
    int c[87];
    memset(c,0,sizeof(c));
    fwrite(c,sizeof(c),1,fp);
        //最后要将bitmap剩余的1279块全部置0，用int数组设置，一个int为4byte
    int rest_of_bitmap=(1279*512)/4;
    int d[rest_of_bitmap];
    memset(d,0,sizeof(d));
    fwrite(d,sizeof(d),1,fp);
     printf("initial bitmap_block success!\n");
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
        return 0;
    }
    printSuccess("Open disk file success!");
    
    // 初始化超级块
    initSuperBlock(fp);

    // 初始化Bitmap

    fclose(fp);
    return 0;
}