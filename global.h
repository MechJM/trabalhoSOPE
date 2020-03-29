#ifndef GLOBAL_H
#define GLOBAL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <linux/limits.h>

#define STR_LEN 50

//Pipe macros
#define READ  0
#define WRITE 1

long int start; //Start time in milisseconds

struct mods
{
    bool all;
    bool bytes;
    double block_size;
    bool count_links;
    bool dereference;
    bool separate_dirs;
    int max_depth;
}mods;


char path[STR_LEN];


#endif