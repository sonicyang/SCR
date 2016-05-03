#ifndef __PACKET_H__
#define __PACKET_H__

typedef enum{
    MESG = 0,
    RECV,
    TERM,
    ACK
}command_t;

struct packet_t{
    command_t command;
    int parameter;
} __attribute__((packed));

void send_packet(int*, command_t, int);
void wait_for_packet(int*, struct packet_t*);


#endif//__PACKET_H__
