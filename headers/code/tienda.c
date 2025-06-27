#include "../tienda.h"
#include "../jugador.h"
#include "../prints.h"

void comprar_objeto(int opcion, char* nombre_obj, int* precio, Entrenador* entrenador) {
    if (opcion == 1) { strcpy(nombre_obj, "MonBall") ; *precio = 100 ; }
    else if (opcion == 2) { strcpy(nombre_obj, "Pocion") ; *precio = 150 ; }
    else if (opcion == 3) { strcpy(nombre_obj, "Revivir") ; *precio = 250 ; }

    if (entrenador -> dinero < *precio) {
        printf("No tienes suficiente dinero para comprar %s.\n", nombre_obj) ;
        return;
    }

    Objeto* obj = list_first(entrenador -> inventario) ;
    int encontrado = 0 ;
    while (obj != NULL) {
        if (strcmp(obj -> nombre, nombre_obj) == 0)  {
            obj -> cantidad += 1 ;
            entrenador -> dinero -= *precio ;
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
        nuevo -> valor = *precio ;
        list_pushBack(entrenador -> inventario, nuevo) ;
        entrenador -> dinero -= *precio ;
        printf("Compraste 1 %s.\n", nombre_obj) ;
    }
    return;
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

    printf("\nSE HAN CURADO TODOS1 LOS MONES DEL EQUIPO.\n") ;
}

void vender_objeto(Entrenador* entrenador, char* nombre_obj) {
    printf("\nInventario Actual:\n") ;
    Objeto* obj = list_first(entrenador -> inventario) ;
    while (obj != NULL) {
        printf("- %s (x%d)\n", obj -> nombre, obj -> cantidad) ;
        obj = list_next(entrenador -> inventario) ;
    }

    printf("Nombre del objeto a vender: ") ;
    char buffer[100];
    fgets(buffer, sizeof(buffer), stdin);
    sscanf(buffer, "%s", nombre_obj);

    obj = list_first(entrenador -> inventario) ;
    while (obj != NULL) {
        if (strcmp(obj -> nombre, nombre_obj) == 0) {
            int cantidad ;
            printf("¿Cuántos deseas vender?: ");
            fgets(buffer, sizeof(buffer), stdin);
            if (sscanf(buffer, "%d", &cantidad) != 1) {
                printf("Entrada inválida para cantidad.\n");
                break;
            }

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

// VER LA TIENDA 
void ver_tienda(Entrenador* entrenador) {
    int opcion;
    while (true) {
        limpiar_pantalla();
        puts("=== CENTRO MON ===");
        printf("Dinero actual: $%d\n", entrenador -> dinero);
        const char* opciones[] = 
        {"Comprar MonBall ($100)", "Comprar Pocion ($150)", "Comprar Revivir ($250)", "Vender Objeto", "Curar a todo el Equipo"};
        imprimir_menu("", opciones, 5);
        puts("(0.) Salir del CENTRO MON");
        printf("Seleccione una opción: ");
        char buffer[100];
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &opcion) != 1) {
            printf("Entrada inválida. Por favor, ingrese un número.\n");
            continue;
        }
        if (opcion == 0) break ;

        char nombre_obj[50] ;
        int precio = 0 ;
        // ...COMPRAR OBJETOS...
        switch (opcion) {
            case 1: case 2: case 3:
                comprar_objeto(opcion, nombre_obj, &precio, entrenador) ;
                esperar_enter();
                break;
            case 4: {
                vender_objeto(entrenador, nombre_obj) ;
                break;
            }
            case 5:
                puts("CURANDO A TODOS LOS MONES....\n") ;
                curar_equipo(entrenador) ;
                esperar_enter() ;
                break;
            default:
                puts("OPCION INVALIDA. Intente de nuevo.\n") ;
                esperar_enter() ;
        }
    }
}