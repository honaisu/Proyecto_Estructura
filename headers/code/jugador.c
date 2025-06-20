#include "../jugador.h"
#include "../batalla.h"
#include "../prints.h"

void inicializar_items(Entrenador* e) {
    Objeto* monball = (Objeto*) malloc (sizeof(Objeto)) ;
    strcpy(monball -> nombre, "MonBall") ;
    monball -> cantidad = 5 ;
    list_pushBack(e ->inventario, monball) ;

    Objeto* pocion = (Objeto*) malloc (sizeof(Objeto)) ;
    strcpy(pocion -> nombre, "Pocion") ;
    pocion -> cantidad = 3 ;
    list_pushBack(e -> inventario, pocion) ;

    Objeto* revivir = (Objeto*) malloc (sizeof(Objeto)) ;
    strcpy(revivir -> nombre, "Revivir") ;
    revivir -> cantidad = 1 ;
    list_pushBack(e -> inventario, revivir) ;
}

void primer_mon_jugador(Entrenador* e) {
    puts("porfavor elija su mon");
    MapPair* pair = map_search(MONDEX, "Arayamon");
    if (pair == NULL) return;
    char opcion[MAX];
    leer_entrada(opcion);
    list_pushBack(e->equipo_mon, pair->value);
}

Entrenador* inicializar_entrenador(void) {
    Entrenador* nuevo_entrenador = (Entrenador*) malloc(sizeof(Entrenador));
    strcpy(nuevo_entrenador->nombre, "Hola Mundo!");
    nuevo_entrenador->id = 1;
    nuevo_entrenador->equipo_mon = list_create();
    nuevo_entrenador->dinero = 100;
    nuevo_entrenador->inventario = list_create();
    inicializar_items(nuevo_entrenador);
    primer_mon_jugador(nuevo_entrenador);

    int* id = malloc(sizeof(int));
    *id = 1;
    return nuevo_entrenador;
}

void gestionar_mones_jugador(Entrenador* e) {
    MapPair* par = map_search(MONDEX, &e->id);
    if (par == NULL) { return; }
    /*
    int opcion;
    do {
        printf("Seleccione el Mon (0 para terminar): ");
        opcion = leer_opcion_valida();
        if (opcion == 0) break;
        
        ptr = list_first(origen);
        for (i = 1; i < opcion && ptr; i++) {
            ptr = list_next(origen);
        }

        if (ptr) {
            Mon* mon_seleccionado = (Mon*)ptr;
            if (capturar) {
                Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon));
                *nuevo_mon = *mon_seleccionado; // Copiar datos
                list_pushBack(e->equipo_mon, nuevo_mon);
                list_popCurrent(origen); // Elimina de la ubicación
                free(mon_seleccionado);
                printf("%s ha sido añadido a tu equipo.\n", nuevo_mon->nombre);
            } else {
                printf("%s ha sido liberado.\n", mon_seleccionado->nombre);
                list_popCurrent(origen);
                free(mon_seleccionado);
            }
        }
    } while (opcion != 0);*/
}

void verInventario(Entrenador *e) {
    limpiar_pantalla() ;
    printf("\n==== INVENTARIO DEL ENTRENADOR ====\n") ;
    if (!list_size(e -> inventario)) {
        printf("El inventario está vacío, no hay nada que ver aqui...\n") ;
        return;
    } 

    printf("Objetos en el inventario: \n") ;
    void *ptr = list_first(e -> inventario) ;
    int i = 1 ;
    while (ptr) {
        Objeto* obj = (Objeto*)ptr ;
        printf("%d. %s: %d unidad(es)\n", i++, obj -> nombre, obj -> cantidad) ;
        ptr = list_next(e -> inventario) ;
    }
}

void mostrar_menu_jugador(void) {
    const char* opciones[] = {"Moverse", "Gestionar Mon", "Ver Inventario", "MonDex"};
    imprimir_menu("Opciones del Jugador", opciones, 4);
    puts("(0.) Salir");
}

void menu_jugador(Map* ubicaciones, Entrenador* entrenador) {
    limpiar_pantalla();
    while(true) {
        mostrar_estado(ubicaciones, entrenador);
        mostrar_menu_jugador();
        int opcion_juego = leer_opcion_valida();
        
        switch (opcion_juego) {
            case 1: 
                mover(ubicaciones, entrenador); 
                break;
            case 2: 
                //gestionar_mones(ubicaciones, entrenador, 0);
                imprimir_mones(entrenador->equipo_mon);
                esperar_enter();
                break;
            case 3: 
                verInventario(entrenador);
                esperar_enter();
                break;
            case 4:
                _mondex(MONDEX);
                break;
            case 5: 
                MapPair* pair = map_search(ubicaciones, &entrenador->id);
                Ubicacion* ubicacion = pair->value;
                printf("Zona: %s\n", ubicacion->tipoZona);
                imprimir_mones(ubicacion->mones);
                esperar_enter();
                break;
            case 0: 
                break;
        }

        if (opcion_juego == 0) break;
        
        limpiar_pantalla();
    }
}