//
// Created by dragos on 29.04.2018.
//

#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

void append(struct Node** head_ref, uintptr_t new_data)
{
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
	struct Node *last = *head_ref;
	new_node->data  = new_data;
	new_node->next = NULL;
	if (*head_ref == NULL)
	{
		*head_ref = new_node;
		return;
	}

	while (last->next != NULL)
		last = last->next;

	last->next = new_node;
	return;
}

void printList(struct Node *node)
{
	while (node != NULL)
	{
		printf(" %d ", (int) node->data);
		node = node->next;
	}
}

int contains(struct Node** head_ref, uintptr_t data)
{
	struct Node *last = *head_ref;
	if (*head_ref == NULL)
	{
		return 0;
	}
	else
	{
		do
		{
			if(data==last->data)
				return 1;
			last = last->next;
		}while (last->next != NULL);
	}

	return 0;
}
