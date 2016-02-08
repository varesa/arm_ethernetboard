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
static mutex_t SPIMutex;

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

    encWriteReg(ERXST, RXSTART_INIT);
    encWriteReg(ERXRDPT, RXSTART_INIT);
    encWriteReg(ERXND, RXSTOP_INIT);
    encWriteReg(ETXST, TXSTART_INIT);
    encWriteReg(ETXND, TXSTOP_INIT);
    encWriteRegByte(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
    encWriteReg(EPMM0, 0x303f);
    encWriteReg(EPMCS, 0xf7f9);
    encWriteRegByte(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    encWriteRegByte(MACON2, 0x00);
    encWriteOp(ENC28J60_BIT_FIELD_SET, MACON3,
               MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
    encWriteReg(MAIPG, 0x0C12);
    encWriteRegByte(MABBIPG, 0x12);
    encWriteReg(MAMXFL, MAX_FRAMELEN);
    encWriteRegByte(MAADR5, mac[0]);
    encWriteRegByte(MAADR4, mac[1]);
    encWriteRegByte(MAADR3, mac[2]);
    encWriteRegByte(MAADR2, mac[3]);
    encWriteRegByte(MAADR1, mac[4]);
    encWriteRegByte(MAADR0, mac[5]);
    encWritePhy(PHCON2, PHCON2_HDLDIS);
    encSetBank(ECON1);
    encWriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

void encInit(uint8_t mac[]) {
    dbg_print("ENC: Starting to init\n");
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

uint8_t encReadOp(uint8_t op, uint8_t address) {
    tx_buffer[0] = op | (address & ADDR_MASK);
    uint8_t result;

    chMtxLock(&SPIMutex);
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
    chMtxUnlock(&SPIMutex);

    return result;
}

void encWriteOp(uint8_t op, uint8_t address, uint8_t data) {
    tx_buffer[0] = op | (address & ADDR_MASK);
    tx_buffer[1] = data;

    chMtxLock(&SPIMutex);
    //spiStart(SPI_ENC, &spicfg);
    spiSelect(SPI_ENC);
    //chThdSleep(1);
    spiExchange(SPI_ENC, 2, tx_buffer, rx_buffer);

    spiUnselect(SPI_ENC);
    //chThdSleep(1);
    //spiReleaseBus(SPI_ENC);
    chMtxUnlock(&SPIMutex);
}

/*
 * Control
 */



static uint8_t enc_address;

void encSetBank(uint8_t address) {
    if((address & BANK_MASK) != enc_address) {
        encWriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
        enc_address = address & BANK_MASK;
        encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, enc_address>>5);
    }
}

/*
 * Control registers
 */

uint8_t encReadRegByte(uint8_t address) {
    encSetBank(address);
    return encReadOp(ENC28J60_READ_CTRL_REG, address);
}

uint16_t encReadReg(uint8_t address) {
    return encReadRegByte(address) + (encReadRegByte((uint8_t)(address + 1)) << 8);
}

void encWriteRegByte(uint8_t address, uint8_t data) {
    encSetBank(address);
    encWriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void encWriteReg(uint8_t address, uint16_t data) {
    encWriteRegByte(address, (uint8_t)(data & 0xFF));
    encWriteRegByte((uint8_t)(address + 1), (uint8_t)((data >> 8) & 0xFF));
}

uint16_t encReadPhyByte(uint8_t address) {
    encWriteRegByte(MIREGADR, address);
    encWriteRegByte(MICMD, MICMD_MIIRD);
    while (encReadRegByte(MISTAT) & MISTAT_BUSY) continue;
    encWriteRegByte(MICMD, 0x00);
    return encReadRegByte(MIRD + 1);
}

void encWritePhy(uint8_t address, uint16_t data) {
    encWriteRegByte(MIREGADR, address);
    encWriteReg(MIWR, data);
    while (encReadRegByte(MISTAT) & MISTAT_BUSY) continue;
}

void encReadBuf(uint16_t  address, uint16_t len, uint8_t* data) {
    if(len == 0) {
        return;
    }
    encWriteReg(ERDPT, address);
    spiSelect(SPI_ENC);
    spiExchange(SPI_ENC, len, tx_buffer, data);
    spiUnselect(SPI_ENC);
}

void encWriteBuf(uint16_t  address, uint16_t len, const uint8_t* data) {
    if(len == 0) {
        return;
    }
    encWriteReg(EWRPT, address);
    spiSelect(SPI_ENC);
    spiExchange(SPI_ENC, len, data, rx_buffer);
    spiUnselect(SPI_ENC);
}