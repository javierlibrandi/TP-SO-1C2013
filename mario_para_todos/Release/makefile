################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include mario_para_todos/comunicacion/subdir.mk
-include mario_para_todos/subdir.mk
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
all: libmario_para_todos.so

dependents:
	-cd /home/utnso/git/tp-20131c-gaturro/commons/Release && $(MAKE) all

# Tool invocations
libmario_para_todos.so: $(OBJS) $(USER_OBJS) /home/utnso/git/tp-20131c-gaturro/commons/Release/libcommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-20131c-gaturro/commons/Release" -shared -o "libmario_para_todos.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES) libmario_para_todos.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/git/tp-20131c-gaturro/commons/Release/libcommons.so:

-include ../makefile.targets
