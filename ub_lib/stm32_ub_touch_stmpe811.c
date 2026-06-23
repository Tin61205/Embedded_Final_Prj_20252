//--------------------------------------------------------------
// File     : stm32_ub_touch_stmpe811.c
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_touch_stmpe811.h"

// Global variable definition (declared extern in header)
Touch_Data_t Touch_Data;


//--------------------------------------------------------------
// internal function
//--------------------------------------------------------------
void P_Touch_Reset(void);
uint8_t P_Touch_FnctCmd(uint8_t Fct, FunctionalState NewState);
void P_Touch_Config(void);
uint16_t P_Touch_ReadID(void);
uint8_t P_Touch_IOAFConfig(uint8_t IO_Pin, FunctionalState NewState);
static uint16_t P_Touch_Read_X(void);
static uint16_t P_Touch_Read_Y(void);
uint16_t P_Touch_Read_16b(uint32_t RegisterAddr);



//--------------------------------------------------------------
// Khoi tao man hinh cam ung
// Return_wert :
//  -> ERROR   , khong tim thay cam ung
//  -> SUCCESS , tim thay
//--------------------------------------------------------------
ErrorStatus UB_Touch_Init(void)
{
  uint16_t stmpe_id=0;

  // KHoi tao I2C
  UB_I2C3_Init();

  // Kiem tra tu STMPE811
  stmpe_id=P_Touch_ReadID();
  if(stmpe_id!=STMPE811_ID) {
    return(ERROR);
  }

  // Reset touch
  P_Touch_Reset();

  // init
  P_Touch_FnctCmd(IOE_ADC_FCT, ENABLE);
  P_Touch_Config();

  return(SUCCESS);
}


//--------------------------------------------------------------
// Doc trang thai cam ung va toa do cam ung
// Return_wert :
//  -> ERROR   , khong tim thay
//  -> SUCCESS ,  ok
//
// Touch_Data.status : [TOUCH_PRESSED, TOUCH_RELEASED]
// Touch_Data.xp     : [0...239]
// Touch_Data.yp     : [0...319] 
//--------------------------------------------------------------
ErrorStatus UB_Touch_Read(void)
{
  uint32_t xDiff, yDiff , x , y;
  static uint32_t _x = 0, _y = 0;
  int16_t i2c_wert;
  
  i2c_wert=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, IOE_REG_TP_CTRL);
  if(i2c_wert<0) return(ERROR);
  
  if((i2c_wert&0x80)==0) {
    Touch_Data.status = TOUCH_RELEASED;
  }
  else {
    Touch_Data.status = TOUCH_PRESSED;
  }

  if(Touch_Data.status==TOUCH_PRESSED) {
    x = P_Touch_Read_X();
    y = P_Touch_Read_Y();
    // Kiem tra loi khi doc
    if((x==0) || (y==0)) return(ERROR);
    if((x==239) || (y==319)) return(ERROR);

    xDiff = x > _x? (x - _x): (_x - x);
    yDiff = y > _y? (y - _y): (_y - y);
    if (xDiff + yDiff > 5)
    {
      _x = x;
      _y = y;
    }
  }

  Touch_Data.xp = _x;
  Touch_Data.yp = _y;
  
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_FIFO_STA, 0x01);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_FIFO_STA, 0x00);

  return(SUCCESS);
}


//--------------------------------------------------------------
// internal function
//--------------------------------------------------------------
void P_Touch_Reset(void)
{
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_SYS_CTRL1, 0x02);

  UB_I2C3_Delay(STMPE811_DELAY);

  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_SYS_CTRL1, 0x00);
}


//--------------------------------------------------------------
// internal function
// return : 0=ok, >0 = error
//--------------------------------------------------------------
uint8_t P_Touch_FnctCmd(uint8_t Fct, FunctionalState NewState)
{
  uint8_t tmp = 0;
  int16_t i2c_wert;

  i2c_wert=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, IOE_REG_SYS_CTRL2);
  if(i2c_wert<0) return(1);

  tmp = (uint8_t)(i2c_wert);

  if (NewState != DISABLE) {
    tmp &= ~(uint8_t)Fct;
  }
  else {
    tmp |= (uint8_t)Fct;
  }
  
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_SYS_CTRL2, tmp);
  
  return(0);
}


//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
void P_Touch_Config(void)
{
  P_Touch_FnctCmd(IOE_TP_FCT, ENABLE);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_ADC_CTRL1, 0x49);
  UB_I2C3_Delay(STMPE811_DELAY);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_ADC_CTRL2, 0x01);
  P_Touch_IOAFConfig((uint8_t)TOUCH_IO_ALL, DISABLE);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_TP_CFG, 0x9A);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_FIFO_TH, 0x01);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_FIFO_STA, 0x01);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_FIFO_STA, 0x00);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_TP_FRACT_XYZ, 0x01);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_TP_I_DRIVE, 0x01);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_TP_CTRL, 0x03);
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_INT_STA, 0xFF);

  Touch_Data.status = TOUCH_RELEASED;
  Touch_Data.xp = 0;
  Touch_Data.yp = 0;
}


//--------------------------------------------------------------
// internal Function
// Read ID
//--------------------------------------------------------------
uint16_t P_Touch_ReadID(void)
{
  uint16_t tmp = 0;
  int16_t i2c_wert1, i2c_wert2;

  i2c_wert1=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, 0);
  i2c_wert2=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, 1);

  if(i2c_wert1<0) return 0;
  if(i2c_wert2<0) return 0;

  tmp = i2c_wert1;
  tmp = (uint32_t)(tmp << 8);
  tmp |= i2c_wert2;

  return (uint16_t)tmp;
}


//--------------------------------------------------------------
// internal Function
// return : 0=ok, >0 = error
//--------------------------------------------------------------
uint8_t P_Touch_IOAFConfig(uint8_t IO_Pin, FunctionalState NewState)
{
  uint8_t tmp = 0;
  int16_t i2c_wert;

  i2c_wert=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, IOE_REG_GPIO_AF);
  if(i2c_wert<0) return(1);

  tmp = i2c_wert;

  if (NewState != DISABLE) {
    tmp |= (uint8_t)IO_Pin;
  }
  else {
    tmp &= ~(uint8_t)IO_Pin;
  }
  // Gui gia tri moi cua thanh ghi GPIO_AF thong qua giao tiep I2C
  UB_I2C3_WriteByte(STMPE811_I2C_ADDR, IOE_REG_GPIO_AF, tmp);

  return(0);
}


//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
static uint16_t P_Touch_Read_X(void)
{
  int32_t x, xr;
  // Doc gia tri 16-bit tu thanh ghi TP_DATA_X va ghi vao x
  x = P_Touch_Read_16b(IOE_REG_TP_DATA_X);

  if(x <= 3000) {
    x = 3870 - x;
  }
  else {
    x = 3800 - x;
  }

  xr = x / 15;

  if(xr <= 0) {
    xr = 0;
  }
  else if (xr >= 240) {
    xr = 239;
  }

  return (uint16_t)(xr);
}


//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
static uint16_t P_Touch_Read_Y(void)
{
  int32_t y, yr;

  y = P_Touch_Read_16b(IOE_REG_TP_DATA_Y);
  y -= 360;
  yr = y / 11;

  if(yr <= 0) {
    yr = 0;
  }
  else if (yr >= 320) {
    yr = 319;
  }

  return (uint16_t)(yr);
}


//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
uint16_t P_Touch_Read_16b(uint32_t RegisterAddr)
{
  uint16_t ret_wert=0;
  int16_t i2c_wert1, i2c_wert2;

  i2c_wert1=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, RegisterAddr);
  i2c_wert2=UB_I2C3_ReadByte(STMPE811_I2C_ADDR, RegisterAddr+1);

  if(i2c_wert1<0) return 0;
  if(i2c_wert2<0) return 0;

  ret_wert=(i2c_wert1<<8)|i2c_wert2;

  return(ret_wert);
}


