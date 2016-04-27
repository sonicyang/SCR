#ifndef __MISC_H__
#define __MISC_H__

#include <stdio.h>
#include <errno.h>

extern inline void die(const char *msg){
    if(!errno)
        fprintf(stderr, "%s\n", msg);
    else
        perror(msg);
    exit(1);
}

#endif//__MISC_H__
