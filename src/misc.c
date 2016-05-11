#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"

void die(const char *msg){
    if(!errno)
        fprintf(stderr, "%s\n", msg);
    else
        perror(msg);
    exit(1);
}

void print_err(const char *msg){
    if(!errno)
        fprintf(stderr, "%s\n", msg);
    else
        fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}
