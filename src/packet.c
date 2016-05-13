#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "packet.h"
#include "misc.h"

void send_packet(int* socket, command_t command, int param){
    struct packet_t packet;

    packet.command = command;
    packet.parameter = param;

    while(write(*socket, &packet, sizeof(struct packet_t)) != sizeof(struct packet_t)){
         print_err("Cannot write Packet");
    }

    return;
}

void wait_for_packet(int* socket, struct packet_t* packet){

    while(read(*socket , packet, sizeof(struct packet_t)) != sizeof(struct packet_t)){
         print_err("Cannot read Packet");
    }

    return;
}
