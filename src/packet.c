#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "packet.h"
#include "misc.h"

void send_packet(int* socket, command_t command, int param){
    int n;
    struct packet_t packet;

    packet.command = command;
    packet.parameter = param;

    n = write(*socket, &packet, sizeof(struct packet_t));

    if(n < 0)
        die("Error on write socket");

    if(n < sizeof(struct packet_t))
        die("Failed on packet write");

    return;
}

void wait_for_packet(int* socket, struct packet_t* packet){
    int n;

    n = read(*socket , packet, sizeof(struct packet_t));

    if(n < 0)
        die("Error on reading socket");

    if(n < sizeof(struct packet_t))
        die("Got non-standard packet");

    return;
}
