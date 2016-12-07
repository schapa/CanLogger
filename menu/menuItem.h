/*
 * menuItem.h
 *
 *  Created on: Sep 27, 2016
 *      Author: shapa
 */

#ifndef MENUITEM_H_
#define MENUITEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct MenuNode Menu_t;

typedef void (*OnMenuEntranceCb) (void);
typedef const char* (*GetItemNameCb) (const uint32_t focusIdx);
typedef Menu_t* (*OnSelectionCb) (const uint32_t focusIdx);

struct MenuNode {
	Menu_t* parent;
	const char *title;
	uint32_t focusIdx;
	uint32_t itemsCount;
	GetItemNameCb onFocus;
	OnMenuEntranceCb onEnter;
	OnMenuEntranceCb onExit;
	OnSelectionCb onSelection;
};

typedef const char* (*GetCurrentValueCb) (void);
typedef _Bool (*GetCurrentStatusCb)(void);
typedef Menu_t* (*OnItemSelectionCb) (void);


typedef struct MenuItemNode {
	const char *title;
	GetCurrentStatusCb getCurrentStatus;
	GetCurrentValueCb getCurrentValue;
	OnItemSelectionCb onSelection;
} MenuItem_t;

static inline _Bool MenuItemsAlwaysPresent(void) { return true; }

#ifdef __cplusplus
}
#endif


#endif /* MENUITEM_H_ */
