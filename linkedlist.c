#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"

bool LinkedListDestroy(LinkedList *node, bool recursive, void deconstructor(void *ptr)) {
  if (node == NULL) {
#ifndef NDEBUG
    fprintf(stderr, "%s: NULL pointer is provided. Ignored!\n", __FUNCTION_NAME__);
#endif
    return false;
  }
  // if deconstructor is not provided, use free from stdlib
  if (deconstructor == NULL) {
    deconstructor = free;
  }
  LinkedList *cur = node, *next;
  do {
    next = cur->next;
    // free a block for something
    if (cur->ptr != NULL) {
      deconstructor(cur->ptr);
    }
    // free a block for node
    free(cur);
  } while (recursive && (cur = next) != NULL);
  return true;
}

LinkedList *LinkedListAppend(LinkedList *parentNode, void *ptr) {
  LinkedList *node = calloc(1, sizeof(LinkedList));

  // initialize node
  node->next = NULL;
  node->ptr = ptr;
  node->length = 1;

  /*
   * if parent node is provided, this is child node.
   * pattern 1) 1->2 => 1->me->2
   * pattern 2) 1 => 1->me
   */
  if (parentNode != NULL) {
    node->next = parentNode->next;
    parentNode->next = node;
    ++parentNode->length;
  }

  // if parent node is not provided, this is parent node

  return node;
}

LinkedList *LinkedListCreate(void *ptr) { return LinkedListAppend(NULL, ptr); }
