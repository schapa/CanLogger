
#include <stdio.h>
#include <stdlib.h>
#include "dbg_base.h"
#include "bsp.h"
#include "HD44780.h"
#include "systemTimer.h"

#if 1
#include "dbg_trace.h"
#endif

int main(int argc, char* argv[]);

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	BSP_init();
	LCD_Init();
	LCD_SetPosition(0, 0);
	DBGMSG_INFO("System Starting");

	while (1) {
		char buffer[128];
		Event_t event;
		BSP_pendEvent(&event);
		if ((event.type == EVENT_SYSTICK) && (event.subType.systick == ES_SYSTICK_SECOND_ELAPSED)) {
			sprintf(buffer, "Hello my tt %02d", event.data.intptr);
			LCD_SetPosition(0, 0);
			LCD_Print(buffer);
		} else {
			if (event.type == EVENT_EXTI) {
				sprintf(buffer, "[%d] %d", event.data.intptr, event.subType.exti);
				LCD_SetPosition(1, 0);
				LCD_Print(buffer);
				DBGMSG_INFO("EXTI %s", buffer);
			}
		}
    }
}
