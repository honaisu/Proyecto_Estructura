#include "../mundo.h"
#include "../entrenadores.h"
#include "../prints.h"
#include "../batalla.h"

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
    FILE* archivo = fopen("data/mundo_mon.csv", "r");
    if (!archivo) {
        perror("Error al cargar mundo_mon.csv");
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
    printf("[🌐] Se cargó correctamente mundo_mon.csv\n");
}

void caso_final(Entrenador* entrenador, Ubicacion* nueva_ubi) {
    Entrenador* lider = elegir_lider(nueva_ubi);
    printf("Lider %s te desafia a un combate por el titulo de campeón!\n", lider->nombre);
    esperar_enter();
    limpiar_pantalla();
    int win = batalla_entrenador(entrenador, lider) ;
    if (win) {
        mensaje_final(entrenador);
        esperar_enter();
    } else {
        puts("Estuviste cerca de ganar, pero no fue suficiente...") ;
        esperar_enter() ;
    }
    
    exit(0);
}

void mover(Map* ubicaciones, Entrenador* e) {
    MapPair* par = map_search(ubicaciones, &e->id);
    if (par == NULL) { puts("Ubicación no encontrada."); return; }
    Ubicacion* ubi = (Ubicacion*) par->value;
    bool movimiento_valido = false;

    do {
        puts("Presione: W (Norte), S (Sur), A (Oeste), D (Este). Otra tecla para cancelar");
        
        char tecla;
        esperar_tecla(&tecla);

        int nueva_ubicacion = -1;
        char* direccion = "Dir";
        switch (tecla) {
            case 'w': case 'W': nueva_ubicacion = ubi->arriba;    direccion = "Norte";  break;
            case 's': case 'S': nueva_ubicacion = ubi->abajo;     direccion = "Sur";    break;
            case 'a': case 'A': nueva_ubicacion = ubi->izquierda; direccion = "Oeste";  break;
            case 'd': case 'D': nueva_ubicacion = ubi->derecha;   direccion = "Este";   break;
            default:
                puts("Viaje cancelado.");
                esperar_enter();
                return;
        }
        if (nueva_ubicacion == -1) { puts("No hay ruta hacia esa dirección.") ; return; } // Porsiacaso

        MapPair* nuevo_par = map_search(ubicaciones, &nueva_ubicacion);
        if (nuevo_par == NULL) { puts("Nueva ubicación no encontrada."); return; }
        
        e->id = nueva_ubicacion;
        Ubicacion* nueva_ubi = (Ubicacion*)nuevo_par->value;
        movimiento_valido = true;
        
        if (nueva_ubi->es_final) {
            caso_final(e, nueva_ubi);
        }
    } while (!movimiento_valido);
}