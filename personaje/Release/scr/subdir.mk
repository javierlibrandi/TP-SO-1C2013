################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/funciones_personaje.c \
../scr/listenerPersonaje.c \
../scr/main_personaje.c 

OBJS += \
./scr/funciones_personaje.o \
./scr/listenerPersonaje.o \
./scr/main_personaje.o 

C_DEPS += \
./scr/funciones_personaje.d \
./scr/listenerPersonaje.d \
./scr/main_personaje.d 


# Each subdirectory must supply rules for building sources it contributes
scr/%.o: ../scr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/git/tp-20131c-gaturro/commons" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


