################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/startup_stm32wl55jcix.s 

OBJS += \
./Application/startup_stm32wl55jcix.o 

S_DEPS += \
./Application/startup_stm32wl55jcix.d 


# Each subdirectory must supply rules for building sources it contributes
Application/startup_stm32wl55jcix.o: ../Application/startup_stm32wl55jcix.s Application/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I../../Application/Core/Inc -I../../Core/Radio -I../../Sensor -x assembler-with-cpp -MMD -MP -MF"Application/startup_stm32wl55jcix.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

