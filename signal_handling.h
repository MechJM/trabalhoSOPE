#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H

#include "global.h"

void sigint_handler(int signo);

void sigcont_handler(int signo);

void sigterm_handler(int signo);

void install_handlers();

void uninstall_handlers();

#endif