#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
    if (argc < 2) fprintf(stderr,"Usage: simpledu -l [pathname] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n");

    DIR* dirp;

    long int size = 0;

    if ((dirp = opendir(argv[1])) == NULL)
    {
        perror(argv[1]);
        exit(1);
    } 

    struct dirent *entry;
    struct stat stat_buf;

    while ((entry = readdir(dirp)) != NULL)
    {
        if (stat(entry->d_name,&stat_buf) < 0)
        {
            fprintf(stderr,"Couldn't get entry stats.\n");
            exit(2);
        }
        if (S_ISDIR(stat_buf.st_mode)) printf("%-25ld%12s\n",size,entry->d_name);
        size += stat_buf.st_blocks/2;
        
    }
    printf("Total: %ld\n",size);
    exit(0);
}