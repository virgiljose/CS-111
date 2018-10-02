#define _GNU_SOURCE
#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)  {

  if(list == NULL) {
    fprintf(stderr, "SortedList pointer is a NULL pointer\n");
    return;
  }
  if(element == NULL) {
    fprintf(stderr, "SortedList element pointer is a NULL pointer\n");
    return;
  }

  SortedListElement_t * curr = list->next; /* curr traverses the list, starting at the first element */

  while(curr != list && curr != NULL) { /* the last element points to the list head */
    if(strcmp(curr->key, element->key) <= 0) /* if our key is "smaller" than curr key, then insert*/
      break;
    curr = curr->next;
  }

  if(opt_yield & INSERT_YIELD)
    sched_yield();

  /* insert element */
  element->next = curr;
  element->prev = curr->prev;
  curr->prev->next = element;
  curr->prev = element;

  return;
}

int SortedList_delete(SortedListElement_t *element) {

  if(element == NULL)
    return 1;

  /* get the pointers to the elements adjacent to "element" */
  SortedListElement_t *prev = element->prev;
  SortedListElement_t *next = element->next;

  if(prev->next != element || next->prev != element) /* check for prev/next corruption */
    return -1;

  if(opt_yield & DELETE_YIELD)
    sched_yield();

  /* simply re-route the adjacent elements' pointers to skip over the element to be deleted */
  if(prev != NULL)
    prev->next = next;
  if(next != NULL)
    next->prev = prev;

  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {

  if(list == NULL || key == NULL)
    return NULL;

  SortedListElement_t *curr = list->next;

  while(curr != NULL && curr != list) {
    if(strcmp(curr->key, key) == 0)
      return curr;
    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
    curr = curr->next;
  }

  return NULL;
}

int SortedList_length(SortedList_t *list) {

  if(list == NULL)
    return 0;

  int length = 0;

  SortedListElement_t *curr = list->next;
  SortedListElement_t *prev = list;

  while(curr != NULL && curr != list) {
    
    if(prev != curr->prev || prev->next != curr) /* check for corrupted prev/next pointers */
      return -1;

    length++;

    if(opt_yield & LOOKUP_YIELD)
      sched_yield();

    prev = curr;
    curr = curr->next;
    }

  return length;
}
