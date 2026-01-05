################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/board/ra6m3_ek/board_init.c \
../ra/board/ra6m3_ek/board_leds.c 

C_DEPS += \
./ra/board/ra6m3_ek/board_init.d \
./ra/board/ra6m3_ek/board_leds.d 

OBJS += \
./ra/board/ra6m3_ek/board_init.o \
./ra/board/ra6m3_ek/board_leds.o 

SREC += \
FileX_block_media_qspi_LevelX_ek_ra6m3_ep.srec 

MAP += \
FileX_block_media_qspi_LevelX_ek_ra6m3_ep.map 


# Each subdirectory must supply rules for building sources it contributes
ra/board/ra6m3_ek/%.o: ../ra/board/ra6m3_ek/%.c
	$(file > $@.in,-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM4 -D_RA_ORDINAL=1 -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/src" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/api" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/instances" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/src/rm_threadx_port" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/threadx/common/inc" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_gen" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/bsp" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/tx" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/fsp/inc/ports" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/filex/common/inc" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/microsoft/azure-rtos/levelx/common" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/fx" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/ra_gen" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/middleware" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/fsp_cfg/azure/lx" -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra_cfg/driver" -I"." -I"D:/ModularmProject/can_ex_repo/FileX_block_media_qspi_LevelX/FileX_block_media_qspi_LevelX_ek_ra6m3_ep/e2studio/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

