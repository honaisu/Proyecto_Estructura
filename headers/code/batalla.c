#include "../batalla.h"
#include "../jugador.h"
#include "../prints.h"
#include <math.h>

void menu_batalla_salvaje(){
    const char* opciones[] = {"Atacar", "Defender", "Usar Objeto", "Huir", "Cambiar de Mon"};
    imprimir_menu("", opciones, 4);
}

void estado_batalla(Mon *mon_jugador, Mon* mon_salvaje){
    printf("Mon: %s, vida: %d, tipo: %s\n", mon_jugador->nombre, mon_jugador->hp_actual, mon_jugador->tipo) ;
    printf("Mon salvaje: %s, vida: %d, tipo: %s\n", mon_salvaje->nombre, mon_salvaje->hp_actual, mon_salvaje->tipo) ;
}

void estado_batalla_entrenador(Entrenador *jugador, Entrenador *rival, Mon *mon_jugador, Mon *mon_rival){
    printf("Tu: %s ---- Mon: %s, vida: %d, tipo: %s\n",jugador->nombre, mon_jugador->nombre, mon_jugador->hp_actual, mon_jugador->tipo) ;
    printf("Rival: %s ---- Mon: %s, vida: %d, tipo: %s\n",rival->nombre, mon_rival->nombre, mon_rival->hp_actual, mon_rival->tipo) ;
}

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


Mon* obtener_primer_mon_vivo(List *equipo) {
    Mon *actual = list_first(equipo);
    while (actual) {
        if (!actual->is_dead && actual->hp_actual > 0) return actual;
        actual = list_next(equipo);
    }
    return NULL;
}

Mon* seleccionar_mon_muerto(List *equipo) {
    Mon *actual = list_first(equipo);
    int index = 1;
    Mon *muertos[10];
    int total_muertos = 0;

    printf("Elige un Mon muerto para revivir:\n");
    while (actual) {
        if (actual->is_dead || actual->hp_actual <= 0) {
            printf("%d. %s (HP: %d)\n", total_muertos + 1, actual->apodo, actual->hp_actual);
            muertos[total_muertos++] = actual;
        }
        actual = list_next(equipo);
    }

    if (total_muertos == 0) {
        printf("No tienes Mons muertos.\n");
        esperar_enter();
        return NULL;
    }

    int opcion;
    printf("Número: ");
    scanf("%d", &opcion);
    getchar(); // limpiar '\n'
    if (opcion < 1 || opcion > total_muertos) return NULL;

    return muertos[opcion - 1];
}

int batalla_pokemon_salvaje(Entrenador *jugador, Mon *mon_salvaje) {
    List *equipo = jugador->equipo_mon;
    Mon *mon_batalla = obtener_primer_mon_vivo(equipo);
    bool jugador_en_pie = true;
    int dano_recibido;
    float defensa_mon = 1;
    float defensa_salvaje = 1;
    float MC;
    int obj_ocupado = 1;
    char tecla;

    while (true) {
        float ef_mon_jugador = conseguir_efectividad(mon_batalla, mon_salvaje);
        float ef_mon_salvaje = conseguir_efectividad(mon_salvaje, mon_batalla);
        int opcion_valida = 0;

        while (!opcion_valida) {
            limpiar_pantalla();
            estado_batalla(mon_batalla, mon_salvaje);
            menu_batalla_salvaje();

            #ifdef _WIN32
                tecla = getch();
            #else
                initscr();
                noecho();
                cbreak();
                tecla = getch();
                endwin();
            #endif

            switch (tecla) {
                case '1': {
                    MC = (rand() % 100 + 1 <= 10) ? 1.5 : 1.0;
                    dano_recibido = ceil((mon_batalla->damage_actual * ef_mon_jugador * MC) - (mon_salvaje->defense_actual * defensa_salvaje));
                    if (dano_recibido <= 0) dano_recibido = 1;
                    mon_salvaje->hp_actual -= dano_recibido;
                    printf("%s le quita %d de vida a %s \n", mon_batalla->apodo, dano_recibido, mon_salvaje->apodo);
                    esperar_enter();
                    opcion_valida = 1;
                    break;
                }
                case '2': {
                    defensa_mon = (rand() % 100 + 1 <= 50) ? 1.5 : defensa_mon;
                    printf("%s aumento su defensa\n", mon_batalla->apodo);
                    esperar_enter();
                    opcion_valida = 1;
                    break;
                }
                case '3': {
                    Objeto *obj = gestionar_inventario(jugador);
                    if (obj != NULL) {
                        if (obj->cantidad == 0) {
                            printf("No tienes ese objeto...\n");
                            esperar_enter();
                            break;
                        }
                        if (!strcmp(obj->nombre, "MonBall")) {
                            float PC = ((float)(mon_salvaje->stats_base.hp_base - mon_salvaje->hp_actual) / mon_salvaje->stats_base.hp_base) * 100;
                            float tiro = rand() % 100 + 1;
                            if (tiro <= PC) {
                                list_pushBack(jugador->equipo_mon, mon_salvaje);
                                printf("%s ha sido capturado! \n", mon_salvaje->apodo);
                                
                                return 2;
                            } else {
                                printf("Has fallado el tiro... \n");
                                esperar_enter();
                            }
                            obj->cantidad -= 1;
                            opcion_valida = 1;
                        } else if (!strcmp(obj->nombre, "Pocion")) {
                            mon_batalla->hp_actual += 4;
                            if (mon_batalla->hp_actual > mon_batalla->stats_base.hp_base)
                                mon_batalla->hp_actual = mon_batalla->stats_base.hp_base;
                            printf("%s recupera algo de vida! \n", mon_batalla->apodo);
                            obj->cantidad -= 1 ;
                            esperar_enter();
                            opcion_valida = 1;
                        } else if (!strcmp(obj->nombre, "Revivir")) {
                            Mon *muerto = seleccionar_mon_muerto(equipo);
                            if (muerto) {
                                muerto->hp_actual = muerto->stats_base.hp_base / 2;
                                muerto->is_dead = false;
                                printf("%s ha sido revivido con mitad de vida!\n", muerto->apodo);
                                obj->cantidad -= 1;
                            } else {
                                printf("No fue posible revivir ningún Mon.\n");
                            }
                            esperar_enter();
                            opcion_valida = 1;
                        }
                    }
                    break;
                }
                case '4': {
                    printf("Huyes de la batalla \n");
                    free(mon_salvaje);
                    return 2;
                }
                default: {
                    printf("Opcion no valida \n");
                    esperar_enter();
                    break;
                }
            }
        }

        if (mon_salvaje->hp_actual <= 0) {
            printf("Mon fue derrotado! \n");
            free(mon_salvaje);
            return 1;
        }

        MC = (rand() % 100 + 1 <= 10) ? 1.5 : 1.0;
        dano_recibido = ceil((mon_salvaje->damage_actual * ef_mon_salvaje * MC) - (mon_batalla->defense_actual * defensa_mon));
        if (dano_recibido <= 0) dano_recibido = 1;
        mon_batalla->hp_actual -= dano_recibido;

        printf("%s le quita %d de vida a %s \n", mon_salvaje->apodo, dano_recibido, mon_batalla->apodo);

        if (mon_batalla->hp_actual <= 0) {
            mon_batalla->hp_actual = 0;
            mon_batalla->is_dead = true;
            printf("%s ha sido derrotado...\n", mon_batalla->apodo);

            mon_batalla = obtener_primer_mon_vivo(equipo);
            if (mon_batalla != NULL) {
                printf("\nSacas al combate a %s \n", mon_batalla->apodo);
            } else {
                printf("No te quedan más Mon, has perdido... \n");
                jugador->vivo = false;
                free(mon_salvaje);
                return 0;
            }
        }

        esperar_enter();
    }

    return 0;
}

// retorna 1 si gana la batalla, retorna 0 si la pierde.
int batalla_entrenador(Entrenador *jugador, Entrenador *rival){
    Mon *mon_jugador = obtener_primer_mon_vivo(jugador->equipo_mon) ;
    Mon *mon_rival = obtener_primer_mon_vivo(rival->equipo_mon) ;
    List *equipo_jugador = jugador->equipo_mon ;
    List *equipo_rival = rival->equipo_mon ;
    bool jugador_en_pie = true ;
    int dano_recibido ; 
    float defensa_mon = 1 ;
    float defensa_rival = 1 ;
    float MC ;
    char tecla ;
     while (1) {
        float ef_mon_jugador = conseguir_efectividad(mon_jugador, mon_rival);
        float ef_mon_salvaje = conseguir_efectividad(mon_rival, mon_jugador);
        int opcion_valida = 0;

        while (!opcion_valida) {
            limpiar_pantalla();
            estado_batalla_entrenador(jugador, rival, mon_jugador, mon_rival);
            menu_batalla_salvaje();

            #ifdef _WIN32
                tecla = getch();
            #else
                initscr();
                noecho();
                cbreak();
                tecla = getch();
                endwin();
            #endif

            switch (tecla) {
                case '1': {
                    MC = (rand() % 100 + 1 <= 10) ? 1.5 : 1.0;
                    dano_recibido = ceil((mon_jugador->damage_actual * ef_mon_jugador * MC) - (mon_rival->defense_actual * defensa_rival));
                    if (dano_recibido <= 0) dano_recibido = 1;
                    mon_rival->hp_actual -= dano_recibido;
                    printf("%s le quita %d de vida a %s \n", mon_jugador->apodo, dano_recibido, mon_rival->apodo);
                    esperar_enter();
                    opcion_valida = 1;
                    break;
                }
                case '2': {
                    defensa_mon = (rand() % 100 + 1 <= 50) ? 1.5 : defensa_mon;
                    printf("%s aumento su defensa\n", mon_jugador->apodo);
                    esperar_enter();
                    opcion_valida = 1;
                    break;
                }
                case '3': {
                    Objeto *obj = gestionar_inventario(jugador);
                    if (obj != NULL) {
                        if (obj->cantidad == 0) {
                            printf("No tienes ese objeto...\n");
                            esperar_enter();
                            break;
                        }
                        if (!strcmp(obj->nombre, "MonBall")) {
                            printf("No puedes usar una MonBall en una pelea contra otro entrenador! \n") ;
                            printf("Eso seria un robo... \n") ;
                            esperar_enter() ;
                        } else if (!strcmp(obj->nombre, "Pocion")) {
                            mon_jugador->hp_actual += 4;
                            if (mon_jugador->hp_actual > mon_jugador->stats_base.hp_base)
                                mon_jugador->hp_actual = mon_jugador->stats_base.hp_base;
                            printf("%s recupera algo de vida! \n", mon_jugador->apodo);
                            obj->cantidad -= 1 ;
                            esperar_enter();
                            opcion_valida = 1;
                        } else if (!strcmp(obj->nombre, "Revivir")) {
                            Mon *muerto = seleccionar_mon_muerto(equipo_jugador);
                            if (muerto) {
                                muerto->hp_actual = muerto->stats_base.hp_base / 2;
                                muerto->is_dead = false;
                                printf("%s ha sido revivido con mitad de vida!\n", muerto->apodo);
                                obj->cantidad -= 1;
                            } else {
                                printf("No fue posible revivir ningún Mon.\n");
                            }
                            esperar_enter();
                            opcion_valida = 1;
                        }
                    }
                    break;
                }
                case '4': {
                    printf("No puedes huir contra un entrenador! \n");
                    esperar_enter() ;
                    break ;
                }
                default: {
                    printf("Opcion no valida \n");
                    esperar_enter();
                    break;
                }
            }
        }

        if (mon_rival->hp_actual <= 0) {
            printf("%s fue derrotado! \n", mon_rival->apodo);
            mon_rival = obtener_primer_mon_vivo(equipo_rival) ;
            if (mon_rival != NULL){
                printf("%s saca a %s !", rival->nombre, mon_rival->nombre) ;
                esperar_enter() ;
                continue ;
            }
            else {
                printf("Has ganado!\n") ;
                printf("Obtienes 200$\n") ;
                jugador->dinero += 200 ;
                return 1;
            }
        }

        MC = (rand() % 100 + 1 <= 10) ? 1.5 : 1.0;
        dano_recibido = ceil((mon_rival->damage_actual * ef_mon_salvaje * MC) - (mon_jugador->defense_actual * defensa_mon));
        if (dano_recibido <= 0) dano_recibido = 1;
        mon_jugador->hp_actual -= dano_recibido;

        printf("%s le quita %d de vida a %s \n", mon_rival->apodo, dano_recibido, mon_rival->apodo);

        if (mon_jugador->hp_actual <= 0) {
            mon_jugador->hp_actual = 0;
            mon_jugador->is_dead = true;
            printf("%s ha sido derrotado...\n", mon_jugador->apodo);

            mon_jugador = obtener_primer_mon_vivo(equipo_jugador);
            if (mon_jugador != NULL) {
                printf("\nSacas al combate a %s \n", mon_jugador->apodo);
            } else {
                printf("No te quedan más Mon, has perdido... \n");
                return 0;
            }
        }

        esperar_enter();
    }

    return 0;
}

