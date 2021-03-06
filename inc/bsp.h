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

#include <stdbool.h>
#include "stm32f10x.h"

/* Should be called Once */
_Bool BSP_init(void);

void BSP_SetRedLedState(FunctionalState);
void BSP_SetGreenLedState(FunctionalState);

void BSP_LcdBacklight(_Bool);


#ifdef __cplusplus
}
#endif

#endif /* BSP_H_ */
