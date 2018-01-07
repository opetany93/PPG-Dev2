################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/STM32L4xx_Nucleo/stm32l4xx_nucleo.c 

OBJS += \
./Utilities/STM32L4xx_Nucleo/stm32l4xx_nucleo.o 

C_DEPS += \
./Utilities/STM32L4xx_Nucleo/stm32l4xx_nucleo.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/STM32L4xx_Nucleo/%.o: ../Utilities/STM32L4xx_Nucleo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32L4 -DSTM32L452RETxP -DNUCLEO_L452RE_P -DDEBUG -DSTM32L452xx -DUSE_HAL_DRIVER -DUSE_STM32L4XX_NUCLEO_64_SMPS -DUSE_ADP5301ACBZ -I"D:/Programowanie/AC6_workspace/PPG-Dev2_v0.1/HAL_Driver/Inc/Legacy" -I"D:/Programowanie/AC6_workspace/PPG-Dev2_v0.1/inc" -I"D:/Programowanie/AC6_workspace/PPG-Dev2_v0.1/CMSIS/device" -I"D:/Programowanie/AC6_workspace/PPG-Dev2_v0.1/CMSIS/core" -I"D:/Programowanie/AC6_workspace/PPG-Dev2_v0.1/HAL_Driver/Inc" -I"D:/Programowanie/AC6_workspace/PPG-Dev2_v0.1/Utilities/STM32L4xx_Nucleo" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


