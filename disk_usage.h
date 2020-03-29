#ifndef DISK_USAGE_H
#define DISK_USAGE_H

#include "global.h"

long int calcFile(struct stat *stat_entry);

long int calcDir(char* path);

#endif