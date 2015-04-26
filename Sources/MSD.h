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
 * @file     MSD.h
 * @brief    Header file for MSD
 * @date     20-01-2014
 * @authors  MpicoSys Team
 *
 */

#ifndef MSD_H_
#define MSD_H_

/* Function prototype */
static void MS_ReadFlashMemory(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD);
void MS_WriteFlashMemory(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD);
static void MassStorageTask(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD);
void SendCommandStatusWrapper(LDD_TDeviceData *USBDevDataPtr, LDD_USB_Device_TTD* pTD);

#endif /* MSD_H_ */
/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
