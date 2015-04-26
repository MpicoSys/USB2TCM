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
 * @file     at25df041_hal.c
 * @brief    Source file for AT25DF041 Flash Memory Hardware Abstraction Layer
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */

#include "at25df041_hal.h"
#include "SPI_FLASH.h"

extern LDD_TDeviceData *WDogPtr;
LDD_TDeviceData *MySPIPtr;

/**
 * @brief HAL initialize
 */
void at25df041_hal_init(void)
{
	/*SPI module configuration*/
	MySPIPtr=SPI_FLASH_Init(NULL);
}

/**
 * @brief HAL deinitialize
 * @note Put everything that was used in initialize function to default (after reset) state
 */
void at25df041_hal_deinit(void)
{
	SPI_FLASH_Deinit(MySPIPtr);
}

/**
 * @brief Read byte
 * @param byte 
 * @return Return readed byte
 */
uint8_t at25df041_hal_byte(uint8_t byte)
{	
	if ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL))  
		SPI_PDD_ReadData8bit(SPI1_BASE_PTR);

	while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_TX_BUFFER_EMPTYG) == 0U);  //Read S_REG until  S.SPTEF==1
	SPI_PDD_WriteData8Bit(SPI1_BASE_PTR, byte);										//Write D_REG, this will S.SPTEF=0
	while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_TX_BUFFER_EMPTYG) == 0U);  //Read S_REG until  S.SPTEF==1

	while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL) == 0U);    //Read S_REG until  S.SPRF==1
	byte = SPI_PDD_ReadData8bit(SPI1_BASE_PTR); 										//Read D_REG this will S.SPRF=0	  
	return byte;
}

/**
 * @brief Send multiple bytes
 * @param data Data to send
 * @param nb Number of bytes
 */
void at25df041_hal_send(uint8_t* data, uint32_t nb)
{
	uint32_t i;

	if((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL))  
		SPI_PDD_ReadData8bit(SPI1_BASE_PTR);
	for( i = 0; i < nb; i++ )
	{
		while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_TX_BUFFER_EMPTYG) == 0U);   //Read S_REG until  S.SPTEF==1
		SPI_PDD_WriteData8Bit(SPI1_BASE_PTR, (uint8_t)(*data));							   //Write D_REG, this will S.SPTEF=0			 
		data++;	  
		while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL) == 0U);     //Read S_REG until  S.SPRF==1
		SPI_PDD_ReadData8bit(SPI1_BASE_PTR); 										       //Read D_REG this will S.SPRF=0	  
	}
}

/**
 * @brief Recive bytes/data
 * @param data Received data
 * @param nb Number of bytes
 */
void at25df041_hal_receive(uint8_t* data, uint32_t nb)
{
	if ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL))  
		SPI_PDD_ReadData8bit(SPI1_BASE_PTR);
	while(nb--)
	{
		while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_TX_BUFFER_EMPTYG) == 0U);     //Read S_REG until  S.SPTEF==1
		SPI_PDD_WriteData8Bit(SPI1_BASE_PTR, (uint8_t)(*data));							     //Write D_REG, this will S.SPTEF=0
		while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL) == 0U);       //Read S_REG until  S.SPRF==1
		*data = SPI_PDD_ReadData8bit(SPI1_BASE_PTR); 										 //Read D_REG this will S.SPRF=0						 			 					  
		data++;
	}
	while ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_TX_BUFFER_EMPTYG) == 0U);         //Read S_REG until  S.SPTEF==1
	if ((SPI_PDD_ReadStatusReg(SPI1_BASE_PTR) & SPI_PDD_RX_BUFFER_FULL))  
		SPI_PDD_ReadData8bit(SPI1_BASE_PTR);
}

/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
