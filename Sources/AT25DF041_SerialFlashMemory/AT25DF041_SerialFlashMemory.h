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
 * @file     AT25DF041_SerialFlashMemory.h
 * @brief    Header file for AT25DF041 Flash Memory
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */

#ifndef AT25F512B_SERIALFLASHMEMORY_H
#define AT25F512B_SERIALFLASHMEMORY_H

#include "at25df041_hal.h"
enum e_AT25_Setup
{
	AT25_SETUP_BLOCK_SIZE_BYTES = 0x1000, // 4kB for cmd=0x20

#ifdef USE_USB2TCM_64kB
	AT25_SETUP_MEMORY_SIZE_BYTES = 0x40000, // 64kB
#elif USE_USB2TCM_256kB
	AT25_SETUP_MEMORY_SIZE_BYTES = 0x40000, // 256kB
#elif USE_USB2TCM_512kB
#warning "Verify memory used for E60."
	AT25_SETUP_MEMORY_SIZE_BYTES = 0x80000, // 512kB
#elif USE_USB2TCM
	AT25_SETUP_MEMORY_SIZE_BYTES = 0x100000,
	AT25_SETUP_MAX_WRITE_SIZE_BYTES = 0x100, //256B
	AT25_SETUP_MEMORY_START_ADDRESS = 0x000000,
	AT25_SETUP_MEMORY_END_ADDRESS = (0x80000-1),
	AT25_SETUP_SECTOR_SIZE_BYTES = 0x10000, // 64kB for each sector protection function
#else
#error "Memory size undefined."
#endif

#ifdef USE_TCM_X
	//Nothing here (different emory used)
#elif USE_USB2TCM
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_0 = 0x00000,
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_1 = AT25_SETUP_SECTOR_SIZE_BYTES,
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_2 = (2*AT25_SETUP_SECTOR_SIZE_BYTES),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_3 = (3*AT25_SETUP_SECTOR_SIZE_BYTES),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_4 = (4*AT25_SETUP_SECTOR_SIZE_BYTES),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_5 = (5*AT25_SETUP_SECTOR_SIZE_BYTES),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_6 = (6*AT25_SETUP_SECTOR_SIZE_BYTES),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_7 = (7*AT25_SETUP_SECTOR_SIZE_BYTES),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_8 = (AT25_SETUP_SECTOR_ADDRESS_SECTOR_7 + 0x8000),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_9 = (AT25_SETUP_SECTOR_ADDRESS_SECTOR_8 + 0x2000),
	AT25_SETUP_SECTOR_ADDRESS_SECTOR_10 = (AT25_SETUP_SECTOR_ADDRESS_SECTOR_9 + 0x2000),
#else
#error "Memory sectors undefined."
#endif
};

/**
 * @brief Opcodes for flash memory
 */
enum e_AT25_Opcodes
{
	AT25_OPCODE_READ_ARRAY = 0x0B,                //!< AT25_OPCODE_READ_ARRAY
	AT25_OPCODE_BLOCK_ERASE_4KB = 0x20,           //!< AT25_OPCODE_BLOCK_ERASE_4KB
	AT25_OPCODE_BLOCK_ERASE_32KB = 0x52,          //!< AT25_OPCODE_BLOCK_ERASE_32KB
	AT25_OPCODE_CHIP_ERASE = 0x60,                //!< AT25_OPCODE_CHIP_ERASE
	AT25_OPCODE_BYTE_PROGRAM = 0x02,              //!< AT25_OPCODE_BYTE_PROGRAM
	AT25_OPCODE_WRITE_ENABLE = 0x06,              //!< AT25_OPCODE_WRITE_ENABLE
	AT25_OPCODE_WRITE_DISABLE = 0x04,             //!< AT25_OPCODE_WRITE_DISABLE
	AT25_OPCODE_PROTECT_SECTOR = 0x36,            //!< AT25_OPCODE_PROTECT_SECTOR
	AT25_OPCODE_UNPROTECT_SECTOR = 0x39,          //!< AT25_OPCODE_UNPROTECT_SECTOR
	AT25_OPCODE_PROGRAM_OTP_REGISTER = 0x9B,      //!< AT25_OPCODE_PROGRAM_OTP_REGISTER
	AT25_OPCODE_READ_OTP_REGISTER = 0x77,         //!< AT25_OPCODE_READ_OTP_REGISTER
	AT25_OPCODE_READ_STATUS_REGISTER = 0x05,      //!< AT25_OPCODE_READ_STATUS_REGISTER
	AT25_OPCODE_WRITE_STATUS_REGISTER = 0x01,     //!< AT25_OPCODE_WRITE_STATUS_REGISTER
	AT25_OPCODE_READ_DEVICE_ID = 0x9F,            //!< AT25_OPCODE_READ_DEVICE_ID
	AT25_OPCODE_DEEP_POWER_DOWN = 0xB9,           //!< AT25_OPCODE_DEEP_POWER_DOWN
	AT25_OPCODE_RESUME_FROM_DEEP_POWER_DOWN = 0xAB//!< AT25_OPCODE_RESUME_FROM_DEEP_POWER_DOWN
};

void AT25_Init(void);
void AT25_DeInit(void);

void AT25_ReadArray(uint32_t addr, uint32_t nb_bytes, uint8_t* buffer);

void AT25_BlockErase4Kb(uint32_t addr);
void AT25_BlockErase32Kb(uint32_t addr);

void AT25_ChipErase(void);
void AT25_ByteProgram(uint32_t start_address, uint32_t nb_bytes, uint8_t* buffer);

void AT25_WriteEnable(void);
void AT25_WriteDisable(void);

uint8_t AT25_ReadStatusRegister(void);
void AT25_WriteStatusRegister(uint8_t status);

uint8_t AT25_ReadManufacturerAndDeviceID(uint8_t* device_id);

void AT25_DeepPowerDown(void);
void AT25_ResumeFromDeepPowerDown(void);

void AT25_ProtectSectors(uint32_t addr);
void AT25_UnprotectSectors(uint32_t addr);

uint8_t AT25_IsBusy();

#endif

/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
