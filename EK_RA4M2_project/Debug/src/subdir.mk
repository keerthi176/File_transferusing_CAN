################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bootloader.c \
../src/hal_entry.c \
../src/hal_warmstart.c 

C_DEPS += \
./src/bootloader.d \
./src/hal_entry.d \
./src/hal_warmstart.d 

CREF += \
ex_1.cref 

OBJS += \
./src/bootloader.o \
./src/hal_entry.o \
./src/hal_warmstart.o 

MAP += \
ex_1.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0 -funsigned-char -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Waggregate-return -Wno-parentheses-equality -Wfloat-equal -g3 -std=c99 -fshort-enums -fno-unroll-loops -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra_gen" -I"." -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra_cfg\\fsp_cfg\\bsp" -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra_cfg\\fsp_cfg" -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\src" -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra\\fsp\\inc" -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra\\fsp\\inc\\api" -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra\\fsp\\inc\\instances" -I"D:\\ModularmProject\\Modularm_Workspace\\CAN_ex1\\ex_1\\ra\\arm\\CMSIS_6\\CMSIS\\Core\\Include" -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -x c "$<" -c -o "$@")
	@clang --target=arm-none-eabi @"$@.in"

