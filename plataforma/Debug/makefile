################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/planificador/subdir.mk
-include src/orquestador/subdir.mk
-include src/inotify/subdir.mk
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
all: plataforma

dependents:
	-cd /home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug && $(MAKE) clean all
	-cd /home/utnso/git/tp-20131c-gaturro/commons/Debug && $(MAKE) clean all

# Tool invocations
plataforma: $(OBJS) $(USER_OBJS) /home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug/libmario_para_todos.so /home/utnso/git/tp-20131c-gaturro/commons/Debug/libcommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-20131c-gaturro/commons/Debug" -L"/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug" -L"/home/utnso/git/tp-20131c-gaturro/memoria/Debug" -L"/home/utnso/git/tp-20131c-gaturro/so-commons-library/Debug" -o "plataforma" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) plataforma
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/git/tp-20131c-gaturro/mario_para_todos/Debug/libmario_para_todos.so:
/home/utnso/git/tp-20131c-gaturro/commons/Debug/libcommons.so:

-include ../makefile.targets
