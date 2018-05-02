//
// Created by Dragos Petrescu on 29.04.2018.
//

#ifndef TEMA3_LINKED_LIST_H
#define TEMA3_LINKED_LIST_H

#include <stdint.h>

void append(uintptr_t new_data);
int contains(uintptr_t data);

#define MAX_NUM_OF_PAGES 32768
/*
 * 2 ^ 15 max num of pages in 32b machines
 */

#endif //TEMA3_LINKED_LIST_H
