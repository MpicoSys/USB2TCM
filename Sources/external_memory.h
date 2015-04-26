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
 * @file     external_memory.h
 * @brief    External memory functions header file
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */

#ifndef EXTERNAL_MEMORY_H
#define EXTERNAL_MEMORY_H

#include "AT25DF041_SerialFlashMemory.h"

#define EM_LPM_ENTER do {__asm("\twfi");} while (0)
#define EM_LPM_EXIT

/**
 * External memory setup settings
 */
enum e_EmSetup
{
	EM_SETUP_BLOCK_SIZE_BYTES = AT25_SETUP_BLOCK_SIZE_BYTES,        //!< EM_SETUP_BLOCK_SIZE_BYTES
	EM_SETUP_MEMORY_SIZE_BYTES = AT25_SETUP_MEMORY_SIZE_BYTES,      //!< EM_SETUP_MEMORY_SIZE_BYTES
	//TODO: rename line to match functionality more precise
	EM_SETUP_MAX_WRITE_SIZE_BYTES = AT25_SETUP_MAX_WRITE_SIZE_BYTES,//!< EM_SETUP_MAX_WRITE_SIZE_BYTES
	EM_SETUP_MEMORY_START_ADDRESS = AT25_SETUP_MEMORY_START_ADDRESS,//!< EM_SETUP_MEMORY_START_ADDRESS
	EM_SETUP_MEMORY_END_ADDRESS =  AT25_SETUP_MEMORY_END_ADDRESS,   //!< EM_SETUP_MEMORY_END_ADDRESS

	EM_SETUP_MEMORY_SECTOR_0_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_0,
	EM_SETUP_MEMORY_SECTOR_1_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_1,
	EM_SETUP_MEMORY_SECTOR_2_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_2,
	EM_SETUP_MEMORY_SECTOR_3_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_3,
	EM_SETUP_MEMORY_SECTOR_4_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_4,
	EM_SETUP_MEMORY_SECTOR_5_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_5,
	EM_SETUP_MEMORY_SECTOR_6_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_6,
	EM_SETUP_MEMORY_SECTOR_7_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_7,
	EM_SETUP_MEMORY_SECTOR_8_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_8,
	EM_SETUP_MEMORY_SECTOR_9_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_9,
	EM_SETUP_MEMORY_SECTOR_10_ADDRESS =  AT25_SETUP_SECTOR_ADDRESS_SECTOR_10,
};

/**
 * External memory status list
 */
typedef enum e_EmStatus
{
	EM_SUCCESS = 1,      //!< EM_SUCCESS
	EM_BUSY = 0,         //!< EM_BUSY
	EM_FAILED = -1,      //!< EM_FAILED
	EM_NOT_SUPPORTED = -2//!< EM_NOT_SUPPORTED

} EmStatus;

/**
 * @brief Get external memory block size
 * @return Block size
 */
static inline uint32_t EM_GetBlockSize(void)
{
	return AT25_SETUP_BLOCK_SIZE_BYTES;
}

/**
 * @brief Get external memory size
 * @return Memory size
 */
static inline uint32_t EM_GetMemorySize(void)
{
	return AT25_SETUP_MEMORY_SIZE_BYTES;
}

EmStatus EM_Init(void);
EmStatus EM_DeInit(void);

EmStatus EM_ReadData(uint8_t* destination, uint32_t source, uint32_t num);
EmStatus EM_ProgramData(uint32_t destination, uint8_t* source, uint32_t num);

EmStatus EM_BlockErase(uint32_t start_address, uint32_t nb_blocks);
EmStatus EM_FullErase(void);

EmStatus EM_ReadStatus(uint8_t* status);
EmStatus EM_ReadDeviceID(uint8_t* device_id);

EmStatus EM_EnterPowerDown(void);
EmStatus EM_ExitPowerDown(void);

#endif
/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
