#ifndef JUGADOR_H
#define JUGADOR_H
#include "mundo.h"

void menu_jugador(Map*, Entrenador*);
Entrenador* inicializar_entrenador(void);
Objeto * gestionar_inventario(Entrenador *) ;
#endif