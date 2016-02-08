//
// Created by esa on 2/7/16.
//

#ifndef ARM_ETHERNETBOARD_ENC28J60_TRANS_H
#define ARM_ETHERNETBOARD_ENC28J60_TRANS_H

//
// Created by esa on 2/7/16.
//

#include <stddef.h>
#include <lwip/pbuf.h>

#include "debug.h"
#include "enc28j60.h"
#include "enc28j60_registers.h"

/*void receive_start(uint8_t header[6], uint16_t *length);
void receive_end(uint8_t header[6]);*/

int enc_read_received_pbuf(struct pbuf **buf);

/*void transmit_start();
void transmit_partial(uint8_t *data, uint16_t length);
void transmit_end(uint16_t length);*/

void enc_transmit_pbuf(struct pbuf *buf);

#endif //ARM_ETHERNETBOARD_ENC28J60_TRANS_H
