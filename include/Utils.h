/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/

#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#endif