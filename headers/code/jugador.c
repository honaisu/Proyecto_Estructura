#include "../jugador.h"
#include "../batalla.h"
#include "../tienda.h"
#include "../prints.h"
#include "../entrenadores.h"

extern List* NPCs;
char NOMBRE_JUGADOR[MAX] = "Ketchup";

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
    char mon_inicial[20] = "\0";
    do {
        char opcion = '\0';
        const char* opciones[] = {
            "Iruiza  (Tipo " ANSI_COLOR_RED "Fuego" ANSI_COLOR_RESET ")", 
            "Alimun  (Tipo " ANSI_COLOR_BLUE "Agua" ANSI_COLOR_RESET ")", 
            "Mecamon (Tipo " ANSI_COLOR_GREEN "Planta" ANSI_COLOR_RESET ")"
        };
        limpiar_pantalla();
        imprimir_menu("Elija su Mon Inicial:", opciones, 3);
        leer_opcion(&opcion);
        switch (opcion) {
            case '1':
                strcpy(mon_inicial, "Arayamon");
                break;
            case '2':
                strcpy(mon_inicial, "Alimun");
                break;
            case '3':
                strcpy(mon_inicial, "Mecamon");
                break;
        }
        if (opcion == '1' || opcion == '2' || opcion == '3') break;
        else { puts("Por favor, elija un Mon Inicial."); esperar_enter(); }
    } while (true);

    MapPair* pair = map_search(MONDEX, mon_inicial);
    if (pair == NULL) { puts("Mon No encontrado"); exit(1); }
    Mon* mon_jugador = (Mon*) malloc(sizeof(Mon));
    copiar_mon(pair->value, mon_jugador);
    list_pushBack(e->equipo_mon, mon_jugador);
}

Entrenador* inicializar_entrenador(void) {
    Entrenador* nuevo_entrenador = (Entrenador*) malloc(sizeof(Entrenador));
    strcpy(nuevo_entrenador->nombre, NOMBRE_JUGADOR);
    nuevo_entrenador->id = 1;
    nuevo_entrenador->equipo_mon = list_create();
    nuevo_entrenador->dinero = 100;
    nuevo_entrenador->inventario = list_create() ;
    nuevo_entrenador->entrenadores_wins = 0 ;
    nuevo_entrenador->mons_capturados = 0 ;
    nuevo_entrenador->mons_wins = 0 ;
    nuevo_entrenador->vivo = true;

    inicializar_items(nuevo_entrenador);
    primer_mon_jugador(nuevo_entrenador);
    return nuevo_entrenador;
}

Objeto * buscar_objeto(Entrenador *jugador, char* obj) {
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
    const char* opciones[] = {"Moverse", "Gestionar Mon", "Ver Inventario", "MonDex", "Entrar a CentroMON"};
    imprimir_menu("Opciones del Jugador", opciones, 5);
    puts("(0.) Salir");
}

void posible_batalla(Map *ubicaciones, Entrenador *entrenador){
    MapPair* pair = map_search(ubicaciones, &entrenador->id);
    Ubicacion* ubicacion = pair->value;
    int randomizador = rand() % 100 + 1 ;
    if (list_size(ubicacion->mones) > 0 && randomizador <= 40) {
        Mon *mon_salvaje = aparicion_salvaje(ubicacion->mones) ;
        puts("Un mon salvaje se acerca!") ;
        esperar_enter() ;
        int win = batalla_pokemon_salvaje(entrenador, mon_salvaje) ;
        if (win == 1) entrenador->mons_wins += 1 ;
        else if (win == 2) entrenador->mons_capturados += 1 ;
        esperar_enter() ;
    }
    else if (*ubicacion->tipoZona == '\0' && randomizador <= 20) {
        Entrenador* npc_batalla = aparicion_npc();
        printf("Un entrenador cabreado llamado %s se acerca!\n", npc_batalla->nombre);
        esperar_enter();
        int win = batalla_entrenador(entrenador, npc_batalla);
        if (win == 1) ++entrenador->entrenadores_wins;
        else { puts("Mala cuea"); entrenador->vivo = false; }
        esperar_enter();
    }
}

void menu_jugador(Map* ubicaciones, Entrenador* entrenador) {
    limpiar_pantalla();
    int se_movio = 0 ;
    while(true) {
        limpiar_pantalla() ;
        if (!entrenador->vivo) { puts("GAME OVER!!!"); return; }
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
                _mondex(MONDEX, nombres);
                break;
            case 5: {
                MapPair* par = map_search(ubicaciones, &entrenador -> id) ;
                if (par == NULL) {
                    puts("ERROR : NO SE PUDO OBTENER LA UBICACION ACTUAL") ;
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