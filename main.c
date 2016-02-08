//
// Created by esa on 2/1/16.
//

#include <stdbool.h>
#include <hal.h>
#include <lwip/ip4_addr.h>
#include "lwipthread.h"
#include "debug.h"

/*
 * Blinker thread #1.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

    (void)arg;

    chRegSetThreadName("blinker");
    while (true) {
        dbg_print("Blink!\n");
        palSetPad(GPIOA, GPIOA_PA0);
        chThdSleepMilliseconds(1000);
        palClearPad(GPIOA, GPIOA_PA0);
        chThdSleepMilliseconds(1000);
    }
}


uint8_t mac[6] = {0x00, 0x04, 0xED, 0x34, 0xC9, 0xC1};

int main() {
    halInit();
    chSysInit();

    chRegSetThreadName("main");

    dbg_init();
    dbg_print("\n\nReboot\n\n");

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);

    lwipthread_opts_t opts;
    IP4_ADDR(&opts.address, 192,168,0,111);
    IP4_ADDR(&opts.netmask, 255, 255, 255, 0);
    IP4_ADDR(&opts.gateway, 192, 168, 0, 10);

    opts.macaddress = mac;

    lwipInit(&opts);

    while(1) {
        chThdSleepSeconds(1);
    }
}

void exit(int a) {
    while(true) {

    }
}