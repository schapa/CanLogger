/*
 * emulator.c
 *
 *  Created on: Dec 7, 2016
 *      Author: shapa
 */

#include "core_cmEmu.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

void Trace_dataAsync(char *buff, size_t size) {
	printf("%s", buff);
}

_Bool BSP_init(void) {
	return true;
}
//uint32_t SysTick_Config(uint32_t ticks) {
//	printf("emu! start tick! %d\n", ticks);
//	return 0;
//}
