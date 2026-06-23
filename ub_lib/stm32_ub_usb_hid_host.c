//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_usb_hid_host.h"

//--------------------------------------------------------------
// Global variable definitions (declared extern in header)
//--------------------------------------------------------------
USB_HID_HOST_STATUS_t USB_HID_HOST_STATUS;
uint32_t hid_autoconnect;
uint32_t hid_new_maus_data;
USB_MOUSE_DATA_t USB_MOUSE_DATA;
USB_KEY_DATA_t USB_KEY_DATA;


//--------------------------------------------------------------
// Biến Global
//--------------------------------------------------------------
USB_OTG_CORE_HANDLE          USB_OTG_Core_dev;
USBH_HOST                    USB_Host;



//--------------------------------------------------------------
// Init USB-OTG-Port or HID-HOST
// (Host f�r HID-Devices z.B. USB-Mouse oder USB-Keyboard)
//--------------------------------------------------------------
void UB_USB_HID_HOST_Init(void)
{
  USB_HID_HOST_STATUS=USB_HID_DEV_DETACHED;
  // reset aller Variabeln
  hid_autoconnect=0;
  hid_new_maus_data=0;
  USB_KEY_DATA.akt_key1=0;
  USB_KEY_DATA.akt_key2=0;
  USB_KEY_DATA.ascii_key1=0;
  USB_KEY_DATA.ascii_key2=0;
  USB_KEY_DATA.akt_shift=0;
  USB_MOUSE_DATA.btn_center=MOUSE_BTN_RELEASED;
  USB_MOUSE_DATA.btn_left=MOUSE_BTN_RELEASED;
  USB_MOUSE_DATA.btn_right=MOUSE_BTN_RELEASED;
  USB_MOUSE_DATA.xpos=USB_MOUSE_XPOS_START;
  USB_MOUSE_DATA.ypos=USB_MOUSE_YPOS_START;

  USBH_Init(&USB_OTG_Core_dev, USB_OTG_HS_CORE_ID, &USB_Host, &HID_cb, &USR_Callbacks);
}


//--------------------------------------------------------------
// USB_HOST-Funktion
// Call this function in a loop
// Return_wert :
//  USB_HID_MOUSE_CONNECTED    , USB-Mouse angeschlossen und bereit
//  USB_HID_KEYBOARD_CONNECTED , USB-Tastatur angeschlossen und bereit
//  !=                         , Device nicht bereit
//
// Hinweis : falls "USB_HID_DEV_NOT_SUPPORTED" muss die
//           Init Procedure wiederholt werden
//           (bzw ein anderes USB-Device benutzt werden)
//--------------------------------------------------------------
USB_HID_HOST_STATUS_t UB_USB_HID_HOST_Do(void)
{
  static uint32_t timeout_cnt=0;

  if(USB_HID_HOST_STATUS!=USB_HID_DEV_NOT_SUPPORTED) {
    // wenn das USB-Device nicht unterst�tzt wird,
    // wird die Funktion nicht mehr aufgerufen
    // damit die Software nicht h�ngenbleibt
    USBH_Process(&USB_OTG_Core_dev, &USB_Host);
  }

  // timeout error (falls Device zu lange nicht erkannt wurde
  if((USB_HID_HOST_STATUS==USB_HID_SPEED_OK) || (USB_HID_HOST_STATUS==USB_HID_SPEED_ERROR)) {
    timeout_cnt++;
    if(timeout_cnt>USB_HID_HOST_TIMEOUT) {
      // wenn Timeout abgelaufen -> fehler
      USB_HID_HOST_STATUS=USB_HID_DEV_NOT_SUPPORTED;
    }
  }
  else {
    timeout_cnt=0;
  }

  // autoconnect wenn HID-Device neu eingesteckt wurde
  if(USB_HID_HOST_STATUS==USB_HID_DEV_CONNECTED) {
    hid_autoconnect=1;
  }

  return(USB_HID_HOST_STATUS);
}


//--------------------------------------------------------------
// Anzhal der gedrueckten Tasten auslesen [0,1,2]
//
// ret_wert : 0  = keine Taste gedrueckt
// ret_wert : 1  = eine Taste ist gedrueckt
//                   -> USB_KEY_DATA.akt_key1 + USB_KEY_DATA.ascii_key1
// ret_wert : 2  = zwei Tasten sind gedrueckt
//                   -> USB_KEY_DATA.akt_key1 + USB_KEY_DATA.ascii_key1
//                   -> USB_KEY_DATA.akt_key2 + USB_KEY_DATA.ascii_key2
//--------------------------------------------------------------
uint8_t UB_USB_HID_HOST_GetKeyAnz(void)
{
  uint8_t ret_wert=0;

  if(USB_HID_HOST_STATUS==USB_HID_KEYBOARD_CONNECTED) {
    ret_wert=0;
    if(USB_KEY_DATA.akt_key1!=0) ret_wert=1;
    if(USB_KEY_DATA.akt_key2!=0) ret_wert=2;
  }

  return(ret_wert);
}


//--------------------------------------------------------------
// auslesen vom aktuellen Shiftwert
// ret_wert : 0           = keine Shift-Taste gedruekt
//            BIT0 (0x01) = linke Shift-Taste
//            BIT1 (0x02) = rechte Shift-Taste
//            BIT2 (0x04) = linke STRG-Taste
//            BIT3 (0x08) = linke ALT-Taste  
//            BIT4 (0x10) = rechte ALT-Taste    
//--------------------------------------------------------------
uint8_t UB_USB_HID_HOST_GetShift(void)
{
  uint8_t ret_wert=0;

  if(USB_HID_HOST_STATUS==USB_HID_KEYBOARD_CONNECTED) {
    ret_wert=USB_KEY_DATA.akt_shift;
  }

  return(ret_wert);
}


//--------------------------------------------------------------
// auslesen der aktuellen Mausdaten
// ret_wert : 0  = keine neue Mausdaten vorhanden
// ret_wert : 1  = Neue Daten in der Struktur "USB_MOUSE_DATA"
//
// USB_MOUSE_DATA.btn_left   : [MOUSE_BTN_PRESSED, MOUSE_BTN_RELEASED]
// USB_MOUSE_DATA.btn_right  : [MOUSE_BTN_PRESSED, MOUSE_BTN_RELEASED]
// USB_MOUSE_DATA.btn_center : [MOUSE_BTN_PRESSED, MOUSE_BTN_RELEASED]
// USB_MOUSE_DATA.xpos       : [0...USB_MOUSE_XPOS_MAX]
// USB_MOUSE_DATA.ypos       : [0...USB_MOUSE_YPOS_MAX]
//--------------------------------------------------------------
uint8_t UB_USB_HID_HOST_GetMouse(void)
{
  uint8_t ret_wert=0;

  if(USB_HID_HOST_STATUS==USB_HID_MOUSE_CONNECTED) {
    if(hid_new_maus_data>0) {
      ret_wert=1;
      hid_new_maus_data=0;
    }
  }

  return(ret_wert);
}


