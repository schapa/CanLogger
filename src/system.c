/*
 * system.c
 *
 *  Created on: Dec 7, 2016
 *      Author: shapa
 */

#include "system.h"
#include "stm32f10x.h"
#include <stddef.h>

static volatile EventQueue_p s_eventQueue = NULL;


void System_queuePush(Event_p pEvent) {
	int primask = System_Lock();
	s_eventQueue = Queue_pushEvent(s_eventQueue, pEvent);
	System_Unlock(primask);
}

void System_pendEvent(Event_p pEvent) {
	while (!s_eventQueue);
	int primask = System_Lock();
	s_eventQueue = Queue_getEvent(s_eventQueue, pEvent);
	System_Unlock(primask);
}
