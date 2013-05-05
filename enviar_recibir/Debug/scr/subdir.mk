################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scr/Socket.c \
../scr/Socket_Cliente.c \
../scr/Socket_Servidor.c \
../scr/main.c 

OBJS += \
./scr/Socket.o \
./scr/Socket_Cliente.o \
./scr/Socket_Servidor.o \
./scr/main.o 

C_DEPS += \
./scr/Socket.d \
./scr/Socket_Cliente.d \
./scr/Socket_Servidor.d \
./scr/main.d 


# Each subdirectory must supply rules for building sources it contributes
scr/%.o: ../scr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


