//--------------------------------------------------------------
// File     : usbh_usr.h
//--------------------------------------------------------------



#ifndef __USH_USR_H__
#define __USH_USR_H__


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_core.h"
#include "usb_conf.h"
#include <stdio.h>
#include "usbh_hid_core.h"




extern  USBH_Usr_cb_TypeDef USR_Callbacks;


//-----------------------------------------------------------------------------
// Bewegungsgrenzen und Startwerte (X,Y,Z) der Maus
// (Maximum = 65535)
// (START muss kleiner als MAX sein)
//-----------------------------------------------------------------------------
#define  USB_MOUSE_XPOS_MAX    239
#define  USB_MOUSE_XPOS_START  0

#define  USB_MOUSE_YPOS_MAX    319
#define  USB_MOUSE_YPOS_START  0




//--------------------------------------------------------------
void USBH_USR_Init(void);
void USBH_USR_DeviceAttached(void);
void USBH_USR_ResetDevice(void);
void USBH_USR_DeviceDisconnected (void);
void USBH_USR_OverCurrentDetected (void);
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed); 
void USBH_USR_Device_DescAvailable(void *);
void USBH_USR_DeviceAddressAssigned(void);
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc);
void USBH_USR_Manufacturer_String(void *);
void USBH_USR_Product_String(void *);
void USBH_USR_SerialNum_String(void *);
void USBH_USR_EnumerationDone(void);
USBH_USR_Status USBH_USR_UserInput(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceNotSupported(void);
void USBH_USR_UnrecoveredError(void);


#endif /* __USBH_USR_H */

