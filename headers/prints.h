#ifndef PRINTS_H
#define PRINTS_H

// Declaraciones para poder imprimir de buena forma
typedef struct List List; 
typedef struct Map Map;
typedef struct Mon Mon;
typedef struct Entrenador Entrenador;
typedef struct Ubicacion Ubicacion;

// Función que recorre la lista mones para imprimirlos
void imprimir_mones(List*);

// Funcion que recorre la lista de la Mondex y los muestra
void imprimir_mondex(List*);

// Función que permite mostrar el estado actual del entrenador
void mostrar_estado(Map*, Entrenador*);

// Función que permite imprimir un menu cualquiera dentro del main u otros
void imprimir_menu(const char*, const char**, int);

// Función que permite ver los items que puedes seleccionar para ser usados en combate
void imprimir_seleccion_items(void) ;

// Mensaje final cuando se gana el juego
void mensaje_final(Entrenador*);

// Función que imprime los datos de un mon
void imprimir_datos_mon(Mon*);

#endif