################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pacman_lib/blinky.c \
../pacman_lib/bot.c \
../pacman_lib/clyde.c \
../pacman_lib/gui.c \
../pacman_lib/inky.c \
../pacman_lib/maze.c \
../pacman_lib/maze_generate.c \
../pacman_lib/menu.c \
../pacman_lib/pacman.c \
../pacman_lib/pinky.c \
../pacman_lib/player.c \
../pacman_lib/random.c \
../pacman_lib/skin.c 

OBJS += \
./pacman_lib/blinky.o \
./pacman_lib/bot.o \
./pacman_lib/clyde.o \
./pacman_lib/gui.o \
./pacman_lib/inky.o \
./pacman_lib/maze.o \
./pacman_lib/maze_generate.o \
./pacman_lib/menu.o \
./pacman_lib/pacman.o \
./pacman_lib/pinky.o \
./pacman_lib/player.o \
./pacman_lib/random.o \
./pacman_lib/skin.o 

C_DEPS += \
./pacman_lib/blinky.d \
./pacman_lib/bot.d \
./pacman_lib/clyde.d \
./pacman_lib/gui.d \
./pacman_lib/inky.d \
./pacman_lib/maze.d \
./pacman_lib/maze_generate.d \
./pacman_lib/menu.d \
./pacman_lib/pacman.d \
./pacman_lib/pinky.d \
./pacman_lib/player.d \
./pacman_lib/random.d \
./pacman_lib/skin.d 


# Each subdirectory must supply rules for building sources it contributes
pacman_lib/%.o pacman_lib/%.su pacman_lib/%.cyclo: ../pacman_lib/%.c pacman_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -DSTM32F4XX -DSTM32 -DSTM32F429ZITx -DSTM32F4 -c -I../Inc -I"F:/stm32_workspace/prj_cki/cmsis" -I"F:/stm32_workspace/prj_cki/cmsis_boot" -I"F:/stm32_workspace/prj_cki/cmsis_lib/include" -I"F:/stm32_workspace/prj_cki/pacman_lib" -I"F:/stm32_workspace/prj_cki/ub_lib" -I"F:/stm32_workspace/prj_cki/ub_lib/bilder" -I"F:/stm32_workspace/prj_cki/ub_lib/font" -I"F:/stm32_workspace/prj_cki/ub_lib/usb_hid_host_lolevel" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-pacman_lib

clean-pacman_lib:
	-$(RM) ./pacman_lib/blinky.cyclo ./pacman_lib/blinky.d ./pacman_lib/blinky.o ./pacman_lib/blinky.su ./pacman_lib/bot.cyclo ./pacman_lib/bot.d ./pacman_lib/bot.o ./pacman_lib/bot.su ./pacman_lib/clyde.cyclo ./pacman_lib/clyde.d ./pacman_lib/clyde.o ./pacman_lib/clyde.su ./pacman_lib/gui.cyclo ./pacman_lib/gui.d ./pacman_lib/gui.o ./pacman_lib/gui.su ./pacman_lib/inky.cyclo ./pacman_lib/inky.d ./pacman_lib/inky.o ./pacman_lib/inky.su ./pacman_lib/maze.cyclo ./pacman_lib/maze.d ./pacman_lib/maze.o ./pacman_lib/maze.su ./pacman_lib/maze_generate.cyclo ./pacman_lib/maze_generate.d ./pacman_lib/maze_generate.o ./pacman_lib/maze_generate.su ./pacman_lib/menu.cyclo ./pacman_lib/menu.d ./pacman_lib/menu.o ./pacman_lib/menu.su ./pacman_lib/pacman.cyclo ./pacman_lib/pacman.d ./pacman_lib/pacman.o ./pacman_lib/pacman.su ./pacman_lib/pinky.cyclo ./pacman_lib/pinky.d ./pacman_lib/pinky.o ./pacman_lib/pinky.su ./pacman_lib/player.cyclo ./pacman_lib/player.d ./pacman_lib/player.o ./pacman_lib/player.su ./pacman_lib/random.cyclo ./pacman_lib/random.d ./pacman_lib/random.o ./pacman_lib/random.su ./pacman_lib/skin.cyclo ./pacman_lib/skin.d ./pacman_lib/skin.o ./pacman_lib/skin.su

.PHONY: clean-pacman_lib

