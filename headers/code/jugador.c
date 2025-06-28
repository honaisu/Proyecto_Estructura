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
                strcpy(mon_inicial, "Iruiza");
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

Mon* buscar_mon_equipo(Entrenador* entrenador, char* abuscar) {
    Mon* mon_jugador = list_first(entrenador->equipo_mon);
    while (mon_jugador != NULL && strcmp(mon_jugador->apodo, abuscar) != 0 ) {
        mon_jugador = list_next(entrenador->equipo_mon);
    }
    return mon_jugador;
}

void gestionar_mones_jugador(Entrenador* entrenador) {
    Mon* mon_jugador = NULL;
    char entrada[MAX];
    const char* opciones[] = 
    {"Ver estadísticas de un Mon", "Mover Mon al Inicio", "Renombrar Mon", "Liberar Mon"};
    while (true) {
        limpiar_pantalla();
        puts("Equipo Actual:");
        imprimir_mones(entrenador->equipo_mon);
        imprimir_menu("Elija una opción", opciones, 4);
        printf("Elegir ('0' - Ninguna): ");
        
        int opcion = leer_opcion_valida();
        if (opcion == 0) break;
        if (opcion < 0 || opcion > 4) { puts("Por favor, inserte una opción válida!"); esperar_enter(); continue; }
        puts("Por favor, elija un Mon de su equipo");
        leer_entrada(entrada);
        mon_jugador = buscar_mon_equipo(entrenador, entrada);
        if (mon_jugador == NULL) { puts("No existe ese Mon en el equipo!"); esperar_enter(); continue; }
        switch (opcion) {
            case 1: {
                imprimir_datos_mon(mon_jugador);
                printf("\n" ANSI_COLOR_WHITE "Vida Actual: " ANSI_COLOR_RESET "%d PC\n", mon_jugador->hp_actual);
                esperar_enter();
                break;
            }
            case 2: {
                Mon* mon_cambio = (Mon*) malloc(sizeof(Mon));
                copiar_mon(mon_jugador, mon_cambio);
                list_popCurrent(entrenador->equipo_mon);
                list_pushFront(entrenador->equipo_mon, mon_cambio);
                break;
            }
            case 3: {
                printf("Inserte el " ANSI_COLOR_WHITE "nuevo apodo" ANSI_COLOR_RESET " de su Mon: ");
                leer_entrada(entrada);
                strcpy(mon_jugador->apodo, entrada);
                printf("El nuevo apodo de su mon es: %s\n", mon_jugador->apodo);
                break;
            }
            case 4: {
                printf("El Mon eliminado del equipo es: %s\n", mon_jugador->apodo);
                list_popCurrent(entrenador->equipo_mon);
                if (!list_size(entrenador->equipo_mon)) {
                    entrenador->vivo = false;
                    return;
                }
                break;
            }
            default:
                puts("Esta opción no existe.");
        }
    }
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
    char tecla ;
    do {
        limpiar_pantalla();
        verInventario(jugador) ;
        putchar('\n');
        imprimir_seleccion_items() ;
        esperar_tecla(&tecla);
        switch(tecla){
            case '1' :
                return buscar_objeto(jugador, "MonBall") ;
            case '2' :
                return buscar_objeto(jugador, "Pocion") ;
            case '3' :
                return buscar_objeto(jugador, "Revivir") ;
            case '4' :
                return NULL ;
            default :
                puts("Opcion invalida, intentelo de nuevo.");
                esperar_enter();
                break ;
        }
    }   
    while (true) ;
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
        if (win == 1) { 
            puts("Obtienes 200$") ;
            entrenador->dinero += 200 ; 
            ++entrenador->entrenadores_wins; 
        }
        esperar_enter();
    }
}

void menu_jugador(Map* ubicaciones, Entrenador* entrenador) {
    limpiar_pantalla();
    bool se_movio = false ;
    while(true) {
        limpiar_pantalla() ;
        if (se_movio == true) {
            posible_batalla(ubicaciones, entrenador) ;
            se_movio = false ;
        }
        if (!entrenador->vivo) {
            limpiar_pantalla(); 
            puts(ANSI_COLOR_WHITE "GAME OVER..." ANSI_COLOR_RESET);
            esperar_enter() ;
            return; 
        }
        mostrar_estado(ubicaciones, entrenador);
        mostrar_menu_jugador();
        int opcion_juego = leer_opcion_valida();
        switch (opcion_juego) {
            case 1: 
                mover(ubicaciones, entrenador);
                se_movio = true; 
                break;
            case 2:
                gestionar_mones_jugador(entrenador);
                break;
            case 3: 
                verInventario(entrenador);
                esperar_enter();
                break;
            case 4:
                _mondex();
                break;
            case 5: {
                MapPair* par = map_search(ubicaciones, &entrenador -> id) ;
                if (par == NULL) {
                    puts("ERROR : NO SE PUDO OBTENER LA UBICACION ACTUAL") ;
                    return ; // En caso de que, de repente se mueva a un lugar nada que ver
                }
                Ubicacion* ubicacion = par -> value ;
                if (ubicacion -> hayTienda) ver_tienda(entrenador) ;
                else puts(ANSI_COLOR_BLUE "NO HAY TIENDA =)" ANSI_COLOR_RESET) ;
                esperar_enter() ;
                break; 
            }
            case 0: 
                break;
            default:
                puts("Por favor, ingrese una opción válida!");
        }

        if (opcion_juego == 0) break;
    }
}