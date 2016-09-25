/*
 * buttons.c
 *
 *  Created on: Sep 25, 2016
 *      Author: shapa
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "bsp.h"
#include "systemTimer.h"
#include "timers.h"

#define KEY_DEBOUNCE_TOUT 50
#define KEY_REPEAT_TOUT 250

static struct {
	uint32_t timerId;
	_Bool state;
	const _Bool isRepeatable;
} s_buttons[BUTTON_LAST]= {
		[BUTTON_MENU] =  { .isRepeatable = false },
		[BUTTON_LEFT] =  { .isRepeatable = true },
		[BUTTON_RIGHT] = { .isRepeatable = true },
		[BUTTON_OK] = 	 { .isRepeatable = false },
};

static void onButtonIsr(Buttons_t button, _Bool state);
static void onDebounceTimer(uint32_t id, void *data);
static void onRepeatTimer(uint32_t id, void *data);

void EXTI0_IRQHandler(void) {
	if (EXTI_GetFlagStatus(EXTI_Line0)) {
		_Bool state = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
		onButtonIsr(BUTTON_RIGHT, state);
		EXTI_ClearFlag(EXTI_Line0);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void) {
	if (EXTI_GetFlagStatus(EXTI_Line1)) {
		_Bool state = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
		onButtonIsr(BUTTON_LEFT, state);
		EXTI_ClearFlag(EXTI_Line1);
	}
}

void EXTI2_IRQHandler(void) {
	if (EXTI_GetFlagStatus(EXTI_Line2)) {
		_Bool state = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2);
		onButtonIsr(BUTTON_MENU, state);
		EXTI_ClearFlag(EXTI_Line2);
	}
}

void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetFlagStatus(EXTI_Line7)) {
		_Bool state = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
		onButtonIsr(BUTTON_OK, state);
		EXTI_ClearFlag(EXTI_Line7);
	}
}

static void onButtonIsr(Buttons_t button, _Bool state) {
	if (button < BUTTON_LAST) {
		Timer_delete(s_buttons[button].timerId);
		s_buttons[button].timerId = INVALID_HANDLE;
		if (state ^ s_buttons[button].state) {
			s_buttons[button].state = state;
			if (state) {
				//press
				s_buttons[button].timerId = Timer_newArmed(KEY_DEBOUNCE_TOUT, false, onDebounceTimer, (void*)button);
			} else {
				Event_t event = { EVENT_EXTI, { ES_EXTI_UP }, .data.intptr = button };
				BSP_queuePush(&event);
			}
		}
	}
}

static void onDebounceTimer(uint32_t id, void *data) {
	Buttons_t button = (Buttons_t)data;
	if ((button < BUTTON_LAST) && (s_buttons[button].timerId == id)) {
		Event_t event = { EVENT_EXTI, { ES_EXTI_DOWN }, .data.intptr = button };
		BSP_queuePush(&event);
		s_buttons[button].timerId = INVALID_HANDLE;
		if (s_buttons[button].isRepeatable) {
			s_buttons[button].timerId = Timer_newArmed(KEY_REPEAT_TOUT, true, onRepeatTimer, (void*)button);
		}
	}
}

static void onRepeatTimer(uint32_t id, void *data) {
	Buttons_t button = (Buttons_t)data;
	if ((button < BUTTON_LAST) && (s_buttons[button].timerId == id)) {
		Event_t event = { EVENT_EXTI, { ES_EXTI_REPEAT }, .data.intptr = button };
		BSP_queuePush(&event);
	}
}