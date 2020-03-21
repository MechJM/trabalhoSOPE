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

#define STR_LEN 50

//Pipe macros
#define READ  0
#define WRITE 1

long int start; //Start time in milisseconds

struct mods
{
    int all;
    int bytes;
    int block_size;
    int count_links;
    int dereference;
    int separate_dirs;
    int max_depth;
}mods;


char path[STR_LEN];


#endif