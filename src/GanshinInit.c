#include "../include/GanshinInit.h"



// 将文件系统的相关信息写入超级块
static void initSuperBlock()
{

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
    

    
    fclose(fp);
    return 0;
}