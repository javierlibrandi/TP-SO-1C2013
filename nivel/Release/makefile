################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include scr/manjo_pantalla/subdir.mk
-include scr/escuchar_personaje/subdir.mk
-include scr/detecto_interbloque_th/subdir.mk
-include scr/conect_plataforma/subdir.mk
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
all: nivel

# Tool invocations
nivel: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Release" -L"/home/utnso/git/tp-20131c-gaturro/commons/Release" -o "nivel" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) nivel
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
