/** ###################################################################
**     Filename  : MassStorage.h
**     Project   : MassStorage
**     Processor : -
**     Compiler  : -
**     Date/Time : 15.6.2011, 13:53
**     Contents  :
**         User source code
**
**     Copyright Freescale Semiconductor, Inc. All Rights Reserved.
** ###################################################################*/

#ifndef __MassStorage_H
#define __MassStorage_H

/* MODULE MassStorage */
#include "PE_LDD.h"
#include "external_memory.h"

#define FLASH_BASE_ADDR      AT25_SETUP_MEMORY_START_ADDRESS /* Flash base address */
#define FLASH_BLOCK_SIZE     AT25_SETUP_BLOCK_SIZE_BYTES     /* Flash block size */
#define FLASH_BLOCK_NUMBER   0x80                            /* Number of block */

const uint8_t MaxLUN = 0x00U;                                /* Logical Unit Number */

typedef struct _CBW 
{
  uint32_t      dCBWSignature;
  uint32_t      dCBWTag;
  uint32_t      dCBWDataTransferLength;
  uint8_t       bmCBWFlags;
  uint8_t       bCBWLUN;
  uint8_t       bCBWCBLength;
  uint8_t       CBWCB[16];
} TCBW,*TCBWPtr;

typedef struct _CSW 
{
  uint32_t      dCSWSignature;
  uint32_t      dCSWTag;
  uint32_t      dCSWDataResidue;
  uint8_t       bCSWStatus;
} TCSW, *TCSWPtr;

const uint8_t InquiryData[36]={
  0x00U,0x80U,0x02U,0x02U,0x1FU,
  0x00U,0x00U,0x00U,
  'M','p','i','c','o','S','y','s',
  ' ','U','S','B','2','T','C','M',
  ' ',' ',' ',' ',' ',' ',' ',' ',
  '1','.','0','0'            
};

const uint8_t SenseData[18] = {
  0x70U, 0x00U, 0x00U,
  0x00U, 0x00U, 0x00U, 0x00U,          /* reserved */
  0x0AU,                               /* Additional Sense Length(10) */
  0x00U, 0x00U, 0x00U, 0x00U,          /* reserved */
  0x00U,                               /* Additional Sense Code (Mandatory) */
  0x00U,                               /* Additional Sense Code Qualifier (Mandatory) */
  0x00U, 0x00U, 0x00U, 0x00U           /* reserved */
};

const uint8_t ReadCapacity[8] = {
  0x00U, 0x00U, (uint8_t)((FLASH_BLOCK_NUMBER-1) >> 8U), (uint8_t)((FLASH_BLOCK_NUMBER-1) & 0xFFU), /* Number of blocks */      
  0x00U, 0x00U, (uint8_t)(FLASH_BLOCK_SIZE >> 8U), (uint8_t)(FLASH_BLOCK_SIZE & 0xFFU) /* Size of block */ 
};

const uint8_t ModeSense[4] = {
  0x03U, 0x00U, 0x00U, 0x00U
};

const uint8_t FormatCapacity[] = {
  0x00U, 0x00U, 0x00U, 0x08U,          /* Header */
  0x00U, 0x00U, (uint8_t)(FLASH_BLOCK_NUMBER >> 8U), (uint8_t)(FLASH_BLOCK_NUMBER & 0xFFU), /* Number of blocks */       
  0x02U,                               /* Descriptor Code = formatted media */
  0x00U, (uint8_t)(FLASH_BLOCK_SIZE >> 8U), (uint8_t)(FLASH_BLOCK_SIZE & 0xFFU), /* Size of block(Block length) */ 
  /* Formattable Capacity Descriptor */
  0x00U, 0x00U, (uint8_t)(FLASH_BLOCK_NUMBER >> 8U), (uint8_t)(FLASH_BLOCK_NUMBER & 0xFFU), /* Number of blocks */
  0x00U,                               /* reserved */
  0x00U, (uint8_t)(FLASH_BLOCK_SIZE >> 8U), (uint8_t)(FLASH_BLOCK_SIZE & 0xFFU) /* Size of block(Block length) */  
};

/* END MassStorage */

#endif

