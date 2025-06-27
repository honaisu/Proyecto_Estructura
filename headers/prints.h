#ifndef PRINTS_H
#define PRINTS_H

typedef struct List List;
typedef struct Map Map;
typedef struct Mon Mon;
typedef struct Entrenador Entrenador;
typedef struct Ubicacion Ubicacion;

void imprimir_mones(List*);
void imprimir_mondex(List*);
void mostrar_puntajes_finales(Entrenador*, int, int);
void mostrar_estado(Map*, Entrenador*);
void imprimir_menu(const char*, const char**, int);
void imprimir_seleccion_items() ;

#endif