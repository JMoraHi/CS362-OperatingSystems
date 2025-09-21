/*
CS 362- Operating Systems by Kevin Scrivnor
Edited by Jose Morales Hilario



*/
#include "list.h"

void
allocate_list(LIST** list, int count)
{
   if (count == 0) {
      return;
   }

   *list = malloc(sizeof(LIST)); //find and give space based on size of the LIST variable
   (*list)->data = NULL; //added line, initialize list variable
   allocate_list(&((*list)->next), --count); //goes next node after incrementing immediately not after.

}

void
free_list_iter(LIST* head)
{
   while (head != NULL)
   {
      LIST* next = head->next; //get pointer to next
      if (head->data != NULL) {// while current data is not null
         free(head->data); //free current data
      }
      free(head);
      head = next;
   }
}

void
free_list_emb(LIST* head)
{
   LIST* curr = head; 

   if (curr->next) { //if next node exists,
      free_list_emb(curr->next);
   }

   if (head->data != NULL) {
      free(curr->data); 
   }

   free(curr);
}

void
free_list_tail(LIST* head)
{
   if (head == NULL) {
      return;
   }

   LIST* next = head->next;

   if (head->data != NULL) {
      free(head->data);
   }

   free(head);
   free_list_tail(next);
}
