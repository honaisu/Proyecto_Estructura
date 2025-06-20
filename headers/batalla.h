#ifndef BATALLA_H
#define BATALLA_H

typedef struct Entrenador Entrenador;
typedef struct Mon Mon;

void desafiar_gimnasio(Entrenador*);
int batalla_pokemon_salvaje(Entrenador*, Mon*);

#endif