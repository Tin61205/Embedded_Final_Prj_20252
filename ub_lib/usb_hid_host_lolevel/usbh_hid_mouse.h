//--------------------------------------------------------------
// File     : usbh_hid_mouse.h
//--------------------------------------------------------------



#ifndef __USBH_HID_MOUSE_H
#define __USBH_HID_MOUSE_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_hid_core.h"


typedef struct _HID_MOUSE_Data
{
  uint8_t              x; 
  uint8_t              y;
  uint8_t              z;               /* Not Supported */ 
  uint8_t              button; 
}
HID_MOUSE_Data_TypeDef;



extern HID_cb_TypeDef HID_MOUSE_cb;
extern HID_MOUSE_Data_TypeDef	 HID_MOUSE_Data;

void  USR_MOUSE_Init (void);
void  USR_MOUSE_ProcessData (HID_MOUSE_Data_TypeDef *data);


#endif /* __USBH_HID_MOUSE_H */

