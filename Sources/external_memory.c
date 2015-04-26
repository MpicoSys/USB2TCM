/************************************************************************************
 * Copyright (c) 2015, MpicoSys-Embedded Pico Systems                                * 			
 * All rights reserved.                                                              *
 *                                                                                   *
 * Redistribution and use in source and binary forms, with or without                *
 * modification, are permitted provided that the following conditions are met:       *
 *                                                                                   *
 * 1. Redistributions of source code must retain the above copyright notice, this    *
 *    list of conditions and the following disclaimer.                               *
 * 2. Redistributions in binary form must reproduce the above copyright notice,      *
 *    this list of conditions and the following disclaimer in the documentation      *
 *    and/or other materials provided with the distribution.                         *
 *                                                                                   *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            *
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR   *
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND       *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT        *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      *
 *                                                                                   *
 * The views and conclusions contained in the software and documentation are those   *
 * of the authors and should not be interpreted as representing official policies,   *
 * either expressed or implied, of the FreeBSD Project.                              *
 *************************************************************************************/

/**
 * @file     external_memory.c
 * @brief    External memory functions source file
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */

#include "external_memory.h"
extern LDD_TDeviceData *WDogPtr;

/**
 * @brief External memory block initialize
 * @return External Memory status
 */
EmStatus EM_Init(void)
{
	AT25_Init();

	while(AT25_IsBusy());

	//uint8_t status = AT25_ReadStatusRegister();

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_0_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_1_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_2_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_3_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_4_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_5_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_6_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_7_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_8_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_9_ADDRESS);

	AT25_WriteEnable();
	AT25_UnprotectSectors(EM_SETUP_MEMORY_SECTOR_10_ADDRESS);	

	return EM_SUCCESS;
}

/**
 * @brief External memory block deinitialize
 * @return External Memory status
 */
EmStatus EM_DeInit(void)
{
	AT25_DeInit();
	return EM_SUCCESS;
}

/**
 * @note EM_ReadData reads any required bytes (nb_bytes) starting from (start_address)
   and puts it into desired (buffer). Returns EM_FAILED if nb_bytes exceds amount
   of available bytes to read.
 * @param destination Destination address
 * @param source Source address
 * @param num Number of bytes
 * @return Return status after read data
 */
EmStatus EM_ReadData(uint8_t* destination, uint32_t source, uint32_t num)
{
	/* If given address is below memory addressing space. */
	if(source < EM_SETUP_MEMORY_START_ADDRESS) return EM_FAILED;

	/* If given address is above memory addressing space. */
	if(source > EM_SETUP_MEMORY_END_ADDRESS) return EM_FAILED;

	/* If number of bytes to read is larger than space from given address to end of memory. */
	if( num > (EM_SETUP_MEMORY_END_ADDRESS+1 - source ) ) return EM_FAILED;

	/* If memory is performing operation. */
	if(AT25_IsBusy()) return EM_BUSY;

	AT25_ReadArray(source, num, destination);

	return EM_SUCCESS;
}

/**
 * @brief
 * @note  This external memory (EM) function provide api functionality to put
   any code of [any]length to desired address.
   - first check if desired blocks are empty, if not return EM_FAILED
   - calculate proper behavior of memory (eg. in this case overlapping
	 of write blocks (per 256b) while loading more than [MAX_WRITE_SIZE_BYTES]
   - remove dependency of memory type and its architecture.
 * @param destination Program data destination
 * @param source Source address
 * @param num Number of bytes
 * @return
 */
EmStatus EM_ProgramData(uint32_t destination, uint8_t* source, uint32_t num)
{
	/* If given address is below memory addressing space. */
	if(destination < EM_SETUP_MEMORY_START_ADDRESS) 
		return EM_FAILED;

	/* If given address is above memory addressing space. */
	if(destination > EM_SETUP_MEMORY_END_ADDRESS) 
		return EM_FAILED;

	/* If number of bytes to read is larger than space from given address to end of memory. */
	if( num > (EM_SETUP_MEMORY_END_ADDRESS+1 - destination) ) 
		return EM_FAILED;

	/* If there is no data to write return error */
	if( num <= 0)
		return EM_FAILED;

	while(AT25_IsBusy());

	/* =============== Write data to memory ===============  */
	/* If the start_addres LSB isn't equals to 0x00 then
	 * - calculate if nb_bytes fits to end of page memory
	 * - if not write bytes to the end of page and the start
	 *   writing data on the next page
	 */
	uint32_t bytes_to_write = 0xFF-(0xFF & destination)+1;
	uint32_t bytes_written = 0;

	AT25_WriteEnable();

	while(AT25_IsBusy());

	if (num < bytes_to_write)
	{
		bytes_to_write = num;
	}

	AT25_ByteProgram(destination, bytes_to_write, source);

	while(AT25_IsBusy());

	AT25_WriteDisable();

	/* Check if program execution ended with SUCCESS status */
	if(AT25_ReadStatusRegister() & 0x20)
	{
		return EM_FAILED;
	}

	while(AT25_IsBusy());

	bytes_written += bytes_to_write;
	num -= bytes_to_write;

	/* If data doesn't fit to one page, then start writing data on the next page */
	while (num)
	{
		if(num > 256)
			bytes_to_write = 256;
		else
			bytes_to_write = num;

		AT25_WriteEnable();

		while(AT25_IsBusy());

		AT25_ByteProgram(destination+bytes_written, bytes_to_write, source+bytes_written);

		while(AT25_IsBusy());

		/* Check if Program execution went ok */
		if(AT25_ReadStatusRegister() & 0x20)
		{
			AT25_WriteDisable();
			return EM_FAILED;
		}

		AT25_WriteDisable();

		while(AT25_IsBusy());

		bytes_written += bytes_to_write;
		num -= bytes_to_write;
	}
	return EM_SUCCESS;
}

/**
 * @brief Block erase function
 * @param start_address Start address block to erase
 * @param nb_blocks NUmber of block
 * @return Return Block Erase function status
 */
EmStatus EM_BlockErase(uint32_t start_address, uint32_t nb_blocks)
{
	/* Can not erase 0 blocks. */
	if(nb_blocks <=0 ){
		return EM_NOT_SUPPORTED;
	}

	/* If try to erase memory which is not physically present than we report
	   that selected memory area is not supported. */
	if((start_address+(nb_blocks*EM_SETUP_BLOCK_SIZE_BYTES)) > EM_SETUP_MEMORY_SIZE_BYTES)
	{
		return EM_NOT_SUPPORTED;
	}

	while(AT25_IsBusy());

	//Check if address is at beginning of block
	if(( start_address & 0xFFF000) != start_address){
		return EM_FAILED;
	}

	while(nb_blocks--)
	{
		AT25_WriteEnable();

		while(AT25_IsBusy());

		AT25_BlockErase4Kb(start_address);
		start_address += AT25_SETUP_BLOCK_SIZE_BYTES;

		while(AT25_IsBusy());

		AT25_WriteDisable();

		while(AT25_IsBusy());
	}

	if(AT25_ReadStatusRegister() & 0x20)
		return EM_FAILED;

	AT25_WriteDisable();

	while(AT25_IsBusy());

	return EM_SUCCESS;
}

/**
 * @brief Full memory erase
 * @return Return erase status
 */
EmStatus EM_FullErase(void)
{
	if(AT25_IsBusy())
	{
		return EM_BUSY;
	}

	AT25_WriteEnable();

	while(AT25_IsBusy());

	AT25_ChipErase();

	while(AT25_IsBusy());

	if(AT25_ReadStatusRegister() & 0x20)
	{
		return EM_FAILED;
	}

	AT25_WriteDisable();

	while(AT25_IsBusy());

	return EM_SUCCESS;
}

/**
 * @brief Read external memory status
 * @param status Status to read
 * @return Return status
 */
EmStatus EM_ReadStatus(uint8_t* status)
{
	*status = AT25_ReadStatusRegister();
	return EM_SUCCESS;
}

/**
 * @brief Read external memory device ID
 * @param device_id Device ID variable to read
 * @return Return read device ID status
 */
EmStatus EM_ReadDeviceID(uint8_t* device_id)
{
	AT25_ReadManufacturerAndDeviceID(device_id);
	if( device_id[0] != 0x1F || device_id[1] != 0x44 || device_id[2] != 0x01 ){
		return EM_FAILED;
	}
	return EM_SUCCESS;
}

/**
 * @brief Enter into power down mode
 * @return Return operation status
 */
EmStatus EM_EnterPowerDown(void)
{
	if(AT25_IsBusy()) return EM_BUSY;
	AT25_DeepPowerDown();
	return EM_SUCCESS;
}

/**
 * @brief Exit from power down mode
 * @return Return operation status
 */
EmStatus EM_ExitPowerDown(void)
{
	if(AT25_IsBusy()){
		return EM_BUSY;
	}
	AT25_ResumeFromDeepPowerDown();
	return EM_SUCCESS;
}

/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
