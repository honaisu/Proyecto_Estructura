#ifndef MAP_H
#define MAP_H
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  void *key;
  void *value;
} MapPair;

typedef struct Map Map;

Map *map_create(long capacity); // unsorted map

void map_insert(Map *map, void *key, void *value);

MapPair *map_remove(Map *map, void *key);

MapPair *map_search(Map *map, void *key);

MapPair *map_first(Map *map);

MapPair *map_next(Map *map);

void map_clean(Map *map) ;

#endif /* MAP_H */