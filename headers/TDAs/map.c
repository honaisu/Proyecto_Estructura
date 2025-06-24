#include "map.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct Map {
    MapPair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

typedef Map Map;

// Variable global para almacenar la función de comparación actual
int (*current_lt)(void *, void *) = NULL;

int pair_lt(void *pair1, void *pair2) {
  return (current_lt(((MapPair *)pair1)->key, ((MapPair *)pair2)->key));
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

Map * createMap(long capacity) {
    Map *mapa = (Map *) malloc(sizeof(Map)) ;
    mapa->current = -1 ;
    mapa->capacity = capacity ;
    mapa->size = 0 ;
    mapa->buckets = (MapPair **) malloc(sizeof(MapPair *) * capacity) ;
    for(long i = 0 ; i < capacity ; i++){
        mapa->buckets[i] = (MapPair *) malloc(sizeof(MapPair)) ;
        mapa->buckets[i] = NULL ;
    }
    return mapa ;
}


void map_insert(Map *map, void *key, void *value) {
  long capacidad = map->capacity ;
  if (capacidad == map->size) return ;
  for(int i = 0 ; i < capacidad ; i++){
      if (map->buckets[i] == NULL) continue ;
      if (is_equal(map->buckets[i]->key, key)) return ;
  }
  long posicion = hash(key, capacidad) % capacidad ;
  while (map->buckets[posicion] != NULL && map->buckets[posicion]->key != NULL)
  {
      posicion = (posicion + 1) % capacidad ;
  }
  MapPair *par = (MapPair *) malloc(sizeof(MapPair)) ;
  par->key = key ;
  par->value = value ;
  map->buckets[posicion] = par ;
  map->size ++ ;
}

void enlarge(Map * map) {
    MapPair **buckets_auxiliar = map->buckets ;
    long old_capacity = map->capacity ; 
    map->capacity *= 2 ;
    long new_capacity = map->capacity ;
    MapPair ** nuevo_buckets = (MapPair **) malloc(sizeof(MapPair *) * new_capacity) ;
    for(long i = 0 ; i < new_capacity ; i++){
        nuevo_buckets[i] = (MapPair *) malloc(sizeof(MapPair)) ;
        nuevo_buckets[i] = NULL ;
    }
    map->buckets = nuevo_buckets ;
    map->size = 0 ;
    for(long i = 0 ; i < old_capacity ; i++){
        if (buckets_auxiliar[i] == NULL) continue ;
        map_insert(map, buckets_auxiliar[i]->key, buckets_auxiliar[i]->value) ;
    }


}

Map * map_create(long capacity) {
    Map *mapa = (Map *) malloc(sizeof(Map)) ;
    mapa->current = -1 ;
    mapa->capacity = capacity ;
    mapa->size = 0 ;
    mapa->buckets = (MapPair **) malloc(sizeof(MapPair *) * capacity) ;
    for(long i = 0 ; i < capacity ; i++){
        mapa->buckets[i] = (MapPair *) malloc(sizeof(MapPair)) ;
        mapa->buckets[i] = NULL ;
    }
    return mapa ;
}

MapPair * map_remove(Map * map,  void * key) {    
    long capacidad = map->capacity ;
    long posible_pos = hash(key, capacidad) ; 
    while (1){
        if (map->buckets[posible_pos] == NULL) return NULL ;
        if (is_equal(map->buckets[posible_pos]->key, key)){
            MapPair *retornar_esto = map->buckets[posible_pos] ;
            map->buckets[posible_pos]->key = NULL ;
            map ->size -- ;
            return retornar_esto ;
        }
        posible_pos = (posible_pos + 1) % capacidad ;
    }
    return NULL ;
}


MapPair * map_search(Map * map,  void * key) {   
    long capacidad = map->capacity ;
    
    long posible_pos = hash(key, capacidad) ;
    if (is_equal(map->buckets[posible_pos]->key, key)){ 
        map->current = posible_pos ;
        return map->buckets[posible_pos] ;
    }
    while (1){
        posible_pos = (posible_pos + 1) % capacidad ;
        MapPair * par = map->buckets[posible_pos] ;
        if (par == NULL) break ;
        if (is_equal(par->key, key)) return par ;
    }
    return NULL ;
}

MapPair * map_first(Map * map) {
    long capacidad = map->capacity ;
    MapPair *par ;
    for(int i = 0 ; i < capacidad ; i++){
        par = map->buckets[i] ;
        if (par != NULL) {
            if (par->key != NULL){
                map->current = i ;
                return par ;
            } 
        }
    }
    return NULL;
}
MapPair * map_next(Map * map) {
    long capacidad = map->capacity ;
    long current = map->current ;
    MapPair *par ;
    for(int i = current + 1 ; i < capacidad ; i++){
        par = map->buckets[i] ;
        if (par != NULL) {
            if (par->key != NULL){
                map->current = i ;
                return par ;
            } 
        }
    }
    return NULL;
}

void map_clean(Map *map) {
    if (map == NULL || map->buckets == NULL) return;

    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            free(map->buckets[i]);      // liberar el par (Pair *)
            map->buckets[i] = NULL;     // evitar punteros colgantes
        }
    }

    map->size = 0;
    map->current = -1;
}