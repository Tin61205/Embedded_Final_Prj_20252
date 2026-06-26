//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_systick.h"

//--------------------------------------------------------------
// Global variable definitions (declared extern in header)
//--------------------------------------------------------------
uint32_t  Gui_Touch_Timer_ms;
uint32_t  Mode_Systic_Timer_ms;
uint32_t  Player_Systick_Timer_ms;
uint32_t  Player2_Systick_Timer_ms;
uint32_t  Blinky_Systic_Timer_ms;
uint32_t  Pinky_Systic_Timer_ms;
uint32_t  Inky_Systic_Timer_ms;
uint32_t  Clyde_Systic_Timer_ms;
uint32_t  UB_Game_Timers_Paused = 0;



#if ((SYSTICK_RESOLUTION!=1) && (SYSTICK_RESOLUTION!=1000))
  #error print WRONG SYSTICK RESOLUTION !
#endif


//--------------------------------------------------------------
// Globale Pausen-Variabeln
//--------------------------------------------------------------
static volatile uint32_t Systick_Delay;  // Globaler Pausenzaehler





//--------------------------------------------------------------
// init counter for keyboard
//--------------------------------------------------------------
void UB_Systick_Init(void) {
  RCC_ClocksTypeDef RCC_Clocks;

  // alle Variabeln zur�cksetzen
  Systick_Delay=0;
  Player_Systick_Timer_ms=0;
  Player2_Systick_Timer_ms=0;
  UB_Game_Timers_Paused=0;
  Gui_Touch_Timer_ms=0;
  Blinky_Systic_Timer_ms=0;
  Mode_Systic_Timer_ms=0;


  #if SYSTICK_RESOLUTION==1
    // Timer 1us 
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000000);
  #else
    // Timer 1ms
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  #endif
}


#if SYSTICK_RESOLUTION==1
//--------------------------------------------------------------
// Chức năng tạm dừng (tính bằng us)
// CPU chờ cho đến khi thời gian hết
//--------------------------------------------------------------
void UB_Systick_Pause_us(volatile uint32_t pause)
{

  Systick_Delay = pause;

  while(Systick_Delay != 0);
}
#endif


//--------------------------------------------------------------
// Pause (in ms)
// CPU wait for time to expire
//--------------------------------------------------------------
void UB_Systick_Pause_ms(volatile uint32_t pause)
{
  #if SYSTICK_RESOLUTION==1
    uint32_t ms;

    for(ms=0;ms<pause;ms++) {
      UB_Systick_Pause_us(1000);
    }
  #else
    Systick_Delay = pause;

    while(Systick_Delay != 0);
  #endif
}


//--------------------------------------------------------------
// Chức năng tạm dừng (tính bằng giây)
// CPU chờ cho đến khi thời gian hết
//--------------------------------------------------------------
void UB_Systick_Pause_s(volatile uint32_t pause)
{
  uint32_t s;

  for(s=0;s<pause;s++) {
    UB_Systick_Pause_ms(1000);
  }
}


//--------------------------------------------------------------
// Systic-Interrupt
//--------------------------------------------------------------
void SysTick_Handler(void)
{
  extern volatile uint32_t UB_Buzzer_Timer_ms;
  extern void UB_Buzzer_Off(void);
  
  if (UB_Buzzer_Timer_ms != 0) {
    UB_Buzzer_Timer_ms--;
    if (UB_Buzzer_Timer_ms == 0) {
      UB_Buzzer_Off();
    }
  }

  // Tick for Pause
  if(Systick_Delay != 0x00) {
    Systick_Delay--;
  }

  if(Gui_Touch_Timer_ms!=0) {
    Gui_Touch_Timer_ms--;
  }

  if(UB_Game_Timers_Paused == 0) {
    if(Mode_Systic_Timer_ms!=0) {
      Mode_Systic_Timer_ms--;
    }

    if(Player_Systick_Timer_ms!=0) {
      Player_Systick_Timer_ms--;
    }

    if(Player2_Systick_Timer_ms!=0) {
      Player2_Systick_Timer_ms--;
    }

    if(Blinky_Systic_Timer_ms!=0) {
      Blinky_Systic_Timer_ms--;
    }

    if(Pinky_Systic_Timer_ms!=0) {
      Pinky_Systic_Timer_ms--;
    }

    if(Inky_Systic_Timer_ms!=0) {
      Inky_Systic_Timer_ms--;
    }

    if(Clyde_Systic_Timer_ms!=0) {
      Clyde_Systic_Timer_ms--;
    }
  }
}


