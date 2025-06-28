#include "../entrenadores.h"
#include "../jugador.h"

// Variables Globales

// Define cuál es el lider de fuego
Entrenador* lider_fuego = NULL;
// Define cuál es el lider de agua
Entrenador* lider_agua = NULL;
// Define cuál es el lider de planta
Entrenador* lider_planta = NULL;
// Lista de NPCs que se encuentran por el mapa
List* NPCs = NULL;

// Inicializa los NPCs con las variables que se encuentren en la fila correspondiente a campos
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

// Permite crear una copia de un NPC con todos los datos que tenga.
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

// Booleano que elige cual lider será cual.
// Recorre cada fila de campos viendo un parametro especifico para identificar al lider.
// Si hay lider retorna true, si no retorna false.
bool es_lider(Entrenador* npc, char** campos) {
    if (campos[3] == NULL || strlen(campos[3]) == 0) return false;

    if (strcmp(campos[3], "PLANTA") == 0)      lider_planta = npc;
    else if (strcmp(campos[3], "AGUA") == 0)   lider_agua = npc;
    else if (strcmp(campos[3], "FUEGO") == 0)  lider_fuego = npc;
    else return false; // No es un tipo válido

    return true;
}

// Carga los archivos de los NPCs desde un CSV
void cargar_archivo_NPCs(void) {
    FILE* archivo = fopen("data/entrenadores.csv", "r");
    if (!archivo) {
        perror("Error al cargar entrenadores.csv");
        return;
    }
    NPCs = list_create(); // Declaración para crear la lista global

    char buffer[MAX];
    fgets(buffer, sizeof(buffer), archivo);
    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Entrenador* npc = inicializar_NPC(campos);
        if (es_lider(npc, campos)) continue; 
        list_pushBack(NPCs, npc);
    }

    fclose(archivo);
    printf("[👥] Se cargó correctamente entrenadores.csv\n");
}

// Escoge un NPC de la lista global de forma aleatoria y retorna una copia de este.
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

// Función que comprueba que lider es el que se va a luchar.
// Retorna el lider correspondiente del gimnasio, si no, retorna nulo (porsiacaso).
Entrenador* elegir_lider(Ubicacion* ubicacion) {
    Entrenador* nuevo_lider = (Entrenador*) malloc(sizeof(Entrenador));
    // Elegir un entrenador final
    if      (strcmp(ubicacion->tipoZona, "Agua") == 0) copiar_npc(lider_agua, nuevo_lider);
    else if (strcmp(ubicacion->tipoZona, "Planta") == 0) copiar_npc(lider_planta, nuevo_lider);
    else if (strcmp(ubicacion->tipoZona, "Fuego") == 0) copiar_npc(lider_fuego, nuevo_lider);
    else return NULL; // Solo porsiacaso la verdad
    return nuevo_lider;
}