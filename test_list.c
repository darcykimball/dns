#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "test.h"
#include "list.h"
#include "test.h"


#define AS_INT(vp) (*(int*)(vp))


bool int_eq(void const* x, void const* y) {
  return AS_INT(x) == AS_INT(y);
}


int main() {
  int xs[5] = { 11, 2, 23, 884, 1115 };
  int not_there = 12;

  list* test_list = nil();

  for (size_t i = 0; i < sizeof(xs)/sizeof(xs[0]); ++i) {
    cons(&xs[i], test_list);
  }

  node* curr = test_list->head;


  TEST_ASSERT(AS_INT(curr->data) == xs[4]);
  curr = curr->next;
  TEST_ASSERT(AS_INT(curr->data) == xs[3]); 
  curr = curr->next;
  TEST_ASSERT(AS_INT(curr->data) == xs[2]);
  curr = curr->next;
  TEST_ASSERT(AS_INT(curr->data) == xs[1]);
  curr = curr->next;
  TEST_ASSERT(AS_INT(curr->data) == xs[0]);


  TEST_ASSERT(!is_empty(test_list));
  TEST_ASSERT(test_list->count == 5);


  TEST_ASSERT(AS_INT(find_list(test_list, &xs[0], &int_eq)) == xs[0]);
  TEST_ASSERT(AS_INT(find_list(test_list, &xs[1], &int_eq)) == xs[1]);
  TEST_ASSERT(AS_INT(find_list(test_list, &xs[2], &int_eq)) == xs[2]);
  TEST_ASSERT(AS_INT(find_list(test_list, &xs[3], &int_eq)) == xs[3]);
  TEST_ASSERT(AS_INT(find_list(test_list, &xs[4], &int_eq)) == xs[4]);


  TEST_ASSERT(find_list(test_list, &not_there, &int_eq) == NULL);


  TEST_ASSERT(AS_INT(snoc(test_list)) == xs[4]);
  TEST_ASSERT(test_list->count == 4);
  TEST_ASSERT(AS_INT(snoc(test_list)) == xs[3]); 
  TEST_ASSERT(test_list->count == 3);
  TEST_ASSERT(AS_INT(snoc(test_list)) == xs[2]);
  TEST_ASSERT(test_list->count == 2);
  TEST_ASSERT(AS_INT(snoc(test_list)) == xs[1]);
  TEST_ASSERT(test_list->count == 1);
  TEST_ASSERT(AS_INT(snoc(test_list)) == xs[0]);
  TEST_ASSERT(test_list->count == 0);


  TEST_ASSERT(snoc(test_list) == NULL);

  return 0;
}
