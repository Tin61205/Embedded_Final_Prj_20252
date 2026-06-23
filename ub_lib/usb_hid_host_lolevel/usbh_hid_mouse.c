//--------------------------------------------------------------
// File     : usbh_hid_mouse.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_hid_mouse.h"






static void  MOUSE_Init (void);
static void  MOUSE_Decode(uint8_t *data);


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
 #pragma pack(4) 
#endif



 
HID_MOUSE_Data_TypeDef HID_MOUSE_Data;
HID_cb_TypeDef HID_MOUSE_cb = 
{
  MOUSE_Init,
  MOUSE_Decode,
};

//--------------------------------------------------------------
static void  MOUSE_Init ( void)
{
 USR_MOUSE_Init();
}
     
       
//--------------------------------------------------------------
static void  MOUSE_Decode(uint8_t *data)
{
  HID_MOUSE_Data.button = data[0];

  HID_MOUSE_Data.x      = data[1];
  HID_MOUSE_Data.y      = data[2];
  
  USR_MOUSE_ProcessData(&HID_MOUSE_Data);

}
