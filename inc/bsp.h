/*
 * bsp.h
 *
 *  Created on: Sep 8, 2016
 *      Author: shapa
 */

#ifndef BSP_H_
#define BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include <stdbool.h>
#include "stm32f10x_can.h"
#include "Queue.h"
#include "buttons.h"

/* Should be called Once */
_Bool BSP_init(void);

void BSP_SetRedLedState(FunctionalState);
void BSP_SetGreenLedState(FunctionalState);

void BSP_queuePush(Event_p pEvent);
void BSP_pendEvent(Event_p pEvent);


void BSP_LcdBacklight(_Bool);


#ifdef __cplusplus
}
#endif

#endif /* BSP_H_ */
