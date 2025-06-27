#include "../prints.h"
#include "../jugador.h"

void imprimir_mones(List* mones) {
    if (!list_size(mones)) {
        printf("Ninguno\n");
        return;
    }
    void* ptr = list_first(mones);
    while (ptr) {
        Mon* mon = (Mon*)ptr;
        printf("    %s (%d PC)", mon->nombre, mon->stats_base.damage_base);
        ptr = list_next(mones);
        if (ptr) printf(", ");
    }
    printf("\n");
}

void imprimir_mondex(List* MONDEX) {
    Mon* pair = list_first(MONDEX);
    unsigned short sep = 0;
    printf("TEST %s", pair->nombre) ;
    while (pair != NULL) {
        printf("(%2d). %s   ", pair->ID, pair->nombre);
        if (sep == 4) { sep = 0; putchar('\n'); }
        ++sep;
        pair = list_next(MONDEX);
    }
    putchar('\n');
}

// Items EN la batalla
void imprimir_seleccion_items(void) {
    const char* opciones[] = {"MonBall", "Pocion", "Revivir", "Salir del Inventario"};
    imprimir_menu("Que objeto quieres usar:", opciones, 4);
}

void mostrar_estado(Map* ubicaciones, Entrenador* e) {
    limpiar_pantalla();
    MapPair* par = map_search(ubicaciones, &e->id);
    if (par == NULL) { puts("Ubicación no encontrada."); return; }
    Ubicacion* ubi = (Ubicacion*)par->value;

    printf("=== Entrenador %s ===\n", e->nombre);
    printf("Ubicación: %s\n", ubi->nombre);
    printf("Descripción: %s\n", ubi->descripcion);
    if (e->equipo_mon != NULL) {
        puts("Equipo:");
        imprimir_mones(e->equipo_mon);
    }
    printf("Rutas: ");
    if (ubi->arriba != -1) printf("Norte ");
    if (ubi->abajo != -1) printf("Sur ");
    if (ubi->izquierda != -1) printf("Oeste ");
    if (ubi->derecha != -1) printf("Este ");
    putchar('\n');
}

void imprimir_menu(const char *titulo, const char* opciones[], int n_opciones) {
    printf("%s\n", titulo);
    for (int i = 0; i < n_opciones; i++) {
        printf("(%d.) %s\n", i + 1, opciones[i]);
    }
}