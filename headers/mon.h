#ifndef MON_H
#define MON_H
#include "TDAs/extras.h"
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <time.h>

// gcc main2.c headers/TDAs/*.c headers/code/*.c -o main -lm

#define MAX 200

// Variable que define cuantos Mones existen en el CSV
#define MAX_MONES 10 

extern Map* MONDEX;
extern List* MONES_FUEGO;
extern List* MONES_AGUA;
extern List* MONES_PLANTA;
extern List *nombres ;

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
	bool is_dead; // Verifica si está muerto el Mon o no
} Mon;

// Función para cargar el CSV MONES
void cargar_archivo_mones(List*);

// Función para ver el menu inicial (1, 2, 0)
void menu_inicial(void);

// Función para ver / acceder la MONDEX
void _mondex(List*);

// Función para seleccionar un mon salvaje dentro de una lista
Mon * aparicion_salvaje(List*) ;

void copiar_mon(Mon*, Mon*);

#endif