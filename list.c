#include <stddef.h>
#include <stdlib.h>

#include "list.h"


// Helper for list destructor
static void destroy_nodes(node* n);


node* new_node(node* next, void* data) {
  node* n = (node*)malloc(sizeof(node)); 
  n->next = next;
  n->data = data;
  return n;
}


list* nil() {
  list* l = (list*)malloc(sizeof(list));
  l->head = NULL;
  l->count = 0;
  return l;
}


void cons(void* data, list* l) {
  node* old_head = l->head;

  l->head = new_node(old_head, data);
  l->count++;
}


void* snoc(list* l) {
  void* data;
  node* old_head = l->head;

  if (!is_empty(l)) {
    l->head = l->head->next; 
    data = old_head->data;
    free(old_head); 
    l->count--;
    return data;
  }

  return NULL;
}


void* find_list(list const* l, void* data, eq_rel eq_fn) {
  node* curr = l->head;

  while(curr != NULL) {
    if (eq_fn(curr->data, data)) {
      return curr->data; 
    }
    curr = curr->next;
  }

  return NULL;
}


void destroy_list(list** lpp) {
  list* l = *lpp;
  destroy_nodes(l->head);
  free(l);
  *lpp = NULL;
}

static void destroy_nodes(node* n) {
  if (n == NULL) {
    return;
  }

  destroy_nodes(n->next);
  free(n);
}
