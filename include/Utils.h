#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *const RED = "\033[31m";
const char *const GREEN = "\033[32m";
const char *const BLACK = "\033[0m";

static void printError(const char *str)
{
    char *print_str = (char *)malloc(strlen(RED) + strlen(str) + strlen(BLACK));
    strcpy(print_str, RED);
    strcat(print_str, str);
    strcat(print_str, BLACK);
    printf("%s\n", print_str);
}

static void printSuccess(const char *str)
{
    char *print_str = (char *)malloc(strlen(GREEN) + strlen(str) + strlen(BLACK));
    strcpy(print_str, GREEN);
    strcat(print_str, str);
    strcat(print_str, BLACK);
    printf("%s\n", print_str);
}

#endif