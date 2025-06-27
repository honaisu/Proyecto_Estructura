#include "../prints.h"
#include "../jugador.h"

void imprimir_mones(List* mones) {
    if (!list_size(mones)) {
        printf("    Ninguno\n");
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
    unsigned short sep = 1;
    while (pair != NULL) {
        printf("(%2d)." ANSI_COLOR_WHITE "%12s     " ANSI_COLOR_RESET, pair->ID, pair->nombre);
        if (sep == 3) { sep = 0; putchar('\n'); }
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

    printf(ANSI_COLOR_WHITE "=== Entrenador %s ===\n" ANSI_COLOR_RESET, e->nombre);
    printf(ANSI_COLOR_GREEN "Ubicación: " ANSI_COLOR_RESET "%s\n", ubi->nombre);
    printf("%s\n", ubi->descripcion);
    if (e->equipo_mon != NULL) {
        puts(ANSI_COLOR_BLUE "Equipo:" ANSI_COLOR_RESET);
        imprimir_mones(e->equipo_mon);
    }
    printf(ANSI_COLOR_WHITE "Rutas: ");
    if (ubi->arriba != -1) printf("Norte ");
    if (ubi->abajo != -1) printf("Sur ");
    if (ubi->izquierda != -1) printf("Oeste ");
    if (ubi->derecha != -1) printf("Este ");
    puts(ANSI_COLOR_RESET);
}

void imprimir_menu(const char *titulo, const char* opciones[], int n_opciones) {
    printf("%s\n", titulo);
    for (int i = 0; i < n_opciones; i++) {
        printf("(%d.) %s\n", i + 1, opciones[i]);
    }
}

void mensaje_final(Entrenador* e) {
    puts("\n+----------------------------------+");
    puts("|      ¡CAMPEÓN DE LOS MONES!      |");
    puts("|    Has conquistado la Liga MON   |");
    puts("+----------------------------------+");
    printf("Equipo: ");
    imprimir_mones(e->equipo_mon);
    printf("\nMons capturados: %d\n", e->mons_capturados) ;
    printf("Batallas contra mons salvaje ganadas: %d\n",e->mons_wins) ;
    printf("Batallas contra entrenadores ganadas: %d\n", e->entrenadores_wins) ;
    printf("Dinero final: $%d\n", e->dinero) ;
}