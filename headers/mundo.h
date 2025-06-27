#ifndef MUNDO_H
#define MUNDO_H
#include "mon.h"

// Se crea el struct objeto, que funciona para almacenar el nombre, las cantidades y los valores de los objetos, que se usan a lo largo de la partida.
typedef struct {
    char nombre[50]; // Nombre del objeto
    int cantidad; // Cantidad de los objetos 
    int valor ; // Valor monetario del objeto para CENTRO-MON
} Objeto;

// Se crea el struct ubicacion, que permite saber donde está el entrenador en cada momento
typedef struct Ubicacion {
    int id; // ID DE LA UBICACION
    char nombre[MAX]; // Nombre del lugar
    char descripcion[MAX]; // Descripción del lugar
    int es_final; // Un int que pregunta si el lugar donde estoy es el final = 1 
    int arriba, abajo, izquierda, derecha; // Un int que permite saber si se puede mover en aquellas direcciones
    char tipoZona[MAX]; // Zona que se está en el momento (AGUA, FUEGO, PLANTA)
    bool hayTienda; // Booleano que permite saber si existe una tienda = true
    Objeto objeto; // inicializacion de los objetos
    List* mones; // Equipo mon en una lista actualizable
} Ubicacion;

// Se crea el struct entrenador, que permite inicializarlo y actualizarlo
typedef struct Entrenador {
    char nombre[MAX]; // Nombre del entrenador
    int id; // Ubicacion del mapa actual, donde spawneará el jugador
    int dinero; // Dinero del jugador actualizable
    List* equipo_mon; // Una lista que almacena el equipo mon del entrenador
    List* inventario; // Una lista de inventario que almacena los objetos del entrenador
    int mons_capturados ; // Un int que permite ver los mons capturados
    int mons_wins ; // Estadistica como contador, que suma cada vez que se derrota un mon salvaje
    int entrenadores_wins ; // Estadistica como contador, que suma cada vez que se derrota un entrenador 
    bool vivo; // Permite poder saber si el jugador se encuentra "derrotado o no". Como un Game Over
} Entrenador;

// Funcion que permite moverse dentro del mapa
void mover(Map*, Entrenador*);

//Función que permite cargar el mapa CSV
void cargar_grafo_desde_csv(Map*);


#endif