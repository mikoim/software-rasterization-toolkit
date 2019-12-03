#ifndef RENDER_LINKEDLIST_H
#define RENDER_LINKEDLIST_H

#include "common.h"

typedef struct tagLinkedList {
  struct tagLinkedList *next;
  void *ptr;
  uint64_t length; // Don't use in child nodes
} LinkedList;

bool LinkedListDestroy(LinkedList *node, bool recursive, void deconstructor(void *ptr));
LinkedList *LinkedListAppend(LinkedList *parentNode, void *ptr);
LinkedList *LinkedListCreate(void *ptr);

#endif // RENDER_LINKEDLIST_H
