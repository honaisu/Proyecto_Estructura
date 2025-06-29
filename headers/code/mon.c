#include "../mon.h"
#include "../prints.h"
#include <ctype.h>

// Declaraciones Globales de las variables que se usan de los mones

// MONDEX: Mapa que guarda como clave el nombre de un Mon y de valor los datos del Mon.
// Posee los datos de absolutamente todos los Mon del juego.
Map* MONDEX = NULL;
// Usado para las zonas de agua
List* MONES_AGUA = NULL;
// Usado para las zonas de fuego
List* MONES_FUEGO = NULL;
// Usado para las zonas de planta
List* MONES_PLANTA = NULL;
// Nombres de los mones, estos se usan para la correcta impresión en la MONDEX
List* NOMBRES_MON = NULL ;

// Inicializa un mon desde cero con los parametros de una fila del archivo (campos)
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
    
    strcpy(mon->descripcion, campos[6]);
    return mon;    
}

// Copia todos los valores que se encuentran en un Mon a otro. Crea una copia de este Mon
void copiar_mon(Mon *copy, Mon *paste){
    paste->ID = copy->ID ;
    strcpy(paste->nombre, copy->nombre) ;
    strcpy(paste->apodo, copy->apodo) ;
	strcpy(paste->descripcion, copy->descripcion) ;
    strcpy(paste->tipo, copy->tipo) ;

    paste->stats_base.damage_base = copy->stats_base.damage_base ;
    paste->stats_base.defense_base = copy->stats_base.defense_base ;
    paste->stats_base.hp_base = copy->stats_base.hp_base ;

	paste->hp_actual = copy->hp_actual ;
    paste->is_dead = false;
}

// Mete los mones de un tipo a su lista global correspondiente
void meter_mon_lista(Mon* mon) {
    if (!strcmp(mon->tipo, "PLANTA")) list_pushBack(MONES_PLANTA, mon);
    else if (!strcmp(mon->tipo, "AGUA")) list_pushBack(MONES_AGUA, mon);
    else if (!strcmp(mon->tipo, "FUEGO")) list_pushBack(MONES_FUEGO, mon);     
}

// Carga los archivos de los mones desde un archivo CSV
// A su vez, inserta en el mapa global (MONDEX) cada mon que se vaya creando
// (También inserta los mones a la lista de nombres para mostrarlos de forma correcta en la MONDEX)
void cargar_archivo_mones(void) {
    FILE* archivo = fopen("data/mones.csv", "r");
    if (!archivo) {
        perror("Error al cargar mones.csv");
        return;
    }

    MONDEX = map_create(100);
    MONES_AGUA = list_create();
    MONES_FUEGO = list_create();
    MONES_PLANTA = list_create();
    NOMBRES_MON = list_create();

    char buffer[500];
    fgets(buffer, sizeof(buffer), archivo); // Saltar encabezado
    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Mon* mon = inicializar_mon(campos);
        meter_mon_lista(mon);
        
        char* clave = strdup(campos[1]);
        list_pushBack(NOMBRES_MON, mon) ;
        map_insert(MONDEX, clave, mon);
    }

    fclose(archivo);
    printf("[🐱] Se cargó correctamente mones.csv\n");
}

// Aparición random que se hace para escoger un Mon salvaje.
Mon* aparicion_salvaje(List *mones){
    int tamano = list_size(mones) ;
    int i = rand() % tamano + 1 ;
    Mon *recorrer = list_first(mones) ;
    for(int k = 1 ; k < i; k++){
        recorrer = list_next(mones) ;
    }
    Mon *mon_final = (Mon *) malloc(sizeof(Mon));
    copiar_mon(recorrer, mon_final);
    return mon_final;
}

// Función de la MONDEX
// Mostrará la lista de mones existentes en el programa y espera que el usuario ingrese un nombre (o salga).
void _mondex(void) {
    while (true) {
        imprimir_mondex(NOMBRES_MON);
        printf("Ingrese el " ANSI_COLOR_WHITE "nombre" ANSI_COLOR_RESET " del Mon que desee buscar ('0' - Ninguno): ");
        char entrada[MAX];
        leer_entrada(entrada);
        
        // Salir si el usuario ingresa '0'
        if (*entrada == '0') {
            puts("Saliendo...");
            break;
        }

        // Validar entrada vacía
        if (*entrada == '\n' || *entrada == '\0') {
            puts("Error: Inserte una opción correcta.");
            esperar_enter();
            limpiar_pantalla();
            continue;
        }

        // Validar entrada: solo letras y espacios
        int entrada_valida = 1;
        for (int i = 0; entrada[i] != '\0'; i++) {
            if (!isalpha(entrada[i]) && !isspace(entrada[i])) {
                entrada_valida = 0;
                break;
            }
        }

        if (!entrada_valida) {
            puts("Error: Carácter no válido. El nombre solo puede contener letras y espacios.");
            esperar_enter();
            limpiar_pantalla();
            continue;
        }

        // Buscar en el mapa
        MapPair* pair = map_search(MONDEX, entrada);
        if (pair == NULL) {
            puts("No se encontró este Mon. Inserte una opción correcta."); 
            esperar_enter() ;
            limpiar_pantalla();
            continue;
        } 
        
        Mon* mon = pair->value;
        limpiar_pantalla();
        imprimir_datos_mon(mon);
        esperar_enter();
        break;
    }
}