#ifndef GLOBAL_H
#define GLOBAL_H

#define STR_LEN 50
//Pipe macros
#define READ  0
#define WRITE 1

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

char log_filename[STR_LEN] = "log_file";
char path[STR_LEN];


#endif