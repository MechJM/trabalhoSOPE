#include "global.h"
#include "calc_time.h"

double timestamp()
{
    struct timespec time_buf;
    
    if (clock_gettime(CLOCK_MONOTONIC,&time_buf) < 0) write(STDERR_FILENO,"Couldn't get time.\n",19);
    return (double) time_buf.tv_nsec/1e6;
}

double getInstant()
{
    return (double) timestamp() - start;
}