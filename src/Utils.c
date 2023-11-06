#include "Utils.h"

const char *const RED = "\033[31m";
const char *const GREEN = "\033[32m";
const char *const BLACK = "\033[0m";

// 打印错误信息(红色)
void printError(const char *str)
{
    char *print_str = (char *)malloc(strlen(RED) + strlen(str) + strlen(BLACK));
    strcpy(print_str, RED);
    strcat(print_str, str);
    strcat(print_str, BLACK);
    printf("%s\n", print_str);
    free(print_str);
}

// 打印成功信息(绿色)
void printSuccess(const char *str)
{
    char *print_str = (char *)malloc(strlen(GREEN) + strlen(str) + strlen(BLACK));
    strcpy(print_str, GREEN);
    strcat(print_str, str);
    strcat(print_str, BLACK);
    printf("%s\n", print_str);
    free(print_str);
}

// 哈希表
/*
哈希表最大多大多大
FileDir per Block  FD_PER_BLK = 508 / 40 = 12
4+12+12*12+12*12*12=1888
*/
// const int FD_PER_BLK = MAX_DATA_IN_BLOCK / sizeof(struct GFileDir);
// const int MAX_HASH_SIZE = 4 + FD_PER_BLK + FD_PER_BLK*FD_PER_BLK + FD_PER_BLK*FD_PER_BLK*FD_PER_BLK;
// BKDRHash unsigned int seed = 131; // 31 131 1313 13131 131313 etc..

// flag取决于是文件还是目录
int hash(const char *in_str)
{
    int hash = 0;
    unsigned int seed = 31;
    for (int i = 0; in_str[i] != '\0'; i++)
    {
        hash = (hash * seed + in_str[i]) % MAX_HASH_SIZE;
    }
    return hash;
}