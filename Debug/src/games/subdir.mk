################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/games/games.c 

OBJS += \
./src/games/games.o 

C_DEPS += \
./src/games/games.d 


# Each subdirectory must supply rules for building sources it contributes
src/games/%.o: ../src/games/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-unknown-linux-gnu-gcc -I../glibc-oabi-toolchain-arm-generic/arm-unknown-linux-gnu/include/c++/4.3.4 -IC:/src/NEW8210/sdk8210/include -IC:/glibc-oabi-toolchain-arm-generic/arm-unknown-linux-gnu/sys-root/usr/include -IC:/src/NEW8210/sdk8210/include/directfb -IC:/glibc-oabi-toolchain-arm-generic/lib/gcc/arm-unknown-linux-gnu/4.3.4/include -IC:/src/NEW8210/fribidi/include -IC:/src/NEW8210/seos/include -O0 -g3 -Wall --sysroot="C:\glibc-oabi-toolchain-arm-generic\arm-unknown-linux-gnu\sys-root/" -Wundef -Wstrict-prototypes -Werror-implicit-function-declaration -Wdeclaration-after-statement -fsigned-char -marm -mapcs -mno-sched-prolog -mabi=apcs-gnu -mlittle-endian -mno-thumb-interwork -msoft-float -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


