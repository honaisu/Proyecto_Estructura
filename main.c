#include <stdio.h>                                              // Incluye la biblioteca para mostrar o leer datos en la consola, como escribir (printf) o leer (scanf)
#include <stdlib.h>                                             // Incluye la biblioteca para reservar espacio en memoria (malloc) o liberarlo (free)
#include <string.h>                                             // Incluye la biblioteca para trabajar con textos, como copiar (strcpy) o comparar (strcmp) textos
#include <math.h>                                               // Incluye la biblioteca para funciones matemáticas, como redondear hacia arriba (ceil)
#include <conio.h>                                              // Incluye la biblioteca para funciones de consola, como leer teclas (getch) sin esperar Enter
#include "headers/TDAs/list.h"                                  // Incluye un archivo personalizado que define una lista para guardar elementos en orden
#include "headers/TDAs/map.h"                                   // Incluye un archivo personalizado que define un mapa para guardar pares clave-valor
#include "headers/TDAs/extras.h"                                // Incluye un archivo con funciones adicionales, como leer archivos CSV (leer_linea_csv)

// probando git o.o

typedef struct {
    char nombre[50];
    float tamano;
    int puntos_combate;
} Mon;

typedef struct {                                                // Define una estructura para representar un Pokémon
    char nombre[50];                                            // Un espacio para guardar el nombre del Pokémon (hasta 50 letras)
    float tamano;                                               // Un número decimal para guardar el tamaño del Pokémon (en kg)
    int puntos_combate;                                         // Un número entero para guardar los puntos de combate del Pokémon
} Mon;                                                      // Llama a esta estructura "Mon" (Pokémon)

typedef struct {                                                // Define una estructura para representar un lugar en el juego
    int id;                                                     // Un número entero para identificar el lugar
    char nombre[100];                                           // Un espacio para el nombre del lugar (hasta 100 letras)
    char descripcion[300];                                      // Un espacio para la descripción del lugar (hasta 300 letras)
    int es_final;                                               // Un número (0 o 1) para indicar si es el lugar final (Liga Pokémon)
    int arriba, abajo, izquierda, derecha;                      // Números para los IDs de lugares vecinos (-1 si no hay conexión)
    List* mones;                                                // Un puntero a una lista que guarda los Pokémon disponibles en este lugar
} Ubicacion;                                                // Llama a esta estructura "Ubicacion"

typedef struct {                                                // Define una estructura para representar al jugador (entrenador)
    int id;                                                     // Un número entero para identificar al entrenador
    List* equipo;                                               // Un puntero a una lista que guarda los Pokémon capturados
    float tamano_equipo;                                        // Un número decimal para el tamaño total de los Pokémon del equipo
    int medallas;                                               // Un número entero para contar las medallas obtenidas
    int energia;                                                // Un número entero para la energía restante del entrenador
    List* historial;                                            // Un puntero a una lista que guarda los IDs de los lugares visitados
} Entrenador;                                               // Llama a esta estructura "Entrenador"

int es_igual_int(void* clave1, void* clave2) {                   // Define una función para comparar dos números enteros (claves) en un mapa
    return *(int*)clave1 == *(int*)clave2;                      // Compara los valores de las claves y devuelve 1 si son iguales, 0 si no
}                                                               // Cierra la función es_igual_int

void imprimir_mones(List* mones) {                              // Define una función para mostrar los Pokémon en una lista
    if (!list_size(mones)) {                                    // Verifica si la lista de Pokémon está vacía
        printf("Ninguno\n");                                    // Si está vacía, muestra "Ninguno"
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    void* ptr = list_first(mones);                              // Obtiene el primer Pokémon de la lista
    while (ptr) {                                               // Mientras haya Pokémon en la lista
        Mon* mon = (Mon*)ptr;                                   // Convierte el puntero en un Pokémon
        printf("%s (%d PC, %.1f kg)", mon->nombre, mon->puntos_combate, mon->tamano); // Muestra el nombre, puntos de combate y tamaño del Pokémon
        ptr = list_next(mones);                                 // Pasa al siguiente Pokémon
        if (ptr) printf(", ");                                  // Si hay más Pokémon, agrega una coma y espacio
    }                                                           // Cierra el bucle while
    printf("\n");                                               // Agrega un salto de línea al final
}                                                               // Cierra la función imprimir_mones

void esperar_tecla() {                                          // Define una función que espera a que el usuario presione una tecla
    printf("Presiona una tecla para continuar...\n");           // Muestra un mensaje pidiendo presionar una tecla
    getch();                                                    // Espera a que el usuario presione una tecla
    while (kbhit()) getch();                                    // Limpia cualquier tecla adicional presionada
}                                                               // Cierra la función esperar_tecla

int leer_opcion_valida(int min, int max, const char* mensaje) { // Define una función para pedir un número válido entre min y max
    char buffer[100];                                           // Crea un espacio para guardar la entrada del usuario
    int opcion;                                                 // Declara una variable para guardar el número ingresado
    while (1) {                                                 // Inicia un bucle infinito hasta obtener una entrada válida
        printf("%s", mensaje);                                  // Muestra el mensaje dado (por ejemplo, "Elige una opción: ")
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;    // Lee la entrada del usuario; si falla, repite
        buffer[strcspn(buffer, "\n")] = '\0';                   // Quita el salto de línea de la entrada
        if (strlen(buffer) == 0) continue;                      // Si la entrada está vacía, repite
        if (sscanf(buffer, "%d", &opcion) != 1) continue;       // Convierte la entrada a número; si falla, repite
        if (opcion < min || opcion > max) continue;             // Si el número está fuera del rango, repite
        while (getchar() != '\n' && !feof(stdin));              // Limpia caracteres sobrantes en la entrada
        return opcion;                                          // Devuelve el número válido
    }                                                           // Cierra el bucle while
}                                                               // Cierra la función leer_opcion_valida

int mostrar_menu_final() {                                      // Define una función para mostrar el menú al final del juego
    return leer_opcion_valida(1, 2, "\n=== Fin de la Aventura ===\n1. Volver al MonDex\n2. Salir\nOpción: "); // Muestra el menú final y devuelve la opción (1 o 2)
}                                                               // Cierra la función mostrar_menu_final

void cargar_mones_desde_csv(Map* datos_mones) {                 // Define una función para leer Pokémon desde mones.csv
    FILE* archivo = fopen("data/mones.csv", "r");               // Abre el archivo mones.csv para lectura
    if (!archivo) {                                             // Verifica si no se pudo abrir el archivo
        printf("Error al cargar mones.csv\n");                  // Muestra un mensaje de error
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    char** campos;                                              // Declara un puntero para guardar los campos del CSV
    fgets(malloc(500), 500, archivo);                           // Lee y descarta la primera línea (encabezado) del archivo
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {   // Lee cada línea del CSV, separada por comas
        int num_campos = 0;                                     // Inicializa un contador para los campos
        for (int i = 0; campos[i]; i++) num_campos++;           // Cuenta cuántos campos hay en la línea
        if (num_campos != 7) continue;                          // Si no hay 7 campos, ignora la línea
        Mon* mon = (Mon*)malloc(sizeof(Mon));                   // Reserva memoria para un nuevo Pokémon
        strcpy(mon->nombre, campos[1]);                         // Copia el nombre (segundo campo) al Pokémon
        mon->puntos_combate = atoi(campos[3]);                  // Convierte el cuarto campo (HP BASE) a número y lo guarda
        mon->tamano = atof(campos[4]);                          // Convierte el quinto campo (DAÑO BASE) a decimal y lo guarda
        char* clave = strdup(campos[1]);                        // Crea una copia del nombre para usarla como clave
        map_insert(datos_mones, clave, mon);                    // Inserta el Pokémon en el mapa con su nombre como clave
    }                                                           // Cierra el bucle while
    fclose(archivo);                                            // Cierra el archivo mones.csv
    printf("Se cargó correctamente mones.csv\n");               // Muestra un mensaje de éxito
}                                                               // Cierra la función cargar_mones_desde_csv

void cargar_grafo_desde_csv(Map* ubicaciones, int* cantidad_ubicaciones) { // Define una función para leer ubicaciones desde zonas_pokemon.csv
    Map* datos_mones = map_create(is_equal_str);                // Crea un mapa para guardar Pokémon, comparando claves como texto
    cargar_mones_desde_csv(datos_mones);                        // Carga los Pokémon desde mones.csv
    if (!datos_mones || map_first(datos_mones) == NULL) {       // Verifica si no se cargaron Pokémon
        printf("Error al cargar mones.csv\n");                  // Muestra un mensaje de error
        free(datos_mones);                                      // Libera la memoria del mapa
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    FILE* archivo = fopen("data/zonas_pokemon.csv", "r"); // Abre el archivo zonas_pokemon.csv
    if (!archivo) {                                             // Verifica si no se pudo abrir
        printf("Error al cargar zonas_pokemon.csv\n");          // Muestra un mensaje de error
        map_clean(datos_mones);                                 // Limpia el mapa de Pokémon
        free(datos_mones);                                      // Libera la memoria del mapa
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    char** campos;                                              // Declara un puntero para los campos del CSV
    fgets(malloc(500), 500, archivo);                           // Lee y descarta la primera línea (encabezado)
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {   // Lee cada línea del CSV, separada por comas
        int num_campos = 0;                                     // Inicializa un contador para los campos
        for (int i = 0; campos[i]; i++) num_campos++;           // Cuenta cuántos campos hay
        if (num_campos != 10 && num_campos != 9) continue;      // Si no hay 9 o 10 campos, ignora la línea
        Ubicacion* ubicacion = (Ubicacion*)malloc(sizeof(Ubicacion)); // Reserva memoria para una nueva ubicación
        ubicacion->id = atoi(campos[0] + (campos[0][0] == 'n' ? 1 : 0)); // Convierte el ID (primer campo), quitando "n" si existe
        strcpy(ubicacion->nombre, campos[1]);                   // Copia el nombre (segundo campo)
        strcpy(ubicacion->descripcion, campos[2]);              // Copia la descripción (tercer campo)
        ubicacion->es_final = (num_campos == 10 && strcmp(campos[9], "Sí") == 0) ? 1 : 0; // Si hay 10 campos y el último es "Sí", marca como final
        ubicacion->arriba = atoi(campos[5]);                    // Convierte el sexto campo (Arriba) a número
        ubicacion->abajo = atoi(campos[6]);                     // Convierte el séptimo campo (Abajo) a número
        ubicacion->izquierda = atoi(campos[7]);                 // Convierte el octavo campo (Izquierda) a número
        ubicacion->derecha = atoi(campos[8]);                   // Convierte el noveno campo (Derecha) a número
        ubicacion->mones = list_create();                       // Crea una lista vacía para los Pokémon de la ubicación
        if (strlen(campos[4]) > 0) {                            // Si el quinto campo (Pokemons) no está vacío
            List* nombres_mones = split_string(campos[4], ";"); // Divide los nombres de Pokémon (separados por ";") en una lista
            for (char* nombre = list_first(nombres_mones); nombre != NULL; nombre = list_next(nombres_mones)) { // Recorre cada nombre
                MapPair* par = map_search(datos_mones, nombre); // Busca el Pokémon en el mapa
                if (par) {                                      // Si encuentra el Pokémon
                    Mon* mon = (Mon*)par->value;                // Obtiene el Pokémon del mapa
                    Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon)); // Reserva memoria para un nuevo Pokémon
                    strcpy(nuevo_mon->nombre, mon->nombre);     // Copia el nombre
                    nuevo_mon->puntos_combate = mon->puntos_combate; // Copia los puntos de combate
                    nuevo_mon->tamano = mon->tamano;            // Copia el tamaño
                    list_pushBack(ubicacion->mones, nuevo_mon); // Añade el Pokémon a la lista de la ubicación
                }                                               // Cierra el bloque if
            }                                                   // Cierra el bucle for
            list_clean(nombres_mones);                          // Limpia la lista de nombres
            free(nombres_mones);                                // Libera la memoria de la lista
        }                                                       // Cierra el bloque if
        int* clave = malloc(sizeof(int));                       // Reserva memoria para la clave (ID) de la ubicación
        *clave = ubicacion->id;                                 // Asigna el ID como clave
        map_insert(ubicaciones, clave, ubicacion);              // Inserta la ubicación en el mapa
        (*cantidad_ubicaciones)++;                              // Aumenta el contador de ubicaciones
    }                                                           // Cierra el bucle while
    fclose(archivo);                                            // Cierra el archivo zonas_pokemon.csv
    printf("Se cargó correctamente zonas_pokemon.csv\n");       // Muestra un mensaje de éxito
    MapPair* par = map_first(datos_mones);                      // Obtiene el primer par (clave-valor) del mapa de Pokémon
    while (par) {                                               // Mientras haya pares en el mapa
        free(par->key);                                         // Libera la clave (nombre del Pokémon)
        free(par->value);                                       // Libera el valor (Pokémon)
        par = map_next(datos_mones);                            // Pasa al siguiente par
    }                                                           // Cierra el bucle while
    map_clean(datos_mones);                                     // Limpia el mapa de Pokémon
    free(datos_mones);                                          // Libera la memoria del mapa
}                                                               // Cierra la función cargar_grafo_desde_csv

void inicializar_entrenador(Entrenador* e) {                    // Define una función para configurar un entrenador nuevo
    e->id = 1;                                                  // Asigna el ID 1 al entrenador
    e->equipo = list_create();                                  // Crea una lista vacía para el equipo de Pokémon
    e->tamano_equipo = 0.0;                                     // Inicializa el tamaño del equipo en 0
    e->medallas = 0;                                            // Inicializa las medallas en 0
    e->energia = 15;                                            // Asigna 15 unidades de energía iniciales
    e->historial = list_create();                               // Crea una lista vacía para el historial de ubicaciones
    int* id = malloc(sizeof(int));                              // Reserva memoria para un ID de ubicación
    *id = 1;                                                    // Asigna el ID 1 (ubicación inicial)
    list_pushBack(e->historial, id);                            // Añade el ID al historial
}                                                               // Cierra la función inicializar_entrenador

void mostrar_estado(Map* ubicaciones, Entrenador* e, int id_entrenador) { // Define una función para mostrar el estado del entrenador
    limpiar_pantalla();                                         // Limpia la pantalla de la consola
    MapPair* par = map_search(ubicaciones, &e->id);             // Busca la ubicación actual del entrenador
    if (!par) {                                                 // Si no encuentra la ubicación
        printf("Error: Ubicación ID %d no encontrada.\n", e->id); // Muestra un mensaje de error
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    Ubicacion* ubi = (Ubicacion*)par->value;                    // Obtiene la ubicación encontrada
    printf("\n=== Entrenador %d ===\n", id_entrenador);         // Muestra el número del entrenador (por ejemplo, "Entrenador 1")
    printf("Ubicación: %s\n", ubi->descripcion);                // Muestra la descripción de la ubicación
    printf("Pokémon disponibles: ");                            // Muestra una etiqueta para los Pokémon disponibles
    imprimir_mones(ubi->mones);                                 // Muestra los Pokémon de la ubicación
    printf("Energía restante: %d\n", e->energia);               // Muestra la energía del entrenador
    printf("Equipo: ");                                         // Muestra una etiqueta para el equipo
    imprimir_mones(e->equipo);                                  // Muestra los Pokémon del equipo
    printf("Tamaño equipo: %.1f kg, Medallas: %d\n", e->tamano_equipo, e->medallas); // Muestra el tamaño del equipo y las medallas
    printf("Rutas: ");                                          // Muestra una etiqueta para las rutas
    if (ubi->arriba != -1) printf("Norte ");                    // Si hay ruta al norte, muestra "Norte"
    if (ubi->abajo != -1) printf("Sur ");                       // Si hay ruta al sur, muestra "Sur"
    if (ubi->izquierda != -1) printf("Oeste ");                 // Si hay ruta al oeste, muestra "Oeste"
    if (ubi->derecha != -1) printf("Este ");                    // Si hay ruta al este, muestra "Este"
    printf("\n");                                               // Agrega un salto de línea
}                                                               // Cierra la función mostrar_estado

void gestionar_mones(Map* ubicaciones, Entrenador* e, int capturar) { // Define una función para capturar o liberar Pokémon
    MapPair* par = map_search(ubicaciones, &e->id);             // Busca la ubicación actual del entrenador
    if (!par) return;                                           // Si no encuentra la ubicación, termina
    Ubicacion* ubi = (Ubicacion*)par->value;                    // Obtiene la ubicación
    List* origen = capturar ? ubi->mones : e->equipo;           // Elige la lista: Pokémon de la ubicación (capturar) o equipo (liberar)
    if (!list_size(origen)) {                                   // Si la lista está vacía
        printf("%s vacío.\n", capturar ? "Ubicación" : "Equipo"); // Muestra un mensaje de lista vacía
        esperar_tecla();                                        // Espera a que el usuario presione una tecla
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    printf("%s:\n", capturar ? "Pokémon disponibles" : "Tu equipo"); // Muestra qué lista se está manejando
    void* ptr = list_first(origen);                             // Obtiene el primer elemento de la lista
    int i = 1;                                                  // Inicializa un contador para numerar Pokémon
    List* indices = list_create();                              // Crea una lista para los índices de Pokémon
    while (ptr) {                                               // Mientras haya Pokémon en la lista
        Mon* mon = (Mon*)ptr;                                   // Convierte el puntero en un Pokémon
        printf("%d. %s (%d PC, %.1f kg)\n", i, mon->nombre, mon->puntos_combate, mon->tamano); // Muestra el número, nombre, puntos y tamaño
        int* indice = malloc(sizeof(int));                      // Reserva memoria para un índice
        *indice = i++;                                          // Asigna el número actual y lo incrementa
        list_pushBack(indices, indice);                         // Añade el índice a la lista
        ptr = list_next(origen);                                // Pasa al siguiente Pokémon
    }                                                           // Cierra el bucle while
    int opcion;                                                 // Declara una variable para la opción del usuario
    do {                                                        // Inicia un bucle para seleccionar Pokémon
        opcion = leer_opcion_valida(0, list_size(indices), "Seleccione Pokémon (0 para terminar): "); // Pide una opción válida
        if (opcion == 0) break;                                 // Si elige 0, sale del bucle
        ptr = list_first(origen);                               // Vuelve al primer Pokémon
        for (i = 1; i < opcion && ptr; i++) ptr = list_next(origen); // Avanza hasta el Pokémon elegido
        if (ptr) {                                              // Si se encontró el Pokémon
            Mon* mon = (Mon*)ptr;                               // Obtiene el Pokémon seleccionado
            if (capturar) {                                     // Si está capturando
                Mon* nuevo_mon = (Mon*)malloc(sizeof(Mon));     // Reserva memoria para un nuevo Pokémon
                strcpy(nuevo_mon->nombre, mon->nombre);         // Copia el nombre
                nuevo_mon->puntos_combate = mon->puntos_combate; // Copia los puntos de combate
                nuevo_mon->tamano = mon->tamano;                // Copia el tamaño
                list_pushBack(e->equipo, nuevo_mon);            // Añade el Pokémon al equipo
                list_popCurrent(origen);                        // Elimina el Pokémon de la ubicación
                free(mon);                                      // Libera el Pokémon original
                e->tamano_equipo += nuevo_mon->tamano;          // Suma el tamaño al equipo
                e->medallas += nuevo_mon->puntos_combate;       // Suma los puntos como medallas
            } else {                                            // Si está liberando
                list_popCurrent(origen);                        // Elimina el Pokémon del equipo
                e->tamano_equipo -= mon->tamano;                // Resta el tamaño del equipo
                e->medallas -= mon->puntos_combate;             // Resta los puntos de las medallas
                free(mon);                                      // Libera el Pokémon
            }                                                   // Cierra el bloque if-else
        }                                                       // Cierra el bloque if
    } while (opcion > 0);                                       // Repite mientras no elija 0
    list_clean(indices);                                        // Limpia la lista de índices
    free(indices);                                              // Libera la memoria de la lista
    e->energia--;                                               // Reduce la energía en 1
    esperar_tecla();                                            // Espera a que presione una tecla
}                                                               // Cierra la función gestionar_mones

void mostrar_puntajes_finales(Entrenador* entrenadores, int num_entrenadores, int id_ganador) { // Define una función para mostrar resultados finales
    printf("\n¡La Liga Pokémon ha concluido!\n");               // Muestra que el juego terminó
    printf("+------------+----------+----------+\n");             // Dibuja la primera línea de una tabla
    printf("| Entrenador | Medallas | Rango    |\n");             // Muestra los encabezados de la tabla
    printf("+------------+----------+----------+\n");             // Dibuja una línea separadora
    for (int i = 0; i < num_entrenadores; i++) {                // Recorre cada entrenador
        const char* rango;                                      // Declara una variable para el rango
        if (entrenadores[i].medallas > 100) rango = "Campeón";  // Si tiene más de 100 medallas, es "Campeón"
        else if (entrenadores[i].medallas >= 50) rango = "Maestro"; // Si tiene 50 o más, es "Maestro"
        else rango = "Entrenador";                              // Si tiene menos de 50, es "Entrenador"
        printf("| %d          | %d       | %s |\n", i + 1, entrenadores[i].medallas, rango); // Muestra una fila de la tabla
    }                                                           // Cierra el bucle for
    printf("+------------+----------+----------+\n");             // Dibuja la línea final de la tabla
    if (id_ganador != 0) {                                      // Si hay un ganador
        printf("¡Entrenador %d es el Campeón Pokémon!\n", id_ganador); // Muestra quién ganó
    } else {                                                    // Si no hay ganador
        printf("Energía agotada. Nadie es Campeón.\n");         // Muestra que nadie ganó por falta de energía
    }                                                           // Cierra el bloque if-else
}                                                               // Cierra la función mostrar_puntajes_finales

void mover(Map* ubicaciones, Entrenador* e, Entrenador* entrenadores, int num_entrenadores, int id_entrenador) { // Define una función para moverse
    MapPair* par = map_search(ubicaciones, &e->id);             // Busca la ubicación actual del entrenador
    if (!par) {                                                 // Si no la encuentra
        printf("Error: Ubicación ID %d no encontrada.\n", e->id); // Muestra un mensaje de error
        return;                                                 // Termina la función
    }                                                           // Cierra el bloque if
    Ubicacion* ubi = (Ubicacion*)par->value;                    // Obtiene la ubicación actual
    int movimiento_valido = 0;                                  // Inicializa una variable para verificar el movimiento
    do {                                                        // Inicia un bucle hasta que el movimiento sea válido
        printf("Presione: w (Norte), s (Sur), a (Oeste), d (Este), otra tecla para cancelar\n"); // Pide una tecla para moverse
        char tecla = getch();                                   // Captura la tecla presionada
        int nueva_ubicacion = -1;                               // Inicializa la nueva ubicación en -1
        char* direccion = NULL;                                 // Declara un puntero para la dirección (por ejemplo, "Norte")
        switch (tecla) {                                        // Evalúa la tecla presionada
            case 'w': case 'W': nueva_ubicacion = ubi->arriba; direccion = "Norte"; break; // Si es 'w', elige el norte
            case 's': case 'S': nueva_ubicacion = ubi->abajo; direccion = "Sur"; break; // Si es 's', elige el sur
            case 'a': case 'A': nueva_ubicacion = ubi->izquierda; direccion = "Oeste"; break; // Si es 'a', elige el oeste
            case 'd': case 'D': nueva_ubicacion = ubi->derecha; direccion = "Este"; break; // Si es 'd', elige el este
            default:                                            // Si es otra tecla
                printf("\nViaje cancelado.\n");                 // Muestra que se canceló el movimiento
                esperar_tecla();                                // Espera una tecla
                return;                                         // Termina la función
        }                                                       // Cierra el bloque switch
        if (nueva_ubicacion != -1) {                            // Si hay una ruta en la dirección
            int costo_energia = (int)ceil((e->tamano_equipo + 1) / 10.0); // Calcula el costo de energía según el equipo
            if (e->energia <= costo_energia) {                  // Si no hay suficiente energía
                printf("\n¡Energía agotada! Has perdido.\n");   // Muestra un mensaje de derrota
                mostrar_puntajes_finales(entrenadores, num_entrenadores, 0); // Muestra los puntajes sin ganador
                esperar_tecla();                                // Espera una tecla
                int opcion = mostrar_menu_final();              // Muestra el menú final
                if (opcion == 1) exit(0);                       // Si elige volver al MonDex, termina el programa
                return;                                         // Termina la función
            }                                                   // Cierra el bloque if
            MapPair* nuevo_par = map_search(ubicaciones, &nueva_ubicacion); // Busca la nueva ubicación
            if (!nuevo_par) {                                   // Si no la encuentra
                printf("Error: Nueva ubicación ID %d no encontrada.\n", nueva_ubicacion); // Muestra un error
                return;                                         // Termina la función
            }                                                   // Cierra el bloque if
            e->id = nueva_ubicacion;                            // Actualiza la ubicación del entrenador
            e->energia -= costo_energia;                        // Resta el costo de energía
            int* id = malloc(sizeof(int));                      // Reserva memoria para el ID
            *id = nueva_ubicacion;                              // Asigna el ID de la nueva ubicación
            list_pushBack(e->historial, id);                    // Añade el ID al historial
            Ubicacion* nueva_ubi = (Ubicacion*)nuevo_par->value; // Obtiene la nueva ubicación
            movimiento_valido = 1;                              // Marca el movimiento como válido
            if (nueva_ubi->es_final) {                          // Si es la ubicación final
                printf("\n+----------------------------------+\n"); // Dibuja una línea decorativa
                printf("| ¡CAMPEÓN POKÉMON!                |\n"); // Muestra un mensaje de victoria
                printf("| Has conquistado la Liga Pokémon  |\n"); // Muestra otro mensaje de victoria
                printf("+----------------------------------+\n"); // Dibuja otra línea decorativa
                esperar_tecla();                                // Espera una tecla
                mostrar_puntajes_finales(entrenadores, num_entrenadores, id_entrenador); // Muestra los puntajes
                printf("Equipo: ");                             // Muestra una etiqueta para el equipo
                imprimir_mones(e->equipo);                      // Muestra los Pokémon del equipo
                printf("\nMedallas: %d\n", e->medallas);        // Muestra las medallas
                esperar_tecla();                                // Espera otra tecla
                int opcion = mostrar_menu_final();              // Muestra el menú final
                if (opcion == 1) exit(0);                       // Si elige volver al MonDex, termina el programa
                return;                                         // Termina la función
            }                                                   // Cierra el bloque if
        } else {                                                // Si no hay ruta
            printf("\nNo hay ruta hacia %s. Intenta otra.\n", direccion); // Muestra un mensaje de error
            esperar_tecla();                                    // Espera una tecla
        }                                                       // Cierra el bloque else
    } while (!movimiento_valido);                               // Repite hasta un movimiento válido
}                                                               // Cierra la función mover

void desafiar_gimnasio(Entrenador* e) {                         // Define una función para desafiar gimnasios
    limpiar_pantalla();                                         // Limpia la pantalla
    printf("\n=== Desafíos de Gimnasio ===\n");                 // Muestra un título
    printf("Resuelve el desafío para ganar una medalla.\n");    // Explica que los desafíos dan medallas
    printf("Cada intento consume 1 unidad de energía. Energía actual: %d\n\n", e->energia); // Muestra la energía
    int opcion;                                                 // Declara una variable para la opción
    do {                                                        // Inicia un bucle para desafíos
        opcion = leer_opcion_valida(0, 3, "Seleccione gimnasio (1: Roca, 2: Agua, 3: Eléctrico, 0: Salir): "); // Pide un gimnasio
        if (opcion == 0) {                                      // Si elige salir
            esperar_tecla();                                    // Espera una tecla
            return;                                             // Termina la función
        }                                                       // Cierra el bloque if
        if (e->energia <= 0) {                                  // Si no hay energía
            printf("\n¡Energía agotada! Has perdido.\n");       // Muestra un mensaje de derrota
            esperar_tecla();                                    // Espera una tecla
            exit(0);                                            // Termina el programa
        }                                                       // Cierra el bloque if
        switch (opcion) {                                       // Evalúa el gimnasio elegido
            case 1:                                             // Gimnasio Roca
                printf("\nGimnasio Roca:\n");                   // Muestra el nombre
                printf("Brock te desafía: Calcula la fuerza de un Geodude que crece con el cubo de su nivel, desde 1 hasta 2.\n"); // Describe el desafío
                printf("(Resuelve: integral de 1 a 2 de x^3 dx)\n"); // Explica la integral
                break;                                          // Termina el caso
            case 2:                                             // Gimnasio Agua
                printf("\nGimnasio Agua:\n");                   // Muestra el nombre
                printf("Misty te reta: Un Staryu tiene un poder que satisface x^2 - 8x + 16 = 0. Multiplica su nivel por 5.\n"); // Describe el desafío
                break;                                          // Termina el caso
            case 3:                                             // Gimnasio Eléctrico
                printf("\nGimnasio Eléctrico:\n");              // Muestra el nombre
                printf("Lt. Surge te desafía: La velocidad de un Voltorb crece como 10x^2. Calcula su cambio en el nivel 1.\n"); // Describe el desafío
                printf("(Deriva f(x) = 10x^2, evalúa en x = 1)\n"); // Explica la derivada
                break;                                          // Termina el caso
        }                                                       // Cierra el bloque switch
        int puntaje = leer_opcion_valida(INT_MIN, INT_MAX, "Ingresa la respuesta: "); // Pide la respuesta al desafío
        switch (opcion) {                                       // Evalúa la respuesta
            case 1:                                             // Gimnasio Roca
                if (puntaje == 4) {                             // Si la respuesta es 4
                    printf("\n¡Correcto! Medalla Roca obtenida:\n"); // Muestra éxito
                    printf("Sigue al sur desde Pueblo Paleta, luego al este.\n"); // Da una pista
                    e->medallas += 10;                          // Suma 10 medallas
                } else {                                        // Si la respuesta es incorrecta
                    printf("Respuesta incorrecta. Intenta de nuevo.\n"); // Muestra error
                }                                               // Cierra el bloque if-else
                break;                                          // Termina el caso
            case 2:                                             // Gimnasio Agua
                if (puntaje == 20) {                            // Si la respuesta es 20
                    printf("\n¡Correcto! Medalla Agua obtenida:\n"); // Muestra éxito
                    printf("Desde Ruta 1, ve al sur y luego al oeste.\n"); // Da una pista
                    e->medallas += 10;                          // Suma 10 medallas
                } else {                                        // Si la respuesta es incorrecta
                    printf("Respuesta incorrecta. Intenta de nuevo.\n"); // Muestra error
                }                                               // Cierra el bloque if-else
                break;                                          // Termina el caso
            case 3:                                             // Gimnasio Eléctrico
                if (puntaje == 20) {                            // Si la respuesta es 20
                    printf("\n¡Correcto! Medalla Eléctrica obtenida:\n"); // Muestra éxito
                    printf("Dirígete al norte desde la Liga Pokémon.\n"); // Da una pista
                    e->medallas += 10;                          // Suma 10 medallas
                } else {                                        // Si la respuesta es incorrecta
                    printf("Respuesta incorrecta. Intenta de nuevo.\n"); // Muestra error
                }                                               // Cierra el bloque if-else
                break;                                          // Termina el caso
        }                                                       // Cierra el bloque switch
        e->energia--;                                           // Reduce la energía en 1
        printf("\nEnergía restante: %d\n", e->energia);         // Muestra la energía restante
        esperar_tecla();                                        // Espera una tecla
    } while (opcion != 0);                                      // Repite hasta que elija salir
}                                                               // Cierra la función desafiar_gimnasio

void liberar_recursos(Map* ubicaciones, Entrenador* entrenadores, int num_entrenadores) { // Define una función para liberar memoria
    if (ubicaciones) {                                          // Si existe el mapa de ubicaciones
        MapPair* par = map_first(ubicaciones);                  // Obtiene el primer par (clave-valor)
        while (par) {                                           // Mientras haya pares
            Ubicacion* ubi = (Ubicacion*)par->value;            // Obtiene la ubicación
            if (ubi->mones) {                                   // Si la ubicación tiene Pokémon
                void* mon = list_first(ubi->mones);             // Obtiene el primer Pokémon
                while (mon) {                                   // Mientras haya Pokémon
                    free(mon);                                  // Libera el Pokémon
                    mon = list_next(ubi->mones);                // Pasa al siguiente
                }                                               // Cierra el bucle while
                list_clean(ubi->mones);                         // Limpia la lista de Pokémon
                free(ubi->mones);                               // Libera la memoria de la lista
            }                                                   // Cierra el bloque if
            free(par->key);                                     // Libera la clave (ID)
            free(ubi);                                          // Libera la ubicación
            par = map_next(ubicaciones);                        // Pasa al siguiente par
        }                                                       // Cierra el bucle while
        map_clean(ubicaciones);                                 // Limpia el mapa de ubicaciones
    }                                                           // Cierra el bloque if
    if (entrenadores && num_entrenadores > 0) {                 // Si hay entrenadores
        for (int i = 0; i < num_entrenadores; i++) {            // Recorre cada entrenador
            if (entrenadores[i].equipo) {                       // Si el entrenador tiene equipo
                void* mon = list_first(entrenadores[i].equipo); // Obtiene el primer Pokémon
                while (mon) {                                   // Mientras haya Pokémon
                    free(mon);                                  // Libera el Pokémon
                    mon = list_next(entrenadores[i].equipo);    // Pasa al siguiente
                }                                               // Cierra el bucle while
                list_clean(entrenadores[i].equipo);             // Limpia la lista del equipo
                free(entrenadores[i].equipo);                   // Libera la memoria del equipo
                entrenadores[i].equipo = NULL;                  // Marca el equipo como nulo
            }                                                   // Cierra el bloque if
            if (entrenadores[i].historial) {                    // Si el entrenador tiene historial
                void* id = list_first(entrenadores[i].historial); // Obtiene el primer ID
                while (id) {                                    // Mientras haya IDs
                    free(id);                                   // Libera el ID
                    id = list_next(entrenadores[i].historial);  // Pasa al siguiente
                }                                               // Cierra el bucle while
                list_clean(entrenadores[i].historial);          // Limpia la lista del historial
                free(entrenadores[i].historial);                // Libera la memoria del historial
                entrenadores[i].historial = NULL;               // Marca el historial como nulo
            }                                                   // Cierra el bloque if
        }                                                       // Cierra el bucle for
    }                                                           // Cierra el bloque if
}                                                               // Cierra la función liberar_recursos

int main() {                                                    // Define la función principal del programa
    Map* ubicaciones = map_create(es_igual_int);                // Crea un mapa para las ubicaciones, comparando claves como números
    int cantidad_ubicaciones = 0;                               // Inicializa un contador para las ubicaciones
    int ejecutando = 1;                                         // Inicializa una variable para mantener el juego activo
    cargar_grafo_desde_csv(ubicaciones, &cantidad_ubicaciones); // Carga las ubicaciones desde zonas_pokemon.csv
    if (!cantidad_ubicaciones) {                                // Si no se cargaron ubicaciones
        printf("No se cargó la región. Verifique los archivos CSV.\n"); // Muestra un mensaje de error
        liberar_recursos(ubicaciones, NULL, 0);                 // Libera los recursos
        free(ubicaciones);                                      // Libera el mapa de ubicaciones
        return 1;                                               // Termina el programa con error
    }                                                           // Cierra el bloque if
    while (ejecutando) {                                        // Mientras el juego esté activo
        int opcion = leer_opcion_valida(1, 4, "=== Pokémon Quest ===\n1. Explorar Región\n2. Iniciar Aventura\n3. Salir\n4. Desafiar Gimnasio\nOpción: "); // Muestra el menú principal
        if (opcion == 1) {                                      // Si elige explorar la región
            printf("Región cargada.\n");                        // Muestra un mensaje de confirmación
            esperar_tecla();                                    // Espera una tecla
        } else if (opcion == 2) {                               // Si elige iniciar una aventura
            int num_entrenadores = leer_opcion_valida(1, 2, "Cuantos entrenadores? (1-2): "); // Pide el número de entrenadores (1 o 2)
            Entrenador entrenadores[2] = {0};                   // Crea un arreglo para hasta 2 entrenadores
            for (int i = 0; i < num_entrenadores; i++) {        // Recorre cada entrenador
                inicializar_entrenador(&entrenadores[i]);       // Configura el entrenador
            }                                                   // Cierra el bucle for
            int ganador = 0;                                    // Inicializa una variable para el ganador
            int indice_entrenador_actual = 0;                   // Inicializa el índice del entrenador actual
            while (1) {                                         // Inicia un bucle para la aventura
                if (ganador || !ejecutando) break;              // Si hay un ganador o el juego terminó, sale
                Entrenador* entrenador_actual = &entrenadores[indice_entrenador_actual]; // Obtiene el entrenador actual
                if (entrenador_actual->energia <= 0) {          // Si el entrenador no tiene energía
                    indice_entrenador_actual = (indice_entrenador_actual + 1) % num_entrenadores; // Pasa al siguiente entrenador
                    continue;                                   // Continúa el bucle
                }                                               // Cierra el bloque if
                int id_entrenador = indice_entrenador_actual + 1; // Asigna un ID al entrenador (1 o 2)
                mostrar_estado(ubicaciones, entrenador_actual, id_entrenador); // Muestra el estado del entrenador
                int opcion_juego = leer_opcion_valida(1, 6, "1. Capturar Pokémon\n2. Liberar Pokémon\n3. Viajar\n4. Volver al MonDex\n5. Reiniciar aventura\n6. Desafiar Gimnasio\nOpción: "); // Muestra el menú del juego
                switch (opcion_juego) {                         // Evalúa la opción del juego
                    case 1:                                     // Si elige capturar Pokémon
                        gestionar_mones(ubicaciones, entrenador_actual, 1); // Llama a la función para capturar
                        break;                                  // Termina el caso
                    case 2:                                     // Si elige liberar Pokémon
                        gestionar_mones(ubicaciones, entrenador_actual, 0); // Llama a la función para liberar
                        break;                                  // Termina el caso
                    case 3:                                     // Si elige viajar
                        mover(ubicaciones, entrenador_actual, entrenadores, num_entrenadores, id_entrenador); // Llama a la función para moverse
                        if (num_entrenadores == 2 && entrenador_actual->energia > 0) { // Si hay 2 entrenadores y aún tiene energía
                            MapPair* par = map_search(ubicaciones, &entrenador_actual->id); // Busca la ubicación actual
                            if (par && ((Ubicacion*)par->value)->es_final) { // Si está en la ubicación final
                                ganador = id_entrenador;            // Marca al entrenador como ganador
                                mostrar_puntajes_finales(entrenadores, num_entrenadores, id_entrenador); // Muestra los puntajes
                                printf("Equipo: ");                 // Muestra una etiqueta para el equipo
                                imprimir_mones(entrenador_actual->equipo); // Muestra el equipo
                                printf("\nMedallas: %d\n", entrenador_actual->medallas); // Muestra las medallas
                                esperar_tecla();                    // Espera una tecla
                                int opcion = mostrar_menu_final();  // Muestra el menú final
                                if (opcion == 1) {                  // Si elige volver al MonDex
                                    liberar_recursos(NULL, entrenadores, num_entrenadores); // Libera los recursos de los entrenadores
                                    continue;                       // Continúa el bucle principal
                                }                                       // Cierra el bloque if
                                liberar_recursos(ubicaciones, entrenadores, num_entrenadores); // Libera todos los recursos
                                free(ubicaciones);                  // Libera el mapa de ubicaciones
                                ejecutando = 0;                     // Termina el juego
                                break;                              // Termina el caso
                            }                                       // Cierra el bloque if
                        }                                           // Cierra el bloque if
                        break;                                      // Termina el caso
                    case 4:                                         // Si elige volver al MonDex
                        liberar_recursos(NULL, entrenadores, num_entrenadores); // Libera los recursos de los entrenadores
                        printf("Volviendo al MonDex...\n");         // Muestra un mensaje
                        esperar_tecla();                            // Espera una tecla
                        goto restart_game;                          // Salta al inicio del bucle principal
                    case 5:                                         // Si elige reiniciar la aventura
                        liberar_recursos(NULL, entrenadores, num_entrenadores); // Libera los recursos de los entrenadores
                        printf("Reiniciando...\n");                 // Muestra un mensaje
                        esperar_tecla();                            // Espera una tecla
                        goto restart_game;                          // Salta al inicio del bucle principal
                    case 6:                                         // Si elige desafiar un gimnasio
                        desafiar_gimnasio(entrenador_actual);       // Llama a la función para desafiar
                        break;                                      // Termina el caso
                }                                               // Cierra el bloque switch
                indice_entrenador_actual = (indice_entrenador_actual + 1) % num_entrenadores; // Pasa al siguiente entrenador
            }                                                   // Cierra el bucle while
restart_game:                                               // Etiqueta para reiniciar el juego
            continue;                                           // Continúa el bucle principal
        } else if (opcion == 3) {                               // Si elige salir
            liberar_recursos(ubicaciones, NULL, 0);             // Libera los recursos de las ubicaciones
            free(ubicaciones);                                  // Libera el mapa de ubicaciones
            ejecutando = 0;                                     // Termina el juego
        } else if (opcion == 4) {                               // Si elige desafiar un gimnasio
            limpiar_pantalla();                                 // Limpia la pantalla
            printf("Inicia una aventura para poder desafiar a los gimnasios.\n"); // Muestra un mensaje de error
            esperar_tecla();                                    // Espera una tecla
        }                                                       // Cierra el bloque else if
    }                                                           // Cierra el bucle while
    liberar_recursos(ubicaciones, NULL, 0);                     // Libera los recursos de las ubicaciones
    free(ubicaciones);                                          // Libera el mapa de ubicaciones
    return 0;                                                   // Termina el programa con éxito
}                                                               // Cierra la función main