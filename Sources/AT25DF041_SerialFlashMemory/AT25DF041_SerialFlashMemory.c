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
 * @file     AT25DF041_SerialFlashMemory.c
 * @brief    Source file for AT25DF041 Flash Memory
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */

#include "AT25DF041_SerialFlashMemory.h"

/**
 * @brief Intefrace initialization 
 */
void AT25_Init(void)
{
	at25df041_hal_init();
}
	
/**
 * @brief Deinitialization of interface
 */
void AT25_DeInit(void)
{
	at25df041_hal_deinit();
}

/**
 * 
 * @param address  Address to read
 * @param nb_bytes Number of bytes to read
 * @param buffer   Buffer pointer to readed data
 */
void AT25_ReadArray(uint32_t address, uint32_t nb_bytes, uint8_t* buffer)
{
	uint8_t data_out[5];
	data_out[0] = AT25_OPCODE_READ_ARRAY;
	data_out[1] = (uint8_t)(address>>16);
	data_out[2] = (uint8_t)(address>>8);
	data_out[3] = (uint8_t)(address);
	data_out[4] = 0x00;

	at25df041_hal_cs_low();

	at25df041_hal_send(data_out,5);

	at25df041_hal_receive(buffer, nb_bytes);

	at25df041_hal_cs_high();
}

/**
 * @brief Erase block of 4Kb size
 * @param address Address block to erase
 */
void AT25_BlockErase4Kb(uint32_t address)
{
	uint8_t data_out[4];
	data_out[0] = AT25_OPCODE_BLOCK_ERASE_4KB;
	data_out[1] = (uint8_t)(address>>16);
	data_out[2] = (uint8_t)(address>>8);
	data_out[3] = (uint8_t)(address);

	at25df041_hal_cs_low();

	at25df041_hal_send(data_out,4);

	at25df041_hal_cs_high();
}

/**
 * @brief Erase block of 32Kb size
 * @param address Address block to erase
 */
void AT25_BlockErase32Kb(uint32_t address)
{
	uint8_t data_out[4];
	data_out[0] = AT25_OPCODE_BLOCK_ERASE_32KB;
	data_out[1] = (uint8_t)(address>>16);
	data_out[2] = (uint8_t)(address>>8);
	data_out[3] = (uint8_t)(address);

	at25df041_hal_cs_low();

	at25df041_hal_send(data_out,4);

	at25df041_hal_cs_high();
}

/**
 * @brief Chip Erase Function
 */
void AT25_ChipErase(void)
{
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_CHIP_ERASE);

	at25df041_hal_cs_high();
}

/**
 * @brief Program Byte Function
 * @param start_address Start address
 * @param nb_bytes Number of bytes
 * @param buffer Pointer to buffer
 */
void AT25_ByteProgram(uint32_t start_address, uint32_t nb_bytes,
		uint8_t* buffer)
{
	uint8_t data_out[4];
	data_out[0] = AT25_OPCODE_BYTE_PROGRAM;
	data_out[1] = (uint8_t)(start_address>>16);
	data_out[2] = (uint8_t)(start_address>>8);
	data_out[3] = (uint8_t)(start_address);

	at25df041_hal_cs_low();

	at25df041_hal_send(data_out,4);

	at25df041_hal_send(buffer, nb_bytes);

	at25df041_hal_cs_high();
}

/**
 * @brief Enable write mode
 */
void AT25_WriteEnable(void)
{
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_WRITE_ENABLE);

	at25df041_hal_cs_high();
}

/**
 * @brief Disable write mode
 */
void AT25_WriteDisable(void)
{
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_WRITE_DISABLE);	

	at25df041_hal_cs_high();
}

/**
 * @brief Read status register
 */
uint8_t AT25_ReadStatusRegister(void)
{
	uint8_t status;
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_READ_STATUS_REGISTER);	
	status = at25df041_hal_byte(0x00);

	at25df041_hal_cs_high();

	return status;
}

/**
 * @brief Write status register
 */
void AT25_WriteStatusRegister(uint8_t status) {}

/*
 *  return bytes in given buffer, no of bytes in return value
 */
/**
 * @brief Read flash manufacturer and chip device ID
 * @note Return bytes in given buffer, no of bytes in return value
 * @param device_id Device ID
 * @return Return device ID in bytes
 */
uint8_t AT25_ReadManufacturerAndDeviceID(uint8_t* device_id)
{
	uint8_t device_id_bytes = 4;
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_READ_DEVICE_ID);
	at25df041_hal_receive(device_id, device_id_bytes);
	at25df041_hal_cs_high();

	return device_id_bytes;
}

/**
 * @brief Memory chip go to Deep Power Down mode
 */
void AT25_DeepPowerDown(void)
{
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_DEEP_POWER_DOWN);


	at25df041_hal_cs_high();
}

/**
 * @brief Memory chip wake up from Deep Power Down mode
 */
void AT25_ResumeFromDeepPowerDown(void)
{
	at25df041_hal_cs_low();

	at25df041_hal_byte(AT25_OPCODE_RESUME_FROM_DEEP_POWER_DOWN);

	at25df041_hal_cs_high();
}

/**
 * @brief Protect selected address
 * @param address Address to protect
 */
void AT25_ProtectSectors(uint32_t address)
{
	AT25_WriteEnable();

	uint8_t data_out[4];
	data_out[0] = AT25_OPCODE_UNPROTECT_SECTOR;
	data_out[1] = (uint8_t)(address>>16);
	data_out[2] = (uint8_t)(address>>8);
	data_out[3] = (uint8_t)(address);

	at25df041_hal_cs_low();

	at25df041_hal_send(data_out,4);

	at25df041_hal_cs_high();
}

/**
 * @brief Unprotect selected address
 * @param address Address to unprotect
 */
void AT25_UnprotectSectors(uint32_t address)
{
	uint8_t data_out[4];
	data_out[0] = AT25_OPCODE_UNPROTECT_SECTOR;
	data_out[1] = (uint8_t)(address>>16);
	data_out[2] = (uint8_t)(address>>8);
	data_out[3] = (uint8_t)(address);

	at25df041_hal_cs_low();

	at25df041_hal_send(data_out,4);

	at25df041_hal_cs_high();
}

/**
 * @brief Busy status
 * @return Busy status
 */
uint8_t AT25_IsBusy(void)
{
	at25df041_hal_cs_low();
	at25df041_hal_byte(AT25_OPCODE_READ_STATUS_REGISTER);
	
	uint8_t status = at25df041_hal_byte(0x00);
	
	at25df041_hal_cs_high();
	return status & 0x1;
}

/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
