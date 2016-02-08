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

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the ST INEMO-M1 Discovery board.
 */

/*
 * Board identifier.
 */
#define BOARD_ST_NUCLEO_F103RB
#define BOARD_NAME              "STMicroelectronics NUCLEO-F103RB"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            0
#define STM32_HSECLK            6250000


/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#ifndef STM32F103xB
#define STM32F103xB
#endif

/*
 * IO pins assignments.
 */
#define GPIOA_PA0               0
#define GPIOA_PA1               1
#define GPIOA_USART_TX          2
#define GPIOA_USART_RX          3
#define GPIOA_PA4               4
#define GPIOA_LED_GREEN         5
#define GPIOA_PA6               6
#define GPIOA_PA7               7
#define GPIOA_PA8               8
#define GPIOA_PA9               9
#define GPIOA_PA10              10
#define GPIOA_PA11              11
#define GPIOA_PA12              12
#define GPIOA_SWDIO             13
#define GPIOA_SWCLK             14
#define GPIOA_PA15              15

#define GPIOB_PB0               0
#define GPIOB_PB1               1
#define GPIOB_PB2               2
#define GPIOB_SWO               3
#define GPIOB_PB4               4
#define GPIOB_PB5               5
#define GPIOB_PB6               6
#define GPIOB_PB7               7
#define GPIOB_PB8               8
#define GPIOB_PB9               9
#define GPIOB_PB10              10
#define GPIOB_PB11              11
#define GPIOB_PB12              12
#define GPIOB_PB13              13
#define GPIOB_PB14              14
#define GPIOB_PB15              15

#define GPIOC_PC0               0
#define GPIOC_PC1               1
#define GPIOC_PC2               2
#define GPIOC_PC3               3
#define GPIOC_PC4               4
#define GPIOC_PC5               5
#define GPIOC_PC6               6
#define GPIOC_PC7               7
#define GPIOC_PC8               8
#define GPIOC_PC9               9
#define GPIOC_PC10              10
#define GPIOC_PC11              11
#define GPIOC_PC12              12
#define GPIOC_PC13              13
#define GPIOC_PC14              14
#define GPIOC_PC15              15

#define GPIOD_OSC_IN            0
#define GPIOD_OSC_OUT           1
#define GPIOD_PD2               2

/*
 * Custom IO labels
 */

#define GPIO_LED_PORT       GPIOA
#define GPIO_LED            GPIOA_PA0

#define GPIO_SPI_PORT       GPIOA
#define GPIO_SPI_SCK        GPIOA_PA5
#define GPIO_SPI_MISO       GPIOA_PA6
#define GPIO_SPI_MOSI       GPIOA_PA7

#define GPIO_ENC_PORT       GPIOC
#define GPIO_ENC_RESET      GPIOC_PC13
#define GPIO_ENC_CS         GPIOC_PC14
#define GPIO_ENC_INT        GPIOC_PC15

/*
 * Other labels
 */

#define SPI_ENC             &SPID1


/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA0  - Push Pull Output          (GPIOA_LED).
 * PA5  - AF                        (GPIOA_SPI_SCK)
 * PA6  - AF                        (GPIOA_SPI_MISO)
 * PA7  - AF                        (GPIOA_SPI_MOSI)
 * PA13 - Pull-up input             (GPIOA_SWDIO).
 * PA14 - Pull-down input           (GPIOA_SWCLK).
 */
#define VAL_GPIOACRL            0xBBB88882      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x88888888      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFBFDF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB3  - Pull-up input             (GPIOA_SWO).
 */
#define VAL_GPIOBCRL            0x88888888      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x88888888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
 * Port C setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x42288888      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIODCRL            0x88888888      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp)

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp)

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
