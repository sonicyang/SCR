#ifndef __MISC_H__
#define __MISC_H__

#include <stdio.h>

extern inline void die(const char *msg){
    perror(msg);
    exit(1);
}

#endif//__MISC_H__
