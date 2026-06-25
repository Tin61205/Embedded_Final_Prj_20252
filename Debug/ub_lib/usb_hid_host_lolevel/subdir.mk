################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ub_lib/usb_hid_host_lolevel/stm32_button_user.c \
../ub_lib/usb_hid_host_lolevel/usb_bsp.c \
../ub_lib/usb_hid_host_lolevel/usb_core.c \
../ub_lib/usb_hid_host_lolevel/usb_hcd.c \
../ub_lib/usb_hid_host_lolevel/usb_hcd_int.c \
../ub_lib/usb_hid_host_lolevel/usbh_core.c \
../ub_lib/usb_hid_host_lolevel/usbh_hcs.c \
../ub_lib/usb_hid_host_lolevel/usbh_hid_core.c \
../ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.c \
../ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.c \
../ub_lib/usb_hid_host_lolevel/usbh_ioreq.c \
../ub_lib/usb_hid_host_lolevel/usbh_stdreq.c \
../ub_lib/usb_hid_host_lolevel/usbh_usr.c 

OBJS += \
./ub_lib/usb_hid_host_lolevel/stm32_button_user.o \
./ub_lib/usb_hid_host_lolevel/usb_bsp.o \
./ub_lib/usb_hid_host_lolevel/usb_core.o \
./ub_lib/usb_hid_host_lolevel/usb_hcd.o \
./ub_lib/usb_hid_host_lolevel/usb_hcd_int.o \
./ub_lib/usb_hid_host_lolevel/usbh_core.o \
./ub_lib/usb_hid_host_lolevel/usbh_hcs.o \
./ub_lib/usb_hid_host_lolevel/usbh_hid_core.o \
./ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.o \
./ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.o \
./ub_lib/usb_hid_host_lolevel/usbh_ioreq.o \
./ub_lib/usb_hid_host_lolevel/usbh_stdreq.o \
./ub_lib/usb_hid_host_lolevel/usbh_usr.o 

C_DEPS += \
./ub_lib/usb_hid_host_lolevel/stm32_button_user.d \
./ub_lib/usb_hid_host_lolevel/usb_bsp.d \
./ub_lib/usb_hid_host_lolevel/usb_core.d \
./ub_lib/usb_hid_host_lolevel/usb_hcd.d \
./ub_lib/usb_hid_host_lolevel/usb_hcd_int.d \
./ub_lib/usb_hid_host_lolevel/usbh_core.d \
./ub_lib/usb_hid_host_lolevel/usbh_hcs.d \
./ub_lib/usb_hid_host_lolevel/usbh_hid_core.d \
./ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.d \
./ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.d \
./ub_lib/usb_hid_host_lolevel/usbh_ioreq.d \
./ub_lib/usb_hid_host_lolevel/usbh_stdreq.d \
./ub_lib/usb_hid_host_lolevel/usbh_usr.d 


# Each subdirectory must supply rules for building sources it contributes
ub_lib/usb_hid_host_lolevel/%.o ub_lib/usb_hid_host_lolevel/%.su ub_lib/usb_hid_host_lolevel/%.cyclo: ../ub_lib/usb_hid_host_lolevel/%.c ub_lib/usb_hid_host_lolevel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis_boot" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/cmsis_lib/include" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/pacman_lib" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/bilder" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/font" -I"F:/git_nhung_prj/Embedded_Final_Prj_20252/ub_lib/usb_hid_host_lolevel" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ub_lib-2f-usb_hid_host_lolevel

clean-ub_lib-2f-usb_hid_host_lolevel:
	-$(RM) ./ub_lib/usb_hid_host_lolevel/stm32_button_user.cyclo ./ub_lib/usb_hid_host_lolevel/stm32_button_user.d ./ub_lib/usb_hid_host_lolevel/stm32_button_user.o ./ub_lib/usb_hid_host_lolevel/stm32_button_user.su ./ub_lib/usb_hid_host_lolevel/usb_bsp.cyclo ./ub_lib/usb_hid_host_lolevel/usb_bsp.d ./ub_lib/usb_hid_host_lolevel/usb_bsp.o ./ub_lib/usb_hid_host_lolevel/usb_bsp.su ./ub_lib/usb_hid_host_lolevel/usb_core.cyclo ./ub_lib/usb_hid_host_lolevel/usb_core.d ./ub_lib/usb_hid_host_lolevel/usb_core.o ./ub_lib/usb_hid_host_lolevel/usb_core.su ./ub_lib/usb_hid_host_lolevel/usb_hcd.cyclo ./ub_lib/usb_hid_host_lolevel/usb_hcd.d ./ub_lib/usb_hid_host_lolevel/usb_hcd.o ./ub_lib/usb_hid_host_lolevel/usb_hcd.su ./ub_lib/usb_hid_host_lolevel/usb_hcd_int.cyclo ./ub_lib/usb_hid_host_lolevel/usb_hcd_int.d ./ub_lib/usb_hid_host_lolevel/usb_hcd_int.o ./ub_lib/usb_hid_host_lolevel/usb_hcd_int.su ./ub_lib/usb_hid_host_lolevel/usbh_core.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_core.d ./ub_lib/usb_hid_host_lolevel/usbh_core.o ./ub_lib/usb_hid_host_lolevel/usbh_core.su ./ub_lib/usb_hid_host_lolevel/usbh_hcs.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_hcs.d ./ub_lib/usb_hid_host_lolevel/usbh_hcs.o ./ub_lib/usb_hid_host_lolevel/usbh_hcs.su ./ub_lib/usb_hid_host_lolevel/usbh_hid_core.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_hid_core.d ./ub_lib/usb_hid_host_lolevel/usbh_hid_core.o ./ub_lib/usb_hid_host_lolevel/usbh_hid_core.su ./ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.d ./ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.o ./ub_lib/usb_hid_host_lolevel/usbh_hid_keybd.su ./ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.d ./ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.o ./ub_lib/usb_hid_host_lolevel/usbh_hid_mouse.su ./ub_lib/usb_hid_host_lolevel/usbh_ioreq.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_ioreq.d ./ub_lib/usb_hid_host_lolevel/usbh_ioreq.o ./ub_lib/usb_hid_host_lolevel/usbh_ioreq.su ./ub_lib/usb_hid_host_lolevel/usbh_stdreq.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_stdreq.d ./ub_lib/usb_hid_host_lolevel/usbh_stdreq.o ./ub_lib/usb_hid_host_lolevel/usbh_stdreq.su ./ub_lib/usb_hid_host_lolevel/usbh_usr.cyclo ./ub_lib/usb_hid_host_lolevel/usbh_usr.d ./ub_lib/usb_hid_host_lolevel/usbh_usr.o ./ub_lib/usb_hid_host_lolevel/usbh_usr.su

.PHONY: clean-ub_lib-2f-usb_hid_host_lolevel

