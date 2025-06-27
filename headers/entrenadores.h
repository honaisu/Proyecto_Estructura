#ifndef ENTRENADORES_H
#define ENTRENADORES_H

typedef struct List List;
typedef struct Entrenador Entrenador;
typedef struct Ubicacion Ubicacion;

void cargar_archivo_NPCs(List*);
Entrenador* aparicion_npc(void);
Entrenador* elegir_lider(Ubicacion*);

#endif