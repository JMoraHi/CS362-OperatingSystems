/*
CS 362- Operating Systems by Kevin Scrivnor
Edited by Jose Morales Hilario



*/




#include "list.h"

void 
load_list(LIST* list)
{
	size_t size = 0;
	while(list != NULL)
	{
		getline(&(list->data), &size, stdin);
		list = list->next; //go next item in list
	}
}

void
print_list(LIST* list)
{
	int count = 1;
	while(list != NULL)
	{
		printf("DATA ITEM %d: %s\n", count++, list->data);
		list = list->next; //next item in list
	}
}

int 
main(void)
{
   LIST* list; //make LIST variable named list

   allocate_list(&list, NUM_OF_NODES); //make
   load_list(list); //get list
   print_list(list); //print the list
   free_list_emb(list); //free the list
 
   allocate_list(&list, NUM_OF_NODES); //segment fault core dump after 2nd input of second list
   load_list(list);
   print_list(list);
   free_list_tail(list);

   allocate_list(&list, NUM_OF_NODES);
   load_list(list);
   print_list(list);
   free_list_iter(list);

   exit(EXIT_SUCCESS);
}
