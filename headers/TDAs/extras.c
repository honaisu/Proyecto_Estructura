#include "extras.h"

#define MAX_LONGITUD_LINEA 1024
#define MAX_CAMPOS 300

char** leer_linea_csv(FILE *archivo, char separador) {
    static char linea[MAX_LONGITUD_LINEA];
    static char *campos[MAX_CAMPOS];
    int idx = 0;

    if (fgets(linea, MAX_LONGITUD_LINEA, archivo) == NULL) {
        return NULL;
    }

    linea[strcspn(linea, "\n")] = '\0';
    linea[strcspn(linea, "\r")] = '\0';

    if (strlen(linea) == 0) {
        return NULL;
    }

    char *ptr = linea;
    char *start = ptr;
    campos[idx] = start;

    while (*ptr && idx < MAX_CAMPOS - 1) {
        if (*ptr == separador) {
            *ptr = '\0';
            campos[idx] = start;
            idx++;
            start = ptr + 1;
            campos[idx] = start;
        }
        ptr++;
    }

    if (ptr > start || idx == 0) {
        campos[idx] = start;
        idx++;
    }

    campos[idx] = NULL;
    return campos;
}

List *split_string(const char *str, const char *delimitador) {
    List *result = list_create();
    char *str_copy = strdup(str); // Copia para evitar modificar str
    char *token = strtok(str_copy, delimitador);

    while (token != NULL) {
        // Eliminar espacios en blanco al inicio del token
        while (*token == ' ') { token++; }
        
        // Eliminar espacios en blanco al final del token
        char *end = token + strlen(token) - 1;
        while (*end == ' ' && end > token) {
            *end = '\0';
            end--;
        }

        // Copiar el token en un nuevo string
        char *new_token = strdup(token);
        list_pushBack(result, new_token);
        token = strtok(NULL, delimitador);
    }

    free(str_copy);
    return result;
}

void limpiar_pantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void imprimir_separador(const char* mensaje, const int delimitador) {
    for (int i = 0; i < delimitador; i++) printf("\033[1;37m=");
    putchar('\n');
    
    int longitud_mensaje = strlen(mensaje);
    int espacios = (delimitador - longitud_mensaje) / 2;
    
    if (espacios > 0) { for (int i = 0; i < espacios; i++) putchar(' '); }
    
    puts(mensaje);
    
    for (int i = 0; i < delimitador; i++) printf("=");
    printf("\033[0m");
    putchar('\n');
}

void esperar_enter() {
    puts("Presione ENTER para continuar...");
    while (getchar() != '\n');
}

int is_equal_int(void *key1, void *key2) {
    return *(int *)key1 == *(int *)key2;
}

int is_equal_str(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}

void leer_entrada(char* string) {
    char temporal[200];
    fgets(temporal, sizeof(temporal), stdin);
    temporal[strcspn(temporal, "\n")] = '\0';
    strcpy(string, temporal);
}

void leer_opcion(char* opcion) { 
    *opcion = '\0';
    printf("Ingrese su opción: ");
    char temporal[200];
    leer_entrada(temporal);
    sscanf(temporal, "%c", opcion); 
}

void imprimir_gato(void) {
    puts(" \\    /\\               |\'/-..--.");
    puts("  )  ( ')             / _ _   ,  ;");
    puts(" (  /  )             `~=`Y'~_<._./");
    puts("  \\(__)|             <`-....__.'");
}

void esperar_tecla(char* tecla) {
    #ifdef _WIN32
        *tecla = getch();
        while (kbhit()) getch();
    #else
        initscr();      // Inicia modo ncurses
        noecho();       // No mostrar la tecla presionada
        cbreak();       // Capturar teclas sin esperar Enter
        *tecla = getch();
        endwin();       // Finaliza modo ncurses
    #endif
}

int leer_opcion_valida(void) {
    char buffer[100];
    int opcion;
    leer_entrada(buffer);
    if (*buffer == '\n' || *buffer == '\0') opcion = -1;
    else opcion = atoi(buffer);
    return opcion;
}

