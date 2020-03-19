#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"


int main(int argc,char* argv[]/*,char* envp[]*/)
{
    if (argc < 2) fprintf(stderr,"Usage: simpledu -l [pathname] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n");

    //Setting default struct values
    mods.all = 0;
    mods.count_links = 1;
    mods.block_size = 0;
    mods.count_links = 0;
    mods.dereference = 0;
    mods.separate_dirs = 0;
    mods.max_depth = 0;

    for (int i = 1; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--all") == 0) mods.all = 1;
        else if (strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--bytes") == 0) mods.bytes = 1;
        else if (strcmp(argv[i],"-B") == 0) mods.block_size = atoi(argv[++i]);
        else if (strstr(argv[i],"--block-size=") != NULL)
        {
            size_t i2;
            for (i2 = 0; i2 < strlen(argv[i]); i2++)
            {
                if (argv[i][i2] == '=') break; 
            }
            mods.block_size = atoi(&argv[i][++i2]);
        }
        else if (strcmp(argv[i],"-L") == 0 || strcmp(argv[i],"--dereference") == 0) mods.dereference = 1;
        else if (strcmp(argv[i],"-S") == 0 || strcmp(argv[i],"--separate-dirs") == 0) mods.separate_dirs = 1;
        else if (strstr(argv[i],"--max-depth=") != NULL)
        {
            size_t i2;
            for (i2 = 0; i2 < strlen(argv[i]); i2++)
            {
                if (argv[i][i2] == '=') break;
            }
            mods.max_depth = atoi(&argv[i][++i2]);
        }
        else if (strcmp(argv[i],"-l") != 0 && strcmp(argv[i],"--count-links") != 0) strcpy(path,argv[i]);
    }
}