//
// Created by esa on 2/6/16.
//

#ifndef ARM_ETHERNETBOARD_ENC28J60_H
#define ARM_ETHERNETBOARD_ENC28J60_H

// buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated

#define RXSTART_INIT        0x0000  // start of RX buffer, (must be zero, Rev. B4 Errata point 5)
#define RXSTOP_INIT         0x0BFF  // end of RX buffer, room for 2 packets

#define TXSTART_INIT        0x0C00  // start of TX buffer, room for 1 packet
#define TXSTOP_INIT         0x11FF  // end of TX buffer

#define SCRATCH_START       0x1200  // start of scratch area
#define SCRATCH_LIMIT       0x2000  // past end of area, i.e. 3 Kb
#define SCRATCH_PAGE_SHIFT  6       // addressing is in pages of 64 bytes
#define SCRATCH_PAGE_SIZE   (1 << SCRATCH_PAGE_SHIFT)
#define SCRATCH_PAGE_NUM    ((SCRATCH_LIMIT-SCRATCH_START) >> SCRATCH_PAGE_SHIFT)
#define SCRATCH_MAP_SIZE    (((SCRATCH_PAGE_NUM % 8) == 0) ? (SCRATCH_PAGE_NUM / 8) : (SCRATCH_PAGE_NUM/8+1))

// area in the enc memory that can be used via enc_malloc; by default 0 bytes; decrease SCRATCH_LIMIT in order
// to use this functionality
#define ENC_HEAP_START      SCRATCH_LIMIT
#define ENC_HEAP_END        0x2000

#define MAX_FRAMELEN 1500

void encInit(uint8_t mac[]);

uint8_t encReadOp(uint8_t op, uint8_t address);
void encWriteOp(uint8_t op, uint8_t address, uint8_t data);
void encSetBank(uint8_t address);

uint8_t encReadRegByte(uint8_t address);
uint16_t encReadReg(uint8_t address);

void encWriteRegByte(uint8_t address, uint8_t data);
void encWriteReg(uint8_t address, uint16_t data);

void encReadBuf(uint16_t  address, uint16_t len, uint8_t* data);
void encWriteBuf(uint16_t  address, uint16_t len, const uint8_t* data);


#endif //ARM_ETHERNETBOARD_ENC28J60_H
