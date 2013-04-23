################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/productorConsumidor.c 

OBJS += \
./scr/productorConsumidor.o 

C_DEPS += \
./scr/productorConsumidor.d 


# Each subdirectory must supply rules for building sources it contributes
scr/productorConsumidor.o: ../scr/productorConsumidor.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/commons" -O0 -g3 -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"scr/productorConsumidor.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


