# USB2TCM Project
## About
USB2TCM Project is the firmware for MpicoSys USB2TCM - easy to use USB Mass Storage device which allows uploading images from computer to MpicoSys Timing Controller Module (TCM).
USB2TCM Project provides an API and examples for using MpicoSys Timing Controller Modules (TCM) for ePaper displays.
## Using USB2TCM Device
For detailed information please follow USB2TCM User's Guide available in the Documentation catalog.
### Formatting
When connected to the computer for the first time Mass Storage Device (MSD) has to be formated. Please note that no matter what OS user is working on, USB2TCM has to be formated in FAT file system. Linux users need to be very careful while formatting the device as any mistake may cause bricking the device. In such case the memory chip replacement will be needed.
### Loading Image to TCM
USB2TCM needs to be attached to TCM on one side and to the computer's USB port on the other. When the device mounts as MSD in operating system, copying an EPD file to the removable drive results in uploading the image to TCM and displaying it on the ePaper display. It is possible to resend the stored image pressing USB2TCM button.

Please note that FAT file system simple implementation loads the first file found on the device memory. Storing two or more EPD files in USB2TCM memory may result in operation error or image mismatch. To display another file first one has to be deleted, and display process will start right after file deletion.

### Errors
When TCM is not detected or sending image is not successful USB2TCM will signal it by blinking orange LED labeled "COM." To try sending image again user needs to press the button again or delete and copy the file one more time.

## FAT12 implementation
Simple parsing of FAT was implemented to search for files stored on mass storage device. Different operating systems format mass storage device memory in slightly different way, so searching algorithm had to be implemented. This algorithm returns address of file contents in external memory. The following list shows how the algorithm works in general. 

1. Read data about the file system. 

        uint16_t numBytesPerSector = upload_image2[12] * 256 + upload_image2[11];
        uint8_t  numSectorsPerCluster = upload_image2[13];
        uint16_t numReservedSectors = upload_image2[15] * 256 + upload_image2[14];
        uint8_t  numFATCopies = upload_image2[16];
        uint16_t numRootEntries = upload_image2[18] * 256 + upload_image2[17];
        uint16_t numSectorsPerFAT = upload_image2[23] * 256 + upload_image2[22];

2. Calculate address of root directory 

        uint32_t addressRootDirectory = (numReservedSectors + numFATCopies * numSectorsPerFAT) * numBytesPerSector;

3. Calculate address of data area 
        
        uint32_t addressDataArea = addressRootDirectory + (numRootEntries * 32);

4. Look inside root directory entries for possible files with EPD extension. Address of first entry that is not deleted file will be returned. 

## SPI with BUSY line
Settings
  *  SPI device type: master
  *  SCLK Frequency: 3MHz for continuous transmission
  *  Polarity — CPOL = 1; clock transition high-to-low on the leading edge and low-to-high on the trailing edge
  *  Phase — CPHA = 1; setup on the leading edge and sample on the trailing edge
  *  Chip select polarity — active low. CS low == interface enabled.
  *  BUSY polarity — active low. BUSY low == device is busy. 

## Dependencies
Currently, the dependencies are Processor Expert Software and Wait module in version 1.056 or newer https://community.freescale.com/docs/DOC-93259. This is likely to change in the future.

## Building
This project uses the Processor Expert Software http://www.freescale.com/webapp/sps/site/homepage.jsp?code=BEAN_STORE_MAIN.  
To build application use CodeWarrior Embedded Software Development Tools or Kinetis Design Studio Integrated Development Environment with Kinetis microcontrolelrs.

## License
This software is licensed under the FreeBSD license.


