#include "../mundo.h"
#include "../prints.h"

extern List* MONES_AGUA;
extern List* MONES_FUEGO;
extern List* MONES_PLANTA;

Ubicacion* agregar_ubicacion(char** campos) {
    Ubicacion* ubicacion = (Ubicacion*)malloc(sizeof(Ubicacion));
    ubicacion->id = atoi(campos[0]);
    strcpy(ubicacion->nombre, campos[1]);
    strcpy(ubicacion->descripcion, campos[2]);
    ubicacion->es_final = !strcmp(campos[9], "Sí") ? 1 : 0;
    ubicacion->arriba = atoi(campos[5]);
    ubicacion->abajo = atoi(campos[6]);
    ubicacion->izquierda = atoi(campos[7]);
    ubicacion->derecha = atoi(campos[8]);
    ubicacion->mones = list_create();
    strcpy(ubicacion->tipoZona, campos[3]);
    ubicacion -> hayTienda = !strcmp(campos[10], "Si") ? true : false;
    return ubicacion;
}


void agregar_lista_mones(Ubicacion* ubicacion) {
    if (strcmp(ubicacion->tipoZona, "Agua") == 0) ubicacion->mones = MONES_AGUA;
    if (strcmp(ubicacion->tipoZona, "Fuego") == 0) ubicacion->mones = MONES_FUEGO;
    if (strcmp(ubicacion->tipoZona, "Planta") == 0) ubicacion->mones = MONES_PLANTA;
}

void cargar_grafo_desde_csv(Map* ubicaciones) {
    FILE* archivo = fopen("data/zonas_pokemon_2.csv", "r");
    if (!archivo) {
        perror("Error al cargar zonas_pokemon.csv");
        return;
    }
    char buffer[MAX];
    fgets(buffer, sizeof(buffer), archivo);

    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Ubicacion* ubicacion = agregar_ubicacion(campos);
        agregar_lista_mones(ubicacion);
        int* clave = malloc(sizeof(int));
        *clave = ubicacion->id;
        map_insert(ubicaciones, clave, ubicacion);
    }
    fclose(archivo);
    printf("Se cargó correctamente zonas_pokemon.csv\n");
}

// ------ MODIFICADO: Manejo de entrada de teclado para Linux ------
void mover(Map* ubicaciones, Entrenador* e, int *se_movio) {
    MapPair* par = map_search(ubicaciones, &e->id);
    if (par == NULL) { puts("Ubicación no encontrada."); return; }
    Ubicacion* ubi = (Ubicacion*) par->value;
    int movimiento_valido = 0;

    do {
        puts("Presione: W (Norte), S (Sur), A (Oeste), D (Este). Otra tecla para cancelar");
        
        char tecla;
        esperar_tecla(&tecla);

        int nueva_ubicacion = -1;
        char* direccion = "Dir";
        switch (tecla) {
            case 'w': case 'W': nueva_ubicacion = ubi->arriba; direccion = "Norte"; break;
            case 's': case 'S': nueva_ubicacion = ubi->abajo; direccion = "Sur"; break;
            case 'a': case 'A': nueva_ubicacion = ubi->izquierda; direccion = "Oeste"; break;
            case 'd': case 'D': nueva_ubicacion = ubi->derecha; direccion = "Este"; break;
            default:
                puts("Viaje cancelado.");
                return;
        }

        if (nueva_ubicacion == -1) { printf("\nNo hay ruta hacia %s.\n", direccion); return; } 
        MapPair* nuevo_par = map_search(ubicaciones, &nueva_ubicacion);
        if (nuevo_par == NULL) { puts("Nueva ubicación no encontrada."); return; }
        e->id = nueva_ubicacion;
        Ubicacion* nueva_ubi = (Ubicacion*)nuevo_par->value;
        movimiento_valido = 1;
        *se_movio = 1 ;
        if (nueva_ubi->es_final) {
            printf("\n+----------------------------------+\n");
            printf("| ¡CAMPEÓN POKÉMON!                |\n");
            printf("| Has conquistado la Liga Pokémon  |\n");
            printf("+----------------------------------+\n");
            printf("Equipo: ");
            imprimir_mones(e->equipo_mon);
            exit(0);
        }
    } while (!movimiento_valido);
}

// CURAR AL 100%
void curar_equipo(Entrenador* entrenador) {
    if (!entrenador || !entrenador -> equipo_mon) return ;

    Mon* mon = list_first(entrenador -> equipo_mon) ;

    while (mon != NULL) {
        if (mon -> hp_actual < mon -> stats_base.hp_base || mon -> is_dead) {
            mon -> hp_actual = mon -> stats_base.hp_base ;
            mon -> is_dead = false ;
        }
        mon = list_next(entrenador -> equipo_mon) ;
    }

    printf("\nSE HAN CURADO TODOS LOS MONES DEL EQUIPO.\n") ;
}



// VER LA TIENDA 

void ver_tienda(Entrenador* entrenador) {
    int opcion;
    while (1) {
        printf("\n=== CENTRO MON ===\n");
        printf("Dinero actual: $%d\n", entrenador -> dinero);
        printf("1. Comprar Monball ($100)\n");
        printf("2. Comprar Poción ($150)\n");
        printf("3. Comprar Revivir ($250)\n");
        printf("4. Vender objeto\n");
        printf("5. Curar a todo el equipo\n") ;
        printf("0. Salir del CENTRO MON\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        if (opcion == 0) break ;

        char nombre_obj[50] ;
        int precio = 0 ;
        // ...COMPRAR OBJETOS...
        if (opcion >= 1 && opcion <= 3) {
            if (opcion == 1) { strcpy(nombre_obj, "MonBall") ; precio = 100 ; }
            else if (opcion == 2) { strcpy(nombre_obj, "Pocion") ; precio = 150 ; }
            else if (opcion == 3) { strcpy(nombre_obj, "Revivir") ; precio = 250 ; }

            if (entrenador -> dinero < precio) {
                printf("No tienes suficiente dinero para comprar %s.\n", nombre_obj) ;
                continue ;
            }

            Objeto* obj = list_first(entrenador -> inventario) ;
            int encontrado = 0 ;
            while (obj != NULL) {
                if (strcmp(obj -> nombre, nombre_obj) == 0)  {
                    obj -> cantidad += 1 ;
                    entrenador -> dinero -= precio ;
                    printf("Compraste 1 %s.\n", nombre_obj) ;
                    encontrado = 1 ;
                    break ;
                }
                obj = list_next(entrenador -> inventario) ;
            }

            if (!encontrado) {
                Objeto* nuevo = malloc(sizeof(Objeto)) ;
                strcpy(nuevo -> nombre, nombre_obj) ;
                nuevo -> cantidad = 1 ;
                nuevo -> valor = precio ;
                list_pushBack(entrenador -> inventario, nuevo) ;
                entrenador -> dinero -= precio ;
                printf("Compraste 1 %s.\n", nombre_obj) ;
            }
        }

        // ...VENDER OBJETOS... SE CAE XD
        else if (opcion == 4) {
            printf("\nInventario Actual:\n") ;
            Objeto* obj = list_first(entrenador -> inventario) ;
            while (obj != NULL) {
                printf("- %s (x%d)\n", obj -> nombre, obj -> cantidad) ;
                obj = list_next(entrenador -> inventario) ;
            }

            printf("Nombre del objeto a vender: ") ;
            scanf("%s", nombre_obj) ;

            obj = list_first(entrenador -> inventario) ;
            while (obj != NULL) {
                if (strcmp(obj -> nombre, nombre_obj) == 0) {
                    int cantidad ;
                    printf("¿Cuantos deseas vender?: ") ;
                    scanf("%d", &cantidad) ;
                    if (cantidad <= 0 || cantidad > obj -> cantidad ) {
                        printf("Cantidad Inválida.\n") ;
                        break ;
                    }
                    int valor_venta = (obj -> valor * cantidad) / 2 ;
                    entrenador -> dinero += valor_venta ;
                    obj -> cantidad -= cantidad ;
                    printf("Vendiste %d %s por $%d\n", cantidad, obj -> nombre, valor_venta) ;
                    if (obj -> cantidad == 0) {
                        list_popCurrent(entrenador -> inventario) ;
                        printf("Ya no tienes mas %s Eliminado del inventario.\n", obj -> nombre) ;
                    }
                    break ;
                }
                obj = list_next(entrenador -> inventario) ;
            }

            if (obj == NULL) printf("No tienes este objeto en tu inventario.\n") ;

        }

        else if (opcion == 5) {
            printf("\nCURANDO A TODOS LOS MONES....\n") ;
            curar_equipo(entrenador) ;
        }

        // ...OPCION INVALIDA...
        else {
            printf("OPCION INVALIDA. Intente de nuevo.\n") ;
        }
    }
}