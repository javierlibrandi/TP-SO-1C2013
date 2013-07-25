################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/conect_plataforma/conect_plataforma.c 

OBJS += \
./scr/conect_plataforma/conect_plataforma.o 

C_DEPS += \
./scr/conect_plataforma/conect_plataforma.d 


# Each subdirectory must supply rules for building sources it contributes
scr/conect_plataforma/%.o: ../scr/conect_plataforma/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -I"/home/utnso/git/tp-20131c-gaturro/commons" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


