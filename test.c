#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "config.h"
#include "kasse.h"
#include "credit_manager.h"

int main(){
	BYTE i=0;
	load_items();
	load_state();
	for (; i < num_items; ++i)
		printf("Eintrag %x: %s (%d Cents, %d mal verkauft)\n", i, status[i].item_name, status[i].price, status[i].times_sold);
}