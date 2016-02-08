## hal.mk

set(SOURCE_FILES ${SOURCE_FILES}
    ${CHIBIOS}/os/hal/src/hal.c
    ${CHIBIOS}/os/hal/src/hal_buffers.c
    ${CHIBIOS}/os/hal/src/hal_queues.c
    ${CHIBIOS}/os/hal/src/hal_mmcsd.c
    ${CHIBIOS}/os/hal/src/adc.c
    ${CHIBIOS}/os/hal/src/can.c
    ${CHIBIOS}/os/hal/src/dac.c
    ${CHIBIOS}/os/hal/src/ext.c
    ${CHIBIOS}/os/hal/src/gpt.c
    ${CHIBIOS}/os/hal/src/i2c.c
    ${CHIBIOS}/os/hal/src/i2s.c
    ${CHIBIOS}/os/hal/src/icu.c
    ${CHIBIOS}/os/hal/src/mac.c
    ${CHIBIOS}/os/hal/src/mmc_spi.c
    ${CHIBIOS}/os/hal/src/pal.c
    ${CHIBIOS}/os/hal/src/pwm.c
    ${CHIBIOS}/os/hal/src/rtc.c
    ${CHIBIOS}/os/hal/src/sdc.c
    ${CHIBIOS}/os/hal/src/serial.c
    ${CHIBIOS}/os/hal/src/serial_usb.c
    ${CHIBIOS}/os/hal/src/spi.c
    ${CHIBIOS}/os/hal/src/st.c
    ${CHIBIOS}/os/hal/src/uart.c
    ${CHIBIOS}/os/hal/src/usb.c
    ${CHIBIOS}/os/hal/src/wdg.c
    )

include_directories(${CHIBIOS}/os/hal/include)

## platform.mk

set(SOURCE_FILES ${SOURCE_FILES}
        ${CHIBIOS}/os/hal/ports/common/ARMCMx/nvic.c 
        ${CHIBIOS}/os/hal/ports/STM32/STM32F1xx/hal_lld.c 
        ${CHIBIOS}/os/hal/ports/STM32/STM32F1xx/adc_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/STM32F1xx/ext_lld_isr.c 
        ${CHIBIOS}/os/hal/ports/STM32/LLD/CANv1/can_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/DACv1/dac_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c 
        ${CHIBIOS}/os/hal/ports/STM32/LLD/EXTIv1/ext_lld.c 
        ${CHIBIOS}/os/hal/ports/STM32/LLD/GPIOv1/pal_lld.c 
        ${CHIBIOS}/os/hal/ports/STM32/LLD/I2Cv1/i2c_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/RTCv1/rtc_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/SDIOv1/sdc_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/SPIv1/spi_lld.c 
        ${CHIBIOS}/os/hal/ports/STM32/LLD/TIMv1/gpt_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/TIMv1/icu_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/TIMv1/pwm_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/TIMv1/st_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/USARTv1/serial_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/USARTv1/uart_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/USBv1/usb_lld.c
        ${CHIBIOS}/os/hal/ports/STM32/LLD/xWDGv1/wdg_lld.c
    )

include_directories(
        ${CHIBIOS}/os/hal/ports/common/ARMCMx 
        ${CHIBIOS}/os/hal/ports/STM32/STM32F1xx
        ${CHIBIOS}/os/hal/ports/STM32/LLD/CANv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/DACv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/DMAv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/EXTIv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/GPIOv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/I2Cv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/RTCv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/SDIOv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/SPIv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/TIMv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/USARTv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/USBv1
        ${CHIBIOS}/os/hal/ports/STM32/LLD/xWDGv1
)

# board.mk (custom board)
##set(SOURCE_FILES ${SOURCE_FILES} ${CHIBIOS}/os/hal/boards/MAPLEMINI_STM32_F103/board.c)
##include_directories(${CHIBIOS}/os/hal/boards/MAPLEMINI_STM32_F103)

# osal.mk
set(SOURCE_FILES ${SOURCE_FILES} ${CHIBIOS}/os/hal/osal/rt/osal.c)
include_directories(${CHIBIOS}/os/hal/osal/rt)

# rt.mk

set(SOURCE_FILES ${SOURCE_FILES}
    ${CHIBIOS}/os/rt/src/chsys.c
    ${CHIBIOS}/os/rt/src/chdebug.c
    ${CHIBIOS}/os/rt/src/chvt.c
    ${CHIBIOS}/os/rt/src/chschd.c
    ${CHIBIOS}/os/rt/src/chthreads.c
    ${CHIBIOS}/os/rt/src/chtm.c
    ${CHIBIOS}/os/rt/src/chstats.c
    ${CHIBIOS}/os/rt/src/chdynamic.c
    ${CHIBIOS}/os/rt/src/chregistry.c
    ${CHIBIOS}/os/rt/src/chsem.c
    ${CHIBIOS}/os/rt/src/chmtx.c
    ${CHIBIOS}/os/rt/src/chcond.c
    ${CHIBIOS}/os/rt/src/chevents.c
    ${CHIBIOS}/os/rt/src/chmsg.c
    ${CHIBIOS}/os/rt/src/chmboxes.c
    ${CHIBIOS}/os/rt/src/chqueues.c
    ${CHIBIOS}/os/rt/src/chmemcore.c
    ${CHIBIOS}/os/rt/src/chheap.c
    ${CHIBIOS}/os/rt/src/chmempools.c
)

include_directories(${CHIBIOS}/os/rt/include)

# port_v7m.mk

# List of the ChibiOS/RT ARMv7M generic port files.
set(SOURCE_FILES ${SOURCE_FILES}
    ${CHIBIOS}/os/rt/ports/ARMCMx/chcore.c
    ${CHIBIOS}/os/rt/ports/ARMCMx/chcore_v7m.c
    ${CHIBIOS}/os/rt/ports/ARMCMx/compilers/GCC/chcoreasm_v7m.s
)

include_directories(
        ${CHIBIOS}/os/rt/ports/ARMCMx
        ${CHIBIOS}/os/rt/ports/ARMCMx/compilers/GCC
)

# Makefile

set(SOURCE_FILES ${SOURCE_FILES} 
        ${CHIBIOS}/os/various/shell.c
        ${CHIBIOS}/os/hal/lib/streams/memstreams.c
        ${CHIBIOS}/os/hal/lib/streams/chprintf.c
)

include_directories(
        ${CHIBIOS}/os/hal/lib/streams 
        ${CHIBIOS}/os/various
)

# rules.mk

