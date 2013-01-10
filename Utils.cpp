#include "Utils.hpp"
#include <sys/time.h>


uint32_t milliseconds()
{
    timeval a;
    gettimeofday(&a, 0);
    return a.tv_sec * 1000 + a.tv_usec/1000;
}


