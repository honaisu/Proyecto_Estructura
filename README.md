# [🐈] THE MON PROJECT

**The Mon Project** es un proyecto basado en Pokémon para el ramo de **Estructura de Datos**. Busca poder otorgar la experiencia ofrecida por los juegos de Pokémon de una forma simplificada y divertida, hecho para poder ser adaptado al lenguaje de programación del curso y aplicando, también, los conocimientos y habilidades formados en el transcurso del ramo. 

Es un juego interactivo que sumerge al usuario en un mundo de aventuras donde puede explorar regiones, capturar criaturas (Mones), hacer compras en tiendas con un sistema de inventario y participar en combates por turnos. El jugador podrá ganar si es que **logra llegar a uno de los gimnasios** y enfrentar al **líder del gimnasio**.

El jugador es capaz de explorar y hacer las diferentes funciones que se presentan dentro del programa. Es recomendable que, a la hora de estar jugando, se utilice o se tenga el **mapa del juego a mano**, con tal de poder saber qué zonas poseen Mones, y cuáles no; no es crucial para la experiencia del juego.

Fue un trabajo colaborativo entre los miembros de este repositorio, cada uno aportó de distintas formas al proyecto, otorgando sus conocimientos individuales y trabajando todos de una manera ordenada en cada parte que nos decidimos.

<details>
<summary> <b> Mapa General del Juego </b> </summary>

<div align="center"> <br/>

![Mapa del Juego](Mapa%20Mon.png)

</div>
</details> 

## Requerimientos
- Tener un compilador de C (como **gcc**) instalado.
    - Para **Windows**, es recomendable utilizar **MinGW**.
    - Para sistemas basados en **Arch Linux**, se puede ejecutar en la terminal el comando `sudo pacman -S gcc` para instalarlo de los **paquetes oficiales** de Arch.
- Aunque no necesario, es recomendable tener instalado **Visual Studio Code** en sistemas **Windows** para poder compilar y ejecutar automáticamente el programa.

### Compilación y ejecución

El repositorio otorga unos archivos que permiten poder compilar y ejecutar el proyecto **de manera automática**. Es necesario **descargar el repositorio** y dirigirse a la carpeta del repositorio para poder probar el programa. 

Los archivos `mon_#.sh` se encargan de poder compilar el proyecto y ejecutarlo de manera instántanea.

En **Windows**:

- Es necesario poder tener **Visual Studio Code** y ejecutar el comando `bash mon_windows.sh` dentro de **Git Bash**.

En **Linux**:

- Para ejecutar el programa, solo es necesario escribir `bash mon_linux.sh` en la terminal de preferencia (no requiere **Visual Studio Code**).

<details>

---

<summary> Otras formas de compilación: </summary>

<details>
<summary>
Para compilarlo de forma manual, dentro del sistema operativo <b>Windows</b> usando <b>Visual Studio Code</b>: </summary>

1. Descargar este **repositorio** como archivo `zip` (haciendo **click** en la sección `code`, y apretando el botón de `Descargar ZIP`).
2. Abrir el **explorador de archivos** y navegar hasta el archivo `zip` para descomprimirlo. Una vez descomprimido, abrir el directorio con los archivos del programa.
3. Abrir el archivo `main.c` del `zip` en **Visual Studio Code**.
4. Dentro de **Visual Studio Code**, abrir el **terminal** y dirigirse a la dirección del repositorio, para poder compilar el programa.
5. Ejecutar el siguiente comando: `gcc *.c headers/code/*.c headers/TDAs/*.c -o main -lm`.
6. Abrir el archivo `programa.exe`, o escribir la línea `./programa.exe` en **Visual Studio Code** para ejecutarlo.
</details>

---
<details>
<summary>
Una forma alternativa de compilación, funcional para <b>Windows</b> y <b>Linux</b> (<b>Arch Linux</b> <i>testeado</i>): </summary>

1. Descargar el **repositorio** como archivo `zip`.
2. Abrir el **explorador de archivos** de su preferencia y navegar hasta encontrar el archivo `zip` para descomprimirlo.
3. Una vez descomprimido, buscar el directorio donde se ubica la carpeta descomprimida y buscar abrir el **terminal** en algunos de los sistemas operativos.

    - En **Windows**, se puede hacer **click derecho** en el directorio para abrir el menú de opciones y seleccionar para **abrir en Terminal**. Alternativamente, abrir **PowerShell** o **Línea de Comandos**, copiar la dirección del repositorio (la ruta `C:\Users\...`) y ejecutar el comando `cd "C:\Users\..."`.
    - En **Linux**, se puede ejecutar, dentro de la **terminal** de su preferencia, el comando `cd`, similar a como se hace en **Windows**. Es necesario copiar la dirección del repositorio (la ruta `"/home/$USER/..."`) y ejecutar el comando `cd "/home/$USER/..."` para acceder a la ruta del programa.
4. Ejecutar el comando, para **Linux**: `gcc *.c headers/code/*.c headers/TDAs/*.c -o main -lncurses -lm`, o, para **Windows**: `gcc *.c headers/code/*.c headers/TDAs/*.c -o main -lm`.
5. Escribir en la misma **terminal** el comando `./programa` / `./programa.exe` para ejecutar la aplicación.
</details>

</details>

## Funciones del programa

El programa incluye dos **menús esenciales** y nuestro **sistema de combate por turnos** que es utilizado para la carga del programa:

<details> 
<summary> <b> Menú Principal: </b> </summary>

1. `Jugar Partida`: El usuario va a ser capaz de iniciar una partida dentro del juego. El jugador será capaz de poder escoger **uno** de **tres Mones iniciales**, siendo **Iruiza (tipo Fuego)**, **Alimun (tipo Agua)**, o **Mecamon (tipo Planta)**. Al escoger un Mon inicial, iniciará en el **Pueblo IBC** dentro del mapa.
2. `Ingresar Nombre`: Permite al usuario poder escoger un nombre propio para su entrenador. Este permanecerá incluso aunque el jugador reinicie o salga de su partida.
3. `Salir del Programa`: Sale del programa.

</details>

---
<details>
<summary> <b> Menú del Jugador: </b> </summary>

El jugador podrá ver los datos de la ubicación en la que se encuentra, junto con su equipo y rutas disponibles hacia donde puede moverse.

Las opciones del jugador son:
1. `Moverse`: Permite poder mover el jugador utilizando las teclas **W (Norte), A (Oeste), S (Sur), D (Este)**. Al momento en que se mueve por el mundo, existe una probabilidad de poder luchar contra un **Mon** o **Entrenador** dependiendo del tipo de zona en el que se encuentre.
    - Si el jugador está en una zona con un **tipo definido** (Agua, Fuego, Planta), tendrá un 40% de probabilidad de **luchar contra un Mon salvaje**, el cuál **será un Mon del tipo de la zona**.
    - Si el jugador está en una zona sin **tipo definido**, tendrá un 20% de probabilidad de **luchar contra un Entrenador aleatorio**.
2. `Gestionar Mon`: Permite al jugador poder gestionar su equipo, permitiéndole poder cambiar de orden a sus mones, ver sus estadísticas actuales, o liberarlos.
3. `Ver Inventario`: Permite ver el inventario del jugador. Mostrará la lista de Objetos que posee y la cantidad de ellos.
4. `MonDex`: Diccionario global que permite poder acceder a la información de un Mon que se escoja.
5. `Entrar a CentroMon`: Función que permite poder entrar a los CentroMon que se ubiquen en el mapa. Estos no aparecen en todas las zonas, ya que solo están disponibles en las zonas de "Pueblo".
6. `Salir`: Sale de la partida actual.

</details>

---

<details>
<summary> <b> Sistema de Combate: </b> </summary>

El sistema de combate utilizado en el programa funciona en base **a turnos**.

</details>


### Headers Propios

Este proyecto posee headers segmentados, encargados cada uno de distintas **funciones** que se implementaron. Al ser algo más enrevesado, se mostrarán los **headers esenciales** creados:

- `Extras`: Encargado de funciones varias. Es la **cabeza de los headers**, implementando los **headers de C** (y **TDAs**) que más ocupamos (`stdio`, `stdlib` y `string`; `list` y `map`) para usarlos en **todos** los demás headers.
- `Mundo`: Es el header que se dedica a poder cargar el grafo utilizado para la **representación del mapa del juego**. Posee las estructuras que se usan en el `Entrenador`, una estructura propia (`Ubicacion`) para poder representar cada nodo del mapa. Se encarga también del **movimiento del jugador a lo largo del mapa**.
- `Mon`: Es la **cabeza general del programa**, ya que se encarga de la lógica interna de los **Mones** del juego. Es el que permite la **inicialización de los Mones** a través de su propio archivo, junto con implementar lo que es conocida en el programa como la **MonDex** (biblioteca para ver datos de los Mones), y la **lógica de aparición aleatoria** de un Mon salvaje al caminar.
- `Jugador`: Header encargado de funciones propias del jugador. Es el que otorga el **menú interactivo** donde el jugador verá sus opciones, junto con todo lo que viene de la **gestión del inventario** y su **inicialización**.
- `Entrenadores`: Header que carga a los **NPCs** que se utilizan en el juego para las batallas aleatorias en nodos sin tipo, o en gimnasios. Similar al header `Mon`, pero dedicado únicamente a los NPCs.
- `Tienda`: Header que implementa la **lógica utilizada para los CentroMon**. Es el que permite la **compra y venta de objetos** para el jugador.
- `Batalla`: Es el encargado de implementar la lógica de las **batallas contra Mones y Entrenadores**, permitiendo que se efectuen de manera correcta.
- `Prints`: Un header únicamente dedicado a poder **imprimir ciertos aspectos del juego**. Posee, también, una función que automatiza la impresión de opciones de un menú.

### Advertencias generales

- Hay ciertos **carácteres especiales** pertenecientes al estándar `UTF-8` que no son mostrados correctamente dentro del programa. Esto puede ser arreglado en **sistemas usando Windows** aplicando la configuración especial `Beta: Use Unicode UTF-8` ubicado en la sección de **opciones de idioma administrativo** (`Administrative language settings`) y **idioma para programas no-Unicode** (`Language for non-Unicode programs`).
- Puede que exista la posibilidad de que el programa **no muestre el coloreado del texto como debería** en sistemas operativos **Windows** (o terminales sin soporte directo a `ANSI`). Esto puede ser **solucionado** si se utiliza la **Terminal de Windows** o una versión moderna de **PowerShell** (con soporte directo), o implementar la compatibilidad con colores `ANSI` en las demás terminales. También, puede ser solucionado usando `Git Bash`, ya que soporta los colores `ANSI` sin problemas.

- Al momento de `leer una opción`, el programa leerá el **primer cáracter ingresado**. Si se **agregan más carácteres** después de eso no se considerará como un "problema", pero **tampoco se leerán**.
- Si se **supera** el límite de la entrada del usuario (**200 carácteres máximo**), el programa no podrá interpretar más allá de lo que se ingrese.

## Ejemplos de ejecución