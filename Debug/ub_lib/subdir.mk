################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ub_lib/stm32_ub_button.c \
../ub_lib/stm32_ub_buzzer.c \
../ub_lib/stm32_ub_font.c \
../ub_lib/stm32_ub_graphic2d.c \
../ub_lib/stm32_ub_joystick.c \
../ub_lib/stm32_ub_lcd_ili9341.c \
../ub_lib/stm32_ub_sdram.c \
../ub_lib/stm32_ub_spi5.c \
../ub_lib/stm32_ub_systick.c \
../ub_lib/stm32_ub_uart.c 

OBJS += \
./ub_lib/stm32_ub_button.o \
./ub_lib/stm32_ub_buzzer.o \
./ub_lib/stm32_ub_font.o \
./ub_lib/stm32_ub_graphic2d.o \
./ub_lib/stm32_ub_joystick.o \
./ub_lib/stm32_ub_lcd_ili9341.o \
./ub_lib/stm32_ub_sdram.o \
./ub_lib/stm32_ub_spi5.o \
./ub_lib/stm32_ub_systick.o \
./ub_lib/stm32_ub_uart.o 

C_DEPS += \
./ub_lib/stm32_ub_button.d \
./ub_lib/stm32_ub_buzzer.d \
./ub_lib/stm32_ub_font.d \
./ub_lib/stm32_ub_graphic2d.d \
./ub_lib/stm32_ub_joystick.d \
./ub_lib/stm32_ub_lcd_ili9341.d \
./ub_lib/stm32_ub_sdram.d \
./ub_lib/stm32_ub_spi5.d \
./ub_lib/stm32_ub_systick.d \
./ub_lib/stm32_ub_uart.d 


# Each subdirectory must supply rules for building sources it contributes
ub_lib/%.o ub_lib/%.su ub_lib/%.cyclo: ../ub_lib/%.c ub_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis_boot" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis_lib/include" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/pacman_lib" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/bilder" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ub_lib

clean-ub_lib:
	-$(RM) ./ub_lib/stm32_ub_button.cyclo ./ub_lib/stm32_ub_button.d ./ub_lib/stm32_ub_button.o ./ub_lib/stm32_ub_button.su ./ub_lib/stm32_ub_buzzer.cyclo ./ub_lib/stm32_ub_buzzer.d ./ub_lib/stm32_ub_buzzer.o ./ub_lib/stm32_ub_buzzer.su ./ub_lib/stm32_ub_font.cyclo ./ub_lib/stm32_ub_font.d ./ub_lib/stm32_ub_font.o ./ub_lib/stm32_ub_font.su ./ub_lib/stm32_ub_graphic2d.cyclo ./ub_lib/stm32_ub_graphic2d.d ./ub_lib/stm32_ub_graphic2d.o ./ub_lib/stm32_ub_graphic2d.su ./ub_lib/stm32_ub_joystick.cyclo ./ub_lib/stm32_ub_joystick.d ./ub_lib/stm32_ub_joystick.o ./ub_lib/stm32_ub_joystick.su ./ub_lib/stm32_ub_lcd_ili9341.cyclo ./ub_lib/stm32_ub_lcd_ili9341.d ./ub_lib/stm32_ub_lcd_ili9341.o ./ub_lib/stm32_ub_lcd_ili9341.su ./ub_lib/stm32_ub_sdram.cyclo ./ub_lib/stm32_ub_sdram.d ./ub_lib/stm32_ub_sdram.o ./ub_lib/stm32_ub_sdram.su ./ub_lib/stm32_ub_spi5.cyclo ./ub_lib/stm32_ub_spi5.d ./ub_lib/stm32_ub_spi5.o ./ub_lib/stm32_ub_spi5.su ./ub_lib/stm32_ub_systick.cyclo ./ub_lib/stm32_ub_systick.d ./ub_lib/stm32_ub_systick.o ./ub_lib/stm32_ub_systick.su ./ub_lib/stm32_ub_uart.cyclo ./ub_lib/stm32_ub_uart.d ./ub_lib/stm32_ub_uart.o ./ub_lib/stm32_ub_uart.su

.PHONY: clean-ub_lib

