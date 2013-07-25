################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/manjo_pantalla/nivel_p.c \
../scr/manjo_pantalla/pantalla.c \
../scr/manjo_pantalla/tad_items.c 

OBJS += \
./scr/manjo_pantalla/nivel_p.o \
./scr/manjo_pantalla/pantalla.o \
./scr/manjo_pantalla/tad_items.o 

C_DEPS += \
./scr/manjo_pantalla/nivel_p.d \
./scr/manjo_pantalla/pantalla.d \
./scr/manjo_pantalla/tad_items.d 


# Each subdirectory must supply rules for building sources it contributes
scr/manjo_pantalla/%.o: ../scr/manjo_pantalla/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/git/tp-20131c-gaturro/commons" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


