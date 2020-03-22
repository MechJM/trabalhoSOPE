#ifndef DISK_USAGE_H
#define DISK_USAGE_H

#include "global.h"

void calcFile(struct stat *stat_entry,long int *size,char* path);

void calcDir(char* path);

#endif