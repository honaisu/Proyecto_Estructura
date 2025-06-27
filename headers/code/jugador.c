#include "../jugador.h"
#include "../batalla.h"
#include "../prints.h"

void inicializar_items(Entrenador* e) {
    Objeto* monball = (Objeto*) malloc (sizeof(Objeto)) ;
    strcpy(monball -> nombre, "MonBall") ;
    monball -> cantidad = 5 ;
    monball -> valor = 100 ;
    list_pushBack(e ->inventario, monball) ;

    Objeto* pocion = (Objeto*) malloc (sizeof(Objeto)) ;
    strcpy(pocion -> nombre, "Pocion") ;
    pocion -> cantidad = 3 ;
    pocion -> valor = 150 ;
    list_pushBack(e -> inventario, pocion) ;

    Objeto* revivir = (Objeto*) malloc (sizeof(Objeto)) ;
    strcpy(revivir -> nombre, "Revivir") ;
    revivir -> cantidad = 1 ;
    revivir -> valor = 250 ;
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
    nuevo_entrenador->dinero = 10000;
    nuevo_entrenador->inventario = list_create();
    inicializar_items(nuevo_entrenador);
    primer_mon_jugador(nuevo_entrenador);

    int* id = malloc(sizeof(int));
    *id = 1;
    return nuevo_entrenador;
}

Objeto * buscar_objeto(Entrenador *jugador, char obj[30]) {
    Objeto *recorrer = list_first(jugador->inventario) ;
    while (recorrer != NULL){
        if (!strcmp(obj, recorrer->nombre)) return recorrer ;
        recorrer = list_next(jugador->inventario) ;
    }
    return NULL ;
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

Objeto * gestionar_inventario(Entrenador *jugador) {
    verInventario(jugador) ;
    printf("Que objeto quieres usar: \n") ;
    imprimir_seleccion_items() ;

    char tecla ;
    
    int opcion_invalida = 1 ;
    do {
        #ifdef _WIN32
            tecla = getch();
        #else
            initscr();
            noecho();
            cbreak();
            tecla = getch();
            endwin();
        #endif

        switch(tecla){
            case '1' :
                return buscar_objeto(jugador, "MonBall") ;
                break ;
            case '2' :
                return buscar_objeto(jugador, "Pocion") ;
                opcion_invalida = 0 ;
                break ;
            case '3' :
                return buscar_objeto(jugador, "Revivir") ;
                opcion_invalida = 0 ;
                break ;
            case '4' :
                return NULL ;
                break ;
            default :
                printf("Opcion invalida, intentelo de nuevo \n") ;
                break ;
        }
    }   
    while (opcion_invalida) ;

    
}

void mostrar_menu_jugador(void) {
    const char* opciones[] = {"Moverse", "Gestionar Mon", "Ver Inventario", "MonDex", "Entrar a Centro Mon"};
    imprimir_menu("Opciones del Jugador", opciones, 5);
    puts("(0.) Salir");
}

void posible_batalla(Map *ubicaciones, Entrenador *entrenador){
    MapPair* pair = map_search(ubicaciones, &entrenador->id);
    Ubicacion* ubicacion = pair->value;
    int randomizador = rand() % 100 + 1 ;
    if (list_size(ubicacion->mones) != 0 && randomizador <= 40) {
        Mon *mon_salvaje = aparicion_salvaje(ubicacion->mones) ;
        printf("Un mon salvaje se acerca! \n") ;
        esperar_enter() ;
        int win = batalla_pokemon_salvaje(entrenador, mon_salvaje) ;
        esperar_enter() ;
    }
}

void menu_jugador(Map* ubicaciones, Entrenador* entrenador) {
    limpiar_pantalla();
    int se_movio = 0 ;
    while(true) {
        limpiar_pantalla() ;
        if (se_movio == 1) {
            posible_batalla(ubicaciones, entrenador) ;
            se_movio = 0 ;
        }
        mostrar_estado(ubicaciones, entrenador);
        mostrar_menu_jugador();
        int opcion_juego = leer_opcion_valida();
        
        switch (opcion_juego) {
            case 1: 
                mover(ubicaciones, entrenador, &se_movio); 
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
            case 5: {
                MapPair* par = map_search(ubicaciones, &entrenador -> id) ;
                if (par == NULL) {
                    printf("ERROR : NO SE PUDO OBTENER LA UBICACION ACTUAL\n") ;
                    break ;
                }
                Ubicacion* ubicacion = par -> value ;
                if (ubicacion -> hayTienda) {
                    ver_tienda(entrenador) ;
                } else {
                    printf("NO HAY TIENDA =)\n") ;
                }
                esperar_enter() ;
                break; 
            }
            case 0: 
                break;
        }

        if (opcion_juego == 0) break;
        
        limpiar_pantalla();
    }
}