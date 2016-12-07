/*
 * lcd.c
 *
 *  Created on: Dec 7, 2016
 *      Author: shapa
 */

#include "HD44780.h"
#include <stdint.h>
#include <stdio.h>

static char s_text[2][16];

void LCD_Init(void) {
	LCD_Clear();
}
void LCD_Clear(void) {
	snprintf(s_text[0], sizeof(s_text[0]), "");
	snprintf(s_text[1], sizeof(s_text[1]), "");

}
void LCD_SetPosition(uint8_t x, uint8_t y) {

}

void LCD_SetText(const char *line1, const char *line2) {
	snprintf(s_text[0], sizeof(s_text[0]), "%s", line1);
	snprintf(s_text[1], sizeof(s_text[1]), "%s", line2);
}

void LCD_Update(void) {
	printf("%s\n", s_text[0]);
	printf("%s\n", s_text[1]);
}
