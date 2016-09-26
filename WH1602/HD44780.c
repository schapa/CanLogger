
#include "HD44780.h"
#include "systemTimer.h"

#define MIN_DELAY 1

#define HD44780_GPIO    	GPIOA
#define HD44780_APB 		RCC_APB2Periph_GPIOA
#define HD44780_PIN_E 		GPIO_Pin_4
#define HD44780_PIN_RW		GPIO_Pin_5
#define HD44780_PIN_RS		GPIO_Pin_6

#define RS  HD44780_PIN_RS
#define RW  HD44780_PIN_RW
#define E   HD44780_PIN_E

#define HD44780_PIN_D4		GPIO_Pin_3
#define HD44780_PIN_D5		GPIO_Pin_2
#define HD44780_PIN_D6		GPIO_Pin_1
#define HD44780_PIN_D7		GPIO_Pin_0


#define Function_8BIT_set 			0b00110011 //8-bit
#define Function_4BIT_set 			0b00100010 //4-bit
#define Function_set 				0b00101000 //4-bit,2 - line mode, 5*8 dots
#define Display_on_off_control		0b00001100 // display on,cursor off,blink off
#define Display_clear				0b00000001
#define Entry_mode_set				0b00000110


static const uint8_t s_russianBitmap[] = {
		0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45,
		0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7,
		0x4D, 0x48, 0x4F, 0xA8, 0x50, 0x43,
		0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB,
		0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF,
		0xB0, 0xB1, 0x61, 0xB2,	0xB3, 0xB4,
		0xE3, 0x65, 0xB6, 0xB7,	0xB8, 0xB9,
		0xBA, 0xBB, 0xBC, 0xBD,	0x6F, 0xBE,
		0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78,
		0xE5, 0xC0,	0xC1, 0xE6, 0xC2, 0xC3,
		0xC4, 0xC5, 0xC6, 0xC7
};
#define UNICODE_BASE 0xC0

static void setDataBits(uint8_t val);
static void sendData(uint8_t data);
static void sendCmd(uint8_t val);
static void Control(uint32_t param, _Bool value);
static void writeCmdNibble(uint8_t cmd);

void LCD_Print(const char *str)
{
	while (*str != '\0') {
		if ((uint8_t)*str < UNICODE_BASE) {
			sendData((uint8_t)*str);
		} else {
			sendData(s_russianBitmap [*str - UNICODE_BASE]);
		}
		str++;
	}
}


void LCD_SetPosition(uint8_t x, uint8_t y)
{
	x = (x > 1) ? 1 : x;
	y = (y > 15) ? 0 : y;
	uint8_t pos = (uint8_t)(0x80 + y + 0x40*(x));
	sendCmd(pos);
}

void LCD_Init(void) {
	System_delayMsDummy(150);
	writeCmdNibble(Function_8BIT_set);
	writeCmdNibble(Function_8BIT_set);
	writeCmdNibble(Function_8BIT_set);
	writeCmdNibble(Function_4BIT_set);

	sendCmd(Function_set);
	System_delayMsDummy(2*MIN_DELAY);
	sendCmd(Display_on_off_control);
	System_delayMsDummy(2*MIN_DELAY);
	sendCmd(Display_clear);
	System_delayMsDummy(2*MIN_DELAY);
    sendCmd(Entry_mode_set);
    LCD_Clear();
}

void LCD_Clear(void)
{
	sendCmd(Display_clear);
}

static void setDataBits(uint8_t val) {
	uint8_t newValue = 0;
	HD44780_GPIO->BRR |= HD44780_PIN_D4 | HD44780_PIN_D5 | HD44780_PIN_D6 | HD44780_PIN_D7;
	if (val & 1)
		newValue |= HD44780_PIN_D4;
	if (val & 2)
		newValue |= HD44780_PIN_D5;
	if (val & 4)
		newValue |= HD44780_PIN_D6;
	if (val & 8)
		newValue |= HD44780_PIN_D7;
	HD44780_GPIO->BSRR |= newValue;
}

static void sendData(uint8_t data) {
	setDataBits(data>>4);
	Control(E | RS,1);
	System_delayMsDummy(MIN_DELAY);
	Control(E,0);
	System_delayMsDummy(MIN_DELAY);
	setDataBits(data);
	Control(E | RS,1);
	System_delayMsDummy(MIN_DELAY);
	Control(E | RW,0);
	System_delayMsDummy(MIN_DELAY);
	Control(RS,0);
}

static void sendCmd(uint8_t cmd) {
	Control(RS,0);
	setDataBits(cmd>>4);
	Control(E,1);
	System_delayMsDummy(MIN_DELAY);
	Control(E,0);
	System_delayMsDummy(MIN_DELAY);
	setDataBits(cmd);
	Control(E,1);
	System_delayMsDummy(MIN_DELAY);
	Control(E | RS | RW, 0);
	System_delayMsDummy(MIN_DELAY);
}

static void Control(uint32_t param, _Bool value) {
	if (value)
		HD44780_GPIO->BSRR |= param;
	else
		HD44780_GPIO->BRR |= param;
}

static void writeCmdNibble(uint8_t cmd) {
	setDataBits(cmd);
	Control(RS,0);
	Control(E,1);
	System_delayMsDummy(MIN_DELAY);
	Control(E,0);
	System_delayMsDummy(MIN_DELAY);
	Control(RS | RW, 0);
	System_delayMsDummy(MIN_DELAY);
}

