
#include <stdio.h>
#include <stdlib.h>
#include "dbg_base.h"
#include "bsp.h"
#include "HD44780.h"
#include "systemTimer.h"
#include "menu.h"

#if 1
#include "dbg_trace.h"
#endif

int main(int argc, char* argv[]);

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	BSP_init();
	LCD_Init();
	Menu_Activate();
	LCD_Update();

	while (true) {
//		char buffer[128];
		Event_t event;
		System_pendEvent(&event);
		switch (event.type) {
			case EVENT_SYSTICK:
				if (event.subType.systick == ES_SYSTICK_SECOND_ELAPSED) {
//					sprintf(buffer, "Testing %02d", event.data.intptr);
//					LCD_SetPosition(0, 0);
//					LCD_SetText(buffer, NULL);
				}
				break;
			case EVENT_EXTI:
				Menu_HandleKey((Buttons_t)event.data.intptr, event.subType.exti);
				break;
			default:
				break;
		}
		LCD_Update();
    }
}
