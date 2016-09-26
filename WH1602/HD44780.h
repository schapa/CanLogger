
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetPosition(uint8_t x, uint8_t y);
void LCD_Print(const char *str);

#ifdef __cplusplus
}
#endif
