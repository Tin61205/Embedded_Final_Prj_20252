################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cmsis_lib/source/misc.c \
../cmsis_lib/source/stm32f4xx_dma2d.c \
../cmsis_lib/source/stm32f4xx_fmc.c \
../cmsis_lib/source/stm32f4xx_gpio.c \
../cmsis_lib/source/stm32f4xx_i2c.c \
../cmsis_lib/source/stm32f4xx_ltdc.c \
../cmsis_lib/source/stm32f4xx_rcc.c \
../cmsis_lib/source/stm32f4xx_rng.c \
../cmsis_lib/source/stm32f4xx_spi.c \
../cmsis_lib/source/stm32f4xx_tim.c \
../cmsis_lib/source/stm32f4xx_usart.c 

OBJS += \
./cmsis_lib/source/misc.o \
./cmsis_lib/source/stm32f4xx_dma2d.o \
./cmsis_lib/source/stm32f4xx_fmc.o \
./cmsis_lib/source/stm32f4xx_gpio.o \
./cmsis_lib/source/stm32f4xx_i2c.o \
./cmsis_lib/source/stm32f4xx_ltdc.o \
./cmsis_lib/source/stm32f4xx_rcc.o \
./cmsis_lib/source/stm32f4xx_rng.o \
./cmsis_lib/source/stm32f4xx_spi.o \
./cmsis_lib/source/stm32f4xx_tim.o \
./cmsis_lib/source/stm32f4xx_usart.o 

C_DEPS += \
./cmsis_lib/source/misc.d \
./cmsis_lib/source/stm32f4xx_dma2d.d \
./cmsis_lib/source/stm32f4xx_fmc.d \
./cmsis_lib/source/stm32f4xx_gpio.d \
./cmsis_lib/source/stm32f4xx_i2c.d \
./cmsis_lib/source/stm32f4xx_ltdc.d \
./cmsis_lib/source/stm32f4xx_rcc.d \
./cmsis_lib/source/stm32f4xx_rng.d \
./cmsis_lib/source/stm32f4xx_spi.d \
./cmsis_lib/source/stm32f4xx_tim.d \
./cmsis_lib/source/stm32f4xx_usart.d 


# Each subdirectory must supply rules for building sources it contributes
cmsis_lib/source/%.o cmsis_lib/source/%.su cmsis_lib/source/%.cyclo: ../cmsis_lib/source/%.c cmsis_lib/source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"D:/Embedded_Final_Prj_20252/cmsis" -I"D:/Embedded_Final_Prj_20252/cmsis_boot" -I"D:/Embedded_Final_Prj_20252/cmsis_lib/include" -I"D:/Embedded_Final_Prj_20252/pacman_lib" -I"D:/Embedded_Final_Prj_20252/ub_lib" -I"D:/Embedded_Final_Prj_20252/ub_lib/bilder" -I"D:/Embedded_Final_Prj_20252/ub_lib/font" -I"D:/Embedded_Final_Prj_20252/ub_lib/usb_hid_host_lolevel" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-cmsis_lib-2f-source

clean-cmsis_lib-2f-source:
	-$(RM) ./cmsis_lib/source/misc.cyclo ./cmsis_lib/source/misc.d ./cmsis_lib/source/misc.o ./cmsis_lib/source/misc.su ./cmsis_lib/source/stm32f4xx_dma2d.cyclo ./cmsis_lib/source/stm32f4xx_dma2d.d ./cmsis_lib/source/stm32f4xx_dma2d.o ./cmsis_lib/source/stm32f4xx_dma2d.su ./cmsis_lib/source/stm32f4xx_fmc.cyclo ./cmsis_lib/source/stm32f4xx_fmc.d ./cmsis_lib/source/stm32f4xx_fmc.o ./cmsis_lib/source/stm32f4xx_fmc.su ./cmsis_lib/source/stm32f4xx_gpio.cyclo ./cmsis_lib/source/stm32f4xx_gpio.d ./cmsis_lib/source/stm32f4xx_gpio.o ./cmsis_lib/source/stm32f4xx_gpio.su ./cmsis_lib/source/stm32f4xx_i2c.cyclo ./cmsis_lib/source/stm32f4xx_i2c.d ./cmsis_lib/source/stm32f4xx_i2c.o ./cmsis_lib/source/stm32f4xx_i2c.su ./cmsis_lib/source/stm32f4xx_ltdc.cyclo ./cmsis_lib/source/stm32f4xx_ltdc.d ./cmsis_lib/source/stm32f4xx_ltdc.o ./cmsis_lib/source/stm32f4xx_ltdc.su ./cmsis_lib/source/stm32f4xx_rcc.cyclo ./cmsis_lib/source/stm32f4xx_rcc.d ./cmsis_lib/source/stm32f4xx_rcc.o ./cmsis_lib/source/stm32f4xx_rcc.su ./cmsis_lib/source/stm32f4xx_rng.cyclo ./cmsis_lib/source/stm32f4xx_rng.d ./cmsis_lib/source/stm32f4xx_rng.o ./cmsis_lib/source/stm32f4xx_rng.su ./cmsis_lib/source/stm32f4xx_spi.cyclo ./cmsis_lib/source/stm32f4xx_spi.d ./cmsis_lib/source/stm32f4xx_spi.o ./cmsis_lib/source/stm32f4xx_spi.su ./cmsis_lib/source/stm32f4xx_tim.cyclo ./cmsis_lib/source/stm32f4xx_tim.d ./cmsis_lib/source/stm32f4xx_tim.o ./cmsis_lib/source/stm32f4xx_tim.su ./cmsis_lib/source/stm32f4xx_usart.cyclo ./cmsis_lib/source/stm32f4xx_usart.d ./cmsis_lib/source/stm32f4xx_usart.o ./cmsis_lib/source/stm32f4xx_usart.su

.PHONY: clean-cmsis_lib-2f-source

