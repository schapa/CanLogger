
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define LCD_WIDTH 16
#define LCD_HEIGTH 2

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetPosition(uint8_t x, uint8_t y);

void LCD_SetText(const char *line1, const char *line2);
void LCD_Update(void);

#ifdef __cplusplus
}
#endif
