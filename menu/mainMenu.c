/*
 * mainMenu.c
 *
 *  Created on: Sep 27, 2016
 *      Author: shapa
 */

#include "menu.h"
#include "memman.h"
#include "dbg_base.h"

#if 1
#include "dbg_trace.h"
#endif


static Menu_t *onSelectionSettings(void);

static MenuItem_t s_menuItemsTemplate[] = {
		{ "Settings", MenuItemsAlwaysPresent, NULL, onSelectionSettings },
		{ "Second", MenuItemsAlwaysPresent, NULL, NULL },
		{ "Third", MenuItemsAlwaysPresent, NULL, NULL },
		{ "a", MenuItemsAlwaysPresent, NULL, NULL },
		{ "aa", MenuItemsAlwaysPresent, NULL, NULL },
		{ "aaa", MenuItemsAlwaysPresent, NULL, NULL },
		{ "aaaa", MenuItemsAlwaysPresent, NULL, NULL },
		{ "aaaaa", MenuItemsAlwaysPresent, NULL, NULL },
};
static size_t s_menuItemsSize = sizeof(s_menuItemsTemplate)/sizeof(*s_menuItemsTemplate);

static MenuItem_t **s_menuItems = NULL;

static void onEntry(void);
static void onExit(void);
static const char* getItemName(const uint32_t focusIdx);
static Menu_t* onSelection(const uint32_t focusIdx);

static Menu_t s_mainMenu = {
		NULL,
		"Root Menu",
		0, 0,
		.onFocus = getItemName,
		.onEnter = onEntry,
		.onExit = onExit,
		.onSelection = onSelection
};

Menu_t *Menu_GetMainMenu(void) {
	return &s_mainMenu;
}

static void onEntry(void) {
	size_t elementsCount = 0;
	for (size_t i = 0; i < s_menuItemsSize; i++) {
		if (s_menuItemsTemplate[i].getCurrentStatus && s_menuItemsTemplate[i].getCurrentStatus()) {
			elementsCount++;
		}
	}
	DBGMSG_M("Will add %d from %d elements", elementsCount, s_menuItemsSize);
	if (!elementsCount) {
		return;
	}
	DBGMSG_M("Allocate %d bytes", elementsCount*sizeof(MenuItem_t*));
	s_menuItems = (MenuItem_t**)MEMMAN_malloc(elementsCount*sizeof(MenuItem_t*));
	if (!s_menuItems) {
		return;
	}
	s_mainMenu.itemsCount = 0;
	for (size_t i = 0; i < s_menuItemsSize; i++) {
		if (s_menuItemsTemplate[i].getCurrentStatus && s_menuItemsTemplate[i].getCurrentStatus()) {
			DBGMSG_M("Add <%s> on %d pos", s_menuItemsTemplate[i].title, s_mainMenu.itemsCount);
			s_menuItems[s_mainMenu.itemsCount++] = &s_menuItemsTemplate[i];
		}
	}
}

static void onExit(void) {
	MEMMAN_free(s_menuItems);
	s_menuItems = NULL;
}

static const char* getItemName(const uint32_t focusIdx) {
	const char *name = NULL;
	if (focusIdx < s_menuItemsSize) {
		name = s_menuItemsTemplate[focusIdx].title;
	}
	return name;
}

static Menu_t* onSelection(const uint32_t focusIdx) {
	Menu_t *menu = &s_mainMenu;
	if (focusIdx < s_menuItemsSize) {
		if (s_menuItemsTemplate[focusIdx].onSelection)
			menu = s_menuItemsTemplate[focusIdx].onSelection();
	}
	return menu;
}

/* menu item handlers */

static Menu_t *onSelectionSettings(void) {
	DBGMSG_M("SELECTED!");
	return Menu_GetMainMenu();
}
