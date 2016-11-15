#include <stdlib.h>

#include "bimap.h"
#include "list.h"

bimap* new_bimap(eq_rel eq_fst, eq_rel eq_snd) {
  bimap* map = (bimap*)malloc(sizeof(bimap));
  map->pairs = nil();
  map->eq_fst = eq_fst;
  map->eq_snd = eq_snd;

  return map;
}


void insert_bimap(bimap* map, void* pair) {
  cons(pair, map->pairs);
}


void* find_by_fst(bimap const* map, void* fst) {
  return find_list(map->pairs, fst, map->eq_fst);
}


void* find_by_snd(bimap const* map, void* snd) {
  return find_list(map->pairs, snd, map->eq_snd);
}


void destroy_bimap(bimap** bmpp) {
  bimap* map = *bmpp;
  destroy_list(&map->pairs);
  free(map);
  *bmpp = NULL;
}
