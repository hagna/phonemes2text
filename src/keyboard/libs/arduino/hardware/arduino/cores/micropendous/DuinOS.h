/*
	duinOS Copyright (C) 2009 Multiplo
	http://multiplo.org
	http://robotgroup.com.ar
	Created by Julián U. da Silva Gillig.

	Adapted for the Micropendous Project by Opendous Inc. - (C) 2010-12
	www.Micropendous.org/LUFAduino

	Based on FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

    This file is part of the duinOS extension of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


#ifndef DuinOS__h
#define DuinOS__h

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>

#include "FreeRTOSConfig.h"

#ifdef __cplusplus
} // extern "C"
#endif

//With the USB AVRs use only 5 priorities:
#define LOW_PRIORITY		(configMAX_PRIORITIES - 5)
#define NORMAL_PRIORITY		(configMAX_PRIORITIES - 4)
#define HIGH_PRIORITY		(configMAX_PRIORITIES - 3)

#define taskLoop(name)\
void name##Function();\
xTaskHandle name;\
void name##_Task(void *pvParameters)\
{\
	for(;;)\
		name##Function();\
}\
void name##Function()

//This macro enables the forward declaration of a task, to allow other tasks previous defined (with the
//taskLoop()macro use and reference them:
#define declareTaskLoop(name) extern xTaskHandle name

#define createTaskLoop(name, priority)\
{\
	xTaskCreate(name##_Task, (signed portCHAR *) #name, configMINIMAL_STACK_SIZE, NULL, priority, &name);\
}

#define createTaskLoopWithStackSize(name, priority, ssize)\
{\
	xTaskCreate(name##_Task, (signed portCHAR *) #name, ssize, NULL, priority, &name);\
}

#define suspend() vTaskSuspend(NULL)

#define suspendTask(name) vTaskSuspend(name)

#define suspendAll() vTaskSuspendAll()

#define resumeTask(name) vTaskResume(name)

#define resumeAll() xTaskResumeAll()

#define nextTask() taskYIELD()

#define delay(ticks) vTaskDelay(ticks)
/*
inline void delay(const portTickType ticks)
{
	portTickType xLastWakeTime = xTaskGetTickCount();

	//Better than vTaskDelay:
	vTaskDelayUntil( &xLastWakeTime, ticks);
}
*/

//This macro is quiet different from setPriority, because this works even in those CPUs wich does not support
//the set/getPriority macros (due to their small RAM memories). And, this only has effect if called in setup().
#define initMainLoopPriority(priority) (mainLoopPriority = priority)

//These only works if INCLUDE_vTaskPrioritySet / INCLUDE_vTaskPriorityGet are != 0
//(disabled for CPUs with less than 2KB RAM):
#if INCLUDE_uxTaskPriorityGet //This #if is to improve the error readability.
	#define getPriority(name) uxTaskPriorityGet(name)
#endif
#if INCLUDE_vTaskPrioritySet
	#define setPriority(name, priority) uxTaskPrioritySet(name, priority)
#endif

//##In bigger CPUs, DuinOS may use cTaskDelete, and uxTaskPrioritySet/Get.

#endif
