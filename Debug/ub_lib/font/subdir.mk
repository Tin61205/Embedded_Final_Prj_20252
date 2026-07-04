################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ub_lib/font/ub_font_arial_7x10.c 

OBJS += \
./ub_lib/font/ub_font_arial_7x10.o 

C_DEPS += \
./ub_lib/font/ub_font_arial_7x10.d 


# Each subdirectory must supply rules for building sources it contributes
ub_lib/font/%.o ub_lib/font/%.su ub_lib/font/%.cyclo: ../ub_lib/font/%.c ub_lib/font/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"d:/Embedded_Final_Prj_20252/cmsis" -I"d:/Embedded_Final_Prj_20252/cmsis_boot" -I"d:/Embedded_Final_Prj_20252/cmsis_lib/include" -I"d:/Embedded_Final_Prj_20252/pacman_lib" -I"d:/Embedded_Final_Prj_20252/ub_lib" -I"d:/Embedded_Final_Prj_20252/ub_lib/bilder" -I"d:/Embedded_Final_Prj_20252/ub_lib/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ub_lib-2f-font

clean-ub_lib-2f-font:
	-$(RM) ./ub_lib/font/ub_font_arial_7x10.cyclo ./ub_lib/font/ub_font_arial_7x10.d ./ub_lib/font/ub_font_arial_7x10.o ./ub_lib/font/ub_font_arial_7x10.su

.PHONY: clean-ub_lib-2f-font

