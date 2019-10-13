#include "linkedlist.h"
#include "assert.h"
#include "stdio.h"

int main() {
	list my_list;
	list *l = &my_list;
	init(l);

	print_list(l); //empty list
	append(l, 21);
	print_list(l); //21
	printf("get 0\n");
	assert(get(l, 0) == 21);

	printf("get 1\n");
	assert(get(l, 1) == -1);

	printf("get 27\n");
	assert(get(l, 27) == -1);

	printf("After asserts\n");
	prepend(l, -7);
	print_list(l); //-7, 21
	assert(get(l, 0) == -7);
	assert(get(l, 1) == 21);
	assert(get(l, 2) == -1);
	
	append(l, 6);
	assert(get(l, 2) == 6);
	print_list(l); //-7, 21, 6
	destroy(l);
	print_list(l); //empty list
	init(l);
	print_list(l); //empty list
	assert(get(l, 0) == -1);
	prepend(l, 1);
	print_list(l); //1
	assert(insert(l, 1, 55) == -1);
	insert(l, 0, 55);
	print_list(l); //1, 55

	assert(remove_element(l, 2) == -1);

	printf("Remove element -1\n");
	assert(remove_element(l, -1) == -1);

	printf("Remove element 0\n");
	assert(remove_element(l, 0) == 0);

	print_list(l); //55
	printf("insert at -1\n");
	assert(insert(l, -1, 100) == -1);
	destroy(l);

	append(l, 71);
	print_list(l); //71
	remove_element(l, 0);
	assert(get(l, 0) == -1);

	append(l, 131);
	append(l, 132);
	append(l, 133);
	print_list(l); //131, 132, 133

	remove_element(l, 1);
	print_list(l); //131, 133

	insert(l, 0, 16);
	print_list(l); //131, 16, 133

	assert(remove_element(l, 1) == 0);
	print_list(l); //131, 133

	destroy(l);
	printf("Completed! Check print outputs\n");
}