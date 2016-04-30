#ifndef __SETTING_H__
#define __SETTING_H__

#include <sys/socket.h>
#include <netinet/in.h>

struct setting_t{
    char address[16];
    int port;
    int max_user;
};

int parse_setting(const char* setting_fn, struct setting_t* setting);
void load_setting(struct setting_t* setting, struct sockaddr_in* address);

#endif//__SETTING_H__
