#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define TEST_ASSERT(e) \
  fprintf(stderr, "%s%s ", "Assertion ", #e); \
  (e) ? fprintf(stderr, "FAILED") : fprintf(stderr, "PASSED")

#define TEST_EQUAL(e1,e2) TEST_ASSERT(e1 == e2)

#endif // TEST_H
