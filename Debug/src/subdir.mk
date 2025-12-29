################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/can_thread_entry.c \
../src/common_utils.c \
../src/filex_thread_entry.c \
../src/hal_entry.c \
../src/rtt_thread_entry.c \
../src/setup_qspi.c \
../src/ymodem.c \
../src/ymodem_port.c 

C_DEPS += \
./src/can_thread_entry.d \
./src/common_utils.d \
./src/filex_thread_entry.d \
./src/hal_entry.d \
./src/rtt_thread_entry.d \
./src/setup_qspi.d \
./src/ymodem.d \
./src/ymodem_port.d 

OBJS += \
./src/can_thread_entry.o \
./src/common_utils.o \
./src/filex_thread_entry.o \
./src/hal_entry.o \
./src/rtt_thread_entry.o \
./src/setup_qspi.o \
./src/ymodem.o \
./src/ymodem_port.o 

SREC += \
FileX_block_media_qspi_LevelX_ek_ra6m3_ep.srec 

MAP += \
FileX_block_media_qspi_LevelX_ek_ra6m3_ep.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM4 -D_RA_ORDINAL=1 -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/src" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/api" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/instances" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/src/rm_threadx_port" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/threadx/common/inc" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_gen" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/bsp" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/tx" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/ports" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/filex/common/inc" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/levelx/common" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/fx" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/ra_gen" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/middleware" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/lx" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/driver" -I"." -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

