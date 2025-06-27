#ifndef BATALLA_H
#define BATALLA_H

//Declaraciones escenciales dentro de una batalla
typedef struct Entrenador Entrenador;
typedef struct Mon Mon;

// Funcion que me permite luchar contra mones salvajes
int batalla_pokemon_salvaje(Entrenador*, Mon*);
int batalla_entrenador(Entrenador*, Entrenador*);

#endif