
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
	Init_lcd();
	Lcd_goto(0, 0);
	DBGMSG_INFO("System Starting");

	while (1) {
		char buffer[128];
		Event_t event;
		BSP_pendEvent(&event);
		if ((event.type == EVENT_SYSTICK) && (event.subType.systick == ES_SYSTICK_SECOND_ELAPSED)) {
			sprintf(buffer, "Hello my tt %02d", event.data.intptr);
			Lcd_goto(0, 0);
			Lcd_write_str(buffer);
		} else {
			if (event.type == EVENT_EXTI) {
				sprintf(buffer, "[%d] %d", event.data.intptr, event.subType.exti);
				Lcd_goto(0, 0);
				Lcd_write_str(buffer);
				DBGMSG_INFO("EXTI %s", buffer);
			}
		}
    }
}
