################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MCNEW_POS.c \
../src/cJSON.c \
../src/constants.c \
../src/helpers.c 

OBJS += \
./src/MCNEW_POS.o \
./src/cJSON.o \
./src/constants.o \
./src/helpers.o 

C_DEPS += \
./src/MCNEW_POS.d \
./src/cJSON.d \
./src/constants.d \
./src/helpers.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-unknown-linux-gnu-gcc -I"C:\src\NEW8210\sdk8210\include" -I"C:\glibc-oabi-toolchain-arm-generic\arm-unknown-linux-gnu\sys-root\usr\include" -I"C:\src\NEW8210\sdk8210\include\directfb" -IC:\glibc-oabi-toolchain-arm-generic\lib\gcc\arm-unknown-linux-gnu\4.3.4\include -I"C:\src\NEW8210\fribidi\include" -I"C:\src\NEW8210\seos\include" -O0 -g3 -Wall --sysroot="C:\glibc-oabi-toolchain-arm-generic\arm-unknown-linux-gnu\sys-root/" -Wundef -Wstrict-prototypes -Werror-implicit-function-declaration -Wdeclaration-after-statement -fsigned-char -marm -mapcs -mno-sched-prolog -mabi=apcs-gnu -mlittle-endian -mno-thumb-interwork -msoft-float -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


