/** ###################################################################
 **     Filename    : Events.c
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
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "USB.h"

uint8_t upload_image_event[255]	= {0x20, 0x01, 0x00, 128};
uint8_t upload_image2_event[2000] = {0};
uint8_t possibleFile = 0;
uint8_t USBTCM_error = 0;  //0- no error, 1-error now , 2-error last communication
uint8_t cycleCounter = 0;
TMSD MassStorageDevice;

//LDD_TDeviceData *timer;

/* User includes (#include below this line is not maintained by Processor Expert) */

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

void Cpu_OnNMIINT(void)
{
	/* Write your code here ... */
}

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
void FLASH1_OnOperationComplete(LDD_TUserData *UserDataPtr)
{
	TMSD *MassStorageDevicePtr = (TMSD *)UserDataPtr;	
	MassStorageDevicePtr->FlashOnOperationComplete = TRUE;    /* Set complete operation flag */
	if (MassStorageDevicePtr->CallbackFnPtr != NULL) {        /* If is requested callback, invoke it */
		MassStorageDevicePtr->CallbackFnPtr(UserDataPtr, NULL); /* Call notification function */
	}
}

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
void FLASH1_OnError(LDD_TUserData *UserDataPtr)
{
	TMSD *MassStorageDevicePtr = (TMSD *)UserDataPtr;	
	MassStorageDevicePtr->FlashOnError = TRUE;                /* Set complete operation flag */
	if (MassStorageDevicePtr->CallbackFnPtr != NULL) {        /* If is requested callback, invoke it */  
		MassStorageDevicePtr->CallbackFnPtr(UserDataPtr, NULL); /* Call notification function */
	}
}

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
void Cpu_OnNMIINT0(void)
{
	/* Write your code here ... */
}

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
void TCM_OnBlockSent(LDD_TUserData *UserDataPtr)
{
	/* Write your code here ... */
}

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
void TCM_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
	/* Write your code here ... */
}

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
void TU2_OnCounterRestart(LDD_TUserData *UserDataPtr)
{
	cycleCounter++;
	if(cycleCounter>=15){
		cycleCounter=0;
		possibleFile=1;
		//LDD_TError result=-1;
		LED_ClrVal(NULL);
		TU2_Disable(NULL);
	}



}

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
void SPI_FLASH_OnBlockSent(LDD_TUserData *UserDataPtr)
{
	/* Write your code here ... */
}

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
void SPI_FLASH_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
	/* Write your code here ... */
}

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
void EInt1_OnInterrupt(LDD_TUserData *UserDataPtr)
{
	possibleFile=1;
	if(USBTCM_error==1)
	{
		USBTCM_error=2;
	}
}

/* END Events */

/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 0.00.00 [05.02]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
