#include "stm32_button_user.h"

static void initButtonUser(void){
	GPIO_InitTypeDef GPIO_InitStructure ;

	// Enable GPIOA Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/*Configure GPIO pin : A0: User Button */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init ( GPIOA ,  & GPIO_InitStructure ) ;
}

uint8_t Read_User_Button(void){
	static uint8_t init_done = 0;
	if (init_done == 0) {
		initButtonUser();
		init_done = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) {
		return PRESSED_BUTTON_USER;
	}
	return PRESSED_BUTTON_NONE;
}
