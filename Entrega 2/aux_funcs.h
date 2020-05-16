#pragma once

struct logInfo
{
    int i;
    int dur;
    int pl;
};

void printLog(int i, int dur, int pl, char* oper);

void cleanPrintLog(void * arg);

void cleanUnlockMutex(void * arg);

void cleanUnlinkFifo(void * arg);

void cleanCloseFile(void * arg);

void sigusr1_handler(int signo);

void sigusr2_handler(int signo);