#include "headers/jugador.h"
#include "headers/entrenadores.h"
#include "headers/prints.h"

// gcc main2.c headers/TDAs/*.c headers/code/*.c -o main.exe -lm

extern Map* MONDEX;
extern List* NOMBRES_MON ; 
extern char NOMBRE_JUGADOR[MAX];

void liberar_recursos(Map* ubicaciones) {
    MapPair* pair = map_first(ubicaciones);
    while (pair) {
        Ubicacion* ubi = (Ubicacion*)pair->value;
        list_clean(ubi->mones);
        pair = map_next(ubicaciones);
    }
    map_clean(ubicaciones);
    map_clean(MONDEX);
    free(MONDEX);
    list_clean(NOMBRES_MON);
    free(NOMBRES_MON);
}

void liberar_jugador(Entrenador* entrenador) {
    list_clean(entrenador->equipo_mon);
    free(entrenador->equipo_mon);
    entrenador->equipo_mon = NULL;

    list_clean(entrenador->inventario);
    free(entrenador->inventario);
    free(entrenador);
}

void mostrar_menu_principal(void) {
    const char* opciones[] = {"Jugar Partida", "Ingresar Nombre"};
    imprimir_separador("THE MON PROJECT", 30);
    imprimir_menu("", opciones, 2);
    puts("(0.) Salir del Programa");
}

int main(void) {
    srand(time(NULL)) ;
    // UBICACIONES
    Map* ubicaciones = map_create(MAX);

    cargar_archivo_mones();
    cargar_archivo_NPCs();
    cargar_grafo_desde_csv(ubicaciones);
    if (ubicaciones == NULL) {
        printf("No se cargaron las regiones. Verifique los archivos CSV.\n");
        liberar_recursos(ubicaciones);
        return 1;
    }
    // Input del usuario para que vea que se cargo todo
    esperar_enter();
    limpiar_pantalla();

    while (true) {
        limpiar_pantalla();
        mostrar_menu_principal();
        int opcion = leer_opcion_valida();
        switch (opcion) {
            case 1: {
                Entrenador* entrenador = inicializar_entrenador();
                menu_jugador(ubicaciones, entrenador);
                liberar_jugador(entrenador);
                break;
            }
            case 2 : {
                printf("Ingresa tu nombre de entrenador: ");
                fgets(NOMBRE_JUGADOR, MAX, stdin);
                NOMBRE_JUGADOR[strcspn(NOMBRE_JUGADOR, "\n")] = 0; // Eliminar salto de línea
                printf("Nombre actualizado: %s\n", NOMBRE_JUGADOR);
                esperar_enter();
                break;
            }
            case 0:
                break;
            default:
                puts("Por favor, ingrese una opción válida");
                break;
        }
        if (opcion == 0) break ;
    }
    liberar_recursos(ubicaciones);
    puts("¡Hasta luego!");
    return 0;
}