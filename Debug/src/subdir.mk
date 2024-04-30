################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MCNEW_POS.c \
../src/auth.c \
../src/cJSON.c \
../src/constants.c \
../src/gui.c \
../src/helpers.c \
../src/lkd.c \
../src/modalYesNo.c \
../src/print.c 

OBJS += \
./src/MCNEW_POS.o \
./src/auth.o \
./src/cJSON.o \
./src/constants.o \
./src/gui.o \
./src/helpers.o \
./src/lkd.o \
./src/modalYesNo.o \
./src/print.o 

C_DEPS += \
./src/MCNEW_POS.d \
./src/auth.d \
./src/cJSON.d \
./src/constants.d \
./src/gui.d \
./src/helpers.d \
./src/lkd.d \
./src/modalYesNo.d \
./src/print.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-unknown-linux-gnu-gcc -I../glibc-oabi-toolchain-arm-generic/arm-unknown-linux-gnu/include/c++/4.3.4 -IC:/src/NEW8210/sdk8210/include -IC:/glibc-oabi-toolchain-arm-generic/arm-unknown-linux-gnu/sys-root/usr/include -IC:/src/NEW8210/sdk8210/include/directfb -IC:/glibc-oabi-toolchain-arm-generic/lib/gcc/arm-unknown-linux-gnu/4.3.4/include -IC:/src/NEW8210/fribidi/include -IC:/src/NEW8210/seos/include -O0 -g3 -Wall --sysroot="C:\glibc-oabi-toolchain-arm-generic\arm-unknown-linux-gnu\sys-root/" -Wundef -Wstrict-prototypes -Werror-implicit-function-declaration -Wdeclaration-after-statement -fsigned-char -marm -mapcs -mno-sched-prolog -mabi=apcs-gnu -mlittle-endian -mno-thumb-interwork -msoft-float -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


