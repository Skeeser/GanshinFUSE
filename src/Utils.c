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
