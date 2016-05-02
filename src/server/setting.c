#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "setting.h"
#include "misc.h"

int parse_setting(const char* setting_fn, struct setting_t* setting){
    char pname[32];
    char pvalue[32];
    FILE *fil;

    fil = fopen(setting_fn, "r");

    if(!fil){
        die("Error opening setting");
    }

    while(fscanf(fil, "%s %s", pname, pvalue) != EOF){
        if(!strcmp("ADDR", pname)){
            if(!strcmp("*", pvalue)){
                strcpy(setting->address, "\0");
            }else{
                strncpy(setting->address, pvalue, 15);
                setting->address[15] = '\0';
            }
        }else if(!strcmp("PORT", pname)){
           setting->port = atoi(pvalue);
        }else if(!strcmp("MAX", pname)){
           setting->max_user = atoi(pvalue);
        }
    }

    fclose(fil);

    return 0;
}

void load_setting(struct setting_t* setting, struct sockaddr_in* address){
    memset((char *) address, 0, sizeof(struct sockaddr_in));

    address->sin_family = AF_INET;
    address->sin_port = htons(setting->port);
    if(!strlen(setting->address)){
       address->sin_addr.s_addr = INADDR_ANY;
    }else{
        if(!inet_pton(AF_INET, setting->address, &(address->sin_addr))){
            die("Bad IPv4 format");
        }
    }

    return;
}
