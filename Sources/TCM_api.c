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
 * @file     TCM_api.c
 * @brief    TCM API source file
 * @date     27-06-2013
 * @author   adam.borkowski
 *
 */

#include "TCM_api.h"
#include "TCM.h"
#include "Events.h"

uint8_t display_update[]  = {0x24, 0x01, 0x00, 0x00, 0x00};
uint8_t tcm_answer[200] ={0};

/**
 * @brief Function that displays the image on the device TCM
 * @return TCM response
 */
uint8_t TCM_DisplayUpdate(void)
{  										
	uint8_t i;

	while(!Busy_GetVal(Busy_DeviceData));
	for(i=0;i<25;i++){}
	TCM_CS_ClrVal(NULL);
	for(i=0;i<25;i++){}
	
	//Send display commands
	TCM_SendReceiveBlock(TCM_DeviceData, display_update, 5,tcm_answer,5);

	while(!TCM_GetBlockSentStatus(TCM_DeviceData));
	for(i=0;i<20;i++){}
	TCM_CS_SetVal(NULL);

	return tcm_answer[0];
}

/**
 * @breif Upload display image
 * @param upload_image_ptr Image pointer
 * @param image_size Image size
 * @return TCM response
 */
uint8_t TCM_ImageUpload(uint8_t *upload_image_ptr, uint8_t image_size)
{
	uint8_t i;

	while(!Busy_GetVal(Busy_DeviceData));//busy
	
	for(i=0;i<25;i++){}
	TCM_CS_ClrVal(NULL);	
	
	for(i=0;i<25;i++){}
	
	//Send image block
	TCM_SendReceiveBlock(TCM_DeviceData, upload_image_ptr, image_size, tcm_answer,image_size);	
		
	while(!TCM_GetBlockSentStatus(TCM_DeviceData));
	
	for(i=0;i<20;i++){}	 
	TCM_CS_SetVal(NULL);

	return tcm_answer[0];
}

/**
 * @brieg Get TCM response
 * @return
 */
uint8_t TCM_GetAnswer(void)
{
	uint8_t i;
	uint8_t nonetable[3]={0,0,0};

	while(!Busy_GetVal(Busy_DeviceData));
	
	tcm_answer[0]=0x0;
	
	for(i=0;i<25;i++){}
	TCM_CS_ClrVal(NULL);	
	
	for(i=0;i<25;i++){}
	
	TCM_SendReceiveBlock(TCM_DeviceData, nonetable, 2, tcm_answer,2);
	
	while(!TCM_GetBlockSentStatus(TCM_DeviceData));
	
	for(i=0;i<20;i++){}	 
	TCM_CS_SetVal(NULL);

	return tcm_answer[0];
}

/**
 * @brief Enable the TCM device
 */
void TCM_enable(void)
{
	TCM_EN_ClrVal(NULL);	
	WAIT_Waitms(40);
}

/**
 * @brief Disable the TCM device
 */
void TCM_disable(void)
{
	WAIT_Waitms(20);
	TCM_EN_SetVal(NULL);			
}

/* ***(C) COPYRIGHT Embedded Pico Systems 2015***   ***END OF FILE***   */
