#ifndef BIMAP_H
#define BIMAP_H

#include "list.h"


// Bidirectional map. Underlying representation is a list of pairs.
typedef struct {
  list* pairs; // List of pairs of values XXX: no type safety here, b/c C
  eq_rel eq_fst;  // Equivalence on first component
  eq_rel eq_snd;  // Equivalence on second component
} bimap;


// Construct an empty bimap
bimap* new_bimap(eq_rel eq_fst, eq_rel eq_snd);


// Insert an entry
void insert_bimap(bimap* map, void* pair);


// Find by first component
void* find_by_fst(bimap* map, void* pair);


// Find by second component
void* find_by_snd(bimap* map, void* pair);


// Destroy a bimap
void destroy_bimap(bimap** bmpp);


#endif // BIMAP_H
