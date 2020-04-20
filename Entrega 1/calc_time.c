#include "global.h"
#include "calc_time.h"

struct timespec timestamp()
{
    
    struct timespec time_buf;
    
    if (clock_gettime(CLOCK_MONOTONIC,&time_buf) < 0) write(STDERR_FILENO,"Couldn't get time.\n",19);
    
    return time_buf;
}

double getInstant()
{
    struct timespec current = timestamp();

    return (double) (current.tv_sec - start.tv_sec) * 1e3 + (current.tv_nsec - start.tv_nsec) / 1e6;
}