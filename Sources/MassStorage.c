/** ###################################################################
 **     Filename  : MassStorage.c
 **     Project   : MassStorage
 **     Processor : -
 **     Compiler  : -
 **     Date/Time : 15.6.2011, 13:53
 **     Contents  :
 **         User source code
 **
 **     Copyright Freescale Semiconductor, Inc. All Rights Reserved.
 ** ###################################################################*/

/* MassStorage MassStorage */

#include "USB1.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "USB.h"
#include "MassStorage.h"
#include "MSD.h"
#include "external_memory.h"
#include <stdio.h>

/* MassStorage device command */
#define MS_SIGNATURE            ToBigEndian32(0x55534243UL) 
#define INQUIRY                 0x12U
#define REQUEST_SENCE           0x03U
#define READ_FORMAT_CAPACITIES  0x23U
#define READ_CAPACITY           0x25U
#define TEST_UNIT_READY         0x00U
#define READ_10                 0x28U
#define MODE_SENCE_6            0x1AU
#define ALLOW_MEDIUM_REMOVAL    0x1EU
#define VERIFY                  0x2FU
#define WRITE_10                0x2AU
#define RESET_MASS_STORAGE      ToBigEndian16(0x21FFU)
#define GET_MAX_LUN             ToBigEndian16(0xA1FEU)

LDD_USB_Device_TTD DataTD;
LDD_TDeviceData *timer;
uint8_t sendingPending=0;
uint8_t NoAlignBuffer[FLASH_BLOCK_SIZE+7];
#define Buffer (uint8_t*)((((uint32_t)NoAlignBuffer) + 3UL) & ~(0x3UL)) /* Address value of data buffer for USB receive method must be aligned (Address mod 4 ) */
TCBW *CBW;
TCSW CSW={ToBigEndian32(0x55534253UL),0x00000000UL,0x00000000UL, 0U};

/*
 ** ===================================================================
 **     Method      :  MS_ReadFlashMemory (User method)
 **
 **     Description :
 **         Read data block from the Flash memory and send data 
 **         to the USB device.
 ** ===================================================================
 */
void MS_ReadFlashMemory(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD)
{
	TMSD *MassStorageDevicePtr = (TMSD *)USBDevDataPtr;    
	EmStatus readStatus;
	(void)pTD;                                               /* Parameter is not used, suppress unused argument warning */

	MassStorageDevicePtr->FlashOnOperationComplete = FALSE; 
	MassStorageDevicePtr->FlashOnError = FALSE;    
	readStatus = EM_ReadData(Buffer,(FLASH_BASE_ADDR+((MassStorageDevicePtr->LogBlockAddr)*FLASH_BLOCK_SIZE)), FLASH_BLOCK_SIZE);

	if (readStatus == EM_SUCCESS) {

		MassStorageDevicePtr->FlashOnError = FALSE;
		MassStorageDevicePtr->FlashOnOperationComplete = TRUE;

	} else {
#ifdef DEBUG_PRINT
		printf("->Read data from Flash operation result: ERROR \n\r");
#endif 	  
		PE_DEBUGHALT();	  
	}
	MassStorageDevicePtr->CurrentTransfer++;                   /* Increment transfer counter */
	if (MassStorageDevicePtr->TransferLength == MassStorageDevicePtr->CurrentTransfer) { /* is last transfer? */ 
		CSW.dCSWDataResidue     = 0x00U;                       /* If yes, then send on end data transfer Command Status Wrapper */
		DataTD.CallbackFnPtr    = &SendCommandStatusWrapper;   /* Set SendCommandStatusWrapper() function pointer */  
	} else {                                                   /* If no, then repeat reading flash memory */ 
		MassStorageDevicePtr->LogBlockAddr += 1U;              /* Increment Logical Block Address value */  
		DataTD.CallbackFnPtr    = &MS_ReadFlashMemory;         /* Set MS_ReadFlashMemory() function pointer */
	}            
	DataTD.Head.EpNum      = EP_INP_NUM;                       /* Initialize receive request for EPx(Inp) */ 
	DataTD.Head.BufferPtr  = (uint8_t *)Buffer;              /* Set pointer to data buffer */           
	DataTD.Head.BufferSize = (uint16_t)FLASH_BLOCK_SIZE;     /* Set data buffer value */ 
	(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *)&DataTD); /* Send data response to the USB Host device */ 
#ifdef DEBUG_PRINT
	printf("->Read data from Flash addr 0x%x \n\r",(FLASH_BASE_ADDR+(MassStorageDevicePtr->LogBlockAddr*FLASH_BLOCK_SIZE)));
#endif    
}

/*
 ** ===================================================================
 **     Method      :  MS_WriteFlashMemory (User method)
 **
 **     Description :
 **         Receive data block from USB host (PC), erase flash memory 
 **         block and write data block to the flash memory.
 ** ===================================================================
 */
void MS_WriteFlashMemory(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD)
{
	TMSD *MassStorageDevicePtr = (TMSD *)USBDevDataPtr;    

	(void)pTD;                                               /* Parameter is not used, suppress unused argument warning */  

	switch (MassStorageDevicePtr->FlashOperationMode) {
	case RECEIVE_DATA:                                       /* Receive data from USB line */
		MassStorageDevicePtr->CurrentTransfer++;          
		DataTD.Head.EpNum       = EP_OUT_NUM;                /* Initialize receive request for EPx(Out) */ 
		DataTD.Head.BufferPtr   = (uint8_t *)Buffer;      
		DataTD.Head.BufferSize  = (uint16_t)FLASH_BLOCK_SIZE;      
		DataTD.CallbackFnPtr    = &MS_WriteFlashMemory;
		MassStorageDevicePtr->FlashOperationMode = ERASE_FLASH;
		(void)USB1_DeviceRecvData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD*) &DataTD);
#ifdef DEBUG_PRINT
		printf("->Receive Flash data\n\r");
#endif          
		break;  
	case ERASE_FLASH:                                      /* Erase Flash memory block */
#ifdef DEBUG_PRINT
		printf("->Erase Flash addr: 0x%x \n\r", (FLASH_BASE_ADDR+(MassStorageDevicePtr->LogBlockAddr*FLASH_BLOCK_SIZE)));
#endif
		MassStorageDevicePtr->FlashOperationMode = WRITE_DATA; 

		/* Erase Flash memory sector */
		EmStatus eraseResult = EM_BlockErase((FLASH_BASE_ADDR+(MassStorageDevicePtr->LogBlockAddr*FLASH_BLOCK_SIZE)),1);
		MS_WriteFlashMemory(MassStorageDevicePtr, NULL);

		if (eraseResult != EM_SUCCESS) {
#ifdef DEBUG_PRINT
			printf("->Erase memory Flash operation result: ERROR \n\r");
#endif 	  
			PE_DEBUGHALT();	  
		}      
		break;       
	case WRITE_DATA:                                                       /* Write data to the Flash memory */
#ifdef DEBUG_PRINT
		printf("->Write data to Flash addr 0x%x \n\r",(FLASH_BASE_ADDR+(MassStorageDevicePtr->LogBlockAddr*FLASH_BLOCK_SIZE)));
#endif        
		if (MassStorageDevicePtr->TransferLength == MassStorageDevicePtr->CurrentTransfer)
		{                                                                  /* is more flash memory block for write? */ 
			MassStorageDevicePtr->FlashOperationMode = OPERATION_COMPLETE; /* if no, then set operation complete and after write flash operation send status wrapper */  
		} 
		else 
		{
			MassStorageDevicePtr->FlashOperationMode = RECEIVE_DATA;       /* if yes, then repeat receive data from USB line, Flash erase, Flash write,... */
		} 
		if (sendingPending==0)
		{
			timer = TU2_Init(NULL);
			TU2_Enable(timer);
		}
		else {
			TU2_Disable(timer);
			TU2_ResetCounter(timer);
			TU2_Enable(timer);
		}
		/* Write data to the flash memory sector */
		EmStatus programResult = EM_ProgramData((FLASH_BASE_ADDR+(MassStorageDevicePtr->LogBlockAddr*FLASH_BLOCK_SIZE)),Buffer,FLASH_BLOCK_SIZE);
		MS_WriteFlashMemory(MassStorageDevicePtr, NULL);

		if (programResult != EM_SUCCESS) {
#ifdef DEBUG_PRINT
			printf("->Write memory Flash operation result: ERROR \n\r");
#endif 	  
			PE_DEBUGHALT();	  
		}       
		MassStorageDevicePtr->LogBlockAddr += 1U;          
		break;         
	case OPERATION_COMPLETE:                               /* Write operation is complete, send status wrapper */
		CSW.dCSWDataResidue = 0x00U;
		MassStorageDevicePtr->CallbackFnPtr = NULL;
		SendCommandStatusWrapper(MassStorageDevicePtr, NULL); 
		break;        
	default:
		MassStorageDevicePtr->FlashOperationMode = RECEIVE_DATA;
		break;
	}      
}

/*
 ** ===================================================================
 **     Method      :  OnSetUSBConfiguration (User method)
 **
 **     Description :
 **         Set receive request for out EP (for receive mass storage 
 **         command transport).
 ** ===================================================================
 */
void OnSetUSBConfiguration(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD)
{
	TMSD *MassStorageDevicePtr = (TMSD *)USBDevDataPtr;
	(void)pTD;                                               /* Parameter is not used, suppress unused argument warning */  

	DataTD.Head.EpNum      = EP_OUT_NUM;                     /* Initialize receive request for EP1(Out), when will be data received, call MassStorageTask routine (received data contains mass storage command) */
	DataTD.Head.BufferPtr  = (uint8_t *)Buffer;              /* Input buffer pointer */
	DataTD.Head.BufferSize = (uint16_t)sizeof(TCBW);         /* Size of input buffer */
	DataTD.Head.Flags      = LDD_USB_DEVICE_TRANSFER_FLAG_EXT_PARAM; /* TD contains extension parameter */    
	DataTD.CallbackFnPtr   = &MassStorageTask;               /* Set pointer to the MassStorage function */
	(void)USB1_DeviceRecvData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *)&DataTD);
}

/*
 ** ===================================================================
 **     Method      :  CmdHandler (User method)
 **
 **     Description :
 **         Decode mass storage request received in the setup packet.
 ** ===================================================================
 */
LDD_TError CmdHandler(LDD_TUserData *UserDataPtr, LDD_USB_TSDP *pSDP) 
{
	TMSD *MassStorageDevicePtr = (TMSD *)UserDataPtr;    
	LDD_USB_Device_TTD tmpTD;
	LDD_TError res = ERR_OK;
	bool SendResponse = FALSE;

	tmpTD.Head.EpNum      =     0x00U;
	tmpTD.Head.BufferPtr  =     NULL;
	tmpTD.Head.BufferSize =     0x01U;
	tmpTD.Head.Flags      =     0x00U;

	switch(*((uint16_t *)pSDP))                            /* Depend on request type call do: */
	{   
	case RESET_MASS_STORAGE:                               /* Mass Storage Reset command */
#ifdef DEBUG_PRINT     
		printf("  Mass Storage Reset\n\r");
#endif
		break;        
	case GET_MAX_LUN:                                      /* Get Max LUN command */
#ifdef DEBUG_PRINT    
		printf("  Get Max LUN\n\r");
#endif
		SendResponse = TRUE;
		tmpTD.Head.BufferPtr = (uint8_t*)&MaxLUN;
		tmpTD.Head.BufferSize = sizeof(MaxLUN);                                  
		break;             
	default:
		res = ERR_NOTAVAIL;
#ifdef DEBUG_PRINT    
		printf("  MassStorage - unknown command 0x%x\n\r", Endian16(*((uint16_t *)pSDP) ));
#endif
		break;    
	}    
	if (res == ERR_OK) 
	{                                       /* Is received supported mass storage request? */
		if (SendResponse) 
		{                                    /* Is awaited data response? If yes, send data packet response */
			if ((tmpTD.Head.BufferPtr != NULL) || (tmpTD.Head.BufferSize == 0U)) 
			{
				(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr, (LDD_USB_Device_TTD *)&tmpTD); /* Send response to the Host device */
			}          
		} 
		else 
		{                                               /* If no, send zero length packet = request acknowledge */
			USB_SendZlp(UserDataPtr);            
		}
	}
	return(res);                                               /* Return mass storage request result state */
}

/*
 ** ===================================================================
 **     Method      :  MassStorageTask (User method)
 **
 **     Description :
 **         Decode received mass storage packet transport.
 ** ===================================================================
 */
static void MassStorageTask(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD)
{
	TMSD *MassStorageDevicePtr = (TMSD *)USBDevDataPtr;    
	(void)pTD;                                                   /* Parameter is not used, suppress unused argument warning */  

	CBW = (TCBW *)Buffer;  
	/* Test of signature + Is request for device "0"? */
	if ((CBW->dCBWSignature == MS_SIGNATURE)&&(CBW->bCBWLUN == 0x00U)) 
	{
		if (DataTD.TransmittedDataSize )
		{
			CSW.dCSWTag = CBW->dCBWTag;                          /* Copy dCBWTag to dCSWTag */
		}  
		CSW.bCSWStatus = 0x00U;                                  /* Command Passed ("good status") */     
		DataTD.Head.EpNum       = EP_INP_NUM;
		DataTD.Head.Flags       = LDD_USB_DEVICE_TRANSFER_FLAG_EXT_PARAM;
		DataTD.CallbackFnPtr    = &SendCommandStatusWrapper;        

		switch (CBW->CBWCB[0])  
		{                                /* Decode MassStorage code command */          
		case INQUIRY:                                            /* Inquiry command */
			DataTD.Head.BufferPtr  = (uint8_t *)InquiryData;     /* Set Inquiry Data buffer response pointer */
			DataTD.Head.BufferSize = sizeof(InquiryData);
			(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *)&DataTD); /* Send command response to the Host device */        
			CSW.dCSWDataResidue = 0x00U; 
#ifdef DEBUG_PRINT
			printf("->Inquiry\n\r");
#endif
			break;                                       
		case REQUEST_SENCE:                                      /* Request Sense command */                                   
			DataTD.Head.BufferPtr  = (uint8_t *)SenseData;       /* Set Sense Data buffer response pointer */
			DataTD.Head.BufferSize = sizeof(SenseData);                 
			(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *) &DataTD); /* Send command response to the Host device */
			CSW.dCSWDataResidue = 0x00U;    
#ifdef DEBUG_PRINT
			printf("->RequestSense\n\r");
#endif
			break;                                     
		case READ_FORMAT_CAPACITIES:                             /* Read Format Capacities command */                                              
			DataTD.Head.BufferPtr  = (uint8_t *)FormatCapacity;  /* Set Capacity List buffer response pointer */                  
			DataTD.Head.BufferSize = sizeof(FormatCapacity);       
			(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *)&DataTD); /* Send command response to the Host device */
			CSW.dCSWDataResidue = 0x00U;     
#ifdef DEBUG_PRINT
			printf("->Read Format Capacities\n\r");
#endif
			break;                                  
		case READ_CAPACITY:                                      /* Read Capacity command */
			DataTD.Head.BufferPtr  = (uint8_t *)&ReadCapacity;   /* Set Read Capacity buffer response pointer */
			DataTD.Head.BufferSize = sizeof(ReadCapacity);                
			(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *)&DataTD); /* Send command response to the Host device */                     
			CSW.dCSWDataResidue  = 0x00UL;
#ifdef DEBUG_PRINT
			printf("->Read Capacity\n\r");
#endif
			break;                                                                          
		case TEST_UNIT_READY:                                    /* Test Unit Ready command */
			CSW.dCSWDataResidue = 0x00U;  
			SendCommandStatusWrapper(USBDevDataPtr, NULL);
#ifdef DEBUG_PRINT
			printf("->Unit Ready\n\r");
#endif
			break;                                
		case READ_10:                                           /* Read(10) data from flash memory command */ 
			/* Calculating Logical Block Address from received command */ 
			MassStorageDevicePtr->LogBlockAddr = (uint32_t)((CBW->CBWCB[2]<<24)|(CBW->CBWCB[3]<<16)|(CBW->CBWCB[4]<<8)| CBW->CBWCB[5]);     
			/* Calculating Transfer length from received command */
			MassStorageDevicePtr->TransferLength = (uint16_t)((CBW->CBWCB[7] << 8)| CBW->CBWCB[8]);                                               
			/* Initializing CurrentTransfer value before flash memory "Read" operation */
			MassStorageDevicePtr->CurrentTransfer = 0x00U;                
			/* Call ReadFlasMemory function */
			MS_ReadFlashMemory(MassStorageDevicePtr, NULL);                                          
#ifdef DEBUG_PRINT
			printf("->Read(10) addr: 0x%x \n\r", (FLASH_BASE_ADDR+((MassStorageDevicePtr->LogBlockAddr)*FLASH_BLOCK_SIZE)));
#endif
			break;                                
		case MODE_SENCE_6:                                     /* Mode Sence(6) command */
			DataTD.Head.BufferPtr  = (uint8_t *)&ModeSense;
			DataTD.Head.BufferSize = sizeof(ModeSense);            
			USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD *)&DataTD); /* Send command response to Host device */            
			CSW.dCSWDataResidue = 0x00U;
#ifdef DEBUG_PRINT
			printf("->Mode Sence(6)\n\r");
#endif
			break;                                                     
		case ALLOW_MEDIUM_REMOVAL:                             /* Prevent/Allow Medium Removal command */
			SendCommandStatusWrapper(USBDevDataPtr, NULL);          
#ifdef DEBUG_PRINT        
			printf("->Prevent/Allow Medium Removal\n\r");
#endif
			break;       
		case VERIFY:                                           /* Verify command */
			SendCommandStatusWrapper(USBDevDataPtr, NULL);   
#ifdef DEBUG_PRINT
			printf("->Verify\n\r");
#endif
			break;                                      
		case WRITE_10:                                         /* Write(10) data to flash memory command */
			/* Calculating Logical Block Address from received command */
			MassStorageDevicePtr->LogBlockAddr = (uint32_t)((CBW->CBWCB[2]<<24)|(CBW->CBWCB[3]<<16)|(CBW->CBWCB[4]<<8)| CBW->CBWCB[5]);
			/* Calculating Transfer length from received command */        
			MassStorageDevicePtr->TransferLength = (uint16_t)((CBW->CBWCB[7] << 8)| CBW->CBWCB[8]);
			/* Initializing CurrentTransfer value before flash memory "Write" operation */
			MassStorageDevicePtr->CurrentTransfer = 0x00U;
			/* Initializing FlashOperation - first step is receive data from USB line */
			MassStorageDevicePtr->FlashOperationMode = RECEIVE_DATA;
			/* Setting CallBack address for write to the flash memory */ 
			MassStorageDevicePtr->CallbackFnPtr = &MS_WriteFlashMemory;        
			/* Call MS_WriteFlashMemory function */       
			MS_WriteFlashMemory(MassStorageDevicePtr, NULL);
#ifdef DEBUG_PRINT
			printf("->Write(10)\n\r");
#endif
			break;                                                                                                              
		default:                                              /* Unknown command */ 
			CSW.bCSWStatus = 0x01U;
			SendCommandStatusWrapper(USBDevDataPtr, NULL);
#ifdef DEBUG_PRINT
			printf("->Unknown Command\n\r");
#endif
			break;          
		}                     
	} 
}    

/*
 ** ===================================================================
 **     Method      :  SendCommandStatusWrapper (User method)
 **
 **     Description :
 **         Send status command transport wrapper.
 ** ===================================================================
 */
void SendCommandStatusWrapper(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD) 
{
	TMSD *MassStorageDevicePtr = (TMSD *)USBDevDataPtr;
	(void)pTD;                                               /* Parameter is not used, suppress unused argument warning */  

	DataTD.Head.EpNum      = EP_INP_NUM;                     /* Initializing USB transfer descriptor for send "CommandStatusWrapper" to Host device */
	DataTD.Head.BufferPtr  = (uint8_t *)&CSW;                /* Pointer to the CSW (CommandStatusWrapper) */
	DataTD.Head.BufferSize = 0x0DU;                          /* Size of "Command Status Wrapper" */
	DataTD.Head.Flags      = LDD_USB_DEVICE_TRANSFER_FLAG_EXT_PARAM;    
	DataTD.CallbackFnPtr   = &OnSetUSBConfiguration;         /* Setting CallBack for receive next mass storage command */ 
	(void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_Device_TTD*) &DataTD); /* Send CSW to the Host device */
}

/* END of MassStorage */

/* ################################################################### */
