/** ###################################################################
 **     Filename    : ProcessorExpert.c
 **     Project     : ProcessorExpert
 **     Processor   : MKL25Z128VLK4
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2012-07-19, 14:07, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/* MODULE ProcessorExpert */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "USB.h"
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

/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include <stdio.h>

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "stdio.h"
#include <string.h>

#include "TCM_api.h"
#include "MSD.h"
#include "external_memory.h"

/* Declaring of global variables */
extern TMSD MassStorageDevice;
LDD_TError error;
extern uint8_t display_update[];

typedef struct
{
	LDD_FLASH_TOperationType CurrentOperation; /* Current operation type */
	LDD_FLASH_TOperationStatus CurrentOperationStatus; /* Current operation status */
	LDD_FLASH_TCommand CurrentCommand; /* Last flash device command */
	LDD_FLASH_TErrorFlags CurrentErrorFlags; /* Current error flags */
	LDD_FLASH_TDataSize CurrentDataSize; /* Data size of the current operation step */
	uint32_t DataCounter; /* Counter of data to be proceeded in the current operation */
	uint8_t *CurrentDataPtr; /* Pointer to the source/destination data for the current operation step */
	LDD_FLASH_TAddress CurrentFlashAddress; /* Address of destination/source in the flash memory for the operation step */
	LDD_TUserData *UserDataPtr; /* Pointer to user data */
} FLASH1_TDeviceData;

/* Global Variables */
#define DATA_SIZE 13

volatile bool SlaveReceivedFlg = FALSE;
volatile bool MasterReceivedFlg = FALSE;
volatile bool MasterSendFlg = FALSE;

uint8_t master_send[16] =
{ 0 };

uint8_t slave_send[16] =
{ 0 };

uint8_t master_receive[16] =
{ 0 };

uint8_t slave_receive[16] =
{ 0 };

uint8_t upload_image[255] =
{ 0x20, 0x01, 0x00, 128 };

uint8_t upload_image2[2000] =
{ 0 };

bool busy = 0;
uint8_t tcm_receive;

/**
 * Function signaling that error have occured through LED on board.
 */ 
void errorSTOP()
{
	TCM_disable();
	USBTCM_error=1;
	possibleFile=0;
	while (USBTCM_error==1 && possibleFile==0) // TCM operation failed
	{ 
		LED_ClrVal(NULL);
		WAIT_Waitms(400);
		LED_SetVal(NULL);
		WAIT_Waitms(400);
	}
}

/**
 * Function check if Busy line is going low after DisplayUpdate command. If not - LED is blinking in infinite loop
 */
void checkBusy()
{
	uint8_t i = 0;

	while (1)
	{
		if (!Busy_GetVal(NULL))
		{
			break;
		}
		else if (i == 36) // Gives timeout about 4s
		{
			errorSTOP();
		}
		WAIT_Waitms(200);
		i++;
	}
}

void checkBusytoHigh()
{
	uint8_t i = 0;

	while (1)
	{
		if (Busy_GetVal(NULL))
		{
			break;
		}
		else if (i == 40) // Gives timeout about 4s
		{
			errorSTOP();
		}
		WAIT_Waitms(200);
		i++;
	}
}

uint8_t tcm_drivescheme = 0x24;
uint8_t tcm_framebuffer_slot = 0;

/**
 * @note Finds the start address of the first file on the filesystem; returns 1 if file was found, 0 if no file was found
 * @param ileStartAddress File start address to find
 * @return Find status
 */
uint8_t findFileStartAddress(uint32_t* fileStartAddress, uint32_t* controlFileStartAddress)
{
    // Read Master Boot Record (sector 0)
    // ------------------------------------
    // 11-12   Number of bytes per setor
    // 13      Number of sectors per cluster
    // 14-15   Number of reserved sectors
    // 16      Number of FAT copies
    // 17-18   Number of root directory entries
    // 19-20   Total number of sectors in the filesystem
    // 22-23   Number of sectors per FAT
 
    EM_ReadData((uint8_t*) &upload_image2, 0, 0x400);
     
    uint16_t numBytesPerSector = upload_image2[12] * 256 + upload_image2[11];
    uint8_t numSectorsPerCluster = upload_image2[13];
    uint16_t numReservedSectors = upload_image2[15] * 256 + upload_image2[14];
    uint8_t numFATCopies = upload_image2[16];
    uint16_t numRootEntries = upload_image2[18] * 256 + upload_image2[17];
    uint16_t numSectorsPerFAT = upload_image2[23] * 256 + upload_image2[22];
 
    uint32_t addressRootDirectory = (numReservedSectors + numFATCopies * numSectorsPerFAT) * numBytesPerSector;
    uint32_t addressDataArea = addressRootDirectory + (numRootEntries * 32);
     
    // Read RootDirectory (first 32 entries)
    // -------------------------------------
    uint8_t fileFound = 0;
    uint8_t fileStartSector = 0;
    uint8_t numRootPages = numRootEntries / 32;      // 32 entries in a 0x400 byte page
    uint8_t rootPageCounter = 0;                     // read rootd irectory in pages of size 0x400
 
    // Find first file
    while (rootPageCounter < numRootPages) {
        uint8_t rootDirectoryEntryNumber = 0;
        EM_ReadData((uint8_t*) &upload_image2, addressRootDirectory + rootPageCounter * 0x400, 0x400);     
        while (upload_image2[rootDirectoryEntryNumber * 32] != 0 && rootDirectoryEntryNumber < 32) {
            // Byte  0 - 0xe5 is used to indicate deleted file
            // Byte 11 - Bit 0-3 are 0 if entry is a file, other values are used for dirctories, long file names, and so on.
            if (upload_image2[rootDirectoryEntryNumber * 32] != 0xe5 && ((upload_image2[rootDirectoryEntryNumber * 32 + 11] & 0x0f) == 0)) {
                // Check if it's an epd file
                if (upload_image2[rootDirectoryEntryNumber * 32 +8]=='E' && upload_image2[rootDirectoryEntryNumber * 32 +9]=='P' && upload_image2[rootDirectoryEntryNumber * 32 +10]=='D'){
                    fileFound = 1;
                    // 20-21 First Logical Cluster Hi
                    // 26-27 First Logical Cluster Lo
                    // Ignore bytes 20 and 21 in FAT12 - no Logical Cluster consists only of 2 bytes.
                    fileStartSector = upload_image2[rootDirectoryEntryNumber * 32 + 27] * 256 + upload_image2[rootDirectoryEntryNumber * 32 + 26];
                 
                    // Very important to use Cluster size instead of sector size! Previous bug was created by that mistake.
                    *fileStartAddress = addressDataArea + (fileStartSector - 2) * numSectorsPerCluster * numBytesPerSector;
                }
                
                if (upload_image2[rootDirectoryEntryNumber * 32 +8]=='E' && upload_image2[rootDirectoryEntryNumber * 32 +9]=='P' && upload_image2[rootDirectoryEntryNumber * 32 +10]=='C'){
					// 20-21 First Logical Cluster Hi
					// 26-27 First Logical Cluster Lo
					// Ignore bytes 20 and 21 in FAT12 - no Logical Cluster consists only of 2 bytes.
					fileStartSector = upload_image2[rootDirectoryEntryNumber * 32 + 27] * 256 + upload_image2[rootDirectoryEntryNumber * 32 + 26];
				 
					// Very important to use Cluster size instead of sector size! Previous bug was created by that mistake.
					*controlFileStartAddress = addressDataArea + (fileStartSector - 2) * numSectorsPerCluster * numBytesPerSector;
				}
            }
            rootDirectoryEntryNumber++;
        }
        rootPageCounter++;
    }
    return fileFound;
}

/**
 * @brief Read and send EPD response
 * @return Status-response
 */
LDD_TError readAndSendEPD()
{
	TCM_enable();
	PE_low_level_init();

	// Pointer to part of an image.
	uint32 ii = 0;
	uint32 a = 0;

	uint32 nr_block_flash = 0;
	LDD_TError Result = -1;

	uint32_t ImageDatatoSend = 0;
	uint32_t image_shift = 0;
	uint8_t fileDetected = 0;
	uint32_t imageStart = 0x7000;
	uint32_t controlFileStartAddress = 0;

	fileDetected = findFileStartAddress(&imageStart, &controlFileStartAddress);
	
	if(controlFileStartAddress)
	{
		EM_ReadData((uint8_t*) &upload_image2, controlFileStartAddress, 16);
		uint32_t len = upload_image2[0];
		if(len > 0)
		{
			tcm_drivescheme = upload_image2[1];
		}
		if(len > 1)
		{
			tcm_framebuffer_slot = upload_image2[2];
		}
	}
	else
	{
		tcm_drivescheme = 0x24;
		tcm_framebuffer_slot = 0; 
	}
	
	display_update[0] = tcm_drivescheme;
	upload_image[2] = tcm_framebuffer_slot;
	display_update[2] = tcm_framebuffer_slot;
	
	if (fileDetected == 1)
	{

		EM_ReadData((uint8_t*) &upload_image2, imageStart, 400);  
		
		uint16_t res_x, res_y, color_bit;
		
		res_x = upload_image2[1] * 256 + upload_image2[2];
		res_y = upload_image2[3] * 256 + upload_image2[4];
		color_bit = upload_image2[5];
		ImageDatatoSend = res_x * res_y / 8;
		if(color_bit)
		{
			ImageDatatoSend *= color_bit;
		}
		
		ImageDatatoSend += 16; //image header
		
		if (ImageDatatoSend < EM_SETUP_MEMORY_SIZE_BYTES)
		{
			LED_ClrVal(NULL);
			nr_block_flash=0;
			while(ImageDatatoSend>0)
			{
				
				EM_ReadData((uint8_t*) &upload_image2,imageStart - image_shift + (nr_block_flash * 400), 400);
				for (ii = 0; ii < 4; ii++)
				{
					// Copying part of image.
					for (a = 0; a < 100; a++)
					{
						upload_image[a + 4] = upload_image2[100 * ii + a];
					}
					
					// Sending image to TCM.
					 if(ImageDatatoSend>=100)
					 {
						 upload_image[3] = 100;
						 tcm_receive=TCM_ImageUpload(upload_image, 104);
					 	 ImageDatatoSend-=100;
					 }
					 else
					 {	 
						 upload_image[3] = ImageDatatoSend;
						 tcm_receive=TCM_ImageUpload(upload_image, ImageDatatoSend+4);
						 ImageDatatoSend=0;
					 }
					 
					 if(tcm_receive != 0x90)
					 {
						 if((ii != 0)||(nr_block_flash != 0))
						 {
							 errorSTOP();
							 return Result;
						 }
					 }	
					 if(ImageDatatoSend==0)
					 {
						 break; 
					 }				 
				}
				nr_block_flash++;
			}
		}
	}
	else
	{
		//if there was no upload then give time for the TCM to start-up (upload is starting with a delay)
		WAIT_Waitms(20); 
	}
	
	if((USBTCM_error == 0) && (fileDetected || controlFileStartAddress))
	{
		WAIT_Waitms(1);         //The period of this WAIT is different ~(x 0.5)
		tcm_receive=TCM_DisplayUpdate();
		if(tcm_receive != 0x90 && fileDetected) //check response for upload command
		 {
			errorSTOP();
			return Result;
		 }	
		WAIT_Waitms(1);
		checkBusy(); 			// Check Busy pin to low
		if(USBTCM_error!=0)
		{
			return Result;
		}
		checkBusytoHigh();		// Check Busy pin to 
		if(USBTCM_error!=0)
		{
			return Result;
		}
		WAIT_Waitms(2);
		
		tcm_receive=TCM_GetAnswer();
		if(tcm_receive != 0x90)
		 {
			errorSTOP();
			return Result;
		 }	
	}
	
	possibleFile = 0;
	TCM_disable();
	return Result;
}

/**
 * Main USB2TCM function
 */
int main(void)
{

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();	
	TCM_disable();
	EM_Init();
	MS_InitDevices();
	
	possibleFile = 0;
	while (1)
	{
		if (possibleFile == 1)
		{
			
			readAndSendEPD();
			USBTCM_error=0;	
			LED_SetVal(NULL);
		}
	}

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
}
