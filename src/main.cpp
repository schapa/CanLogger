
#include <stdio.h>
#include <stdlib.h>
#include "dbg_base.h"
#include "bsp.h"
#include "HD44780.h"
#include "systemTimer.h"
#include "menuNode.h"
#include "timers.h"

#if 1
#include "dbg_trace.h"
#endif

static void onTimerFire(uint32_t id);

int main(int argc, char* argv[]);

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

//	FILE *f = fopen("aaa","w+");
//	if (!f)
//		return -1;

	Timer_init(onTimerFire);
	BSP_init();
	LCD_Init();
	Menu_Activate();
	LCD_Update();

	while (true) {
		Event_t event;
		System_pendEvent(&event);
		switch (event.type) {
			case EVENT_SYSTICK:
				if (event.subType.systick == ES_SYSTICK_SECOND_ELAPSED) {
//					char buffer[128];
//					sprintf(buffer, "Testing %02d", event.data.intptr);
//					LCD_SetPosition(0, 0);
//					LCD_SetText(buffer, NULL);
				}
				break;
			case EVENT_TIMCALL:
				Timer_onTimerCb(event.data.intptr);
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

static void onTimerFire(uint32_t id) {

	Event_t evt = { EVENT_TIMCALL };
	evt.data.intptr = (intptr_t)id;
	System_queuePush(&evt);
}
