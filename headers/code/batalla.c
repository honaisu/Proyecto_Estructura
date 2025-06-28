#include "../batalla.h"
#include "../jugador.h"
#include "../prints.h"
#include <math.h>

// Comprueba si es batalla contra entrenador o no
bool es_entrenador = false;

// Imprime el menu que se ve en la batalla
void menu_batalla(void){
    int cant_opciones = 3;
    const char* opciones[] = {"Atacar", "Defender", "Usar Objeto", "Huir"};
    if (!es_entrenador) ++cant_opciones;
    imprimir_menu("", opciones, cant_opciones);
}

// Muestra el estado de batalla contra un mon salvaje
// Imprime los mones, su vida y su tipo
void estado_batalla(Mon *mon_jugador, Mon* mon_salvaje){
    printf("Mon: " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET ", vida: %d, tipo: %s\n", mon_jugador->nombre, mon_jugador->hp_actual, mon_jugador->tipo) ;
    printf("Mon salvaje: " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET ", vida: %d, tipo: %s\n", mon_salvaje->nombre, mon_salvaje->hp_actual, mon_salvaje->tipo) ;
}

// Muestra el estado de batalla contra un entrenador
// Muestra los entrenadores, sus mones y los datos de los mones (vida y tipo)
void estado_batalla_entrenador(Entrenador *jugador, Entrenador *rival, Mon *mon_jugador, Mon *mon_rival){
    printf("Tu: " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET 
        "  ---- Mon: " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET 
        ", vida: %d, tipo: %s\n",jugador->nombre, mon_jugador->nombre, mon_jugador->hp_actual, mon_jugador->tipo) ;
    printf("Rival: " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET 
        " ---- Mon: " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET 
        ", vida: %d, tipo: %s\n",rival->nombre, mon_rival->nombre, mon_rival->hp_actual, mon_rival->tipo) ;
}

// Consigue la efectividad que se usa para la batalla
// Esta esta basada en el sistema de cachipun
// Fuego -> Planta -> Agua -> Fuego
// Si no es efectivo, se le quita multiplicador
float conseguir_efectividad(Mon *mon_atacante, Mon *mon_en_contra){
    const char* tipos[3] = {"PLANTA", "FUEGO", "AGUA"};
    // Tabla de efectividad
    //  PLANTA  FUEGO  AGUA
    float tabla[3][3] = {
        {1.0,   0.75,  1.25}, // PLANTA
        {1.25,  1.0,   0.75}, // FUEGO
        {0.75,  1.25,  1.0 }  // AGUA
    };

    int pos_atacante = -1, pos_contra = -1;
    for (int i = 0; i < 3; i++) {
        if (strcmp(mon_atacante->tipo, tipos[i]) == 0) pos_atacante = i;
        if (strcmp(mon_en_contra->tipo, tipos[i]) == 0) pos_contra = i;
    }
    if (pos_atacante == -1 || pos_contra == -1) return 1.0; // Tipo desconocido

    return tabla[pos_atacante][pos_contra];
}

// Obtiene el primer mon vivo de la lista del jugador
Mon* obtener_primer_mon_vivo(List *equipo) {
    Mon *actual = list_first(equipo);
    while (actual) {
        if (!actual->is_dead && actual->hp_actual > 0) return actual;
        actual = list_next(equipo);
    }
    return NULL;
}

// Selecciona un mon muerto dentro del equipo del jugador para poder darle un revivir
Mon* seleccionar_mon_muerto(List *equipo) {
    Mon *actual = list_first(equipo);
    int index = 1;
    Mon *muertos[10];
    int total_muertos = 0;

    puts("Elige un Mon muerto para revivir:");
    while (actual) {
        if (actual->is_dead || actual->hp_actual <= 0) {
            printf("%d. %s (HP: %d)\n", total_muertos + 1, actual->apodo, actual->hp_actual);
            muertos[total_muertos++] = actual;
        }
        actual = list_next(equipo);
    }

    if (total_muertos == 0) {
        puts("No tienes Mons muertos.");
        esperar_enter();
        return NULL;
    }

    int opcion;
    printf("Elige un número: ");
    scanf("%d", &opcion);
    getchar(); // limpiar '\n'
    if (opcion < 1 || opcion > total_muertos) return NULL;

    return muertos[opcion - 1];
}

// Booleano que se encarga de poder verificar si se logro atrapar un Mon o no
bool atrapar_mon(Entrenador* jugador, Mon* mon_salvaje) {
    // Probabilidad de Captura
    float PC = ((float)(mon_salvaje->stats_base.hp_base - mon_salvaje->hp_actual) / mon_salvaje->stats_base.hp_base) * 100;
    // Tiro, valor aleatorio que es usado para darle más sazon al intentar capturar
    float tiro = rand() % 100 + 1;
    if (tiro <= PC) {
        list_pushBack(jugador->equipo_mon, mon_salvaje);
        printf("%s ha sido capturado! \n", mon_salvaje->apodo);
        return true;
    } else {
        puts("Has fallado el tiro...");
    }
    return false;
}

// Imprime el daño que le hace un mon a otro en batalla
void imprimir_dano(Mon* mon_batalla, Mon* mon_contrario, int dano_recibido) {
    printf(ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET 
    " le quita %d de vida a " 
    ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET "\n", mon_batalla->apodo, dano_recibido, mon_contrario->apodo);
}

// Usa un item del jugador en medio de la pelea
// Retorna:
// 0 -> Se ocupo item
// 1 -> Se ve si es una captura
// 2 -> No hizo nada
int usar_item(Entrenador* jugador, Mon* mon_batalla, Mon* mon_contrario) {
    Objeto *obj = gestionar_inventario(jugador);
    if (obj == NULL) return 2;

    if (obj->cantidad == 0) {
        puts("No tienes ese objeto...");
        return 2;
    }
    switch (*obj->nombre) { // Toma el primer caracter del objeto a usar
        case 'M': // MonBall
            if (es_entrenador) {
                puts("No puedes usar una MonBall en una pelea contra otro entrenador!") ;
                puts("Eso seria un robo...") ;
                return 2;
            }
            if (atrapar_mon(jugador, mon_contrario)) { --obj->cantidad; return 1; }
            else { --obj->cantidad; }
            break;
        case 'P': { // Pocion
            mon_batalla->hp_actual += 4;
            if (mon_batalla->hp_actual > mon_batalla->stats_base.hp_base)
                mon_batalla->hp_actual = mon_batalla->stats_base.hp_base;
            printf("%s recupera algo de vida! \n", mon_batalla->apodo);
            --obj->cantidad;
            break;
        }
        case 'R': { // Revivir
            Mon *muerto = seleccionar_mon_muerto(jugador->equipo_mon);
            if (muerto) {
                muerto->hp_actual = muerto->stats_base.hp_base / 2;
                muerto->is_dead = false;
                printf("%s ha sido revivido con mitad de vida!\n", muerto->apodo);
            } else {
                puts("No fue posible revivir ningún Mon.");
                return 2 ;
            }
            --obj->cantidad;
            break;
        }
    }
    return 0;
}

// Daño efectuado a un mon sobre otro
// Este esta basado en la formula que se ocupa para el daño
// Posee un multiplicador que otorga un beneficio de 1.5x
int efectuar_dano(Mon* mon_batalla, Mon* mon_contrario, float ef, float defensa) {
    int dano_recibido;
    float MC = (rand() % 100 + 1 <= 10) ? 1.5 : 1.0;
    if (MC == 1.5) puts("Crítico!!!");
    dano_recibido = ceil((mon_batalla->stats_base.damage_base * ef * MC) - (mon_batalla->stats_base.defense_base * defensa));
    if (dano_recibido <= 0) dano_recibido = 1;
    mon_contrario->hp_actual -= dano_recibido;
    return dano_recibido;
}

// Aplica la defensa a un mon en batalla
void aplicar_defensa(float* defensa_mon, Mon* mon_batalla) {
    *defensa_mon = (rand() % 100 + 1 <= 50) ? 1.5 : 1.0;
    if (*defensa_mon == 1.0) {
        printf("%s no logró aumentar la defensa!\n", mon_batalla->apodo);
    } else printf("%s aumentó su defensa\n", mon_batalla->apodo);
}

// Verifica si los mones del jugador estan vivos o no
// Retorna true si estan vivos y false si no
bool verificar_vivos(Mon** mon_batalla, List* equipo) {
    if ((*mon_batalla)->hp_actual > 0) return true;
    (*mon_batalla)->hp_actual = 0;
    (*mon_batalla)->is_dead = true;
    printf("%s ha sido derrotado...\n", (*mon_batalla)->apodo);
    esperar_enter();

    *mon_batalla = obtener_primer_mon_vivo(equipo);
    if (*mon_batalla != NULL) {
        printf("\nSacas al combate a %s \n", (*mon_batalla)->apodo);
    } else {
        puts("No te quedan más Mon, has perdido...");
        return false;
    }
    esperar_enter() ;
    return true;
}

// Batalla mon salvaje
// Muestra el menu de opciones que utiliza el jugador para batallar contra este mon
// 0 -> Perdiste
// 1 -> Gano
// 2 -> Capturo
int batalla_pokemon_salvaje(Entrenador *jugador, Mon *mon_salvaje) {
    List *equipo = jugador->equipo_mon;
    Mon *mon_batalla = obtener_primer_mon_vivo(equipo);
    if (mon_batalla == NULL) { puts("No tienes un Mon vivo!!!"); return 0; }

    es_entrenador = false;
    int dano_recibido;
    float defensa_mon = 1.0;
    float ef_mon_jugador = conseguir_efectividad(mon_batalla, mon_salvaje);;
    float ef_mon_salvaje = conseguir_efectividad(mon_salvaje, mon_batalla);;
    int obj_ocupado = 1;
    char tecla;
    int pre_vida ;
    int post_vida ;
    bool opcion_valida = false;
    while (true) {
        defensa_mon = 1.0;
        opcion_valida = false;
        do {
            limpiar_pantalla();
            estado_batalla(mon_batalla, mon_salvaje);
            menu_batalla();
            tecla = '\0';
            esperar_tecla(&tecla);
            switch (tecla) {
                case '1': { // Atacar
                    pre_vida = mon_salvaje->hp_actual ;
                    dano_recibido = efectuar_dano(mon_batalla, mon_salvaje, ef_mon_jugador, defensa_mon);
                    post_vida = pre_vida - mon_salvaje->hp_actual ;
                    imprimir_dano(mon_batalla, mon_salvaje, post_vida);
                    opcion_valida = true;
                    break;
                }
                case '2': { // Defender
                    aplicar_defensa(&defensa_mon, mon_batalla);
                    opcion_valida = true;
                    break;
                }
                case '3': { // Usar objeto
                    // En caso que se capture al Mon (Bool) retorna 2
                    int item = usar_item(jugador, mon_batalla, mon_salvaje) ;
                    if (item == 1) return 2 ; // Se ve si es captura
                    else if (item == 2) break ; // No hace nada
                    opcion_valida = true;
                    break;
                }
                case '4': { // Huir
                    puts("Huyes de la batalla...");
                    free(mon_salvaje);
                    return 2;
                }
                default: {
                    puts("Opción no válida.");
                    break;
                }
            }
        } while(!opcion_valida);
        esperar_enter();

        // Comprobar si muere el mon salvaje o no.
        if (mon_salvaje->hp_actual <= 0) {
            puts("El Mon salvaje fue derrotado!");
            free(mon_salvaje);
            return 1;
        }

        // ATAQUE DEL MON SALVAJE
        pre_vida = mon_batalla->hp_actual ;
        dano_recibido = efectuar_dano(mon_salvaje, mon_batalla, ef_mon_salvaje, defensa_mon);
        post_vida = pre_vida - mon_batalla->hp_actual ;
        imprimir_dano(mon_salvaje, mon_batalla, post_vida);
        esperar_enter();

        // Verificar vivos del jugador
        if (verificar_vivos(&mon_batalla, equipo)) { 
            continue;
        } else {
            jugador->vivo = false;
            free(mon_salvaje);
            return 0;
        }
    }

    return 0;
}

// Batalla contra un entrenador
// Es la que muestra el menú que se usa para el combate contra un entrenador
// Retorna true si gana la batalla, retorna false si la pierde.
int batalla_entrenador(Entrenador *jugador, Entrenador *rival){
    es_entrenador = true;
    Mon *mon_jugador = obtener_primer_mon_vivo(jugador->equipo_mon) ;
    if (mon_jugador == NULL) { puts("No tienes un Mon vivo!!!"); return 0; }

    Mon *mon_rival = obtener_primer_mon_vivo(rival->equipo_mon) ;
    List *equipo_jugador = jugador->equipo_mon ;
    List *equipo_rival = rival->equipo_mon ;
    bool jugador_en_pie = true ;
    int dano_recibido ; 
    float defensa_mon = 1 ;
    float defensa_rival = 1 ;
    float ef_mon_jugador = conseguir_efectividad(mon_jugador, mon_rival);;
    float ef_mon_rival = conseguir_efectividad(mon_rival, mon_jugador);;
    int pre_vida ;
    int post_vida ;
    bool opcion_valida = false;

    char tecla ;
    while (true) {
        defensa_mon = 1.0;
        opcion_valida = false;
        do {
            limpiar_pantalla();
            estado_batalla_entrenador(jugador, rival, mon_jugador, mon_rival);
            menu_batalla();
            tecla = '\0';
            esperar_tecla(&tecla);
            switch (tecla) {
                case '1': { // Atacar
                    pre_vida = mon_rival->hp_actual ;
                    dano_recibido = efectuar_dano(mon_jugador, mon_rival, ef_mon_jugador, defensa_rival);
                    post_vida = pre_vida - mon_rival->hp_actual ;
                    imprimir_dano(mon_jugador, mon_rival, post_vida);
                    opcion_valida = true;
                    break;
                }
                case '2': { // Defender
                    aplicar_defensa(&defensa_mon, mon_jugador);
                    opcion_valida = true;
                    break;
                }
                case '3': { // Usar item
                    int item = usar_item(jugador, mon_jugador, mon_rival) ;
                    if (item == 2) break ;
                    opcion_valida = true;
                    break;   
                }
                default: {
                    puts("Opción no válida.");
                    break;
                }
            }
        } while (!opcion_valida);
        esperar_enter();
        defensa_rival = 1.0;

        // Verifica si el mon rival fue derrotado o no.
        // Si fue derrotado, y en caso de que el rival tenga mas, saca a otro mon.
        // Si no, el jugador gana la batalla
        if (mon_rival->hp_actual <= 0) {
            printf("%s fue derrotado! \n", mon_rival->apodo);
            mon_rival = obtener_primer_mon_vivo(equipo_rival) ;
            if (mon_rival != NULL){
                printf("%s saca a " ANSI_COLOR_WHITE "%s" ANSI_COLOR_RESET "!\n", rival->nombre, mon_rival->nombre) ;
                esperar_enter() ;
                continue ;
            } else {
                printf("Le has ganado a %s!\n", rival->nombre) ;
                free(mon_rival);
                free(rival);
                return true;
            }
        }
        
        // Una probabilidad chica de que haga defensa.
        if (rand() % 100 <= 30) {
            aplicar_defensa(&defensa_rival, mon_rival);
        } else {
            // ATAQUE RIVAL
            pre_vida = mon_jugador->hp_actual ;
            dano_recibido = efectuar_dano(mon_rival, mon_jugador, ef_mon_rival, defensa_mon);
            post_vida = pre_vida - mon_jugador->hp_actual ;
            imprimir_dano(mon_rival, mon_jugador, post_vida);
        }

        esperar_enter();

        // Verifica los mones vivos del jugador
        if (verificar_vivos(&mon_jugador, jugador->equipo_mon)) continue; 
        else { 
            jugador->vivo = false; 
            free(mon_rival);
            free(rival);
            return false; 
        }
    }

    return false;
}