################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/inotify/inotify_thr.c 

OBJS += \
./src/inotify/inotify_thr.o 

C_DEPS += \
./src/inotify/inotify_thr.d 


# Each subdirectory must supply rules for building sources it contributes
src/inotify/%.o: ../src/inotify/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-20131c-gaturro/commons" -I"/home/utnso/git/tp-20131c-gaturro/memoria" -I"/home/utnso/git/tp-20131c-gaturro/so-commons-library" -I"/home/utnso/git/tp-20131c-gaturro/mario_para_todos" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


