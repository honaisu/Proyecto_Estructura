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

    while (1) {
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
                                obj->cantidad -= 1;
                                esperar_enter();
                                return 2;
                            } else {
                                printf("Has fallado el tiro... \n");
                                esperar_enter();
                            }
                            opcion_valida = 1;
                        } else if (!strcmp(obj->nombre, "Pocion")) {
                            mon_batalla->hp_actual += 4;
                            if (mon_batalla->hp_actual > mon_batalla->stats_base.hp_base)
                                mon_batalla->hp_actual = mon_batalla->stats_base.hp_base;
                            printf("%s recupera algo de vida! \n", mon_batalla->apodo);
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
                free(mon_salvaje);
                return 0;
            }
        }

        esperar_enter();
    }

    return 0;
}

// retorna 1 si gana la batalla, retorna 2 si la pierde.
int batalla_entrenador(Entrenador *jugador, Entrenador *rival){
    Mon *mon_jugador = list_first(jugador->equipo_mon) ;
    Mon *mon_rival = list_first(rival->equipo_mon) ;
    bool jugador_en_pie = true ;
    int dano_recibido ; 
    float defensa_mon = 1 ;
    float defensa_rival = 1 ;
    float MC ;
    while (1){
        float ef_mon_jugador = conseguir_efectividad(mon_jugador, mon_rival) ;
        float ef_mon_salvaje = conseguir_efectividad(mon_rival, mon_jugador) ;
        estado_batalla(mon_jugador, mon_rival) ;
        menu_batalla_salvaje() ;
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
        int obj_ocupado ;
        do 
            switch(tecla){
                case 1: {
                    if (rand() % 100 + 1 <= 10) MC = 1.5 ;
                    else MC = 1.0 ;
                    dano_recibido = ceil((mon_rival->damage_actual * ef_mon_jugador * MC) - (mon_rival->defense_actual * defensa_rival)) ;
                    mon_rival->hp_actual -= dano_recibido ; // Falta factor random de 0.9-1.1
                    printf("%s le quita %d de vida a %s \n", mon_jugador->apodo, dano_recibido, mon_rival->apodo) ;
                    esperar_enter() ;
                    break ;
                }
                case 2 : {
                    if (rand() % 100 + 1 <= 50) defensa_mon = 1.5 ; 
                    // se podria hacer que la defensa solo dure una cantidad de turnos...
                    break ;
                }
                case 3 : {
                    Objeto * obj = gestionar_inventario(jugador) ;
                    
                    if (obj != NULL) {
                        if (obj->cantidad == 0) {
                            printf("No tienes ese objeto...") ;
                            break ;
                        }
                        if (!strcmp(obj->nombre, "MonBall")) {
                            printf("No puedes usar una MonBall en una batalla contra otro entrenador! \n") ;
                            printf("Eso seria robo... \n") ;
                            break ;
                        }
                        else if (!strcmp(obj->nombre, "Pocion")){
                            mon_jugador->hp_actual += 4 ;
                            if (mon_jugador->hp_actual > mon_jugador->stats_base.hp_base < mon_jugador->hp_actual){
                            }
                            printf("%s recupera algo de vida! \n", mon_jugador->apodo) ;
                        }
                        obj_ocupado = 0 ;
                        esperar_enter() ;
                    }
                    break ;
                }
                case 4 :
                    // va a huir si o si? se podria implementar que sea una probabilidad como en los juegos
                    break ;
                default :
                    printf("Opcion no valida \n") ;

            
        }
        while (tecla != '1' && tecla != '2' && tecla !='4' && obj_ocupado) ;
        if (tecla == '4'){
            printf("Huyes de la batalla \n") ;
            break ;

        } 
        if (mon_rival->hp_actual <= 0){
            mon_rival = list_next(rival->equipo_mon) ;
            if (mon_rival != NULL){
                printf("%s saca a %s", rival, mon_rival) ;
            }
            else {
                printf("%s fue derrotado! \n", rival) ; // falta el nombre del rival
                return 1 ;
            }
            
            esperar_enter() ;
        }
        if (rand() % 100 + 1 <= 10) MC = 1.5 ;
        else MC = 1.0 ;
        dano_recibido = (mon_rival->damage_actual * ef_mon_salvaje * MC) - (mon_jugador->defense_actual * defensa_mon) ;
        mon_jugador->hp_actual -= dano_recibido ; // Falta factor random de 0.9-1.1
        printf("%s le quita %d de vida a %s \n", mon_rival->apodo, dano_recibido, mon_jugador) ;
        if (mon_jugador->hp_actual <= 0) {
            mon_jugador = list_next(jugador->equipo_mon) ;
            printf("%s ha sido derrotado...\n") ;
            if (mon_jugador != NULL){
                printf("Sacas al combate a %s", mon_jugador->apodo) ; 
            }
            else {
                printf("No te quedan mas mon, has perdido...") ;
                return 0 ;
            }
        }
        esperar_enter() ;
        
    }

    return 0 ;
}


