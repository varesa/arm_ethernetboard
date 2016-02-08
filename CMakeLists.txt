cmake_minimum_required(VERSION 3.4)
project(arm_ethernetboard)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -std=c++11 -nostartfiles")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -O0 -g -nostartfiles")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

set(CHIBIOS deps/ChibiOS_16.1.2)
set(LWIP deps/lwip)

include_directories(
        ${CHIBIOS}/os/ext/CMSIS/include
        ${CHIBIOS}/os/ext/CMSIS/ST/STM32F1xx
)

include(cmakefiles/startup.cmake)
include(cmakefiles/chibios.cmake)
include(cmakefiles/lwip.cmake)
set(SOURCE_FILES ${SOURCE_FILES} deps/tinyprintf/tinyprintf.c)
include_directories(deps/tinyprintf)

enable_language(ASM)

add_definitions(-DSTM32F103xB)

include_directories(.)
include_directories(config/)
set(SOURCE_FILES ${SOURCE_FILES} main.c board/board.c enc28j60.c lwipthread.c enc28j60_trans.c debug.c)
add_executable(arm_ethernetboard ${SOURCE_FILES})

