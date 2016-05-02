#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "misc.h"

void die(const char *msg){
    if(!errno)
        fprintf(stderr, "%s\n", msg);
    else
        perror(msg);
    exit(1);
}
