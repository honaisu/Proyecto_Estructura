#ifndef MUNDO_H
#define MUNDO_H
#include "mon.h"

typedef struct {
    char nombre[50]; 
    int cantidad;
    int valor ;    
} Objeto;

typedef struct Ubicacion {
    int id;
    char nombre[MAX];
    char descripcion[MAX];
    int es_final;
    int arriba, abajo, izquierda, derecha;
    char tipoZona[MAX];
    bool hayTienda;
    Objeto objeto;
    List* mones;
} Ubicacion;

typedef struct Entrenador {
    char nombre[MAX];
    int id;
    int dinero;
    List* equipo_mon;
    List* inventario;
} Entrenador;

void mover(Map*, Entrenador*, int*);
void cargar_grafo_desde_csv(Map*);
void ver_tienda(Entrenador*) ;

#endif