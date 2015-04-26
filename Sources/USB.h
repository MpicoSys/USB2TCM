/** ###################################################################
**     Filename  : USB.h
**     Project   : ProcessorExpert
**     Processor : MK40DX256ZVMD10
**     Component : Events
**     Version   : Driver 01.00
**     Compiler  : CodeWarrior ARM C Compiler
**     Date/Time : 2011-06-29, 15:04, # CodeGen: 4
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         USB_LDD_OnDeviceReset       - void USB_LDD_OnDeviceReset(LDD_TUserData *UserDataPtr);
**         USB_LDD_OnDeviceSpeedDetect - void USB_LDD_OnDeviceSpeedDetect(LDD_TUserData *UserDataPtr,...
**         USB_LDD_OnDeviceSuspend     - void USB_LDD_OnDeviceSuspend(LDD_TUserData *UserDataPtr);
**         USB_LDD_OnDeviceResume      - void USB_LDD_OnDeviceResume(LDD_TUserData *UserDataPtr);
**         USB_LDD_OnDeviceSetupPacket - void USB_LDD_OnDeviceSetupPacket(LDD_TUserData *UserDataPtr, uint8_t EpNum);
**         USB_LDD_OnDeviceError       - void USB_LDD_OnDeviceError(LDD_TUserData *UserDataPtr, uint8_t ErrorSource,...
**
** ###################################################################*/

#ifndef __USB_H
#define __USB_H
/* MODULE USB */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "USB1.h"
#include "CsIO1.h"
#include "IO1.h"
#include "TCM.h"
#include "TCM_CS.h"
#include "TCM_EN.h"
#include "Busy.h"
#include "LED.h"
#include "TU2.h"
#include "WAIT.h"
#include "GPIO1.h"
#include "SPI_FLASH.h"
#include "FLASH_CS.h"
#include "EInt1.h"
#include "PE_LDD.h"

extern uint8_t USBMounted;


#define EP_OUT_NUM     1               /* Output endpoint number */
#define EP_OUT_SIZE  32U               /* Output endpoint size */

#define EP_INP_NUM     1               /* Input endpoint number */
#define EP_INP_SIZE  32U               /* Input endpoint number */

#if defined(CPU_FAMILY_Kinetis)
  #define ToLittleEndian16(X) X
  #define ToLittleEndian32(X) X
  #define ToBigEndian16(X) ((uint16_t)((((uint16_t)X >> 8)&0xFFU) | (((uint16_t)X << 8)&0xFF00U)))
  #define ToBigEndian32(X) (uint32_t)(((X & 0xFFUL)<<24U)|((X & 0xFF00UL)<<8U)|((X & 0xFF0000UL)>>8U)|((X & 0xFF000000UL)>>24U))
#elif defined(CPU_FAMILY_ColdFirePlus)  
  #define ToLittleEndian16(X) ((uint16_t)((((uint16_t)X >> 8)&0xFFU) | (((uint16_t)X << 8)&0xFF00U)))
  #define ToLittleEndian32(X) (uint32_t)(((X & 0xFFUL)<<24U)|((X & 0xFF00UL)<<8U)|((X & 0xFF0000UL)>>8U)|((X & 0xFF000000UL)>>24U))
  #define ToBigEndian16(X) X
  #define ToBigEndian32(X) X
#else 
  #error "Endian convert family isn't defined in USB.h module!!!"  
#endif

typedef void (LDD_Flash_Device_TOperationDoneCallback)(LDD_TDeviceData *DevDataPtr, struct LDD_USB_Device_TTD_Struct *TrParamPtr);

typedef enum {
  RECEIVE_DATA,
  ERASE_FLASH,
  WRITE_DATA,
  OPERATION_COMPLETE
} TFlashOperationMode;            

typedef struct _MSD {
  LDD_TDeviceData         *USBDeviceDataPtr;
  LDD_TDeviceData         *FlashDeviceDataPtr;
  uint32_t                 LogBlockAddr;
  uint16_t                 TransferLength;   
  uint16_t                 CurrentTransfer;  
  TFlashOperationMode      FlashOperationMode; 
  bool                     FlashOnError;
  bool                     FlashOnOperationComplete;   
  LDD_Flash_Device_TOperationDoneCallback *CallbackFnPtr;  
} TMSD,*TMSDPtr;

extern TMSD MassStorageDevice;

void USB_LDD_OnDeviceSetupPacket(LDD_TUserData *UserDataPtr, uint8_t EpNum);
/*
** ===================================================================
**     Event       :  USB_LDD_OnDeviceSetupPacket (module USB)
**
**     Component   :  USB_LDD [USB_LDD]
**     Description :
**         Called after the setup packet is received into the internal
**         buffer, OnSetupPacket() event is enabled and USB module is
**         enabled. See SetEventMask() and GetEventMask() methods. Use
**         the method GetSetupPacket() to copy setup packet to the user
**         buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**         EpNum           - Endpoint number.
**     Returns     : Nothing
** ===================================================================
*/

void USB_LDD_OnDeviceError(LDD_TUserData *UserDataPtr, uint8_t ErrorSource, uint8_t ErrorCode);
/*
** ===================================================================
**     Event       :  USB_LDD_OnDeviceError (module USB)
**
**     Component   :  USB_LDD [USB_LDD]
**     Description :
**         Called after the Error is detected, OnError() event is
**         enabled and USB module is enabled. See SetEventMask() and
**         GetEventMask() methods.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**         ErrorSource     - Error code. HW specific.
**         ErrorCode       - 
**     Returns     : Nothing
** ===================================================================
*/

void MS_InitDevices(void);
/*
** ===================================================================
**     Method      :  MS_InitDevices (User method)
**
**     Description :
**         Initialize USB + FLASH device.
** ===================================================================
*/

LDD_TError CmdHandler(LDD_TUserData *UserDataPtr, LDD_USB_TSDP *pSDP); 
/*
** ===================================================================
**     Method      :  CmdHandler (User method)
**
**     Description :
**         Decode mass storage request received in the setup packet.
** ===================================================================
*/

void USB_LDD_OnDeviceSpeedDetect(LDD_TUserData *UserDataPtr, LDD_USB_TBusSpeed CurrentSpeed);
/*
** ===================================================================
**     Event       :  USB_LDD_OnDeviceSpeedDetect (module USB)
**
**     Component   :  USB_LDD [USB_LDD]
**     Description :
**         Called after the device detects bus speed, OnBusSpedDetect()
**         event is enabled and USB module is enabled. See
**         SetEventMask() and GetEventMask() methods. EP 0 should be
**         enabled and configured after the USB speed is known.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**         CurrentSpeed    - Current device speed.
**     Returns     : Nothing
** ===================================================================
*/

void USB_SendZlp(LDD_TUserData *UserDataPtr);
/*
** ===================================================================
**     Method      :  USB_SendZlp (User method)
**
**     Description :
**         Send zero length packet through EP0 to the Host device.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**     Returns     : Nothing
** ===================================================================
*/

void OnSetUSBConfiguration(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD);
/*
** ===================================================================
**     Method      :  OnSetUSBConfiguration (User method)
**
**     Description :
**         Set receive request for out EP (for receive mass storage 
**         command transport).
** ===================================================================
*/

void USB1_OnDeviceSpeedDetect(LDD_TUserData *UserDataPtr, LDD_USB_TBusSpeed CurrentSpeed);
/*
** ===================================================================
**     Event       :  USB1_OnDeviceSpeedDetect (module USB)
**
**     Component   :  USB1 [USB_LDD]
**     Description :
**         Called after the device detects bus speed, OnBusSpedDetect()
**         event is enabled and USB module is enabled. See
**         SetEventMask() and GetEventMask() methods. EP 0 should be
**         enabled and configured after the USB speed is known.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**         CurrentSpeed    - Current device speed.
**     Returns     : Nothing
** ===================================================================
*/

void USB1_OnDeviceSetupPacket(LDD_TUserData *UserDataPtr, uint8_t EpNum);
/*
** ===================================================================
**     Event       :  USB1_OnDeviceSetupPacket (module USB)
**
**     Component   :  USB1 [USB_LDD]
**     Description :
**         Called after the setup packet is received into the internal
**         buffer, OnSetupPacket() event is enabled and USB module is
**         enabled. See SetEventMask() and GetEventMask() methods. Use
**         the method GetSetupPacket() to copy setup packet to the user
**         buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**         EpNum           - Endpoint number.
**     Returns     : Nothing
** ===================================================================
*/

void USB1_OnDeviceError(LDD_TUserData *UserDataPtr, uint8_t ErrorSource, uint8_t ErrorCode);
/*
** ===================================================================
**     Event       :  USB1_OnDeviceError (module USB)
**
**     Component   :  USB1 [USB_LDD]
**     Description :
**         Called after the Error is detected, OnError() event is
**         enabled and USB module is enabled. See SetEventMask() and
**         GetEventMask() methods.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**         ErrorSource     - Error code. HW specific.
**         ErrorCode       - 
**     Returns     : Nothing
** ===================================================================
*/

/*
** ===================================================================
**     Event       :  USB1_OnDeviceSuspend (module USB)
**
**     Component   :  USB1 [USB_LDD]
*/
/*!
**     @brief
**         Called after the suspend condition is met, OnDeviceSuspend()
**         event is enabled and USB module is enabled. See
**         SetEventMask() and GetEventMask() methods.
**     @param
**         UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
*/
/* ===================================================================*/
void USB1_OnDeviceSuspend(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  USB1_OnDeviceReset (module USB)
**
**     Component   :  USB1 [USB_LDD]
*/
/*!
**     @brief
**         Called after the reset signal is detected on the USB bus,
**         OnDeviceReset() event is enabled and USB module is enabled.
**         See SetEventMask() and GetEventMask() methods. All pending
**         transfers are cancelled and all EPs are disabled before this
**         event is called.  Device address is set to "0". 
**     @param
**         UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
*/
/* ===================================================================*/
void USB1_OnDeviceReset(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  USB1_OnDeviceResume (module USB)
**
**     Component   :  USB1 [USB_LDD]
*/
/*!
**     @brief
**         Called after the resume signaling is done, OnDeviceResume()
**         event is enabled and USB module is enabled. See
**         SetEventMask() and GetEventMask() methods.
**     @param
**         UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
*/
/* ===================================================================*/
void USB1_OnDeviceResume(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  USB1_OnDeviceSof (module USB)
**
**     Component   :  USB1 [USB_LDD]
*/
/*!
**     @brief
**         Called after the Sof is received, OnSof() event is enabled
**         and USB module is enabled. See SetEventMask() and
**         GetEventMask() methods.
**     @param
**         UserDataPtr     - User data pointer. This
**                           pointer is passed as the parameter of Init
**                           method.
**     @param
**         FrameNumber     - Current frame number.
*/
/* ===================================================================*/
void USB1_OnDeviceSof(LDD_TUserData *UserDataPtr, uint16_t FrameNumber);

/* END USB */
#endif /* __USB_H*/

/*
** ###################################################################
**
**     This file was created by Processor Expert 0.00.00 [05.00]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
