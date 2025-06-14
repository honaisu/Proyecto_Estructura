#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include "headers/TDAs/list.h"
#include "headers/TDAs/map.h"
#include "headers/TDAs/extras.h"

// probando git o.o

typedef struct {
    char nombre[50];
    float tamano;
    int puntos_combate;
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
    float tamano_equipo;
    int medallas;
    int energia;
    List* historial;
} Entrenador;

int es_igual_int(void* clave1, void* clave2) {
    return *(int*)clave1 == *(int*)clave2;
}

void imprimir_mones(List* mones) {
    if (!list_size(mones)) {
        printf("Ninguno\n");
        return;
    }
    void* ptr = list_first(mones);
    while (ptr) {
        Mon* mon = (Mon*)ptr;
        printf("%s (%d PC, %.1f kg)", mon->nombre, mon->puntos_combate, mon->tamano);
        ptr = list_next(mones);
        if (ptr) printf(", ");
    }
    printf("\n");
}

void esperar_tecla() {
    printf("Presiona una tecla para continuar...\n");
    getch();
    while (kbhit()) getch();
}

int leer_opcion_valida(int min, int max, const char* mensaje) {
    char buffer[100];
    int opcion;
    while (1) {
        printf("%s", mensaje);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 0) continue;
        if (sscanf(buffer, "%d", &opcion) != 1) continue;
        if (opcion < min || opcion > max) continue;
        while (getchar() != '\n' && !feof(stdin));
        return opcion;
    }
}

int mostrar_menu_final() {
    return leer_opcion_valida(1, 2, "\n=== Fin de la Aventura ===\n1. Volver al MonDex\n2. Salir\nOpción: ");
}

void cargar_mones_desde_csv(Map* datos_mones) {
    FILE* archivo = fopen("C:\\Users\\victor\\OneDrive\\Desktop\\Proyecto_Estructura\\data\\mones.csv", "r");
    if (!archivo) {
        printf("Error al cargar mones.csv\n");
        return;
    }
    char** campos;
    fgets(malloc(500), 500, archivo); // Saltar encabezado
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        int num_campos = 0;
        for (int i = 0; campos[i]; i++) num_campos++;
        if (num_campos != 7) continue;
        Mon* mon = (Mon*)malloc(sizeof(Mon));
        strcpy(mon->nombre, campos[1]); // NOMBRE
        mon->puntos_combate = atoi(campos[3]); // HP BASE
        mon->tamano = atof(campos[4]); // DAÑO BASE
        char* clave = strdup(campos[1]);
        map_insert(datos_mones, clave, mon);
    }
    fclose(archivo);
    printf("Se cargó correctamente mones.csv\n");
}

void cargar_grafo_desde_csv(Map* ubicaciones, int* cantidad_ubicaciones) {
    Map* datos_mones = map_create(is_equal_str);
    cargar_mones_desde_csv(datos_mones);
    if (!datos_mones || map_first(datos_mones) == NULL) {
        printf("Error al cargar mones.csv\n");
        free(datos_mones);
        return;
    }

    FILE* archivo = fopen("C:\\Users\\victor\\OneDrive\\Desktop\\Proyecto_Estructura\\data\\zonas_pokemon.csv", "r");
    if (!archivo) {
        printf("Error al cargar zonas_pokemon.csv\n");
        map_clean(datos_mones);
        free(datos_mones);
        return;
    }
    char** campos;
    fgets(malloc(500), 500, archivo); // Saltar cabecera
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        int num_campos = 0;
        for (int i = 0; campos[i]; i++) num_campos++;
        if (num_campos != 10 && num_campos != 9) continue;

        Ubicacion* ubicacion = (Ubicacion*)malloc(sizeof(Ubicacion));
        ubicacion->id = atoi(campos[0] + (campos[0][0] == 'n' ? 1 : 0)); // Quitar 'n' del ID
        strcpy(ubicacion->nombre, campos[1]);
        strcpy(ubicacion->descripcion, campos[2]);
        ubicacion->es_final = (num_campos == 10 && strcmp(campos[9], "Sí") == 0) ? 1 : 0;
        ubicacion->arriba = atoi(campos[5]);
        ubicacion->abajo = atoi(campos[6]);
        ubicacion->izquierda = atoi(campos[7]);
        ubicacion->derecha = atoi(campos[8]);
        ubicacion->mones = list_create();

        if (strlen(campos[4]) > 0) {
            List* nombres_mones = split_string(campos[4], ";");
            for (char* nombre = list_first(nombres_mones); nombre != NULL; nombre = list_next(nombres_mones)) {
                MapPair* par = map_search(datos_mones, nombre);
                if (par) {
                    Mon* mon = (Mon*)par->value;
                    Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon));
                    strcpy(nuevo_mon->nombre, mon->nombre);
                    nuevo_mon->puntos_combate = mon->puntos_combate;
                    nuevo_mon->tamano = mon->tamano;
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

    MapPair* par = map_first(datos_mones);
    while (par) {
        free(par->key);
        free(par->value);
        par = map_next(datos_mones);
    }
    map_clean(datos_mones);
    free(datos_mones);
}

void inicializar_entrenador(Entrenador* e) {
    e->id = 1;
    e->equipo = list_create();
    e->tamano_equipo = 0.0;
    e->medallas = 0;
    e->energia = 15;
    e->historial = list_create();
    int* id = malloc(sizeof(int));
    *id = 1;
    list_pushBack(e->historial, id);
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
    printf("Energía restante: %d\n", e->energia);
    printf("Equipo: ");
    imprimir_mones(e->equipo);
    printf("Tamaño equipo: %.1f kg, Medallas: %d\n", e->tamano_equipo, e->medallas);
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
        printf("%s vacío.\n", capturar ? "Ubicación" : "Equipo");
        esperar_tecla();
        return;
    }

    printf("%s:\n", capturar ? "Pokémon disponibles" : "Tu equipo");
    void* ptr = list_first(origen);
    int i = 1;
    List* indices = list_create();
    while (ptr) {
        Mon* mon = (Mon*)ptr;
        printf("%d. %s (%d PC, %.1f kg)\n", i, mon->nombre, mon->puntos_combate, mon->tamano);
        int* indice = malloc(sizeof(int));
        *indice = i++;
        list_pushBack(indices, indice);
        ptr = list_next(origen);
    }

    int opcion;
    do {
        opcion = leer_opcion_valida(0, list_size(indices), "Seleccione Pokémon (0 para terminar): ");
        if (opcion == 0) break;
        ptr = list_first(origen);
        for (i = 1; i < opcion && ptr; i++) ptr = list_next(origen);
        if (ptr) {
            Mon* mon = (Mon*)ptr;
            if (capturar) {
                Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon));
                strcpy(nuevo_mon->nombre, mon->nombre);
                nuevo_mon->puntos_combate = mon->puntos_combate;
                nuevo_mon->tamano = mon->tamano;
                list_pushBack(e->equipo, nuevo_mon);
                list_popCurrent(origen);
                free(mon);
                e->tamano_equipo += nuevo_mon->tamano;
                e->medallas += nuevo_mon->puntos_combate;
            } else {
                list_popCurrent(origen);
                e->tamano_equipo -= mon->tamano;
                e->medallas -= mon->puntos_combate;
                free(mon);
            }
        }
    } while (opcion > 0);

    list_clean(indices);
    free(indices);
    e->energia--;
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
        printf("| %d          | %d       | %s |\n", i + 1, entrenadores[i].medallas, rango);
    }
    printf("+------------+----------+----------+\n");
    if (id_ganador != 0) {
        printf("¡Entrenador %d es el Campeón Pokémon!\n", id_ganador);
    } else {
        printf("Energía agotada. Nadie es Campeón.\n");
    }
}

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
        char tecla = getch();
        int nueva_ubicacion = -1;
        char* direccion = NULL;
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
            int costo_energia = (int)ceil((e->tamano_equipo + 1) / 10.0);
            if (e->energia <= costo_energia) {
                printf("\n¡Energía agotada! Has perdido.\n");
                mostrar_puntajes_finales(entrenadores, num_entrenadores, 0);
                esperar_tecla();
                int opcion = mostrar_menu_final();
                if (opcion == 1) exit(0);
                return;
            }
            MapPair* nuevo_par = map_search(ubicaciones, &nueva_ubicacion);
            if (!nuevo_par) {
                printf("Error: Nueva ubicación ID %d no encontrada.\n", nueva_ubicacion);
                return;
            }
            e->id = nueva_ubicacion;
            e->energia -= costo_energia;
            int* id = malloc(sizeof(int));
            *id = nueva_ubicacion;
            list_pushBack(e->historial, id);
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
                int opcion = mostrar_menu_final();
                if (opcion == 1) exit(0);
                return;
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
    printf("Cada intento consume 1 unidad de energía. Energía actual: %d\n\n", e->energia);
    int opcion;
    do {
        opcion = leer_opcion_valida(0, 3, "Seleccione gimnasio (1: Roca, 2: Agua, 3: Eléctrico, 0: Salir): ");
        if (opcion == 0) {
            esperar_tecla();
            return;
        }
        if (e->energia <= 0) {
            printf("\n¡Energía agotada! Has perdido.\n");
            esperar_tecla();
            exit(0);
        }
        switch (opcion) {
            case 1:
                printf("\nGimnasio Roca:\n");
                printf("Brock te desafía: Calcula la fuerza de un Geodude que crece con el cubo de su nivel, desde 1 hasta 2.\n");
                printf("(Resuelve: integral de 1 a 2 de x^3 dx)\n");
                break;
            case 2:
                printf("\nGimnasio Agua:\n");
                printf("Misty te reta: Un Staryu tiene un poder que satisface x^2 - 8x + 16 = 0. Multiplica su nivel por 5.\n");
                break;
            case 3:
                printf("\nGimnasio Eléctrico:\n");
                printf("Lt. Surge te desafía: La velocidad de un Voltorb crece como 10x^2. Calcula su cambio en el nivel 1.\n");
                printf("(Deriva f(x) = 10x^2, evalúa en x = 1)\n");
                break;
        }
        int puntaje = leer_opcion_valida(INT_MIN, INT_MAX, "Ingresa la respuesta: ");
        switch (opcion) {
            case 1:
                if (puntaje == 4) {
                    printf("\n¡Correcto! Medalla Roca obtenida:\n");
                    printf("Sigue al sur desde Pueblo Paleta, luego al este.\n");
                    e->medallas += 10;
                } else {
                    printf("Respuesta incorrecta. Intenta de nuevo.\n");
                }
                break;
            case 2:
                if (puntaje == 20) {
                    printf("\n¡Correcto! Medalla Agua obtenida:\n");
                    printf("Desde Ruta 1, ve al sur y luego al oeste.\n");
                    e->medallas += 10;
                } else {
                    printf("Respuesta incorrecta. Intenta de nuevo.\n");
                }
                break;
            case 3:
                if (puntaje == 20) {
                    printf("\n¡Correcto! Medalla Eléctrica obtenida:\n");
                    printf("Dirígete al norte desde la Liga Pokémon.\n");
                    e->medallas += 10;
                } else {
                    printf("Respuesta incorrecta. Intenta de nuevo.\n");
                }
                break;
        }
        e->energia--;
        printf("\nEnergía restante: %d\n", e->energia);
        esperar_tecla();
    } while (opcion != 0);
}

void liberar_recursos(Map* ubicaciones, Entrenador* entrenadores, int num_entrenadores) {
    if (ubicaciones) {
        MapPair* par = map_first(ubicaciones);
        while (par) {
            Ubicacion* ubi = (Ubicacion*)par->value;
            if (ubi->mones) {
                void* mon = list_first(ubi->mones);
                while (mon) {
                    free(mon);
                    mon = list_next(ubi->mones);
                }
                list_clean(ubi->mones);
                free(ubi->mones);
            }
            free(par->key);
            free(ubi);
            par = map_next(ubicaciones);
        }
        map_clean(ubicaciones);
    }
    if (entrenadores && num_entrenadores > 0) {
        for (int i = 0; i < num_entrenadores; i++) {
            if (entrenadores[i].equipo) {
                void* mon = list_first(entrenadores[i].equipo);
                while (mon) {
                    free(mon);
                    mon = list_next(entrenadores[i].equipo);
                }
                list_clean(entrenadores[i].equipo);
                free(entrenadores[i].equipo);
                entrenadores[i].equipo = NULL;
            }
            if (entrenadores[i].historial) {
                void* id = list_first(entrenadores[i].historial);
                while (id) {
                    free(id);
                    id = list_next(entrenadores[i].historial);
                }
                list_clean(entrenadores[i].historial);
                free(entrenadores[i].historial);
                entrenadores[i].historial = NULL;
            }
        }
    }
}

int main() {
    Map* ubicaciones = map_create(es_igual_int);
    int cantidad_ubicaciones = 0;
    int ejecutando = 1;

    cargar_grafo_desde_csv(ubicaciones, &cantidad_ubicaciones);
    if (!cantidad_ubicaciones) {
        printf("No se cargó la región. Verifique los archivos CSV.\n");
        liberar_recursos(ubicaciones, NULL, 0);
        free(ubicaciones);
        return 1;
    }

    while (ejecutando) {
        int opcion = leer_opcion_valida(1, 4, "=== Pokémon Quest ===\n1. Explorar Región\n2. Iniciar Aventura\n3. Salir\n4. Desafiar Gimnasio\nOpción: ");
        if (opcion == 1) {
            printf("Región cargada.\n");
            esperar_tecla();
        } else if (opcion == 2) {
            int num_entrenadores = leer_opcion_valida(1, 2, "Cuantos entrenadores? (1-2): ");
            Entrenador entrenadores[2] = {0};
            for (int i = 0; i < num_entrenadores; i++) {
                inicializar_entrenador(&entrenadores[i]);
            }
            int ganador = 0;
            int indice_entrenador_actual = 0;
            while (1) {
                if (ganador || !ejecutando) break;
                Entrenador* entrenador_actual = &entrenadores[indice_entrenador_actual];
                if (entrenador_actual->energia <= 0) {
                    indice_entrenador_actual = (indice_entrenador_actual + 1) % num_entrenadores;
                    continue;
                }
                int id_entrenador = indice_entrenador_actual + 1;
                mostrar_estado(ubicaciones, entrenador_actual, id_entrenador);
                int opcion_juego = leer_opcion_valida(1, 6, "1. Capturar Pokémon\n2. Liberar Pokémon\n3. Viajar\n4. Volver al MonDex\n5. Reiniciar aventura\n6. Desafiar Gimnasio\nOpción: ");
                switch (opcion_juego) {
                    case 1:
                        gestionar_mones(ubicaciones, entrenador_actual, 1);
                        break;
                    case 2:
                        gestionar_mones(ubicaciones, entrenador_actual, 0);
                        break;
                    case 3:
                        mover(ubicaciones, entrenador_actual, entrenadores, num_entrenadores, id_entrenador);
                        if (num_entrenadores == 2 && entrenador_actual->energia > 0) {
                            MapPair* par = map_search(ubicaciones, &entrenador_actual->id);
                            if (par && ((Ubicacion*)par->value)->es_final) {
                                ganador = id_entrenador;
                                mostrar_puntajes_finales(entrenadores, num_entrenadores, id_entrenador);
                                printf("Equipo: ");
                                imprimir_mones(entrenador_actual->equipo);
                                printf("\nMedallas: %d\n", entrenador_actual->medallas);
                                esperar_tecla();
                                int opcion = mostrar_menu_final();
                                if (opcion == 1) {
                                    liberar_recursos(NULL, entrenadores, num_entrenadores);
                                    continue;
                                }
                                liberar_recursos(ubicaciones, entrenadores, num_entrenadores);
                                free(ubicaciones);
                                ejecutando = 0;
                                break;
                            }
                        }
                        break;
                    case 4:
                        liberar_recursos(NULL, entrenadores, num_entrenadores);
                        printf("Volviendo al MonDex...\n");
                        esperar_tecla();
                        goto restart_game;
                    case 5:
                        liberar_recursos(NULL, entrenadores, num_entrenadores);
                        printf("Reiniciando...\n");
                        esperar_tecla();
                        goto restart_game;
                    case 6:
                        desafiar_gimnasio(entrenador_actual);
                        break;
                }
                indice_entrenador_actual = (indice_entrenador_actual + 1) % num_entrenadores;
            }
restart_game:
            continue;
        } else if (opcion == 3) {
            liberar_recursos(ubicaciones, NULL, 0);
            free(ubicaciones);
            ejecutando = 0;
        } else if (opcion == 4) {
            limpiar_pantalla();
            printf("Inicia una aventura para poder desafiar a los gimnasios.\n");
            esperar_tecla();
        }
    }

    liberar_recursos(ubicaciones, NULL, 0);
    free(ubicaciones);
    return 0;
}
