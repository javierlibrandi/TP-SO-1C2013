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
	-cd /home/utnso/git/tp-20131c-gaturro/commons/Release && $(MAKE) clean all
	-cd /home/utnso/git/tp-20131c-gaturro/mario_para_todos/Release && $(MAKE) clean all

# Tool invocations
personaje: $(OBJS) $(USER_OBJS) /home/utnso/git/tp-20131c-gaturro/commons/Release/libcommons.so /home/utnso/git/tp-20131c-gaturro/mario_para_todos/Release/libmario_para_todos.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Release" -L"/home/utnso/git/tp-20131c-gaturro/commons/Release" -o "personaje" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) personaje
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/git/tp-20131c-gaturro/commons/Release/libcommons.so:
/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Release/libmario_para_todos.so:

-include ../makefile.targets
