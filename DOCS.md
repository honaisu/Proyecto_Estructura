# Roles

- Victor: **Coordinador**.
- Ignacio: **Comunicador**.
- Diego: **Gestor de Calidad**.
- Benjamín: **Responsable de Integración y Consistencia**.

## Explicación

- **Coordinador**: Organizar el trabajo global del equipo, planifica las tareas semanales.
- **Comunicador**: Actúa como enlace con el profesor y los ayudantes, toma nota de las reuniones y registra decisiones importantes. Organiza y mantiene actualizada la documentación de la aplicación (requisitos y diseño).
- **Gestor de Calidad**: Define la forma de trabajo del equipo, promueve revisiones de código y documentación, organiza reuniones de seguimiento para evaluar el progreso.
- **Responsable de Integración y Consistencia**: Se asegura que las distintas partes del proyecto funcionen de manera coherente. Verifica que las implementaciones estén bien integradas.


# Estructuras

### Tipo **Base**

```c
typedef struct {
	int hp_base;
	int damage_base;
	int defense_base;
} Base_M;
```
 
 ### Tipo **Mon**

```c
typedef struct {
	int ID; // Identificador del Mon
	char nombre[MAX]; // Nombre del Mon
	char apodo[MAX]; // Apodo ingresado por el jugador para el Mon
	char tipo[MAX]; // Tipo del Mon
	
	Base_M stats_base; // Base para los stats de cierto Mon
	int hp_actual;
	int damage_actual;
	int defense_actual;
	
	int nivel_actual;
	int experiencia;
	bool is_dead; // Verifica si está muerto el Mon o no
} Mon;
```

### Tipo **Jugador**

```c
typedef struct {
	List* mones;
	int mones_totales;
	int monballs_actual;
	
	int pos_x;
	int pos_y;
} Player;
```

### Tipo **NPC**

```c
typedef struct {
	List* mones;
	int mones_totales;
	char nombre[MAX];
} NPC;
```

### Tipo **Mundo**:

```c
typedef struct {
    Grafo* mapa; // Grafos para conexiones entre zonas
    Matriz* zona_actual; // Matriz para la zona donde está el jugador (hierba, agua, etc.)
    Mapa* lista_mones; // Todos los Mones que existan
    int tipo_zona; // Pradera, cueva, ciudad, etc.
} Mundo;
```

### Tipo **Batalla**:

```c
typedef struct {
    Mon* jugador; // Mon actual
    Mon* oponente;
    int resultado; // 0: en curso, 1: gana jugador, 2: gana oponente
} Batalla;
```

## `TDAs`

- **Grafos**: Podrá ser usado para la generación del mundo.
- **Mapas**:  Va a ser implementado de manera que se guarden todos los mones existentes dentro del programa, por ID como clave.
- **Listas**:  Se podrá ser utilizado de manera que se pueda recorrer el "inventario" (mones) del jugador.

# Funcionalidades

### **Menú Inicial**

- Iniciar Partida.
- Instrucciones de Uso.
- Pistas del Juego.
- Salir.

### **Menú Partida (Mundo)**

- Moverse.
- Inventario:
	- Lista de `Mones`.
- Reiniciar Partida.
- Salir.

### **Combate por turnos**
- Se pueda decidir ciertas opciones:
	- Atacar.
	- Defenderse.
	- Huir.
- Dependiendo de si quedan `Mones` dentro del campo de batalla, se terminará el combate o no.

