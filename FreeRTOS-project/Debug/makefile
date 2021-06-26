################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: FreeRTOS-project.axf

# Tool invocations
FreeRTOS-project.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\Miguelito\ISEL\Sem_8\SE2\workspace\CMSIS_CORE_LPC17xx\Debug" -L"D:\Miguelito\ISEL\Sem_8\SE2\workspace\FreeRTOS-SE1920\Debug" -L"D:\Miguelito\ISEL\Sem_8\SE2\workspace\FreeRTOS-Kernel\Debug" -L"D:\Miguelito\ISEL\Sem_8\SE2\workspace\MQTTPacket\Debug" -Xlinker -Map="FreeRTOS-project.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "FreeRTOS-project_Debug.ld" -o "FreeRTOS-project.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) FreeRTOS-project.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "FreeRTOS-project.axf"; # arm-none-eabi-objcopy -v -O binary "FreeRTOS-project.axf" "FreeRTOS-project.bin" ; # checksum -p LPC1769 -d "FreeRTOS-project.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
