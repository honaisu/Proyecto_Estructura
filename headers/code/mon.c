#include "../mon.h"
#include "../prints.h"

Map* MONDEX = NULL;
List* MONES_AGUA = NULL;
List* MONES_FUEGO = NULL;
List* MONES_PLANTA = NULL;

Mon* inicializar_mon(char** campos) {
    Mon* mon = (Mon*)malloc(sizeof(Mon));
    mon->ID = atoi(campos[0]);
    strcpy(mon->nombre, campos[1]);
    strcpy(mon->apodo, mon->nombre);
    strcpy(mon->tipo, campos[2]);

    mon->stats_base.hp_base = atoi(campos[3]);
    mon->stats_base.damage_base = atoi(campos[4]);
    mon->stats_base.defense_base = atoi(campos[5]);
    mon->hp_actual = mon->stats_base.hp_base;
    mon->damage_actual = mon->stats_base.damage_base;
    mon->defense_actual = mon->stats_base.defense_base;
    
    strcpy(mon->descripcion, campos[6]);
    return mon;    
}

void copiar_mon(Mon *copy, Mon *paste){

    paste->ID = copy->ID ;
    strcpy(paste->nombre, copy->nombre) ;
    strcpy(paste->apodo, copy->apodo) ;
	strcpy(paste->descripcion, copy->descripcion) ;
    strcpy(paste->tipo, copy->tipo) ;
    
    paste->stats_base.damage_base = copy->stats_base.damage_base ;
    paste->stats_base.defense_base = copy->stats_base.defense_base ;

    paste->stats_base.hp_base = copy->stats_base.hp_base ;

	paste->hp_actual = paste->stats_base.hp_base ;
	paste->damage_actual = paste->stats_base.damage_base ;
    paste->defense_actual = paste->stats_base.defense_base ;
}

Mon * aparicion_salvaje(List *mones){
    int tamano = list_size(mones) ;
    int i = rand() % tamano + 1 ;
    Mon *recorrer = list_first(mones) ;
    for(int k = 1 ; k < i; k++){
        recorrer = list_next(mones) ;
    }
    Mon *mon_final = (Mon *) malloc(sizeof(Mon));
    copiar_mon (recorrer, mon_final);

}

void meter_mon_lista(Mon* mon) {
    if (!strcmp(mon->tipo, "PLANTA")) list_pushBack(MONES_PLANTA, mon);
    else if (!strcmp(mon->tipo, "AGUA")) list_pushBack(MONES_AGUA, mon);
    else if (!strcmp(mon->tipo, "FUEGO")) list_pushBack(MONES_FUEGO, mon);     
}

void cargar_archivo_mones(Map* datos_mones) {
    FILE* archivo = fopen("data/mones.csv", "r");
    if (!archivo) {
        perror("Error al cargar mones.csv");
        return;
    }

    MONES_AGUA = list_create();
    MONES_FUEGO = list_create();
    MONES_PLANTA = list_create();

    char buffer[500];
    fgets(buffer, sizeof(buffer), archivo); // Saltar encabezado
    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Mon* mon = inicializar_mon(campos);
        meter_mon_lista(mon);
        
        char* clave = strdup(campos[1]);
        map_insert(datos_mones, clave, mon);
    }

    fclose(archivo);
    printf("Se cargó correctamente mones.csv\n");
}

void _mondex(Map* MONDEX) {
    imprimir_mondex(MONDEX);
    printf("Ingrese el nombre del Mon que desee buscar ('0' - Ninguno): ");
    char entrada[MAX];
    leer_entrada(entrada);
    if (*entrada == '0' || *entrada == '\n') return;
    
    MapPair* pair = map_search(MONDEX, entrada);
    if (pair == NULL) { puts("No se encontró este Mon."); return; }
    Mon* mon = pair->value;

    limpiar_pantalla();
    imprimir_separador("DATOS DEL MON", 40);
    printf("ID. %d  Nombre: %s  Tipo: %s\n", mon->ID, mon->nombre, mon->tipo);
    printf("Descripción: %s\n", mon->descripcion);
    puts("\nStats");
    printf("Vida: %d\n", mon->stats_base.hp_base);
    printf("Daño: %d\n", mon->stats_base.damage_base);
    printf("Defensa: %d\n", mon->stats_base.defense_base);
    esperar_enter();
}
