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

void imprimir_mondex(Map* MONDEX) {
    MapPair* pair = map_first(MONDEX);
    unsigned short sep = 0;

    while (pair != NULL) {
        Mon* mon = pair->value;
        printf("(%2d). %s   ", mon->ID, mon->nombre);
        if (sep == 4) { sep = 0; putchar('\n'); }
        ++sep;
        pair = map_next(MONDEX);
    }
    putchar('\n');
}

void imprimir_seleccion_items(){
    puts("1) Monball") ;
    puts("2) Pocion") ;
    puts("3) Revivir") ;
    puts("4) Salir del inventario") ;
}

/*
void mostrar_puntajes_finales(Entrenador* entrenadores, int num_entrenadores, int id_ganador) {
    printf("\n¡La Liga Pokémon ha concluido!\n");
    printf("+------------+----------+----------+\n");
    printf("| Entrenador | Medallas | Rango    |\n");
    printf("+------------+----------+----------+\n");
    for (int i = 0; i < num_entrenadores; i++) {
        const char* rango;
        if (entrenadores[i].medallas > 100) rango = "Campeón";
        else if (entrenadores[i].medallas >= 50) rango = "Maestro";
        else rango = "Entrenador";
        printf("| %-10d | %-8d | %-8s |\n", i + 1, entrenadores[i].medallas, rango);
    }
    printf("+------------+----------+----------+\n");
    if (id_ganador != 0) {
        printf("¡Entrenador %d es el Campeón Pokémon!\n", id_ganador);
    } else {
        printf("Energía agotada. Nadie es Campeón.\n");
    }
}
*/
void mostrar_estado(Map* ubicaciones, Entrenador* e) {
    limpiar_pantalla();
    MapPair* par = map_search(ubicaciones, &e->id);
    if (par == NULL) { puts("Ubicación no encontrada."); return; }
    Ubicacion* ubi = (Ubicacion*)par->value;

    printf("=== Entrenador %s ===\n", e->nombre);
    printf("Ubicación: %s\n", ubi->nombre);
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