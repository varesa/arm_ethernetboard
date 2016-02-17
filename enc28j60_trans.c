//
// Created by esa on 2/7/16.
//

#include <stddef.h>
#include <lwip/pbuf.h>

#include "debug.h"
#include "enc28j60.h"
#include "enc28j60_registers.h"



static uint16_t next_frame_location = 0;
static uint16_t rxbufsize = RXSTOP_INIT;

void receive_start(uint8_t header[6], uint16_t *length)
{
    encReadBuf(next_frame_location, 6, header);
    *length = (uint16_t)(header[2] | ((header[3] & 0x7f) << 8));
}

void receive_end(uint8_t header[6])
{
    next_frame_location = header[0] + (header[1] << 8);

    /* workaround for 80349c.pdf (errata) #14 start.
     *
     * originally, this would have been
     * enc_WCR16(dev, ENC_ERXRDPTL, next_location);
     * but thus: */
    if (next_frame_location == /* enc_RCR16(dev, ENC_ERXSTL) can be simplified because of errata item #5 */ 0)
        encWriteReg(ERXRDPT, encReadReg(ERXND));
    else
        encWriteReg(ERXRDPT, next_frame_location - 1);

    /* workaround end */

    //dbg_print_val8("Packages pending before: ", encReadRegByte(EPKTCNT));
    encSetBank(ECON2);
    encWriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    //dbg_print_val8("packages pending after: ", encReadRegByte(EPKTCNT));

    dbg_print_wrapper("Receive vector: %X %X %X %X, next packet loc: %X %X\n", header[5], header[4], header[3], header[2], header[1], header[0]);
}


int enc_read_received_pbuf(struct pbuf **buf)
{
    uint8_t header[6];
    uint16_t length;

    if (*buf != NULL)
        return 1;

    receive_start(header, &length);

    *buf = pbuf_alloc(PBUF_RAW, length, PBUF_RAM);
    if (*buf == NULL)
        dbg_print_val("failed to allocate buf, discarding, length: ", length);
    else
        encReadBuf(ENC_READLOCATION_ANY, length, (*buf)->payload);

    receive_end(header);

    return (*buf == NULL) ? 2 : 0;
}

/* Partial function of enc_transmit. Always call this as transmit_start /
 * {transmit_partial * n} / transmit_end -- and use enc_transmit or
 * enc_transmit_pbuf unless you're just implementing those two */
void transmit_start()
{
    /* according to section 7.1 */
    uint8_t control_byte = 0; /* no overrides */

    /* 1. */
    /** @todo we only send a single frame blockingly, starting at the end of rxbuf */
    encWriteReg(ETXST, rxbufsize);
    /* 2. */
    encWriteBuf(rxbufsize, 1, &control_byte);
}

void transmit_partial(uint8_t *data, uint16_t length)
{
    encWriteBufRaw(data, length);
}

void transmit_end(uint16_t length)
{
    /* calculate checksum */

//	enc_WCR16(dev, ENC_EDMASTL, start + 1);
//	enc_WCR16(dev, ENC_EDMANDL, start + 1 + length - 3);
//	enc_BFS(dev, ENC_ECON1, ENC_ECON1_CSUMEN | ENC_ECON1_DMAST);
//	while (enc_RCR(dev, ENC_ECON1) & ENC_ECON1_DMAST);
//	uint16_t checksum = enc_RCR16(dev, ENC_EDMACSL);
//	checksum = ((checksum & 0xff) << 8) | (checksum >> 8);
//	enc_WBM(dev, &checksum, start + 1 + length - 2, 2);

    /* 3. */
    encWriteReg(ETXND, rxbufsize+1+length-1);

    /* 4. */
    /* skipped because not using interrupts yet */
    /* 5. */
    encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

    /* block */
    while (encReadRegByte(ECON1) & ECON1_TXRTS);

    uint8_t tmp[45];
    encReadBuf(rxbufsize, 45, tmp);

    uint8_t result[7];
    encReadBuf(rxbufsize + 1 + length, 7, result);
    //("transmitted. %02x %02x %02x %02x %02x %02x %02x\n", result[0], result[1], result[2], result[3], result[4], result[5], result[6]);
    dbg_print_val("Transmitted: [0]", result[0]);
    dbg_print_val("Transmitted: [1]", result[1]);
    dbg_print_val("Transmitted: [2]", result[2]);
    dbg_print_val("Transmitted: [3]", result[3]);
    dbg_print_val("Transmitted: [4]", result[4]);
    dbg_print_val("Transmitted: [5]", result[5]);


    /** @todo parse that and return reasonable state */
}

void enc_transmit_pbuf(struct pbuf *buf)
{
    uint16_t length = buf->tot_len;

    /** @todo check buffer size */
    transmit_start();
    while(1) {
        transmit_partial(buf->payload, buf->len);
        if (buf->len == buf->tot_len)
            break;
        buf = buf->next;
    }
    transmit_end(length);
}