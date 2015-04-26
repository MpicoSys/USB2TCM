# USB2TCM
### About
This USB2TCM application provides an API and examples for using TCM modules. 
### Using USB2TCM
#### Formatting
When connected to PC for the first time, MSD has to be formated. Please note that no matter what OS user is working on, USB2TCM has to be formated in FAT file system. Linux user need to be very careful while formatting device because mistake may cause bricking the device and memory replacement will be needed.
#### Loading Image to TCM
USB2TCM need to be attached to TCM on one side and to PC's USB port on other. When device mounts as mass storage device in operating system, simple copying prepared epd file onto removable drive is needed to upload and display image to TCM. It's possible to resend image when it's stored on memory pressing USB2TCM's button.

Please remember that FAT file system simple implementation always loads the first file found on device's memory. When there are two files on removable drive, only image that was copied first will be displayed on TCM. To display another file first one have to be deleted, and display process will start right after file deletion.

#### Errors
When TCM is not detected or sending an image will be not successful USB2TCM will signal it with blinking LED. To try sending image again user needs only to press button again or copy file one more time.

### FAT12 implementation
Simple parsing of fat was implemented to search for files stored on mass storage device. Different oeprating systems format mass storage device memory in slight different way, so searching algorithm had to be implemented. This algorithm returns address of file contents in external memory. Following list shows how the algorithm works in general. 
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

### SPI with BUSY line
Settings
  *  SPI device type: master
  *  SCLK Frequency: 3MHz for continuous transmission
  *  Polarity — CPOL = 1; clock transition high-to-low on the leading edge and low-to-high on the trailing edge
  *  Phase — CPHA = 1; setup on the leading edge and sample on the trailing edge
  *  Chip select polarity — active low. CS low == interface enabled.
  *  BUSY polarity — active low. BUSY low == device is busy. 

### Dependencies
Currently, the dependencies are Processor Expert Software and Wait module in version 1.056 or newer [https://community.freescale.com/docs/DOC-93259]. This is likely to change in the future.
### Building
This project uses the Processor Expert Software [http://www.freescale.com/webapp/sps/site/homepage.jsp?code=BEAN_STORE_MAIN].  
To build application use CodeWarrior Embedded Software Development Tools or Kinetis Design Studio Integrated Development Environment with Kinetis microcontrolelrs.
License
----

This application is licensed under the FreeBSD

### Version
1.1
### Todo's
 - Write Tests
