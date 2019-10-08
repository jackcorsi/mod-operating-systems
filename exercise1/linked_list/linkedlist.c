/**
 *  Source file for a linked list in C
 *
 * @authors:   		Michael Denzel
 * @creation_date:	2016-09-05
 * @contact:		m.denzel@cs.bham.ac.uk
 */

//standard includes
#include "stdlib.h"
#include "stdio.h"

//own includes
#include "linkedlist.h"


// TODO: Implement those methods!
int get(list * l, unsigned int index){
	list_node *n = l->hd;

	for (; index > 0; index--) {
		if (n)
			n = n->next;
		else
			return -1;
	}

	return n->data;
}

int prepend(list * l, int data){
	list_node *new_node = (list_node *) malloc(sizeof(list_node));
	if (!new_node)
		return -1;

	new_node->data = data;
	if (l->hd)
		new_node->next = l->hd->next;

	l->hd = new_node;
	return 0;
}

int append(list * l, int data){
	list_node *new_node = (list_node *) malloc(sizeof(list_node));
	if (!new_node)
		return -1;

	new_node->data = data;
	new_node->next = NULL;

	if (!l->hd)
		l->hd = new_node;
	else {
		list_node *n = l->hd;
		while (n->next)
			n = n->next;

		n->next = new_node;
	}

	return 0;
}

int remove_element(list * l, unsigned int index){
	if (index == 0) {
		list_node *target = l->hd;
		if (!target)
			return -1;
		else {
			l->hd = target->next;
			free(target);
			return 0;
		}
	} else {
		list_node *n = l->hd;
		for (; index > 1; index--) { //Stop one early
			if (n)
				n = n->next;
			else
				return -1;
		}

		list_node *target = n->next;
		if (!target)
			return -1;
		else {
			n->next = target->next;
			free(target);
			return 0;
		}
	}
}

int insert(list * l, unsigned int index, int data){
	list_node *new_node = (list_node *) malloc(sizeof(list_node));
	if (!new_node)
		return -1;

	list_node *n = l->hd;

	if (index == 0) {
		if (!n)
			return -1;
		else {
			new_node->next = n->next;
			n->next = new_node;
		}
	} else {
		for (; index > 0; index--) {
			if (n->next)
				n = n->next;
			else
				return -1;
		}

		new_node->next = n->next;
		n->next = new_node;
	}

	new_node->data = data;
	return 0;
}

void print_list(list * l){
	list_node *n = l->hd;
	if (!n) {
		printf("empty list\n");
		return;
	}

	do {
		printf("%i ", n->data);
		n = n->next;
	} while (n);

	printf("\n");
}

void init(list * l){
	l->hd = NULL;
}

void destroy(list *l){
	list_node *n = l->hd;
	l->hd = NULL;

	while (n) {
		list_node *next = n->next;
		free(n);
		n = next;
	}
}


