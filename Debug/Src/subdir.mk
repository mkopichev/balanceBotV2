################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/common.c \
../Src/delayMs.c \
../Src/main.c \
../Src/stepperMotor.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/userLed.c 

OBJS += \
./Src/common.o \
./Src/delayMs.o \
./Src/main.o \
./Src/stepperMotor.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/userLed.o 

C_DEPS += \
./Src/common.d \
./Src/delayMs.d \
./Src/main.d \
./Src/stepperMotor.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/userLed.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/common.cyclo ./Src/common.d ./Src/common.o ./Src/common.su ./Src/delayMs.cyclo ./Src/delayMs.d ./Src/delayMs.o ./Src/delayMs.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/stepperMotor.cyclo ./Src/stepperMotor.d ./Src/stepperMotor.o ./Src/stepperMotor.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/userLed.cyclo ./Src/userLed.d ./Src/userLed.o ./Src/userLed.su

.PHONY: clean-Src

