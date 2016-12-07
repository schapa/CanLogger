/*
 * system.h
 *
 *  Created on: Dec 7, 2016
 *      Author: shapa
 */

#pragma once

#include "Queue.h"

/* locks isr. Critical section entry. return isr state before call */
int System_Lock(void);

/* Unlocks isr. Critical section exit. accept isr state before crit section entry */
void System_Unlock(int primask);

void System_queuePush(Event_p pEvent);
void System_pendEvent(Event_p pEvent);
