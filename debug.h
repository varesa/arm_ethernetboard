//
// Created by esa on 2/7/16.
//

#ifndef ARM_ETHERNETBOARD_DEBUG_H
#define ARM_ETHERNETBOARD_DEBUG_H

void dbg_print(const char *msg);
void dbg_print_val8(const char *msg, uint8_t val);

void HardFault_Handler(void);

#endif //ARM_ETHERNETBOARD_DEBUG_H
