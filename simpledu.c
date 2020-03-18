#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
    if (argc < 2) fprintf(stderr,"Usage: simpledu [pathname]\n");

    struct stat stat_buf;
    stat(argv[1],&stat_buf);
    printf("Size of dir: %ld\n",stat_buf.st_blocks/2);
}