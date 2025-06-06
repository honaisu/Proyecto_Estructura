#include "extras.h"

#define MAX_LONGITUD_LINEA 1024
#define MAX_CAMPOS 300

char** leer_linea_csv(FILE *archivo, char separador) {
    static char linea[MAX_LONGITUD_LINEA];
    static char *campos[MAX_CAMPOS];
    char *ptr, *start;
    int idx = 0;

    if (fgets(linea, MAX_LONGITUD_LINEA, archivo) == NULL) {
        return NULL; // No hay más líneas para leer
    }

    // Eliminar salto de linea
    linea[strcspn(linea, "\n")] = '\0';

    ptr = start = linea;
    while (*ptr) {
        if (idx >= MAX_CAMPOS - 1) break;

        if (*ptr == '\"') { // Inicio de un campo entrecomillado
            start = ++ptr;    // Saltar la comilla inicial
            while (*ptr && !(*ptr == '\"' && *(ptr + 1) == separador)) ptr++;
        } else { // Campo sin comillas
            start = ptr;
            while (*ptr && *ptr != separador) ptr++;
        }

        if (*ptr) { 
            *ptr = '\0'; // Reemplazar comilla final o separador por terminación
            ptr++;
            if (*ptr == separador) ptr++;
        }

        // Quitar comilla final si existe
        if (*(ptr - 2) == '\"') *(ptr - 2) = '\0';
        campos[idx++] = start;
    }

    campos[idx] = NULL; // Marcar el final del array
    return campos;
}

List *split_string(const char *str, const char *delimitador) {
  List *result = list_create();
  char *token = strtok((char *)str, delimitador);

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

    // Agregar el nuevo string a la lista
    list_pushBack(result, new_token);

    // Obtener el siguiente token
    token = strtok(NULL, delimitador);
  }

  return result;
}

//---//

void limpiar_pantalla() {
  // Verifica si es que el OS es Windows o "UNIX" (Linux/MacOS)
  #ifdef _WIN32
    system("cls");
  #else
    printf("\033[H\033[J"); 
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
  return strcmp((char *)key1, (char *)key2) == 0 ;
}

void leer_entrada(char* string) {
    char temporal[200];
    fgets(temporal, sizeof(temporal), stdin);
    temporal[strcspn(temporal, "\n")] = '\0';
    strcpy(string, temporal);
}

void leer_opcion(char* opcion) { 
  // Asigna el puntero (primer valor) al valor final de una cadena (\0).
  // En caso de que el usuario no ingrese nada.
  *opcion = '\0';
  printf("Ingrese su opción: ");
  char temporal[200];
  leer_entrada(temporal);
  sscanf(temporal, "%c", opcion); 
}

void imprimir_gato(void) {
  puts(" \\    /\\               |\'/-..--.") ;
  puts("  )  ( ')             / _ _   ,  ;")  ;
  puts(" (  /  )             `~=`Y'~_<._./")  ;
  puts("  \\(__)|             <`-....__.'")   ;
}