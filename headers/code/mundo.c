#include "../mundo.h"
#include "../prints.h"

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
    strcpy(ubicacion->tipoZona, campos[4]);
    return ubicacion;
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
        /*
        List* nombres_mones = split_string(campos[], ";");
        for (char* nombre = list_first(nombres_mones); nombre != NULL; nombre = list_next(nombres_mones)) {
            MapPair* par = map_search(MONDEX, nombre);
            if (par) {
                Mon* mon_base = (Mon*)par->value;
                Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon));
                *nuevo_mon = *mon_base; // Copia la estructura completa
                list_pushBack(ubicacion->mones, nuevo_mon);
            }
        }
        list_clean(nombres_mones);
        free(nombres_mones);*/

        int* clave = malloc(sizeof(int));
        *clave = ubicacion->id;
        map_insert(ubicaciones, clave, ubicacion);
    }
    fclose(archivo);
    printf("Se cargó correctamente zonas_pokemon.csv\n");
}

// ------ MODIFICADO: Manejo de entrada de teclado para Linux ------
void mover(Map* ubicaciones, Entrenador* e) {
    MapPair* par = map_search(ubicaciones, &e->id);
    if (par == NULL) { puts("Ubicación no encontrada."); return; }
    Ubicacion* ubi = (Ubicacion*) par->value;
    int movimiento_valido = 0;

    do {
        printf("Presione: w (Norte), s (Sur), a (Oeste), d (Este), otra tecla para cancelar\n");
        
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