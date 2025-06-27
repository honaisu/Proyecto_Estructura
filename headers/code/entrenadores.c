#include "../entrenadores.h"
#include "../jugador.h"

Entrenador* inicializar_NPC(char** campos) {
    Entrenador* nuevo_NPC = (Entrenador*) malloc(sizeof(Entrenador));
    strcpy(nuevo_NPC->nombre, campos[1]);
    nuevo_NPC->equipo_mon = list_create();
    List* mones_nombres = split_string(campos[2], ";");
    char* mones = list_first(mones_nombres);
    while (mones != NULL) {
        MapPair* pair = map_search(MONDEX, mones);
        Mon* mon_nuevo = (Mon*) malloc(sizeof(Mon));
        copiar_mon(pair->value, mon_nuevo);
        list_pushBack(nuevo_NPC->equipo_mon, mon_nuevo);
        mones = list_next(mones_nombres);
    }
    nuevo_NPC->vivo = true;
    return nuevo_NPC;
}

List* NPCs = NULL;

void cargar_archivo_NPCs(List* NPCs) {
    FILE* archivo = fopen("data/entrenadores.csv", "r");
    if (!archivo) {
        perror("Error al cargar entrenadores.csv");
        return;
    }
    char buffer[MAX];
    fgets(buffer, sizeof(buffer), archivo);
    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Entrenador* npc = inicializar_NPC(campos);
        list_pushBack(NPCs, npc);
    }

    fclose(archivo);
    printf("[👥] Se cargó correctamente entrenadores.csv\n");
}

void copiar_npc(Entrenador* copy, Entrenador* paste) {
    strcpy(paste->nombre, copy->nombre);
    paste->equipo_mon = list_create();
    Mon* mones_copy = list_first(copy->equipo_mon);
    while (mones_copy != NULL) {
        Mon* nuevo_mon = malloc(sizeof(Mon));
        copiar_mon(mones_copy, nuevo_mon);
        list_pushBack(paste->equipo_mon, nuevo_mon);
        mones_copy = list_next(copy->equipo_mon);
    }
    paste->vivo = true;
}

Entrenador* aparicion_npc(void) {
    int tamano = list_size(NPCs) ;
    int i = rand() % tamano + 1 ;
    Entrenador* recorrer = list_first(NPCs) ;
    for(int k = 1 ; k < i; k++){
        recorrer = list_next(NPCs) ;
    }
    Entrenador *npc_final = (Entrenador*) malloc(sizeof(Entrenador));
    copiar_npc(recorrer, npc_final);
    return npc_final;
}