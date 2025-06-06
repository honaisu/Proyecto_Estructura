#include "headers/extras.h"

// Compilación: gcc main.c headers/*.c headers/TDAs/*.c -o main

int main(void) {
    limpiar_pantalla();
    imprimir_separador("hola mundo", 30);
    imprimir_gato();
    return 0;
}