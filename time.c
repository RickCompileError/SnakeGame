#include "time.h"

time_t milliseconds(){
    struct timeval time_now = {};
    gettimeofday(&time_now, NULL);
    time_t msecs = time_now.tv_sec*1000 + time_now.tv_usec/1000;
    return msecs;
}
