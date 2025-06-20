#include "../mon.h"
#include "../prints.h"

Map* MONDEX = NULL;

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

void cargar_archivo_mones(Map* datos_mones) {
    FILE* archivo = fopen("data/mones.csv", "r");
    if (!archivo) {
        perror("Error al cargar mones.csv");
        return;
    }
    char buffer[500];
    fgets(buffer, sizeof(buffer), archivo); // Saltar encabezado
    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Mon* mon = inicializar_mon(campos);
        
        char* clave = strdup(campos[1]);
        map_insert(datos_mones, clave, mon);
    }

    fclose(archivo);
    printf("Se cargó correctamente mones.csv\n");
}

//---//

void _mondex(Map* MONDEX) {
    imprimir_mondex(MONDEX);
    printf("Ingrese el Mon que desee buscar ('0' - Ninguno):");
    char entrada[MAX];
    leer_entrada(entrada);
    if (*entrada == '0') return;
    
    MapPair* pair = map_search(MONDEX, entrada);
    if (pair == NULL) { puts("No se encontró este Mon."); return; }
    limpiar_pantalla();
    Mon* mon = pair->value;
    imprimir_separador("DATOS DEL MON", 40);
    printf("ID. %d\nNombre\n", mon->ID,mon->nombre);
    esperar_enter();
}