#include "headers/TDAs/extras.h"
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#define MAX 50

// BASE DE LOS MONES. Tendrá los valores Base de cada Mon.
typedef struct Mon_Base {
	int hp_base;
	int damage_base;
	int defense_base;
} Mon_Base;

// VALORES DE LOS MONES.
typedef struct Mon {
	int ID; // Identificador del Mon
	char nombre[MAX]; // Nombre del Mon
	char apodo[MAX]; // Apodo ingresado por el jugador para el Mon
    char descripcion[MAX];
	char tipo[MAX]; // Tipo del Mon
	
	Mon_Base stats_base; // Base para los stats de cierto Mon
	int hp_actual;
	int damage_actual;
	int defense_actual;
	/* STAND BY **
	int nivel_actual;
	int experiencia;
	** STAND BY */
	bool is_dead; // Verifica si está muerto el Mon o no
} Mon;

typedef struct {
    int id;
    char nombre[100];
    char descripcion[300];
    int es_final;
    int arriba, abajo, izquierda, derecha;
    List* mones;
} Ubicacion;

typedef struct {
    int id;
    List* equipo;
    int medallas;
    List* historial;
} Entrenador;

void imprimir_mones(List* mones) {
    if (!list_size(mones)) {
        printf("Ninguno\n");
        return;
    }
    void* ptr = list_first(mones);
    while (ptr) {
        Mon* mon = (Mon*)ptr;
        printf("%s (%d PC)", mon->nombre, mon->stats_base.damage_base);
        ptr = list_next(mones);
        if (ptr) printf(", ");
    }
    printf("\n");
}

Mon* inicializar_mon(char** campos) {
    Mon* mon = (Mon*)malloc(sizeof(Mon));
    mon->ID = atoi(campos[0]);
    strcpy(mon->nombre, campos[1]);
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

void cargar_mones_desde_csv(Map* datos_mones) {
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

Map* MONDEX;

void cargar_grafo_desde_csv(Map* ubicaciones, int* cantidad_ubicaciones) {
    FILE* archivo = fopen("data/zonas_pokemon.csv", "r");
    if (!archivo) {
        perror("Error al cargar zonas_pokemon.csv");
        return;
    }
    char buffer[500];
    fgets(buffer, sizeof(buffer), archivo); // Saltar cabecera
    char** campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
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

        if (strlen(campos[4]) > 0) {
            List* nombres_mones = split_string(campos[4], ";");
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
            free(nombres_mones);
        }

        int* clave = malloc(sizeof(int));
        *clave = ubicacion->id;
        map_insert(ubicaciones, clave, ubicacion);
        (*cantidad_ubicaciones)++;
    }
    fclose(archivo);
    printf("Se cargó correctamente zonas_pokemon.csv\n");
}


Entrenador* inicializar_entrenador() {
    Entrenador* nuevo_entrenador = (Entrenador*) malloc(sizeof(Entrenador));
    nuevo_entrenador->id = 1;
    nuevo_entrenador->equipo = list_create();
    
    nuevo_entrenador->medallas = 0;
    nuevo_entrenador->historial = list_create();
    int* id = malloc(sizeof(int));
    *id = 1;
    list_pushBack(nuevo_entrenador->historial, id);
    return nuevo_entrenador;
}

void mostrar_estado(Map* ubicaciones, Entrenador* e, int id_entrenador) {
    limpiar_pantalla();
    MapPair* par = map_search(ubicaciones, &e->id);
    if (!par) {
        printf("Error: Ubicación ID %d no encontrada.\n", e->id);
        return;
    }
    Ubicacion* ubi = (Ubicacion*)par->value;

    printf("\n=== Entrenador %d ===\n", id_entrenador);
    printf("Ubicación: %s\n", ubi->descripcion);
    printf("Pokémon disponibles: ");
    imprimir_mones(ubi->mones);
    printf("Equipo: ");
    imprimir_mones(e->equipo);
    printf(" Medallas: %d\n", e->medallas);
    printf("Rutas: ");
    if (ubi->arriba != -1) printf("Norte ");
    if (ubi->abajo != -1) printf("Sur ");
    if (ubi->izquierda != -1) printf("Oeste ");
    if (ubi->derecha != -1) printf("Este ");
    printf("\n");
}

void gestionar_mones(Map* ubicaciones, Entrenador* e, int capturar) {
    MapPair* par = map_search(ubicaciones, &e->id);
    if (!par) return;
    Ubicacion* ubi = (Ubicacion*)par->value;
    List* origen = capturar ? ubi->mones : e->equipo;

    if (!list_size(origen)) {
        printf("%s vacío.\n", capturar ? "No hay Pokémon aquí" : "Tu equipo está");
        esperar_tecla();
        return;
    }

    printf("%s:\n", capturar ? "Pokémon disponibles para capturar" : "Tu equipo actual");
    void* ptr = list_first(origen);
    int i = 1;
    while (ptr) {
        Mon* mon = (Mon*)ptr;
        printf("%d. %s (%d PC)\n", i++, mon->nombre, mon->stats_base.damage_base);
        ptr = list_next(origen);
    }

    int opcion;
    do {
        opcion = leer_opcion_valida();
        printf("Seleccione Pokémon (0 para terminar): ");
        if (opcion == 0) break;
        
        ptr = list_first(origen);
        for (i = 1; i < opcion && ptr; i++) {
            ptr = list_next(origen);
        }

        if (ptr) {
            Mon* mon_seleccionado = (Mon*)ptr;
            if (capturar) {
                Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon));
                *nuevo_mon = *mon_seleccionado; // Copiar datos
                list_pushBack(e->equipo, nuevo_mon);
                list_popCurrent(origen); // Elimina de la ubicación
                free(mon_seleccionado);
                e->medallas += nuevo_mon->stats_base.damage_base;
                printf("%s ha sido añadido a tu equipo.\n", nuevo_mon->nombre);
            } else {
                e->medallas -= mon_seleccionado->stats_base.damage_base;
                printf("%s ha sido liberado.\n", mon_seleccionado->nombre);
                list_popCurrent(origen);
                free(mon_seleccionado);
            }
        }
    } while (opcion != 0);

    esperar_tecla();
}


void mostrar_puntajes_finales(Entrenador* entrenadores, int num_entrenadores, int id_ganador) {
    printf("\n¡La Liga Pokémon ha concluido!\n");
    printf("+------------+----------+----------+\n");
    printf("| Entrenador | Medallas | Rango    |\n");
    printf("+------------+----------+----------+\n");
    for (int i = 0; i < num_entrenadores; i++) {
        const char* rango;
        if (entrenadores[i].medallas > 100) rango = "Campeón";
        else if (entrenadores[i].medallas >= 50) rango = "Maestro";
        else rango = "Entrenador";
        printf("| %-10d | %-8d | %-8s |\n", i + 1, entrenadores[i].medallas, rango);
    }
    printf("+------------+----------+----------+\n");
    if (id_ganador != 0) {
        printf("¡Entrenador %d es el Campeón Pokémon!\n", id_ganador);
    } else {
        printf("Energía agotada. Nadie es Campeón.\n");
    }
}

// ------ MODIFICADO: Manejo de entrada de teclado para Linux ------
void mover(Map* ubicaciones, Entrenador* e, Entrenador* entrenadores, int num_entrenadores, int id_entrenador) {
    MapPair* par = map_search(ubicaciones, &e->id);
    if (!par) {
        printf("Error: Ubicación ID %d no encontrada.\n", e->id);
        return;
    }
    Ubicacion* ubi = (Ubicacion*)par->value;
    int movimiento_valido = 0;

    do {
        printf("Presione: w (Norte), s (Sur), a (Oeste), d (Este), otra tecla para cancelar\n");
        
        char tecla;
        #ifdef _WIN32
            tecla = getch();
        #else
            initscr();
            noecho();
            cbreak();
            tecla = getch();
            endwin();
        #endif

        int nueva_ubicacion = -1;
        char* direccion = "una dirección inválida";
        switch (tecla) {
            case 'w': case 'W': nueva_ubicacion = ubi->arriba; direccion = "Norte"; break;
            case 's': case 'S': nueva_ubicacion = ubi->abajo; direccion = "Sur"; break;
            case 'a': case 'A': nueva_ubicacion = ubi->izquierda; direccion = "Oeste"; break;
            case 'd': case 'D': nueva_ubicacion = ubi->derecha; direccion = "Este"; break;
            default:
                printf("\nViaje cancelado.\n");
                esperar_tecla();
                return;
        }
        if (nueva_ubicacion != -1) {
            MapPair* nuevo_par = map_search(ubicaciones, &nueva_ubicacion);
            if (!nuevo_par) {
                printf("Error: Nueva ubicación ID %d no encontrada.\n", nueva_ubicacion);
                return;
            }
            e->id = nueva_ubicacion;
            int* id_historial = malloc(sizeof(int));
            *id_historial = nueva_ubicacion;
            list_pushBack(e->historial, id_historial);
            
            Ubicacion* nueva_ubi = (Ubicacion*)nuevo_par->value;
            movimiento_valido = 1;
            
            if (nueva_ubi->es_final) {
                printf("\n+----------------------------------+\n");
                printf("| ¡CAMPEÓN POKÉMON!                |\n");
                printf("| Has conquistado la Liga Pokémon  |\n");
                printf("+----------------------------------+\n");
                esperar_tecla();
                mostrar_puntajes_finales(entrenadores, num_entrenadores, id_entrenador);
                printf("Equipo: ");
                imprimir_mones(e->equipo);
                printf("\nMedallas: %d\n", e->medallas);
                esperar_tecla();
                exit(0); // El juego termina aquí con un ganador
            }
        } else {
            printf("\nNo hay ruta hacia %s. Intenta otra.\n", direccion);
            esperar_tecla();
        }
    } while (!movimiento_valido);
}

void desafiar_gimnasio(Entrenador* e) {
    limpiar_pantalla();
    printf("\n=== Desafíos de Gimnasio ===\n");
    printf("Resuelve el desafío para ganar una medalla.\n");
    int opcion;
    do {
        opcion = leer_opcion_valida(); 
        printf("Seleccione gimnasio (1: Roca, 2: Agua, 3: Eléctrico, 0: Salir): ");
        if (opcion == 0) {
            return;
        }
        
        int respuesta_correcta = 0;
        int puntaje_otorgado = 0;
        
        switch (opcion) {
            case 1:
                printf("\nGimnasio Roca:\nBrock te desafía: Calcula la fuerza de un Geodude (integral de x^3 de 1 a 2).\n");
                respuesta_correcta = 4; // (2^4/4) - (1^4/4) = 4 - 0.25 ≈ 4
                puntaje_otorgado = 10;
                break;
            case 2:
                printf("\nGimnasio Agua:\nMisty te reta: Un Staryu tiene un poder que satisface x^2 - 8x + 16 = 0. Multiplica su nivel por 5.\n");
                respuesta_correcta = 20; // (x-4)^2 = 0 => x=4. 4*5=20
                puntaje_otorgado = 10;
                break;
            case 3:
                printf("\nGimnasio Eléctrico:\nLt. Surge te desafía: La velocidad de un Voltorb es 10x^2. Calcula su cambio en el nivel 1 (deriva y evalúa en x=1).\n");
                respuesta_correcta = 20; // f'(x) = 20x. f'(1) = 20.
                puntaje_otorgado = 10;
                break;
        }

        int respuesta_usuario = leer_opcion_valida();
        printf("Ingresa la respuesta: ");

        if (respuesta_usuario == respuesta_correcta) {
            printf("\n¡Correcto! Has ganado una medalla y %d puntos.\n", puntaje_otorgado);
            e->medallas += puntaje_otorgado;
        } else {
            printf("\nRespuesta incorrecta. Intenta de nuevo.\n");
        }
        esperar_tecla();
    } while (opcion != 0);
}


void liberar_recursos(Map* ubicaciones, Entrenador* entrenadores, int num_entrenadores) {
    if (ubicaciones) {
        MapPair* par = map_first(ubicaciones);
        while (par) {
            Ubicacion* ubi = (Ubicacion*)par->value;
            list_clean(ubi->mones);
            free(ubi->mones);
            free(par->key);
            free(ubi);
            par = map_next(ubicaciones);
        }
        map_clean(ubicaciones);
    }
    if (entrenadores) {
        for (int i = 0; i < num_entrenadores; i++) {
            if (entrenadores[i].equipo) {
                list_clean(entrenadores[i].equipo);
                free(entrenadores[i].equipo);
                entrenadores[i].equipo = NULL;
            }
            if (entrenadores[i].historial) {
                list_clean(entrenadores[i].historial);
                free(entrenadores[i].historial);
                entrenadores[i].historial = NULL;
            }
        }
    }
}

void menu_jugador(Map* ubicaciones, Entrenador* entrenadores, int indice_entrenador_actual, int num_entrenadores) {
    while(true) {
        Entrenador* entrenador_actual = &entrenadores[indice_entrenador_actual];
        
        mostrar_estado(ubicaciones, entrenador_actual, indice_entrenador_actual + 1);
        printf("1. Capturar Pokémon\n2. Liberar Pokémon\n3. Viajar\n4. Desafiar Gimnasio\n0. Volver al menú principal\nOpción: ");
        int opcion_juego = leer_opcion_valida();
        
        switch (opcion_juego) {
            case 1: 
                gestionar_mones(ubicaciones, entrenador_actual, 1);
                break;
            case 2: 
                gestionar_mones(ubicaciones, entrenador_actual, 0);
                break;
            case 3: 
                mover(ubicaciones, entrenador_actual, entrenadores, num_entrenadores, indice_entrenador_actual + 1); 
                break;
            case 4: 
                desafiar_gimnasio(entrenador_actual); 
                break;
            case 0: 
                break;
        }

        if (opcion_juego == 0) {
            liberar_recursos(NULL, entrenadores, num_entrenadores);
            break;
        }
        
        limpiar_pantalla();
        // Cambiar de turno
        indice_entrenador_actual = (indice_entrenador_actual + 1) % num_entrenadores;
    }
}

void imprimir_mondex(Map* MONDEX) {
    MapPair* pair = map_first(MONDEX);

    while (pair != NULL) {
        Mon* mon = pair->value;
        printf("(%2d). %s\n", mon->ID, mon->nombre);
        pair = map_next(MONDEX);
    }
}

void _mondex(Map* MONDEX) {
    imprimir_mondex(MONDEX);
    printf("Ingrese el Mon que desee buscar ('0' - Ninguno):");
    char buffer[2];
    leer_opcion(buffer);
    
    MapPair* pair = map_search(MONDEX, buffer);
    if (pair == NULL) { puts("No se encontró este Mon."); return; }
    limpiar_pantalla();
    Mon* mon = pair->value;
    imprimir_separador("DATOS DEL MON", 40);
    printf("ID. %d\nNombre\n", mon->ID,mon->nombre);
}

int main(void) {
    Map* ubicaciones = map_create(is_equal_int);
    int cantidad_ubicaciones = 0;

    MONDEX = map_create(is_equal_int);
    cargar_grafo_desde_csv(ubicaciones, &cantidad_ubicaciones);
    cargar_mones_desde_csv(MONDEX);
    if (!cantidad_ubicaciones) {
        printf("No se cargó la región. Verifique los archivos CSV.\n");
        liberar_recursos(ubicaciones, NULL, 0);
        free(ubicaciones);
        return 1;
    }

    while (true) {
        printf("=== Pokémon Quest ===\n1. Iniciar Aventura\n2. DEBUG MODE (No implementado)\n3. MonDex\n0. Salir\nOpción: ");
        int opcion = leer_opcion_valida();
        
        switch (opcion) {
            case 1:
                int num_entrenadores = 1;
                Entrenador* entrenadores = inicializar_entrenador();
                int indice_entrenador_actual = 0;
                menu_jugador(ubicaciones, entrenadores, indice_entrenador_actual, num_entrenadores);
                break;
            case 2:
                // DEBUG MODE
                break;
            case 3:
                _mondex(MONDEX);
                esperar_enter();
                break;
            default:
                break;
        }
        if (opcion == 0) break;
        limpiar_pantalla();
    }

    liberar_recursos(ubicaciones, NULL, 0);
    free(ubicaciones);
    printf("¡Hasta luego!\n");
    return 0;
}