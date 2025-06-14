#ifndef EXTRAS_H
#define EXTRAS_H
#include "list.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <conio.h>
#else
    #include <ncurses.h>
#endif

//--- LISTA PROPIA CON FUNCIONES (Que agarré o creé) ---//

//--- Funciones (de ejemplos y la base): ---//
// Separa el contenido de un string con un delimitador, y lo devuelve en formato List.
List *split_string(const char*, const char*) ;
// Agarra un archivo, y por cada línea que pasa va leyendo su contenido. Lo devuelve en formato de doble puntero a char (lista de strings).
char** leer_linea_csv(FILE *archivo, char separador);
// Compara enteros, y da un resultado si es que comprueba si es igual o no.
int is_equal_int(void*, void*);
// Compara strings, y da un resultado si es que comprueba si es igual o no.
int is_equal_str(void*, void*);

//--- Funciones propias: ---//
// Optimizado para funcionar en otros OS. Limpia el contenido mostrado en la terminal.
void limpiar_pantalla(void) ;
// Espera que el input del usuario sea un ENTER (salto de línea) para continuar.
void esperar_enter(void) ;
// Imprime un mensaje con un separador automáticon't. Necesita de un mensaje y un tamaño del separador para imprimirlo.
void imprimir_separador(const char*, const int) ;
// Lee lo que el usuario ingrese y modifica un string.
void leer_entrada(char*) ;
// Lee lo que el usuario ingrese, pero asigna sólo el primer carácter a una opción.
void leer_opcion(char*) ;
// Solo imprime gatitos.
void imprimir_gato(void);

void esperar_tecla(void);

int leer_opcion_valida(void);


#endif