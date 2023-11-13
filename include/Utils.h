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
#include "Config.h"

void printError(const char *str);
void printSuccess(const char *str);

int hash(const char *in_str);
int getDebugByteData(FILE *fp);
int min(int a, int b);

#endif