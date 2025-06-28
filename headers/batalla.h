#ifndef BATALLA_H
#define BATALLA_H

//Declaraciones escenciales dentro de una batalla
typedef struct Entrenador Entrenador;
typedef struct Mon Mon;

// Funcion que me permite luchar contra mones salvajes
// 0 -> Perdiste
// 1 -> Ganaste
// 2 -> Capturaste al Mon
int batalla_pokemon_salvaje(Entrenador*, Mon*);
// Funcion que permite poder luchar contra un NPC
// false -> Perdiste
// true -> Ganaste
int batalla_entrenador(Entrenador*, Entrenador*);

#endif