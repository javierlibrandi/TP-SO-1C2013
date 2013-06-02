################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/nivel.c 

OBJS += \
./scr/nivel.o 

C_DEPS += \
./scr/nivel.d 


# Each subdirectory must supply rules for building sources it contributes
scr/nivel.o: ../scr/nivel.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Dropbox/git-note-lg/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/Dropbox/git-note-lg/tp-20131c-gaturro/commons" -I"/home/utnso/Dropbox/git-note-lg/tp-20131c-gaturro/commons/Debug" -I"/home/utnso/Dropbox/git-note-lg/tp-20131c-gaturro/mario_para_todos/Debug" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"scr/nivel.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


