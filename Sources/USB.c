/** ###################################################################
**     Filename  : USB.c
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
**         USB1_OnDeviceReset       - void USB1_OnDeviceReset(LDD_TUserData *UserDataPtr);
**         USB1_OnDeviceSpeedDetect - void USB1_OnDeviceSpeedDetect(LDD_TUserData *UserDataPtr,...
**         USB1_OnDeviceSuspend     - void USB1_OnDeviceSuspend(LDD_TUserData *UserDataPtr);
**         USB1_OnDeviceResume      - void USB1_OnDeviceResume(LDD_TUserData *UserDataPtr);
**         USB1_OnDeviceSetupPacket - void USB1_OnDeviceSetupPacket(LDD_TUserData *UserDataPtr, uint8_t EpNum);
**         USB1_OnDeviceError       - void USB1_OnDeviceError(LDD_TUserData *UserDataPtr, uint8_t ErrorSource,...
**
** ###################################################################*/
/* MODULE USB */

#include "Cpu.h"
#include "Events.h"
#include "USB.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdio.h>

/* USB standard request code */
#define STD_SET_ADDRESS               ToBigEndian16(0x0005U)
#define STD_GET_DESCRIPTOR            ToBigEndian16(0x8006U)
#define STD_GET_CONFIGURATION         ToBigEndian16(0x8008U)
#define STD_SET_CONFIGURATION         ToBigEndian16(0x0009U)

#define STD_GET_STATUS_ZERO           ToBigEndian16(0x8000U)
#define STD_GET_STATUS_INTERFACE      ToBigEndian16(0x8100U)
#define STD_GET_STATUS_ENDPOINT       ToBigEndian16(0x8200U)

#define STD_CLEAR_FEATURE_ZERO        ToBigEndian16(0x0001U)
#define STD_CLEAR_FEATURE_INTERFACE   ToBigEndian16(0x0101U)
#define STD_CLEAR_FEATURE_ENDPOINT    ToBigEndian16(0x0201U)

#define GET_DEVICE_DESCRIPTOR         ToLittleEndian16(0x0100U)
#define GET_CONFIGURATION_DESCRIPTOR  ToLittleEndian16(0x0200U)
#define GET_STRING_DESCRIPTOR_0       ToLittleEndian16(0x0300U)
#define GET_STRING_DESCRIPTOR_1       ToLittleEndian16(0x0301U)
#define GET_STRING_DESCRIPTOR_2       ToLittleEndian16(0x0302U)
#define GET_STRING_DESCRIPTOR_3       ToLittleEndian16(0x0303U)
#define GET_STRING_DESCRIPTOR_4       ToLittleEndian16(0x0304U)

TMSD MassStorageDevice;                /* Declaring of global variables */
LDD_USB_Device_TTD  DataTDPtr;
LDD_USB_Device_TTD  HskTDPtr;
extern uint8_t USBMounted;


/***************************************************************************/
/* Device Descriptor                                                         */
/***************************************************************************/
const uint8_t DeviceDescriptor[]={
  0x12U,                               /* Descriptor Size */
  LDD_USB_DT_DEVICE,                   /* Descriptor type: Device descriptor */
  0x10U,0x01U,                         /* USB Spec. Release Number in BCD = 1.1 */
  0x00U,                               /* Device Class Code */
  0x00U,                               /* Device Subclass Code */
  0x00U,                               /* Device Protocol Code */
  0x20U,                               /* Maximum Packet Size for EP0 */
  0x34U,0x12U,                         /* Vendor ID */
  0x78U,0x56U,                         /* Product ID */
  0x00U,0x01U,                         /* Device Release Number in BCD */
  0x01U,                               /* Index of String Desc. for Manufacturer */
  0x02U,                               /* Index of String Desc. for Product */
  0x03U,                               /* Index of String Desc. for SerNo */
  0x01U                                /* Number of possible Configurations */
};

const uint8_t Configuration_1[]={
  /***************************************************************************/
  /* Configuration 1 Descriptor                                              */
  /***************************************************************************/
  0x09U,                               /* Descriptor Size */
  LDD_USB_DT_CONFIGURATION,            /* Descriptor type */
  0x20U,0x00U,                         /* Total Length of Data for this Configuration */
  0x01U,                               /* No of Interfaces supported by this Configuraton */
  0x01U,                               /* Designator Value for this Configuration */
  0x04U,                               /* Index of String Desc for this Configuration */
  0x80U,                               /* Configuration characteristics */
  0x96U,                               /* Max. Power Consumption in this Config. (*2mA) */
  /***************************************************************************/
  /* Interface 0 Alternate setting 0 Descriptor ()                           */
  /***************************************************************************/
  0x09U,                               /* Descriptor Size */
  LDD_USB_DT_INTERFACE,                /* Descriptor type: Interface descriptor */
  0x00U,                               /* Number of this Interface (0..) */
  0x00U,                               /* Alternative for this Interface (if any) */
  0x02U,                               /* No of EPs used by this IF (excl. EP0) */
  0x08U,                               /* Class Code - 0x08 Mass Storage */
  0x06U,                               /* Class Code - 0x06 */
  0x50U,                               /* Class Code - 0x50 */
  0x00U,                               /* Index of String Desc for this Interface */
  /***************************************************************************/
  /* Endpoint 1  Descriptor, Bulk IN ()                                      */
  /***************************************************************************/
  0x07U,                               /* Descriptor size */
  LDD_USB_DT_ENDPOINT,                 /* Descriptor type: Endpoint descriptor */
  (uint8_t)(0x80U | EP_INP_NUM),       /* Address: 1, direction: IN */
  0x02U,                               /* Transfer type: Bulk */
  (uint8_t)(EP_INP_SIZE & 0xFFU),      /* Max. packet size: n byte(s) */
  (uint8_t)(EP_INP_SIZE >> 8),
  0x01U,                               /* Maximum NAK rate: 0x01 microframe(s) */
  /***************************************************************************/
  /* Endpoint 1  Descriptor, Bulk OUT ()                                     */
  /***************************************************************************/
  0x07U,                               /* Descriptor size */
  LDD_USB_DT_ENDPOINT,                 /* Descriptor type: Endpoint descriptor */
  (uint8_t)EP_OUT_NUM,                 /* Address: 1, direction: OUT */
  0x02U,                               /* Transfer type: Bulk */
  (uint8_t)(EP_OUT_SIZE & 0xFFU),      /* Max. packet size: n byte(s) */
  (uint8_t)(EP_OUT_SIZE >> 8),
  0x01U                                /* Maximum NAK rate: 0x01 microframe(s) */
};

static const uint8_t StringDescriptor_0[]={
  0x04U,                               /* Descriptor size */
  LDD_USB_DT_STRING,                   /* Descriptor type: String descriptor */
  0x09U,0x04U                          /* 0x0409 English (United States) */
};

static const uint8_t StringDescriptor_1[]= {
  /* Index=1, Descriptor name: Manufacturer, value: "FSL USB_LDD FlashDisk Demo" */
  0x3CU,                               /* Descriptor Size */
  LDD_USB_DT_STRING,                   /* Descriptor Type (=3) */
  'M',0x00U,'p',0x00U,'i',0x00U,'c',0x00U,
  'o',0x00U,'S',0x00U,'y',0x00U,'s',0x00U,
  ' ',0x00U,'E',0x00U,'m',0x00U,'b',0x00U,
  'e',0x00U,'d',0x00U,'d',0x00U,'e',0x00U,
  'd',0x00U,' ',0x00U,'P',0x00U,'i',0x00U,
  'c',0x00U,'o',0x00U,' ',0x00U,'S',0x00U,
  'y',0x00U,'s',0x00U,'t',0x00U,'e',0x00U,
  'm',0x00U,'s',0x00U
};

static const uint8_t StringDescriptor_2[]= {
  /* Index=2, Descriptor name: Product, value: "Processor Expert USB Device component" */
  0x0DU,                               /* Descriptor Size */
  LDD_USB_DT_STRING,                   /* Descriptor Type (=3) */
  'U',0x00U,'S',0x00U,'B',0x00U,'2',0x00U,
  'T',0x00U,'C',0x00U,'M'
};

static const uint8_t StringDescriptor_3[]= {
  /* Index=3, value: "123456789ABCDEF" */
  0x20U,                               /* Descriptor Size */
  LDD_USB_DT_STRING,                   /* Descriptor Type (=3) */
  '1',0x00U,'2',0x00U,'3',0x00U,'4',0x00U,
  '5',0x00U,'6',0x00U,'7',0x00U,'8',0x00U,
  '9',0x00U,'A',0x00U,'B',0x00U,'C',0x00U,
  'D',0x00U,'E',0x00U,'F',0x00U
};

static const uint8_t StringDescriptor_4[]= {
  /* Index=4,  value: "Configuration 1" */
  0x20U,                               /* Descriptor Size */
  LDD_USB_DT_STRING,                   /* Descriptor Type (=3) */
  'C',0x00U,'o',0x00U,'n',0x00U,'f',0x00U,
  'i',0x00U,'g',0x00U,'u',0x00U,'r',0x00U,
  'a',0x00U,'t',0x00U,'i',0x00U,'o',0x00U,
  'n',0x00U,' ',0x00U,'1',0x00U
};

/*
** ===================================================================
**     Method      :  USB_SendZlp (User method)
**
**     Description :
**         Send zero length packet through EP0 to the Host device.
** ===================================================================
*/
void USB_SendZlp(LDD_TUserData *UserDataPtr)    /* Send zero length packet to the Host device */
{
  TMSD *MassStorageDevicePtr = (TMSD *)UserDataPtr;  
  DataTDPtr.Head.EpNum       = 0x00U;
  DataTDPtr.Head.BufferPtr   = NULL;
  DataTDPtr.Head.BufferSize  = 0x00U;
  DataTDPtr.Head.Flags       = 0x00U;
  (void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,&DataTDPtr);
}

/*
** ===================================================================
**     Method      :  MS_InitDevices (User method)
**
**     Description :
**         Set Address Callback - Sets USB device address, is invoked 
**         after sending acknowledge on "Set address USB request" 
**         request.
** ===================================================================
*/
static void SetAddressCallback(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD)
{ 
  USB1_DeviceSetAddress(USBDevDataPtr, (uint8_t)ToLittleEndian16((uint16_t )pTD->ParamPtr));  
  #ifdef DEBUG_PRINT
  printf("SET_USB_DEVICE_ADDRESS 0x%x \n\r", ToLittleEndian16(((uint16_t )pTD->ParamPtr)));
  #endif
}

/*
** ===================================================================
**     Event       :  USB1_OnDeviceSpeedDetect (module USB)
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
void USB1_OnDeviceSpeedDetect(LDD_TUserData *UserDataPtr, LDD_USB_TBusSpeed CurrentSpeed)
{
  TMSD *MassStorageDevicePtr = (TMSD *)UserDataPtr;
  (void)CurrentSpeed;		// Parameter is not used, suppress unused argument warning
	  
  USB1_DeviceInitEndpoint(MassStorageDevicePtr->USBDeviceDataPtr,(LDD_USB_TEpDescriptor *)&DeviceDescriptor, (uint8_t)0);
}

/*
** ===================================================================
**     Event       :  USB1_OnDeviceSetupPacket (module USB)
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
void USB1_OnDeviceSetupPacket(LDD_TUserData *UserDataPtr, uint8_t EpNum)
{
  TMSD *MassStorageDevicePtr = (TMSD *)UserDataPtr;
  LDD_USB_TEpDescriptor EpDescriptorPtr;
  //LDD_TError res = ERR_OK;
  LDD_USB_TSDP pSDP;    
    
  /* Read setup packet data */
  USB1_DeviceReadSetupData(MassStorageDevicePtr->USBDeviceDataPtr, EpNum,&pSDP);

  /* Initialize Data packet transfer descriptor */
  DataTDPtr.Head.EpNum      = 0x00U;
  DataTDPtr.Head.BufferPtr  = NULL;
  DataTDPtr.Head.BufferSize = 0x01U;
  DataTDPtr.Head.Flags      = 0x00U;
    
  /* Initialize handshake packet transfer descriptor */
  HskTDPtr.Head.EpNum      = 0x00U;
  HskTDPtr.Head.BufferPtr  = NULL;
  HskTDPtr.Head.BufferSize = 0x00U;
  HskTDPtr.Head.Flags      = 0x00U;
    
  /* Handle supported standard device request */ 
  switch (*((uint16_t *)&pSDP)) {
    case STD_GET_DESCRIPTOR:                      
      switch (pSDP.wValue) {        
        case GET_DEVICE_DESCRIPTOR:                        /* Return Device Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)DeviceDescriptor;
          DataTDPtr.Head.BufferSize = sizeof(DeviceDescriptor);           
          break;      
        case GET_CONFIGURATION_DESCRIPTOR:                 /* Return Configuration Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)Configuration_1;
          DataTDPtr.Head.BufferSize = sizeof(Configuration_1);          
          break;          
        case GET_STRING_DESCRIPTOR_0:                      /* Return language string Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)StringDescriptor_0;
          DataTDPtr.Head.BufferSize = sizeof(StringDescriptor_0);            
          break;          
        case GET_STRING_DESCRIPTOR_1:                      /* Return Manufacturer Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)StringDescriptor_1;
          DataTDPtr.Head.BufferSize = sizeof(StringDescriptor_1);            
          break;          
        case GET_STRING_DESCRIPTOR_2:                      /* Return Product Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)StringDescriptor_2;
          DataTDPtr.Head.BufferSize = sizeof(StringDescriptor_2);            
          break;          
        case GET_STRING_DESCRIPTOR_3:                      /* Return Serial number Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)StringDescriptor_3;
          DataTDPtr.Head.BufferSize = sizeof(StringDescriptor_3);            
          break;          
        case GET_STRING_DESCRIPTOR_4:                      /* Return interface Descriptor */
          DataTDPtr.Head.BufferPtr  = (LDD_TData*)StringDescriptor_4;
          DataTDPtr.Head.BufferSize = sizeof(StringDescriptor_4);            
          break;          
        default:                                           /* Unsupported Descriptor */          
          USB1_DeviceStallEndpoint(MassStorageDevicePtr->USBDeviceDataPtr, 0, 0); /* USB SendStall */  
          #ifdef DEBUG_PRINT
          printf("UNKNOWN_DESCRIPTOR 0x%x\n\r",pSDP.wValue);
          #endif
          return;
          break;  
      }        
      (void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,&DataTDPtr); /* Send response to the Host(PC) device */      
      HskTDPtr.Head.BufferSize = 0x00U;
      (void)USB1_DeviceRecvData(MassStorageDevicePtr->USBDeviceDataPtr,&HskTDPtr); /* Receive acknowledge USB packet */ 
      #ifdef DEBUG_PRINT
      printf("GET_DESCRIPTOR \n\r");
      #endif
      break;      
    case STD_SET_ADDRESS:
      DataTDPtr.Head.BufferSize    = 0x00U;
      DataTDPtr.Head.Flags         = LDD_USB_DEVICE_TRANSFER_FLAG_EXT_PARAM; /* Transfer descriptor contains extended parameters */
      DataTDPtr.CallbackFnPtr      = &SetAddressCallback;  /* Set CallBack, which will be call after transmission zero length packet */
      DataTDPtr.ParamPtr           = (uint8_t*)(pSDP.wValue); /* Set CallBack parameter = new USB device address */
      (void)USB1_DeviceSendData(MassStorageDevicePtr->USBDeviceDataPtr,&DataTDPtr);   /* Send zero length packet and then invoke SetAddressCallback */
      #ifdef DEBUG_PRINT
      printf("SET_ADDRESS 0x%x \n\r",pSDP.wValue);
      #endif  
      break;       
    case STD_SET_CONFIGURATION:
      /* Configure and enable output endpoint */
      EpDescriptorPtr.wMaxPacketSize = ToLittleEndian16(EP_OUT_SIZE); /* Set size of output EP */  
      EpDescriptorPtr.bmAttributes   = 0x02U;              /* Bulk transfer */
      EpDescriptorPtr.bEndpointAddress = EP_OUT_NUM;       /* EP1 out */                
      USB1_DeviceInitEndpoint(MassStorageDevicePtr->USBDeviceDataPtr,&EpDescriptorPtr,0x00U); /* QueueSize=0,ZLP=no */
      /* Configure and enable input endpoint */  
      EpDescriptorPtr.wMaxPacketSize = ToLittleEndian16(EP_INP_SIZE); /* Set size of input EP */  
      EpDescriptorPtr.bmAttributes   = 0x02U;              /* Bulk transfer */      
      EpDescriptorPtr.bEndpointAddress = (uint8_t)(0x80U | EP_INP_NUM); /* EP1 in */
      USB1_DeviceInitEndpoint(MassStorageDevicePtr->USBDeviceDataPtr,&EpDescriptorPtr,0x00U); /* QueueSize=0,ZLP=no */  
      /* Send zero length packet = acknowledge to the host device (PC)*/        
      USB_SendZlp(UserDataPtr);                            /* Send zero length packet */          
      /* Initialize receive request of output EP */ 
      OnSetUSBConfiguration(MassStorageDevicePtr, NULL);          
      #ifdef DEBUG_PRINT
      printf("SET_CONFIGURATION \n\r");
      #endif
      break;      
    default:
      if (CmdHandler(UserDataPtr, &pSDP) == ERR_OK) {      /* Is received supported request? */
        HskTDPtr.Head.BufferSize = 0x00U;                  /* If yes, then set receive acknowledge packet */
        (void)USB1_DeviceRecvData(MassStorageDevicePtr->USBDeviceDataPtr,&HskTDPtr);            
      } else {                                             /* If no, then stall endpoint */
        USB1_DeviceStallEndpoint(MassStorageDevicePtr->USBDeviceDataPtr, 0, 0); 
      }    
      break;
  }
}

/*
** ===================================================================
**     Event       :  USB1_OnDeviceError (module USB)
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
void USB1_OnDeviceError(LDD_TUserData *UserDataPtr, uint8_t ErrorSource, uint8_t ErrorCode)
{
  PE_DEBUGHALT();
}

/*
** ===================================================================
**     Method      :  MS_InitDevices (User method)
**
**     Description :
**         Initialize USB + FLASH device.
** ===================================================================
*/
void MS_InitDevices(void)
{    
  /* Initializing variables */  
  MassStorageDevice.LogBlockAddr = 0x00UL;
  MassStorageDevice.TransferLength = 0x00U;
  MassStorageDevice.CallbackFnPtr = NULL;
  
 // printf("InitFlash_LDD component...\n\r");
 // MassStorageDevice.FlashDeviceDataPtr = FLASH1_Init(&MassStorageDevice); /* Call Init() method of the FLAS_LDD component */  
  
  printf("InitUSB_LDD component... \n\r");  
  MassStorageDevice.USBDeviceDataPtr = USB1_Init(&MassStorageDevice); /* Call Init() method of the USB_LDD component */
  (void)USB1_Enable(MassStorageDevice.USBDeviceDataPtr);  
  
  printf("Components have been initialized. \n\r");  
}


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
void USB1_OnDeviceSuspend(LDD_TUserData *UserDataPtr)
{	

  /* Write your code here ... */
}

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
void USB1_OnDeviceReset(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
}

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
void USB1_OnDeviceResume(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
}

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
void USB1_OnDeviceSof(LDD_TUserData *UserDataPtr, uint16_t FrameNumber)
{
  /* Write your code here ... */
}

/* END USB */

/*
** ###################################################################
**
**     This file was created by Processor Expert 0.00.00 [05.00]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
