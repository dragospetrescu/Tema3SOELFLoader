//
// Created by dragos on 29.04.2018.
//

#ifndef TEMA3_LINKED_LIST_H
#define TEMA3_LINKED_LIST_H

#include <stdint.h>

typedef struct Node
{
	uintptr_t data;
	struct Node *next;
} LINKED_LIST;


void append(struct Node** head_ref, uintptr_t new_data);
int contains(struct Node* head_ref, uintptr_t data);
void printList(struct Node *node);

#endif //TEMA3_LINKED_LIST_H
