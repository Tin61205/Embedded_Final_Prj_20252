################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ub_lib/bilder/Skin1.c \
../ub_lib/bilder/background_menu.c 

OBJS += \
./ub_lib/bilder/Skin1.o \
./ub_lib/bilder/background_menu.o 

C_DEPS += \
./ub_lib/bilder/Skin1.d \
./ub_lib/bilder/background_menu.d 


# Each subdirectory must supply rules for building sources it contributes
ub_lib/bilder/%.o ub_lib/bilder/%.su ub_lib/bilder/%.cyclo: ../ub_lib/bilder/%.c ub_lib/bilder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis_boot" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis_lib/include" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/pacman_lib" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/bilder" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ub_lib-2f-bilder

clean-ub_lib-2f-bilder:
	-$(RM) ./ub_lib/bilder/Skin1.cyclo ./ub_lib/bilder/Skin1.d ./ub_lib/bilder/Skin1.o ./ub_lib/bilder/Skin1.su ./ub_lib/bilder/background_menu.cyclo ./ub_lib/bilder/background_menu.d ./ub_lib/bilder/background_menu.o ./ub_lib/bilder/background_menu.su

.PHONY: clean-ub_lib-2f-bilder

