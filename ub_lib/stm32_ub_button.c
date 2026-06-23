//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_button.h"



//--------------------------------------------------------------
// Hàm
//--------------------------------------------------------------
#if BUTTON_USE_TIMER==1
  void P_Button_InitTIM(void);
  void P_Button_InitNVIC(void);
#endif




//--------------------------------------------------------------
// Định nghĩa các Button
// Widerstand : [GPIO_PuPd_NOPULL,GPIO_PuPd_UP,GPIO_PuPd_DOWN]
//--------------------------------------------------------------
BUTTON_t BUTTON[] = {
  // Name    ,PORT , PIN       , CLOCK              ,Widerstand      , Status
  {BTN_UP    ,GPIOC,GPIO_Pin_2  ,RCC_AHB1Periph_GPIOC,GPIO_PuPd_UP, Bit_RESET},  // PC2=Button-UP
  {BTN_RIGHT ,GPIOC,GPIO_Pin_3  ,RCC_AHB1Periph_GPIOC,GPIO_PuPd_UP, Bit_RESET},  // PC3=Button-RIGHT
  {BTN_DOWN  ,GPIOC,GPIO_Pin_4  ,RCC_AHB1Periph_GPIOC,GPIO_PuPd_UP, Bit_RESET},  // PC4=Button-DOWN
  {BTN_LEFT  ,GPIOC,GPIO_Pin_5  ,RCC_AHB1Periph_GPIOC,GPIO_PuPd_UP, Bit_RESET},  // PC5=Button-LEFT
  {BTN_CENTER, GPIOA, GPIO_Pin_0, RCC_AHB1Periph_GPIOA, GPIO_PuPd_NOPULL, Bit_RESET}   // PA0=Button-CENTER
};


//--------------------------------------------------------------
// Init aller Buttons
//--------------------------------------------------------------
void UB_Button_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  BUTTON_NAME_t btn_name;
  
  for(btn_name=0;btn_name<BUTTON_ANZ;btn_name++) {
    // Clock Enable
    RCC_AHB1PeriphClockCmd(BUTTON[btn_name].BUTTON_CLK, ENABLE);
  
    // Config als Digital-Eingang
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = BUTTON[btn_name].BUTTON_R;
    GPIO_InitStructure.GPIO_Pin = BUTTON[btn_name].BUTTON_PIN;
    GPIO_Init(BUTTON[btn_name].BUTTON_PORT, &GPIO_InitStructure);
  }

#if BUTTON_USE_TIMER==1
  // Init vom Timer
  P_Button_InitTIM();
  // Init vom NVIC
  P_Button_InitNVIC();
#endif
}


//--------------------------------------------------------------
// Đọc trạng thái từ nút
//  -> Khi nhả nút hoặc không bấm = BTN_RELEASED
//  -> Khi đang bấm   = BTN_PRESSED
//--------------------------------------------------------------
BUTTON_STATUS_t UB_Button_Read(BUTTON_NAME_t btn_name)
{
  uint8_t wert;

  wert=GPIO_ReadInputDataBit(BUTTON[btn_name].BUTTON_PORT, BUTTON[btn_name].BUTTON_PIN);
  if(wert==Bit_RESET) {
    return(BTN_RELEASED);
  }
  else {
    return(BTN_PRESSED);
  }
} 


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Button OnPressed Auswertung (entprellt)
// ret_wert, ist solange true wie der Button bet�tigt ist
//--------------------------------------------------------------
bool UB_Button_OnPressed(BUTTON_NAME_t btn_name)
{
  uint8_t wert;

  wert=BUTTON[btn_name].BUTTON_AKT;

  if(wert==Bit_RESET) {
    return(false);
  }
  else {
    return(true);
  }
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Button OnClick Auswertung (entprellt)
// ret_wert, ist nur einmal true wenn der Button bettigt wurde
//--------------------------------------------------------------
bool UB_Button_OnClick(BUTTON_NAME_t btn_name)
{
  uint8_t wert;
  static uint8_t old_wert[5] = {Bit_SET, Bit_SET, Bit_SET, Bit_SET, Bit_RESET};

  wert = GPIO_ReadInputDataBit(BUTTON[btn_name].BUTTON_PORT, BUTTON[btn_name].BUTTON_PIN);

  if (btn_name == BTN_CENTER) {
    // Active HIGH (pressed = Bit_SET)
    bool pressed = (wert == Bit_SET);
    bool old_pressed = (old_wert[btn_name] == Bit_SET);
    old_wert[btn_name] = wert;
    return (pressed && !old_pressed);
  } else {
    // Active LOW (pressed = Bit_RESET)
    bool pressed = (wert == Bit_RESET);
    bool old_pressed = (old_wert[btn_name] == Bit_RESET);
    old_wert[btn_name] = wert;
    return (pressed && !old_pressed);
  }
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Button OnRelease Auswertung (entprellt)
// ret_wert, ist nur einmal true wenn der Button losgelassen wurde
//--------------------------------------------------------------
bool UB_Button_OnRelease(BUTTON_NAME_t btn_name)
{
  uint8_t wert,old;
  static uint8_t old_wert[BUTTON_ANZ];

  wert=BUTTON[btn_name].BUTTON_AKT;
  old=old_wert[btn_name];
  old_wert[btn_name]=wert;

  if(wert!=Bit_RESET) {
    return(false);
  }
  else if(old==Bit_RESET) {
    return(false);
  }
  else {
    return(true);
  }
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// interne Funktion
// init vom Timer
//--------------------------------------------------------------
void P_Button_InitTIM(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  // Clock enable
  RCC_APB1PeriphClockCmd(UB_BUTTON_TIM_CLK, ENABLE);

  // Timer init
  TIM_TimeBaseStructure.TIM_Period =  UB_BUTTON_TIM_PERIODE;
  TIM_TimeBaseStructure.TIM_Prescaler = UB_BUTTON_TIM_PRESCALE;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(UB_BUTTON_TIM, &TIM_TimeBaseStructure);

  // Timer enable
  TIM_ARRPreloadConfig(UB_BUTTON_TIM, ENABLE);
  TIM_Cmd(UB_BUTTON_TIM, ENABLE);
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// interne Funktion
// init vom NVIC
//--------------------------------------------------------------
void P_Button_InitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  //---------------------------------------------
  // init vom Timer Interrupt
  //---------------------------------------------
  TIM_ITConfig(UB_BUTTON_TIM,TIM_IT_Update,ENABLE);

  // NVIC konfig
  NVIC_InitStructure.NVIC_IRQChannel = UB_BUTTON_TIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// ISR vom Timer
//--------------------------------------------------------------
void UB_BUTTON_TIM_ISR_HANDLER(void)
{
  BUTTON_NAME_t btn_name;
  uint8_t wert;

  // es gibt hier nur eine Interrupt Quelle
  TIM_ClearITPendingBit(UB_BUTTON_TIM, TIM_IT_Update);

  // alle Buttons einlesen
  // Note: buttons use pull-up (active LOW), so invert:
  //   Pin LOW  (pressed)  -> BUTTON_AKT = Bit_SET
  //   Pin HIGH (released) -> BUTTON_AKT = Bit_RESET
  for(btn_name=0;btn_name<BUTTON_ANZ;btn_name++) {
    wert=GPIO_ReadInputDataBit(BUTTON[btn_name].BUTTON_PORT, BUTTON[btn_name].BUTTON_PIN);
    if(wert == Bit_RESET) {
      BUTTON[btn_name].BUTTON_AKT = Bit_SET;   // pressed
    } else {
      BUTTON[btn_name].BUTTON_AKT = Bit_RESET; // released
    }
  }
}
#endif
