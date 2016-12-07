/*
 * system.c
 *
 *  Created on: Dec 7, 2016
 *      Author: shapa
 */

#include "system.h"
#include "stm32f10x.h"

int System_Lock(void) {
	int primask = 0;
	return primask;
}

void System_Unlock(int primask) {
	if (!primask) {
		;
	}
}
