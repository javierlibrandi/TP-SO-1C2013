################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/detecto_interbloque_th/detecto_interbloque_th.c 

OBJS += \
./scr/detecto_interbloque_th/detecto_interbloque_th.o 

C_DEPS += \
./scr/detecto_interbloque_th/detecto_interbloque_th.d 


# Each subdirectory must supply rules for building sources it contributes
scr/detecto_interbloque_th/%.o: ../scr/detecto_interbloque_th/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/git/tp-20131c-gaturro/commons" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


