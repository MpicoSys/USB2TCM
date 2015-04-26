/* CodeWarrior ARM Runtime Support Library 
 * Copyright © 2012 FReescale. All rights reserved. 
 * 
 * $Date: 2012/06/25 20:12:19 $ 
 * $Revision: 1.1.2.1 $ 
 */ 

/*

FILE
	__arm_end.c

DESCRIPTION

	Use this file for C.
	
	Interface for board-level termination.
	
*/
#ifdef __cplusplus
extern "C" {
#endif
#include <ansi_parms.h>
#include <CWCPlusLib.h>


#if defined(__SEMIHOSTING)
extern void sys_exit(int status);
#endif

/*
 *	_ExitProcess
 *
 */
extern void _ExitProcess(int status) _EWL_WEAK;
extern void _ExitProcess(int status)
{
#if defined(__SEMIHOSTING)
	//sys_exit(status);
#endif
	while (1);
}
#ifdef __cplusplus
}
#endif
