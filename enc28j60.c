//
// Created by esa on 2/6/16.
//

#include <string.h>
#include <stdlib.h>
#include "ch.h"
#include "hal.h"

#include "enc28j60.h"
#include "enc28j60_registers.h"
#include "debug.h"

// Private functions
void encSpiInit();
void encReset(bool reset);

#define SPI_BUFFER_SIZE 16

// SPI Buffers
static uint8_t tx_buffer[SPI_BUFFER_SIZE];
static uint8_t rx_buffer[SPI_BUFFER_SIZE];

// Mutex
mutex_t SPIMutex;

// Function declarations

uint8_t     encReadOp   (uint8_t op, uint8_t address);
void        encWriteOp  (uint8_t op, uint8_t address, uint8_t data);

void        encSetBank  (uint8_t address);

uint8_t     encReadRegByte  (uint8_t address);
uint16_t    encReadReg      (uint8_t address);
void        encWriteRegByte (uint8_t address, uint8_t data);
void        encWriteReg     (uint8_t address, uint16_t data);

uint16_t    encReadPhy(uint8_t address);
void        encWritePhy(uint8_t address, uint16_t data);

/*
 * Initializations
 */

void encInitRegisters(uint8_t mac[]) {
    encWriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    chThdSleepSeconds(2); // errata B7/2
    while (!encReadOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY) continue;

    // Discard packages
    encSetBank(ECON1);
    encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST | ECON1_RXRST);
    while(encReadRegByte(EPKTCNT)) {
        encWriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    }
    encWriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST | ECON1_RXRST);

    // Set RX Buffer pointers
    encWriteReg(ERXST, RXSTART_INIT);
    encWriteReg(ERXRDPT, RXSTART_INIT);
    encWriteReg(ERXND, RXSTOP_INIT);

    // Set TX Buffer pointers
    encWriteReg(ETXST, TXSTART_INIT);
    encWriteReg(ETXND, TXSTOP_INIT);

    // Set rx filters
    encWriteRegByte(ERXFCON, ERXFCON_BCEN | ERXFCON_MCEN | ERXFCON_HTEN | ERXFCON_PMEN | ERXFCON_PMEN | ERXFCON_UCEN);

    // MAC control registers
    encWriteRegByte(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    encWriteRegByte(MACON2, 0x00);
    encWriteRegByte(MACON3, MACON3_FULDPX | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_PADCFG0 | MACON3_PADCFG1 | MACON3_PADCFG2);

    // Full duplex
    encWritePhy(PHCON1, PHCON1_PDPXMD);

    // Pattern matching
    //encWriteReg(EPMM0, 0x303f);
    //encWriteReg(EPMCS, 0xf7f9);

    // Magic=
    encWriteReg(MAIPG, 0x0C12);

    // ?
    encWriteRegByte(MABBIPG, 0x12);
    encWriteReg(MAMXFL, MAX_FRAMELEN);

    // Set MAC addr
    encWriteRegByte(MAADR5, mac[0]);
    encWriteRegByte(MAADR4, mac[1]);
    encWriteRegByte(MAADR3, mac[2]);
    encWriteRegByte(MAADR2, mac[3]);
    encWriteRegByte(MAADR1, mac[4]);
    encWriteRegByte(MAADR0, mac[5]);

    // Enable reception
    encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

void encInit(uint8_t mac[]) {
    dbg_print("ENC: Start init\n");
    // Initialize SPI
    encSpiInit();

    // Take the IC out of reset
    encReset(true);
    chThdSleep(10);
    encReset(false);
    chThdSleep(10);

    // Init the mutex
    chMtxObjectInit(&SPIMutex);

    encInitRegisters(mac);

    uint8_t revision = encReadRegByte(EREVID);
    dbg_print_val8("Initialized. Rev: ", revision);
}

const SPIConfig spicfg  = {
    NULL,
    GPIO_ENC_PORT,
    GPIO_ENC_CS,
    0
};

void encSpiInit() {
    palSetPad(GPIO_ENC_PORT,GPIO_ENC_CS);

    spiInit();
    spiStart(SPI_ENC, &spicfg);
}

void encReset(bool reset) {
    if(reset) {
        palClearPad(GPIO_ENC_PORT, GPIO_ENC_RESET);
    } else {
        palSetPad(GPIO_ENC_PORT, GPIO_ENC_RESET);
    }
}

/*
 * Communication
 */

uint8_t _encReadOp(uint8_t op, uint8_t address) {
    encSetBank(address);
    tx_buffer[0] = op | (address & ADDR_MASK);
    uint8_t result;

    //spiStart(SPI_ENC, &spicfg);
    spiSelect(SPI_ENC);
    //chThdSleep(1);
    if(!(address & 0x80)) { // E Registers
        spiExchange(SPI_ENC, 2, tx_buffer, rx_buffer);
        result = rx_buffer[1];
    } else {                // I and M registers
        spiExchange(SPI_ENC, 3, tx_buffer, rx_buffer);
        result = rx_buffer[2];
    }
    spiUnselect(SPI_ENC);
    //chThdSleep(1);
    //spiReleaseBus(SPI_ENC);

    return result;
}

uint8_t encReadOp(uint8_t op, uint8_t address) {
    chMtxLock(&SPIMutex);
    uint8_t val = _encReadOp(op, address);
    chMtxUnlock(&SPIMutex);
    return val;
}

void _encWriteOp(uint8_t op, uint8_t address, uint8_t data) {
    tx_buffer[0] = op | (address & ADDR_MASK);
    tx_buffer[1] = data;

    //spiStart(SPI_ENC, &spicfg);
    spiSelect(SPI_ENC);
    //chThdSleep(1);
    spiExchange(SPI_ENC, 2, tx_buffer, rx_buffer);

    spiUnselect(SPI_ENC);
    //chThdSleep(1);
    //spiReleaseBus(SPI_ENC);
}

void encWriteOp(uint8_t op, uint8_t address, uint8_t data) {
    chMtxLock(&SPIMutex);
    encSetBank(address);
    _encWriteOp(op, address, data);
    chMtxUnlock(&SPIMutex);
}

/*
 * Control
 */



static uint8_t enc_address;

void encSetBank(uint8_t address) {
    if((address & BANK_MASK) != enc_address) {
        enc_address = address & BANK_MASK;
        _encWriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
        _encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, enc_address>>5);
    }
}

/*
 * Control registers
 */

uint8_t _encReadRegByte(uint8_t address) {
    uint8_t byte = _encReadOp(ENC28J60_READ_CTRL_REG, address);
    return byte;
}

uint8_t encReadRegByte(uint8_t address) {
    chMtxLock(&SPIMutex);
    uint8_t byte = _encReadRegByte(address);
    chMtxUnlock(&SPIMutex);
    return byte;
}

uint16_t _encReadReg(uint8_t address) {
    uint16_t value = _encReadRegByte(address) + (_encReadRegByte((uint8_t)(address + 1)) << 8);
    return value;
}

uint16_t encReadReg(uint8_t address) {
    chMtxLock(&SPIMutex);
    uint16_t value = _encReadReg(address);
    chMtxUnlock(&SPIMutex);
    return value;
}

void _encWriteRegByte(uint8_t address, uint8_t data) {
    encSetBank(address);
    _encWriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void encWriteRegByte(uint8_t address, uint8_t data) {
    chMtxLock(&SPIMutex);
    _encWriteRegByte(address, data);
    chMtxUnlock(&SPIMutex);
}

void _encWriteReg(uint8_t address, uint16_t data) {
    _encWriteRegByte(address, (uint8_t)(data & 0xFF));
    _encWriteRegByte((uint8_t)(address + 1), (uint8_t)((data >> 8) & 0xFF));
}

void encWriteReg(uint8_t address, uint16_t data) {
    chMtxLock(&SPIMutex);
    _encWriteReg(address, data);
    chMtxUnlock(&SPIMutex);
}

uint16_t encReadPhy(uint8_t address) {
    chMtxLock(&SPIMutex);
    _encWriteRegByte(MIREGADR, address);
    _encWriteRegByte(MICMD, MICMD_MIIRD);
    while (_encReadRegByte(MISTAT) & MISTAT_BUSY) continue;
    _encWriteRegByte(MICMD, 0x00);
    uint16_t val = _encReadReg(MIRD);
    chMtxUnlock(&SPIMutex);
    return val;
}

void encWritePhy(uint8_t address, uint16_t data) {
    chMtxLock(&SPIMutex);
    while (_encReadRegByte(MISTAT) & MISTAT_BUSY) continue;
    _encWriteRegByte(MIREGADR, address);
    _encWriteReg(MIWR, data);
    while (_encReadRegByte(MISTAT) & MISTAT_BUSY) continue;
    chMtxUnlock(&SPIMutex);
}

void encReadBuf(uint16_t  address, uint16_t len, uint8_t* data) {
    if(len == 0) {
        return;
    }
    chMtxLock(&SPIMutex);
    _encWriteReg(ERDPT, address);
    spiSelect(SPI_ENC);
    tx_buffer[0] = ENC28J60_READ_BUF_MEM | 0x1A;
    spiSend(SPI_ENC, 1, tx_buffer);
    spiReceive(SPI_ENC, len, data);
    spiUnselect(SPI_ENC);
    chMtxUnlock(&SPIMutex);
}

void _encWriteBufRaw(const uint8_t *data, uint16_t len) {
    spiSelect(SPI_ENC);
    tx_buffer[0] = ENC28J60_WRITE_BUF_MEM | 0x1A;
    spiSend(SPI_ENC, 1, tx_buffer);
    spiSend(SPI_ENC, len, data);
    spiUnselect(SPI_ENC);
}


void encWriteBufRaw(const uint8_t *data, uint16_t len) {
    if(len == 0) {
        return;
    }
    chMtxLock(&SPIMutex);
    _encWriteBufRaw(data, len);
    chMtxUnlock(&SPIMutex);
}

void encWriteBuf(uint16_t  address, uint16_t len, const uint8_t* data) {
    if(len == 0) {
        return;
    }
    chMtxLock(&SPIMutex);
    _encWriteReg(EWRPT, address);
    _encWriteBufRaw(data, len);
    chMtxUnlock(&SPIMutex);
}