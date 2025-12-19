################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra_gen/can_app_thread.c \
../ra_gen/common_data.c \
../ra_gen/filex_thread.c \
../ra_gen/hal_data.c \
../ra_gen/main.c \
../ra_gen/pin_data.c \
../ra_gen/rtt_thread.c \
../ra_gen/vector_data.c 

C_DEPS += \
./ra_gen/can_app_thread.d \
./ra_gen/common_data.d \
./ra_gen/filex_thread.d \
./ra_gen/hal_data.d \
./ra_gen/main.d \
./ra_gen/pin_data.d \
./ra_gen/rtt_thread.d \
./ra_gen/vector_data.d 

OBJS += \
./ra_gen/can_app_thread.o \
./ra_gen/common_data.o \
./ra_gen/filex_thread.o \
./ra_gen/hal_data.o \
./ra_gen/main.o \
./ra_gen/pin_data.o \
./ra_gen/rtt_thread.o \
./ra_gen/vector_data.o 

SREC += \
FileX_block_media_qspi_LevelX_ek_ra6m3_ep.srec 

MAP += \
FileX_block_media_qspi_LevelX_ek_ra6m3_ep.map 


# Each subdirectory must supply rules for building sources it contributes
ra_gen/%.o: ../ra_gen/%.c
	$(file > $@.in,-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM4 -D_RA_ORDINAL=1 -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/src" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/api" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/instances" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/src/rm_threadx_port" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/threadx/common/inc" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_gen" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/bsp" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/tx" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/ports" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/filex/common/inc" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/levelx/common" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/fx" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/ra_gen" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/middleware" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/lx" -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/driver" -I"." -I"C:/Users/User/Downloads/r20an0560eu0165-ek-ra6m3-exampleprojects/ek_ra6m3/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

