#include "global.h"
#include "calc_time.h"
#include "disk_usage.h"
#include "log_file.h"
#include "signal_handling.h"

int main(int argc,char* argv[])
{
    start = timestamp();




    install_handlers();
    ancestor = getpid();
    block_signal(SIGINT);
    childIndex = 0;



    strcpy(log_filename,"log_filename"); //Setting default value

    if (getenv("LOG_FILENAME") != NULL) strcpy(log_filename,getenv("LOG_FILENAME"));

    if (argc < 2) write(STDERR_FILENO,"Usage: simpledu -l [pathname] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",76);

    strcpy(arguments,"");
    sprintf(arguments+strlen(arguments),"%s",argv[0]);
    char firstEntryContent[STR_LEN] = "";
    sprintf(firstEntryContent+strlen(firstEntryContent),"%s",argv[0]);

    //Setting default struct values
    mods.all = 0;
    mods.bytes = 0;
    mods.block_size = 1024;
    mods.count_links = 1;
    mods.dereference = 0;
    mods.separate_dirs = 0;
    mods.max_depth = -1;

    for (int i = 1; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--all") == 0) {mods.all = 1; sprintf(arguments+strlen(arguments)," %s",argv[i]);}
        else if (strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--bytes") == 0) {mods.bytes = 1; sprintf(arguments+strlen(arguments)," %s",argv[i]);}
        else if (strcmp(argv[i],"-B") == 0) {mods.block_size = atoi(argv[++i]); sprintf(arguments+strlen(arguments)," %s",argv[i]);}
        else if (strstr(argv[i],"--block-size=") != NULL)
        {
            size_t i2;
            for (i2 = 0; i2 < strlen(argv[i]); i2++)
            {
                if (argv[i][i2] == '=') break; 
            }
            mods.block_size = atoi(&argv[i][++i2]);
            sprintf(arguments+strlen(arguments)," %s",argv[i]);
        }
        else if (strcmp(argv[i],"-L") == 0 || strcmp(argv[i],"--dereference") == 0) {mods.dereference = 1; sprintf(arguments+strlen(arguments)," %s",argv[i]);}
        else if (strcmp(argv[i],"-S") == 0 || strcmp(argv[i],"--separate-dirs") == 0) {mods.separate_dirs = 1; sprintf(arguments+strlen(arguments)," %s",argv[i]);}
        else if (strstr(argv[i],"--max-depth=") != NULL)
        {
            size_t i2;
            for (i2 = 0; i2 < strlen(argv[i]); i2++)
            {
                if (argv[i][i2] == '=') break;
            }
            mods.max_depth = atoi(&argv[i][++i2]);
            sprintf(arguments+strlen(arguments)," %s",argv[i]);
        }
        else if (strcmp(argv[i],"-l") != 0 && strcmp(argv[i],"--count-links") != 0) strcpy(path,argv[i]);
        sprintf(firstEntryContent+strlen(firstEntryContent)," %s",argv[i]);
    }

    printFirstLogEntry(log_filename,getInstant(),getpid(),arguments);

    calcDir(path,0);


    printLogEntry(log_filename,getInstant(),getpid(),EXIT,"0");
    return 0;
}