/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
 * **** This file incorporates work covered by the following copyright and ****
 * **** permission notice:                                                 ****
 *
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * @file lwipthread.c
 * @brief LWIP wrapper thread code.
 * @addtogroup LWIP_THREAD
 * @{
 */

#include "hal.h"
#include "evtimer.h"

#include "lwipthread.h"

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <lwip/tcpip.h>
#include <mac.h>
#include <lwip/tcp.h>
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "enc28j60.h"
#include "enc28j60_trans.h"

#if LWIP_DHCP
#include <lwip/dhcp.h>
#endif

#define PERIODIC_TIMER_ID       1
#define FRAME_RECEIVED_ID       2

/*
 * Initialization.
 */
static void low_level_init(struct netif *netif) {
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an Ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* Do whatever else is needed to initialize interface. */
    encInit(netif->hwaddr);
}

/*
 * Transmits a frame.
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p) {
    dbg_print("Sending a frame\n");
    enc_transmit_pbuf(p);
    //LWIP_DEBUGF(NETIF_DEBUG, ("sent %d bytes.\n", p->tot_len));
    dbg_print_val("Sent bytes: ", p->tot_len);
}

/*
 * Receives a frame.
 */
static struct pbuf *low_level_input(struct netif *netif) {

    err_t result;
    struct pbuf *buf = NULL;

    uint8_t epktcnt;

    epktcnt = encReadRegByte(EPKTCNT);

    if (epktcnt) {
        dbg_print("Incoming\n");
        if (enc_read_received_pbuf(&buf) == 0)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("incoming: %d packages, first read into %x\n", epktcnt, (unsigned int)(buf)));
            result = netif->input(buf, netif);
            LWIP_DEBUGF(NETIF_DEBUG, ("received with result %d\n", result));
        } else {
            /* FIXME: error reporting */
            LWIP_DEBUGF(NETIF_DEBUG, ("didn't receive.\n"));
        }
    }

}

/*
 * Initialization.
 */
static err_t ethernetif_init(struct netif *netif) {
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LWIP_LINK_SPEED);

    netif->state = NULL;
    netif->name[0] = LWIP_IFNAME0;
    netif->name[1] = LWIP_IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/*
 * Suspension point for initialization procedure.
 */
thread_reference_t lwip_trp = NULL;

/*
 * Stack area for the LWIP-MAC thread.
 */
static THD_WORKING_AREA(wa_lwip_thread, 1024);

void print_regs() {
    dbg_print_val8("ECON1 ", encReadRegByte(ECON1));
    dbg_print_val8("ECON2 ", encReadRegByte(ECON2));
    dbg_print_val8("ESTAT ", encReadRegByte(ESTAT));
    dbg_print_val8("EIR   ", encReadRegByte(EIR));

}

/**
 * @brief LWIP handling thread.
 *
 * @param[in] p pointer to a @p lwipthread_opts structure or @p NULL
 * @return The function does not return.
 */
static THD_FUNCTION(lwip_thread, p) {
    struct netif thisif;
    ip4_addr_t ip, gateway, netmask;

    chRegSetThreadName("lwip");

    tcpip_init(NULL, NULL);

    lwipthread_opts_t *opts = p;
    unsigned i;

    for (i = 0; i < 6; i++)
        thisif.hwaddr[i] = opts->macaddress[i];

    ip = opts->address;
    gateway = opts->gateway;
    netmask = opts->netmask;

    dbg_print("Adding interface\n");
    netif_add(&thisif, &ip, &netmask, &gateway, NULL, ethernetif_init, tcpip_input);
    netif_set_default(&thisif);
    dbg_print("up'ing interface\n");
    netif_set_up(&thisif);

    dbg_print("Entering loop\n");
    int x = 0;
    while(1) {
        low_level_input(&thisif);
        chThdSleep(50);
        if(x++ == 50) {
            etharp_gratuitous(&thisif);

            print_regs();
            x = 0;
        }
    }

}

/**
 * @brief   Initializes the lwIP subsystem.
 * @note    The function exits after the initialization is finished.
 *
 * @param[in] opts      pointer to the configuration structure, if @p NULL
 *                      then the static configuration is used.
 */
void lwipInit(const lwipthread_opts_t *opts) {

    /* Creating the lwIP thread (it changes priority internally).*/
    chThdCreateStatic(wa_lwip_thread, sizeof (wa_lwip_thread),
                      chThdGetPriorityX() - 1, lwip_thread, (void *)opts);

    /* Waiting for the lwIP thread complete initialization. Note,
       this thread reaches the thread reference object first because
       the relative priorities.*/
    chSysLock();
    chThdSuspendS(&lwip_trp);
    chSysUnlock();
}

/** @} */
