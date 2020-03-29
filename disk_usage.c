#include "disk_usage.h"

long int calcFile(struct stat *stat_entry)
{
    if (!mods.bytes) 
    {
        return (long int)(stat_entry->st_blocks/(double)(mods.block_size/512));
    }
    else
    {
        return (long int) stat_entry->st_size;
    }
}

long int calcDir(char* path,int depth)
{
    struct stat stat_entry;
    if (lstat(path,&stat_entry) < 0)
    {
        write(STDERR_FILENO,"Couldn't get entry statistics.\n",31);
        exit(1);
    }

    if (S_ISREG(stat_entry.st_mode))
    {
        printf("%ld\t%s\n",calcFile(&stat_entry),path);
    }
    else
    {
        long int dirSize = 0;

        DIR* dirp;
        if ((dirp = opendir(path)) == NULL) 
        {
            write(STDERR_FILENO,"Couldn't open directory.\n",25);
            exit(1);
        }

        struct dirent *dentry;
        while ((dentry = readdir(dirp)) != NULL)
        {
            if (strcmp(dentry->d_name,".") == 0 || strcmp(dentry->d_name,"..") == 0) 
            {
                if (mods.bytes) dirSize += 2048 * (1024/mods.block_size);
                else dirSize += 2 * (1024/mods.block_size);
                continue;
            }
            char full_path[1000];
            strcpy(full_path,path);
            sprintf(full_path,"%s/%s",path,dentry->d_name);
            if (lstat(full_path, &stat_entry) < 0)
            {
                write(STDERR_FILENO,"Couldn't get entry statistics 2.\n",33);
                exit(1);
            }

            if (S_ISREG(stat_entry.st_mode))
            {
                long int currentFileSize = calcFile(&stat_entry);
                dirSize += currentFileSize;
                if (mods.all && ((mods.max_depth != 0 && depth < mods.max_depth) || mods.max_depth == 0)) printf("%ld\t%s\n",currentFileSize,full_path);
            }
            else if (S_ISLNK(stat_entry.st_mode) && !mods.dereference) continue;
            else
            {
                int fd[2];
                if (pipe(fd) < 0) 
                {
                    write(STDERR_FILENO,"Couldn't create pipe.\n",22);
                    exit(1);
                }

                pid_t pid = fork();

                if (pid == 0)
                {
                    close(fd[READ]);
                    long int currentDirSize = calcDir(full_path,++depth);
                    if (currentDirSize == -1) exit(2);
                    if (write(fd[WRITE],&currentDirSize,sizeof(currentDirSize)) < 0)
                    {
                        write(STDERR_FILENO,"Couldn't write to pipe.\n",24);
                        exit(1);
                    }
                    close(fd[WRITE]);
                    exit(0);
                }
                else if (pid > 0)
                {
                    close(fd[WRITE]);
                    long int currentDirSize_parent;
                    if (read(fd[READ],&currentDirSize_parent,sizeof(currentDirSize_parent)) < 0)
                    {
                        write(STDERR_FILENO,"Couldn't read from pipe.\n",25);
                        exit(1);
                    }
                    close(fd[READ]);
                    if (!mods.separate_dirs) dirSize += currentDirSize_parent;
                    int status;
                    waitpid(pid,&status,WNOHANG);
                    if (WEXITSTATUS(status) == 1) return -1;
                }
                else 
                {
                    write(STDERR_FILENO,"Couldn't create child process.\n",31);
                    exit(1);
                }
                strcpy(full_path,path);
            }
        }
        if ((mods.max_depth != 0 && depth <= mods.max_depth) || mods.max_depth == 0) printf("%ld\t%s\n",dirSize,path);
        return dirSize;
    }
    return -1;
}