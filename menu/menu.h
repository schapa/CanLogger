/*
 * menu.h
 *
 *  Created on: Sep 27, 2016
 *      Author: shapa
 */

#ifndef MENU_H_
#define MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "menuItem.h"
#include "bsp.h"

void Menu_Activate(void);
void Menu_Dectivate(void);

void Menu_HandleKey(Buttons_t key, ExtiSubTypes_t type);

Menu_t *Menu_GetMainMenu(void);

#ifdef __cplusplus
}
#endif


#endif /* MENU_H_ */
