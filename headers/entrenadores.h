#ifndef ENTRENADORES_H
#define ENTRENADORES_H

typedef struct List List;
typedef struct Entrenador Entrenador;

void cargar_archivo_NPCs(List*);
Entrenador* aparicion_npc(void);

#endif