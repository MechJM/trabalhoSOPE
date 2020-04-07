#include "log_file.h"

void printFirstLogEntry(char* log_filename, double instant, pid_t pid,char* info)
{
    FILE* logPtr = fopen(log_filename,"w");
    char action_string[STR_LEN] = "";

    strcpy(action_string,"CREATE");

    fprintf(logPtr,"%.2F - %-8d - %s - %s\n",instant,pid,action_string,info);
    fclose(logPtr);
}

void printLogEntry(char* log_filename, double instant, pid_t pid,enum action action_struct,char* info)
{
    FILE* logPtr = fopen(log_filename,"a");
    char action_string[STR_LEN] = "";

    switch (action_struct)
    {
    case CREATE:
        strcpy(action_string,"CREATE");
        break;
    case EXIT:
        strcpy(action_string,"EXIT");
        break;
    case RECV_SIGNAL:
        strcpy(action_string,"RECV_SIGNAL");
        break;
    case SEND_SIGNAL:
        strcpy(action_string,"SEND_SIGNAL");
        break;
    case RECV_PIPE:
        strcpy(action_string,"RECV_PIPE");
        break;
    case SEND_PIPE:
        strcpy(action_string,"SEND_PIPE");
        break;
    case ENTRY:
        strcpy(action_string,"ENTRY");
        break;
    default:
        fprintf(stderr,"Wrong enum value.\n");
        break;
    }

    fprintf(logPtr,"%.2F - %8d - %s - %s\n",instant,pid,action_string,info);
    fclose(logPtr);
}