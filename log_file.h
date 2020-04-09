#pragma once

#include "global.h"

void printFirstLogEntry(char* log_filename, double instant, pid_t pid,char* info);

void printLogEntry(char* log_filename, double instant, pid_t pid,enum action action_struct,char* info);

