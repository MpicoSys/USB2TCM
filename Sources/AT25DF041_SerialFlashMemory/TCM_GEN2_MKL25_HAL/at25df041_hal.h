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
 * @file     at25df041_hal.h
 * @brief    Header file for AT25DF041 Flash Memory Hardware Abstraction Layer
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */
#ifndef AT25DF021_HAL_H
#define AT25DF021_HAL_H

#include "stdint.h"
#include "IO_Map.h"
#include "SPI_FLASH.h"

//! SPI read and write buffer size 
#define SSP_BUFSIZE		16
#define FIFOSIZE		8

//! SSP Status register
#define SSPSR_TFE       (0x1<<0)
#define SSPSR_TNF       (0x1<<1)
#define SSPSR_RNE       (0x1<<2)
#define SSPSR_RFF       (0x1<<3)
#define SSPSR_BSY       (0x1<<4)

//! SSP CR1 register
#define SSPCR1_LBM      (0x1<<0)
#define SSPCR1_SSE      (0x1<<1)
#define SSPCR1_MS       (0x1<<2)
#define SSPCR1_SOD      (0x1<<3)

//! SSP Interrupt Mask Set/Clear register
#define SSPIMSC_RORIM   (0x1<<0)
#define SSPIMSC_RTIM    (0x1<<1)
#define SSPIMSC_RXIM    (0x1<<2)
#define SSPIMSC_TXIM    (0x1<<3)

//! SSP0 Interrupt Status register
#define SSPRIS_RORRIS   (0x1<<0)
#define SSPRIS_RTRIS    (0x1<<1)
#define SSPRIS_RXRIS    (0x1<<2)
#define SSPRIS_TXRIS    (0x1<<3)

//! SSP0 Masked Interrupt register
#define SSPMIS_RORMIS   (0x1<<0)
#define SSPMIS_RTMIS    (0x1<<1)
#define SSPMIS_RXMIS    (0x1<<2)
#define SSPMIS_TXMIS    (0x1<<3)

//! SSP0 Interrupt clear register
#define SSPICR_RORIC    (0x1<<0)
#define SSPICR_RTIC     (0x1<<1)

/**
 * @brief Chip select pin in high
 */
static inline void at25df041_hal_cs_high()
{
#ifdef USE_USB2TCM
	FGPIOD_PSOR = (1<<4);
#else
	LPC_GPIO2->MASKED_ACCESS[(1<<6)] = (1<<6);
#endif
}

/**
 * @brief Chip select pin in low
 */
static inline void at25df041_hal_cs_low()
{
	FGPIOD_PCOR = (1<<4);
}

/**
 * @brief Write protect pin in high 
 */
static inline void at25f512b_hal_wp_high()
{
	//!TODO not implemented yet
}

/**
 * @brief Write protect pin in low 
 */
static inline void at25f512b_hal_wp_low()
{
	//!TODO not implemented yet
}

/**
 * @brief Hold pin in high 
 */
static inline void at25f512b_hal_hold_high()
{
	//!TODO not implemented yet
}

/**
 * @brief Hold pin in low
 */
static inline void at25f512b_hal_hold_low()
{
	//!TODO not implemented yet
}

void at25df041_hal_init(void);
uint8_t at25df041_hal_byte(uint8_t byte);
void at25df041_hal_send(uint8_t* data, uint32_t nb);
void at25df041_hal_receive(uint8_t* data, uint32_t nb);
void at25df041_hal_deinit(void);

#endif
/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
