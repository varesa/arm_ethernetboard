//
// Created by esa on 2/7/16.
//

#ifndef ARM_ETHERNETBOARD_DEBUG_H
#define ARM_ETHERNETBOARD_DEBUG_H

void dbg_init();
void dbg_print(const char *msg);
void dbg_print_val(const char *msg, uint32_t val);
void dbg_print_wrapper(const char *format, ...);

#define dbg_print_val8 dbg_print_val

void HardFault_Handler(void);

#endif //ARM_ETHERNETBOARD_DEBUG_H
