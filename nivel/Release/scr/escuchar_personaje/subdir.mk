################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/escuchar_personaje/personaje_thr.c 

OBJS += \
./scr/escuchar_personaje/personaje_thr.o 

C_DEPS += \
./scr/escuchar_personaje/personaje_thr.d 


# Each subdirectory must supply rules for building sources it contributes
scr/escuchar_personaje/%.o: ../scr/escuchar_personaje/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/git/tp-20131c-gaturro/commons" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


