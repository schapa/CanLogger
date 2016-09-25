
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void Init_lcd(void);
void Lcd_clear(void);
void Lcd_goto(uint8_t x, uint8_t y);
void Lcd_write_str(char *STRING);

#ifdef __cplusplus
}
#endif
