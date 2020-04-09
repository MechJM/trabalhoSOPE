#pragma once

#include "global.h"
#include "log_file.h"
#include "calc_time.h"

long int calcFile(struct stat *stat_entry);

long int calcDir(char* path, int depth);

