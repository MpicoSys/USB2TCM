

#include "Events.h"
#include "TCM_SendReceiveBlock.h"

typedef struct {
  LDD_SPIMASTER_TError ErrFlag;        /* Error flags */
  uint16_t InpRecvDataNum;             /* The counter of received characters */
  uint8_t *InpDataPtr;                 /* The buffer pointer for received characters */
  uint16_t InpDataNumReq;              /* The counter of characters to receive by ReceiveBlock() */
  uint16_t OutSentDataNum;             /* The counter of sent characters */
  uint8_t *OutDataPtr;                 /* The buffer pointer for data to be transmitted */
  uint16_t OutDataNumReq;              /* The counter of characters to be send by SendBlock() */
  uint8_t SerFlag;                     /* Flags for serial communication */
  LDD_TUserData *UserData;             /* User device data structure */
} TCM_TDeviceData;                     /* Device data structure type */

typedef TCM_TDeviceData* TCM_TDeviceDataPtr ; /* Pointer to the device data structure. */


LDD_TError TCM_SendReceiveBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData *BufferPtr, uint16_t Size,LDD_TData *BufferPtrR, uint16_t SizeR)
{
  if (((TCM_TDeviceDataPtr)DeviceDataPtr)->OutDataNumReq != 0x00U) { //Is the previous transmit operation pending? 
    return ERR_BUSY;                   // If yes then error 
  }
  // {Default RTOS Adapter} Critical section begin, general PE function is used 
  EnterCritical();
  ((TCM_TDeviceDataPtr)DeviceDataPtr)->InpDataPtr = BufferPtrR; // Store a pointer to the input data. 
  ((TCM_TDeviceDataPtr)DeviceDataPtr)->InpDataNumReq = SizeR; // Store a number of characters to be received. 
  ((TCM_TDeviceDataPtr)DeviceDataPtr)->InpRecvDataNum = 0x00U; // Set number of received characters to zero. 

  // {Default RTOS Adapter} Critical section end, general PE function is used 
  
  
  ((TCM_TDeviceDataPtr)DeviceDataPtr)->OutDataPtr = BufferPtr; // Set a pointer to the output data. 
  ((TCM_TDeviceDataPtr)DeviceDataPtr)->OutDataNumReq = Size; // Set the counter of characters to be sent. 
  ((TCM_TDeviceDataPtr)DeviceDataPtr)->OutSentDataNum = 0x00U; // Clear the counter of sent characters. 
//  SPI_PDD_EnableInterruptMask(SPI0_BASE_PTR, SPI_PDD_TX_BUFFER_EMPTY); // Enable Tx buffer empy interrupt 
  SPI_PDD_EnableInterruptMask(SPI0_BASE_PTR, SPI_PDD_RX_BUFFER_FULL_OR_FAULT | SPI_PDD_TX_BUFFER_EMPTY); // Enable Rx buffer full interrupt 
  // {Default RTOS Adapter} Critical section end, general PE function is used 
  ExitCritical();
  return ERR_OK;                       // OK 
}
