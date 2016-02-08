get_filename_component(LINKER_SCRIPT ${CHIBIOS}/os/common/ports/ARMCMx/compilers/GCC/ ABSOLUTE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L ${LINKER_SCRIPT} -L ${LINKER_SCRIPT}/ld/ -T STM32F103xB.ld \
    -Wl,--defsym=__process_stack_size__=0x400 -Wl,--defsym=__main_stack_size__=0x400")

## Startup

set(SOURCE_FILES ${SOURCE_FILES}
        ${CHIBIOS}/os/common/ports/ARMCMx/compilers/GCC/crt1.c
        ${CHIBIOS}/os/common/ports/ARMCMx/compilers/GCC/vectors.c
        ${CHIBIOS}/os/common/ports/ARMCMx/compilers/GCC/crt0_v7m.s
)

include_directories(
        ${CHIBIOS}/os/common/ports/ARMCMx/compilers/GCC
        ${CHIBIOS}/os/common/ports/ARMCMx/devices/STM32F1xx
)