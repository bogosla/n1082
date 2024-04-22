################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/games/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: MCNEW_POS

# Tool invocations
MCNEW_POS: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-unknown-linux-gnu-gcc -L"C:\src\NEW8210\sdk8210\lib" -L"C:\src\NEW8210\fribidi\lib" -L"C:\src\NEW8210\seos\lib" -marm -mapcs -mno-sched-prolog -mabi=apcs-gnu -mlittle-endian -mno-thumb-interwork -msoft-float -Wl,-Map,"C:\Users\James DESTINE\workspace\MCNEW_POS\Debug/MCNEW_POS.map" -o "MCNEW_POS" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) MCNEW_POS
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
