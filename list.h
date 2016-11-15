#ifndef LIST_H
#define LIST_H


#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>


// A list node
typedef struct node {
  struct node* next;
  void* data;
} node;


// node constructor
node* new_node(node* next, void* data);


// A singly linked list
typedef struct {
  node* head;
  size_t count;
} list;


// Empty list constructor
list* nil();


#define is_empty(l) ((l)->count == 0)


void cons(void* data, list* l);


// Remove the front element
void* snoc(list* l);


// The type of equivalence relations
typedef bool (*eq_rel)(void const*, void const*);


// Find the first equivalent element to a given one
void* find_list(list const* l, void* data, eq_rel eq_fn);


// Destructor
// TODO
void destroy_list(list** lpp);


#endif // LIST_H
