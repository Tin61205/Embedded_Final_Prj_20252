//--------------------------------------------------------------
// File     : stm32_ub_usb_hid_host.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_USB_HIDHOST_H
#define __STM32F4_UB_USB_HIDHOST_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "usbh_usr.h"


#define   USB_HID_HOST_TIMEOUT   500000 // nicht kleiner machen !!


//--------------------------------------------------------------
// Status der USB-Verbindung
//--------------------------------------------------------------
typedef enum {
  USB_HID_HOST_NO_INIT =0,   // USB-Schnittstelle noch nicht initialisiert
  USB_HID_DEV_DETACHED,      // kein Device angeschlossen
  USB_HID_SPEED_OK,          // USB-Speed wird unterst�tzt
  USB_HID_SPEED_ERROR,       // USB-Speed wird nicht unterst�tzt
  USB_HID_DEV_NOT_SUPPORTED, // Device wird nicht unters�tzt
  USB_HID_OVER_CURRENT,      // �berstrom erkannt
  USB_HID_DEV_CONNECTED,     // Device verbunden noch nicht bereit
  USB_HID_MOUSE_CONNECTED,   // Mouse verbunden und bereit
  USB_HID_KEYBOARD_CONNECTED // Tastatur verbunden und bereit
}USB_HID_HOST_STATUS_t;
extern USB_HID_HOST_STATUS_t USB_HID_HOST_STATUS;



//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
extern uint32_t hid_autoconnect;
extern uint32_t hid_new_maus_data;



//--------------------------------------------------------------
typedef enum {
  MOUSE_BTN_RELEASED =0,      // Maus Taste losgelassen
  MOUSE_BTN_PRESSED           // Maus Taste bet�tigt
}USB_HID_BTN;


//--------------------------------------------------------------
// Globale Struktur der Mouse-Daten
//--------------------------------------------------------------
typedef struct {
  USB_HID_BTN btn_left;     // Status der linken Maustaste
  USB_HID_BTN btn_right;    // Status der rechten Maustaste
  USB_HID_BTN btn_center;   // Status der mittleren Maustaste
  uint16_t xpos;            // aktuelle X-Pos
  uint16_t ypos;            // aktuelle Y-Pos
}USB_MOUSE_DATA_t;
extern USB_MOUSE_DATA_t USB_MOUSE_DATA;


//--------------------------------------------------------------
// Globale Struktur der Keyboard-Daten
//--------------------------------------------------------------
typedef struct {
  uint8_t akt_key1;         // Tastencode der Taste 1
  uint8_t akt_key2;         // Tastencode der Taste 2
  uint8_t ascii_key1;       // Asciicode der Taste 1
  uint8_t ascii_key2;       // Asciicode der Taste 2
  uint8_t akt_shift;        // Shift-Status
}USB_KEY_DATA_t;
extern USB_KEY_DATA_t USB_KEY_DATA;


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_USB_HID_HOST_Init(void);
USB_HID_HOST_STATUS_t UB_USB_HID_HOST_Do(void);
uint8_t UB_USB_HID_HOST_GetKeyAnz(void);
uint8_t UB_USB_HID_HOST_GetShift(void);
uint8_t UB_USB_HID_HOST_GetMouse(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_USB_HIDHOST_H
