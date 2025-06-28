#ifndef ENTRENADORES_H
#define ENTRENADORES_H

// Solo define que es esa estructura para el header
typedef struct Entrenador Entrenador;
typedef struct Ubicacion Ubicacion;

// Carga el archivo CSV correspondiente a entrenadores
void cargar_archivo_NPCs(void);
// Escoge un npc random para meterlo a la batalla aleatoria
Entrenador* aparicion_npc(void);
// Elige un lider de una ubicacion (ve el tipo)
Entrenador* elegir_lider(Ubicacion*);

#endif