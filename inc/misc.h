#ifndef __MISC_H__
#define __MISC_H__

void die(const char *msg);

typedef enum{
    MESG = 0,
    TERM,
    ACK
}command_t;

#endif//__MISC_H__
