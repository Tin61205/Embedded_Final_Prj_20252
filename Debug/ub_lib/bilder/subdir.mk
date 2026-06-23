################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ub_lib/bilder/Skin1.c 

OBJS += \
./ub_lib/bilder/Skin1.o 

C_DEPS += \
./ub_lib/bilder/Skin1.d 


# Each subdirectory must supply rules for building sources it contributes
ub_lib/bilder/%.o ub_lib/bilder/%.su ub_lib/bilder/%.cyclo: ../ub_lib/bilder/%.c ub_lib/bilder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"F:/stm32_workspace/prj_cki/cmsis" -I"F:/stm32_workspace/prj_cki/cmsis_boot" -I"F:/stm32_workspace/prj_cki/cmsis_lib/include" -I"F:/stm32_workspace/prj_cki/pacman_lib" -I"F:/stm32_workspace/prj_cki/ub_lib" -I"F:/stm32_workspace/prj_cki/ub_lib/bilder" -I"F:/stm32_workspace/prj_cki/ub_lib/font" -I"F:/stm32_workspace/prj_cki/ub_lib/usb_hid_host_lolevel" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ub_lib-2f-bilder

clean-ub_lib-2f-bilder:
	-$(RM) ./ub_lib/bilder/Skin1.cyclo ./ub_lib/bilder/Skin1.d ./ub_lib/bilder/Skin1.o ./ub_lib/bilder/Skin1.su

.PHONY: clean-ub_lib-2f-bilder

