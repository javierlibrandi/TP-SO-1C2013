################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/inicial.c 

OBJS += \
./scr/inicial.o 

C_DEPS += \
./scr/inicial.d 


# Each subdirectory must supply rules for building sources it contributes
scr/%.o: ../scr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/git/tp-20131c-gaturro/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

