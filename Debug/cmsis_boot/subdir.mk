################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cmsis_boot/system_stm32f4xx.c 

OBJS += \
./cmsis_boot/system_stm32f4xx.o 

C_DEPS += \
./cmsis_boot/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
cmsis_boot/%.o cmsis_boot/%.su cmsis_boot/%.cyclo: ../cmsis_boot/%.c cmsis_boot/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"D:/Embedded_Final_Prj_20252/cmsis" -I"D:/Embedded_Final_Prj_20252/cmsis_boot" -I"D:/Embedded_Final_Prj_20252/cmsis_lib/include" -I"D:/Embedded_Final_Prj_20252/pacman_lib" -I"D:/Embedded_Final_Prj_20252/ub_lib" -I"D:/Embedded_Final_Prj_20252/ub_lib/bilder" -I"D:/Embedded_Final_Prj_20252/ub_lib/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-cmsis_boot

clean-cmsis_boot:
	-$(RM) ./cmsis_boot/system_stm32f4xx.cyclo ./cmsis_boot/system_stm32f4xx.d ./cmsis_boot/system_stm32f4xx.o ./cmsis_boot/system_stm32f4xx.su

.PHONY: clean-cmsis_boot

