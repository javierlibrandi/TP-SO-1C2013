################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include scr/subdir.mk
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
all: personaje

dependents:
	-cd /home/utnso/git/tp-20131c-gaturro/commons/Debug && $(MAKE) all
	-cd /home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug && $(MAKE) all

# Tool invocations
personaje: $(OBJS) $(USER_OBJS) /home/utnso/git/tp-20131c-gaturro/commons/Debug/libcommons.so /home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug/libmario_para_todos.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug" -L"/home/utnso/git/tp-20131c-gaturro/commons/Debug" -o "personaje" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) personaje
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/git/tp-20131c-gaturro/commons/Debug/libcommons.so:
/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug/libmario_para_todos.so:

-include ../makefile.targets
