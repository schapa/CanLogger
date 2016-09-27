/*
 * menu.c
 *
 *  Created on: Sep 27, 2016
 *      Author: shapa
 */


#include "menu.h"
#include <assert.h>
#include "HD44780.h"
#include "dbg_base.h"

#if 1
#include "dbg_trace.h"
#endif

 Menu_t *s_root = NULL;

static void activateMenu(void);
static void backupMenu(void);
static void updateMenu(void);

void Menu_Activate(void) {
	if (s_root) {
		DBGMSG_ERR("Menu already active");
		return;
	}
	s_root = Menu_GetMainMenu();
	activateMenu();
}

void Menu_Dectivate(void) {
	if (!s_root) {
		DBGMSG_M("No active menu");
		return;
	}
	while (s_root) {
		if (s_root->onExit) {
			s_root->onExit();
		}
		s_root = s_root->parent;
	}
}

void Menu_HandleKey(Buttons_t key, ExtiSubTypes_t type) {
	if (!s_root) {
		DBGMSG_M("No active menu");
		Menu_Activate();
//		return;
	}
	switch (key) {
	case BUTTON_BACK:
		if (type == ES_EXTI_DOWN) {
			backupMenu();
		}
		break;
	case BUTTON_LEFT:
		if (type != ES_EXTI_UP) {
			if (s_root->focusIdx) {
				s_root->focusIdx--;
				updateMenu();
			}
		}
		break;
	case BUTTON_RIGHT:
		if (type != ES_EXTI_UP) {
			if (s_root->focusIdx < s_root->itemsCount - 1) {
				s_root->focusIdx++;
				updateMenu();
			}
		}
		break;
	case BUTTON_OK:
		if (type == ES_EXTI_DOWN) {
			Menu_t *newMenu = s_root->onSelection(s_root->focusIdx);
			if (newMenu && newMenu != s_root) {
				newMenu->parent = s_root;
				s_root = newMenu;
				activateMenu();
			}
		}
		break;
	default:
		break;
	}
}

static void activateMenu(void) {
	assert(s_root);

	if (s_root->onEnter) {
		s_root->onEnter();
	}
	LCD_SetText(s_root->title, s_root->onFocus(0));
}

static void backupMenu(void) {
	assert(s_root);

	if (s_root->onExit) {
		s_root->onExit();
	}
	s_root = s_root->parent;
	if (s_root) {
		updateMenu();
	}

}

static void updateMenu(void) {
	assert(s_root);
	assert(s_root->onFocus);
	const char *title = s_root->onFocus(s_root->focusIdx);
	const char *title2 = s_root->focusIdx + 1 < s_root->itemsCount ? s_root->onFocus(s_root->focusIdx + 1) : "";
	DBGMSG_M("Updating menu, Focus %d <%s> <%s>", s_root->focusIdx, title, title2);
	LCD_SetText(title, title2);
}
