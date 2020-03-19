#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

struct mods
{
    int all;
    int bytes;
    int block_size;
    int count_links;
    int dereference;
    int separate_dirs;
    int max_depth
}mods;

int main(int argc,char* argv[])
{
    if (argc < 2) fprintf(stderr,"Usage: simpledu -l [pathname] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n");

    mods.count_links = 1;

    for (int i = 0;argv[i] != NULL;i++)
    {
        if (strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--all") == 0) mods.all = 1;
        if (strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--bytes") == 0) mods.bytes = 1;
        if (strcmp(argv[i],"-B") == 0)
        {
            i++;
            mods.block_size = atoi(argv[i]);
        }
        if (strstr(argv[i],"--block-size=") != NULL)
        {
            char* eq_pos = strstr(argv[i],"=");
            char* num_pos = eq_pos + sizeof(char);
            mods.block_size = atoi(num_pos);
        }
    }
}