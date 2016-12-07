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

void RCC_GetClocksFreq(RCC_ClocksTypeDef *clk) {
	if (!clk)
		return;
	clk->SYSCLK_Frequency = 72*1000*1000;
	clk->HCLK_Frequency = clk->SYSCLK_Frequency/2;
	clk->PCLK1_Frequency = clk->HCLK_Frequency/2;
	clk->PCLK2_Frequency = clk->HCLK_Frequency/2;
	clk->ADCCLK_Frequency = clk->PCLK2_Frequency/2;
}
