//
// Created by Dragos Petrescu on 29.04.2018.
//

#include "already_mapped_pages.h"

uintptr_t used[MAX_NUM_OF_PAGES];
int pos_in_used;

void append(uintptr_t new_data)
{
	used[pos_in_used] = new_data;
	pos_in_used++;
}

int contains(uintptr_t data)
{
	for (int i = 0; i < pos_in_used; ++i)
		if (used[i] == data)
			return 1;
	return 0;
}
