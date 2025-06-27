#ifndef JUGADOR_H
#define JUGADOR_H
#include "mundo.h"

// Una cadena que guarda el nombre del jugador
extern char NOMBRE_JUGADOR[MAX];
extern List* NPCs;

// Función que llama al menu del jugador dentro de la partida
void menu_jugador(Map*, Entrenador*);

// Funcion que inicializa el entrenador
Entrenador* inicializar_entrenador(void);

// Función que gestiona el inventario dentro del combate, añade o elimina despues de ser usados
Objeto * gestionar_inventario(Entrenador *) ;
#endif