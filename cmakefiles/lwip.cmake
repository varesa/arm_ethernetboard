# Chibios Bindings

set(SOURCE_FILES ${SOURCE_FILES} ${CHIBIOS}/os/various/lwip_bindings/arch/sys_arch.c)

include_directories(${CHIBIOS}/os/various/lwip_bindings/arch/)

# lwip

aux_source_directory(${LWIP}/src/core/      SRC_LWIP_CORE)
aux_source_directory(${LWIP}/src/core/ipv4/  SRC_LWIP_V4)
aux_source_directory(${LWIP}/src/api/       SRC_LWIP_API)

set(SOURCE_FILES ${SOURCE_FILES} ${SRC_LWIP_CORE} ${SRC_LWIP_V4} ${SRC_LWIP_API} ${LWIP}/src/netif/etharp.c)

include_directories(${CHIBIOS}/os/various/lwip_bindings)
include_directories(${LWIP}/src/include)
include_directories(${LWIP}/src/include/ipv4)