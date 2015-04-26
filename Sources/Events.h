/** ###################################################################
 **     Filename    : Events.h
 **     Project     : ProcessorExpert
 **     Processor   : MKL25Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2012-07-19, 14:07, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
 **
 ** ###################################################################*/

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

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

extern uint8_t possibleFile;
extern uint8_t USBTCM_error;

bool ReadFlash(LDD_TDeviceData *DeviceData, LDD_FLASH_TAddress FromAddress, LDD_TData* DataPtr, LDD_FLASH_TDataSize Size, uint8_t* OpStatus);

bool EraseFlash(LDD_TDeviceData *DeviceData, LDD_FLASH_TAddress FromAddress, LDD_FLASH_TDataSize Size, uint8_t* OpStatus);
LDD_TError readAndSendEPD();

void Cpu_OnNMIINT(void);
/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMIINT (module Events)
 **
 **     Component   :  Cpu [MKL25Z128LK4]
 **     Description :
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the <NMI
 **         interrrupt> property is set to 'Enabled'.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */

void FLASH1_OnOperationComplete(LDD_TUserData *UserDataPtr);
/*
 ** ===================================================================
 **     Event       :  FLASH1_OnOperationComplete (module Events)
 **
 **     Component   :  FLASH1 [FLASH_LDD]
 **     Description :
 **         Called at the end of the whole write / erase operation. if
 **         the event is enabled. See SetEventMask() and GetEventMask()
 **         methods.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **       * UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 **     Returns     : Nothing
 ** ===================================================================
 */

void FLASH1_OnError(LDD_TUserData *UserDataPtr);
/*
 ** ===================================================================
 **     Event       :  FLASH1_OnError (module Events)
 **
 **     Component   :  FLASH1 [FLASH_LDD]
 **     Description :
 **         Called when an error condition occures, if the OnError event
 **         is enabled. See SetEventMask() and GetEventMask() methods.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **       * UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 **     Returns     : Nothing
 ** ===================================================================
 */

void Cpu_OnNMIINT0(void);
/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMIINT0 (module Events)
 **
 **     Component   :  Cpu [MKL25Z128LK4]
 **     Description :
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the <NMI
 **         interrrupt> property is set to 'Enabled'.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */

void TCM_OnBlockSent(LDD_TUserData *UserDataPtr);
/*
 ** ===================================================================
 **     Event       :  TCM_OnBlockSent (module Events)
 **
 **     Component   :  TCM [SPIMaster_LDD]
 **     Description :
 **         This event is called after the last character from the
 **         output buffer is moved to the transmitter. This event is
 **         available only if the SendBlock method is enabled.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **       * UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer is passed
 **                           as the parameter of Init method. 
 **     Returns     : Nothing
 ** ===================================================================
 */

void TCM_OnBlockReceived(LDD_TUserData *UserDataPtr);
/*
 ** ===================================================================
 **     Event       :  TCM_OnBlockReceived (module Events)
 **
 **     Component   :  TCM [SPIMaster_LDD]
 **     Description :
 **         This event is called when the requested number of data is
 **         moved to the input buffer. This method is available only if
 **         the ReceiveBlock method is enabled.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **       * UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer is passed
 **                           as the parameter of Init method. 
 **     Returns     : Nothing
 ** ===================================================================
 */

/*
 ** ===================================================================
 **     Event       :  TU2_OnCounterRestart (module Events)
 **
 **     Component   :  TU2 [TimerUnit_LDD]
 */
/*!
 **     @brief
 **         Called if counter overflow/underflow or counter is
 **         reinitialized by modulo or compare register matching.
 **         OnCounterRestart event and Timer unit must be enabled. See
 **         <SetEventMask> and <GetEventMask> methods. This event is
 **         available only if a <Interrupt> is enabled.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer passed as
 **                           the parameter of Init method.
 */
/* ===================================================================*/
void TU2_OnCounterRestart(LDD_TUserData *UserDataPtr);

/*
 ** ===================================================================
 **     Event       :  SPI_FLASH_OnBlockSent (module Events)
 **
 **     Component   :  SPI_FLASH [SPIMaster_LDD]
 */
/*!
 **     @brief
 **         This event is called after the last character from the
 **         output buffer is moved to the transmitter. This event is
 **         available only if the SendBlock method is enabled.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer is passed
 **                           as the parameter of Init method. 
 */
/* ===================================================================*/
void SPI_FLASH_OnBlockSent(LDD_TUserData *UserDataPtr);

/*
 ** ===================================================================
 **     Event       :  SPI_FLASH_OnBlockReceived (module Events)
 **
 **     Component   :  SPI_FLASH [SPIMaster_LDD]
 */
/*!
 **     @brief
 **         This event is called when the requested number of data is
 **         moved to the input buffer. This method is available only if
 **         the ReceiveBlock method is enabled.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer is passed
 **                           as the parameter of Init method. 
 */
/* ===================================================================*/
void SPI_FLASH_OnBlockReceived(LDD_TUserData *UserDataPtr);

void EInt1_OnInterrupt(LDD_TUserData *UserDataPtr);
/*
 ** ===================================================================
 **     Event       :  EInt1_OnInterrupt (module Events)
 **
 **     Component   :  EInt1 [ExtInt_LDD]
 **     Description :
 **         This event is called when an active signal edge/level has
 **         occurred.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **       * UserDataPtr     - Pointer to RTOS device
 **                           data structure pointer.
 **     Returns     : Nothing
 ** ===================================================================
 */

/* END Events */
#endif /* __Events_H*/

/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 0.00.00 [05.02]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
