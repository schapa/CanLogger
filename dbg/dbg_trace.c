/*
 * dbg_trace.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "stm32f10x.h"
#include "dbg_trace.h"
#include "memman.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "tracer.h"
#include "systemTimer.h"

static char s_msgBuffer[1024];
static size_t s_msgBufferSize = sizeof(s_msgBuffer);

void dbgmsg(const char *color, const char *siverity, const char *file, const char *func, int line, const char *fmt, ...) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	size_t occupied = 0;
	if (line) {
		occupied = (size_t)snprintf(s_msgBuffer, s_msgBufferSize, "[%8lu.%03lu] %s::%s (%d)%s %s: ",
				System_getUptime(), System_getUptimeMs(), file, func, line, color, siverity);
	} else {
		occupied = (size_t)snprintf(s_msgBuffer, s_msgBufferSize, "[%8lu.%03lu] %s ",
				System_getUptime(), System_getUptimeMs(), color);
	}
	if (occupied < s_msgBufferSize) {
		va_list ap;
		va_start (ap, fmt);
		occupied += (size_t)vsnprintf(&s_msgBuffer[occupied], s_msgBufferSize - occupied, fmt, ap);
		va_end (ap);
	}
	if (occupied < s_msgBufferSize) {
		occupied += (size_t)snprintf(&s_msgBuffer[occupied], s_msgBufferSize - occupied, ANSI_ESC_DEFAULT"\r\n");
	}
	if (occupied > s_msgBufferSize) {
		char *trim = "...";
		size_t size = strlen(trim) + 1;
		snprintf(&s_msgBuffer[s_msgBufferSize-size], size, trim);
	}
	char *newBuff = (char*)MEMMAN_malloc(occupied);
	if (newBuff) {
		memcpy((void*)newBuff, (void*)s_msgBuffer, occupied);
	}
	if (!primask) {
		__enable_irq();
	}
	if (newBuff) {
		Trace_dataAsync(newBuff, occupied);
	}
}
