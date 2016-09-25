/*
 * tracer.h
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#ifndef TRACER_H_
#define TRACER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

void Trace_InitUSART1(void);
void Trace_dataAsync(char *buff, size_t size);
void Trace_dataAsyncFlush(void);
void Trace_dataSync(const char *buff, size_t size);


#ifdef __cplusplus
}
#endif


#endif /* TRACER_H_ */
