//--------------------------------------------------------------
// File     : usbh_usr.c
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include "stm32_ub_usb_hid_host.h"


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void HID_MOUSE_UpdatePosition (int8_t x, int8_t y);
void HID_MOUSE_ButtonPressed(uint8_t button_idx);
void HID_MOUSE_ButtonReleased(uint8_t button_idx);


//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
int16_t akt_xp=USB_MOUSE_XPOS_START;
int16_t akt_yp=USB_MOUSE_YPOS_START;






USBH_Usr_cb_TypeDef USR_Callbacks =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  NULL,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};



//--------------------------------------------------------------
// wird einmal beim Init vom USB-Host aufgerufen
//--------------------------------------------------------------
void USBH_USR_Init(void)
{
  // leer
}


//--------------------------------------------------------------
// wird beim stecken vom Device aufgerufen
//--------------------------------------------------------------
void USBH_USR_DeviceAttached(void)
{   
  USB_HID_HOST_STATUS=USB_HID_DEV_DETACHED;
}


//--------------------------------------------------------------
void USBH_USR_UnrecoveredError (void)
{
  USB_HID_HOST_STATUS=USB_HID_DEV_DETACHED;
}


//--------------------------------------------------------------
// wird beim ziehen vom Device aufgerufen
//--------------------------------------------------------------
void USBH_USR_DeviceDisconnected (void)
{
  USB_HID_HOST_STATUS=USB_HID_DEV_DETACHED;
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
//--------------------------------------------------------------
void USBH_USR_ResetDevice(void)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if ((DeviceSpeed != HPRT0_PRTSPD_FULL_SPEED)&&(DeviceSpeed != HPRT0_PRTSPD_LOW_SPEED))
  {
    USB_HID_HOST_STATUS=USB_HID_SPEED_ERROR;
  }
  else {
    USB_HID_HOST_STATUS=USB_HID_SPEED_OK;
  }
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_DeviceAddressAssigned(void)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Product_String(void *ProductString)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  // leer
} 


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
//--------------------------------------------------------------
void USBH_USR_EnumerationDone(void)
{
  USB_HID_HOST_STATUS=USB_HID_DEV_CONNECTED;
} 


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// wenn das Device nicht untrestützt wird
//--------------------------------------------------------------
void USBH_USR_DeviceNotSupported(void)
{
  USB_HID_HOST_STATUS=USB_HID_DEV_NOT_SUPPORTED;
}  


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
//--------------------------------------------------------------
USBH_USR_Status USBH_USR_UserInput(void)
{
  
  USBH_USR_Status usbh_usr_status;
  
  usbh_usr_status = USBH_USR_NO_RESP;  

  // check ob USB-Device verbunden werden soll
  if(hid_autoconnect != 0)
  {
    // verbindet das USB-Device
    usbh_usr_status = USBH_USR_RESP_OK;
    hid_autoconnect=0;  
  }

  return usbh_usr_status;
  
} 


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// wenn die Überstrom Überwachung angeschlagen hat
//--------------------------------------------------------------
void USBH_USR_OverCurrentDetected (void)
{
  USB_HID_HOST_STATUS=USB_HID_OVER_CURRENT;
}


//--------------------------------------------------------------
// wird beim ziehen vom Device aufgerufen
//--------------------------------------------------------------
void USBH_USR_DeInit(void)
{
  USB_HID_HOST_STATUS=USB_HID_DEV_DETACHED;
}


//--------------------------------------------------------------
// wird einmal beim Init der USB-Maus aufgerufen
//--------------------------------------------------------------
void USR_MOUSE_Init(void)
{
  USB_HID_HOST_STATUS=USB_HID_MOUSE_CONNECTED;
  USB_MOUSE_DATA.btn_center=MOUSE_BTN_RELEASED;
  USB_MOUSE_DATA.btn_left=MOUSE_BTN_RELEASED;
  USB_MOUSE_DATA.btn_right=MOUSE_BTN_RELEASED;
  USB_MOUSE_DATA.xpos=USB_MOUSE_XPOS_START;
  USB_MOUSE_DATA.ypos=USB_MOUSE_YPOS_START;
  akt_xp=USB_MOUSE_XPOS_START;
  akt_yp=USB_MOUSE_YPOS_START;
  hid_new_maus_data=0;
}


//--------------------------------------------------------------
// wird jedesmal aufgerufen, wenn die USB-Maus Daten
// gesendet hat
//--------------------------------------------------------------
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{
  
  uint8_t idx = 1;   
  static uint8_t b_state[3] = { 0, 0 , 0};
  
  if ((data->x != 0) && (data->y != 0))
  {
    HID_MOUSE_UpdatePosition(data->x , data->y);
  }
  
  for ( idx = 0 ; idx < 3 ; idx ++)
  {
    
    if(data->button & 1 << idx) 
    {
      if(b_state[idx] == 0)
      {
        HID_MOUSE_ButtonPressed (idx);
        b_state[idx] = 1;
      }
    }
    else
    {
      if(b_state[idx] == 1)
      {
        HID_MOUSE_ButtonReleased (idx);
        b_state[idx] = 0;
      }
    }
  }
}


//--------------------------------------------------------------
// wird einmal beim Init der USB-Tastatur aufgerufen
//--------------------------------------------------------------
void  USR_KEYBRD_Init (void)
{
  USB_HID_HOST_STATUS=USB_HID_KEYBOARD_CONNECTED;
  USB_KEY_DATA.akt_key1=0;
  USB_KEY_DATA.akt_key2=0;
  USB_KEY_DATA.ascii_key1=0;
  USB_KEY_DATA.ascii_key2=0;
  USB_KEY_DATA.akt_shift=0;
}


//--------------------------------------------------------------
// wird jedesmal aufgerufen, wenn eine Taste
// an der USB-Tastatur gedrückt wurde
//--------------------------------------------------------------
void  USR_KEYBRD_ProcessData (uint8_t data)
{
  // Tastenwert speichern
  USB_KEY_DATA.akt_key1=data;
}

//--------------------------------------------------------------
// wird jedesmal aufgerufen, wenn eine Taste
// an der USB-Tastatur gedrückt wurde
//--------------------------------------------------------------
void  USR_UB_KEYBRD_ProcessData (uint8_t k1, uint8_t k2, uint8_t a1, uint8_t a2, uint8_t shift)
{
  // Tastenwerte speichern
  USB_KEY_DATA.akt_key1=k1;
  USB_KEY_DATA.akt_key2=k2;
  USB_KEY_DATA.ascii_key1=a1;
  USB_KEY_DATA.ascii_key2=a2;
  USB_KEY_DATA.akt_shift=shift;
}


//--------------------------------------------------------------
// berechnet neue Maus X/Y-Koordinaten
// wird jedesmal aufgerufen, wenn eine Taste gedrückt wurde
//--------------------------------------------------------------
void HID_MOUSE_UpdatePosition (int8_t x, int8_t y)
{
  if ((x != 0) || (y != 0))
  {
    akt_xp+=x/2;
    akt_yp+=y/2;

    if(akt_xp>USB_MOUSE_XPOS_MAX) akt_xp=USB_MOUSE_XPOS_MAX;
    if(akt_xp<0) akt_xp=0;

    if(akt_yp>USB_MOUSE_YPOS_MAX) akt_yp=USB_MOUSE_YPOS_MAX;
    if(akt_yp<0) akt_yp=0;

    // neue Positionen speichern
    USB_MOUSE_DATA.xpos=(uint16_t)(akt_xp);
    USB_MOUSE_DATA.ypos=(uint16_t)(akt_yp);

    hid_new_maus_data=1;
  }
}


//--------------------------------------------------------------
void HID_MOUSE_ButtonPressed(uint8_t button_idx)
{
  switch (button_idx)
  {
    case 0 :
      USB_MOUSE_DATA.btn_left=MOUSE_BTN_PRESSED;
    break;
    case 1 :
      USB_MOUSE_DATA.btn_right=MOUSE_BTN_PRESSED;
    break;
    case 2 :
      USB_MOUSE_DATA.btn_center=MOUSE_BTN_PRESSED;
    break;
  }
  hid_new_maus_data=1;
}


//--------------------------------------------------------------
void HID_MOUSE_ButtonReleased(uint8_t button_idx)
{

  switch (button_idx)
  {
    case 0 :
      USB_MOUSE_DATA.btn_left=MOUSE_BTN_RELEASED;
    break;
    case 1 :
      USB_MOUSE_DATA.btn_right=MOUSE_BTN_RELEASED;
    break;
    case 2 :
      USB_MOUSE_DATA.btn_center=MOUSE_BTN_RELEASED;
    break;
  }
  hid_new_maus_data=1;
}


