#include "disk_usage.h"

void calcFile(struct stat *stat_entry,long int *size,char* path)
{
    if (!mods.bytes) 
    {
        *size += (long int)(stat_entry->st_blocks/(double)(mods.block_size/512));
        if (mods.all) printf("%ld\t%s\n",(long int)(stat_entry->st_blocks/(double)(mods.block_size/512)),path);
    }
    else
    {
        *size += stat_entry->st_size;
        if (mods.all) printf("%ld,\t%s\n",stat_entry->st_size,path);
    }
}

void calcDir(char* path)
{
    struct stat stat_entry;
    if (stat(path,&stat_entry) < 0)
    {
        write(STDERR_FILENO,"Couldn't get entry statistics.\n",31);
        exit(1);
    }
    long int size = 0;
    if (S_ISREG(stat_entry.st_mode))
    {
        
    }
    else
    {
        
        DIR* dirp;

        if ((dirp = opendir(path)) == NULL) 
        {
            write(STDERR_FILENO,"Couldn't open directory.\n",25);
            exit(1);
        }

        struct dirent *dentry;
        while ((dentry = readdir(dirp)) != NULL)
        {
            stat(dentry->d_name, &stat_entry);
            size += (long int)(stat_entry.st_blocks/(double)(mods.block_size/512));
            if (S_ISDIR(stat_entry.st_mode))
            {
                pid_t pid;
                pid = fork();
                if (pid == 0)
                {
                    calcDir(dentry->d_name);
                    exit(0);
                }
                else if (pid < 0)
                {
                    write(STDERR_FILENO,"Couldn't create child process.\n",31);
                    exit(1);
                }
                else
                {
                    int status;
                    waitpid(pid,&status,WNOHANG);
                } 
            }
            else
            {
                calcFile(&stat_entry,&size,)
            }
            
        }
        printf("%ld\t%s\n",size,path);
    }
    
}