#include "headers/jugador.h"
#include "headers/batalla.h"
#include "headers/prints.h"

// gcc main2.c headers/TDAs/*.c headers/code/*.c -o main -lm

extern Map* MONDEX;

void liberar_recursos(Map* ubicaciones, Entrenador* entrenador) {
    if (ubicaciones) {
        MapPair* pair = map_first(ubicaciones);
        while (pair) {
            Ubicacion* ubi = (Ubicacion*)pair->value;
            list_clean(ubi->mones);
            free(ubi->mones);
            free(pair->key);
            free(ubi);
            pair = map_next(ubicaciones);
        }
        map_clean(ubicaciones);
    }

    if (entrenador) {
        list_clean(entrenador->equipo_mon);
        free(entrenador->equipo_mon);
        entrenador->equipo_mon = NULL;

        list_clean(entrenador->inventario);
        free(entrenador->inventario);
        free(entrenador);
    }
}

void mostrar_menu_principal(void) {
    const char* opciones[] = {"Jugar Partida"};
    imprimir_separador("THE MON PROJECT", 30);
    imprimir_menu("", opciones, 1);
    puts("(0.) Salir");
}

int main(void) {
    srand(time(NULL)) ;
    Map* ubicaciones = map_create(is_equal_int);

    MONDEX = map_create(is_equal_int);
    cargar_archivo_mones(MONDEX);

    cargar_grafo_desde_csv(ubicaciones);
    if (ubicaciones == NULL) {
        printf("No se cargó la región. Verifique los archivos CSV.\n");
        liberar_recursos(ubicaciones, NULL);
        free(ubicaciones);
        return 1;
    }

    while (true) {
        mostrar_menu_principal();
        int opcion = leer_opcion_valida();
        
        switch (opcion) {
            case 1: {
                Entrenador* entrenador = inicializar_entrenador();
                menu_jugador(ubicaciones, entrenador);
                liberar_recursos(NULL, entrenador);   
                break;
            }
            case 0:
                break;
            default:
                puts("Por favor, ingrese una opción válida");
                break;
        }
        if (opcion == 0){
            break ;
        } 
        limpiar_pantalla();
    }
    liberar_recursos(ubicaciones, NULL);
    map_clean(MONDEX);
    free(MONDEX);

    free(ubicaciones);
    puts("¡Hasta luego!\n");
    return 0;
}