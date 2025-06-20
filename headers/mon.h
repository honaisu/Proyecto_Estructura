#ifndef MON_H
#define MON_H
#include "TDAs/extras.h"
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <time.h>

// gcc main2.c headers/TDAs/*.c headers/*.c -o main -lncurses -lm

#define MAX 200

// Variable que define cuantos Mones existen en el CSV
#define MAX_MONES 10 

extern Map* MONDEX;
extern List* MONES_FUEGO;
extern List* MONES_AGUA;
extern List* MONES_PLANTA;

// BASE DE LOS MONES. Tendrá los valores Base de cada Mon.
typedef struct Mon_Base {
	int hp_base;
	int damage_base;
	int defense_base;
} Mon_Base;

// VALORES DE LOS MONES.
typedef struct Mon {
	int ID; // Identificador del Mon
	char nombre[MAX]; // Nombre del Mon
	char apodo[MAX]; // Apodo ingresado por el jugador para el Mon
    char descripcion[MAX];
	char tipo[MAX]; // Tipo del Mon
	
	Mon_Base stats_base; // Base para los stats de cierto Mon
	int hp_actual;
	int damage_actual;
	int defense_actual;
	/* STAND BY **
	int nivel_actual;
	int experiencia;
	** STAND BY */
	bool is_dead; // Verifica si está muerto el Mon o no
} Mon;

void cargar_archivo_mones(Map*);
void menu_inicial(void);
void _mondex(Map*);
Mon * aparicion_salvaje(List*) ;

#endif