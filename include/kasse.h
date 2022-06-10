#ifndef KASSE_H
#define KASSE_H

#include "general.h"

void print_the_buffer(void);
void print_kasse_screen_items(BYTE current_selection);

enum kasse_menu { MENU_UNDEFINED = 0, MENU_MAIN };

#endif
