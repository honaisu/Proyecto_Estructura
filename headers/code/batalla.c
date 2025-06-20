#include "../batalla.h"
#include "../jugador.h"
#include "../prints.h"
#include <math.h>

void menu_batalla_salvaje(){
    const char* opciones[] = {"Atacar", "Defender", "Usar Objeto", "Huir"};
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
/*
int efectuar_ataque(Mon* mon_jugador, Mon* mon_salvaje) {
    float ef_mon_jugador = conseguir_efectividad(mon_jugador, mon_salvaje) ;
    float ef_mon_salvaje = conseguir_efectividad(mon_salvaje, mon_jugador) ;
    float MC = 0;
    int dano_recibido = 0; 

    if (rand() % 100 + 1 <= 10) MC = 1.5 ;
    else MC = 1.0 ;
    
    dano_recibido = (mon_salvaje->damage_actual * ef_mon_jugador * MC) - mon_salvaje->defense_actual;
    mon_salvaje->hp_actual -= dano_recibido ; // Falta factor random de 0.9-1.1
    printf("%s le quita %d de vida a %s\n", mon_jugador->apodo, dano_recibido, mon_salvaje->nombre) ;
    
    if (mon_salvaje->hp_actual <= 0) {
        printf("Ganaste!");
        return 1;
    } else if (mon_jugador->hp_actual <= 0) {
        printf("Perdiste");
        return 0;
    }

}*/

#define MON_VIVO(mon) mon->hp_actual > 0

int batalla_pokemon_salvaje(Entrenador *jugador, Mon *mon_salvaje){
    List *equipo = jugador->equipo_mon ;
    Mon *mon_batalla = list_first(equipo) ;
    bool jugador_en_pie = true ;
    int dano_recibido ; 
    float defensa_mon = 1 ;
    float defensa_salvaje = 1 ;
    float MC ;
    int obj_ocupado = 1 ;
    while (mon_batalla->hp_actual > 0){
        limpiar_pantalla() ;
        float ef_mon_jugador = conseguir_efectividad(mon_batalla, mon_salvaje) ;
        float ef_mon_salvaje = conseguir_efectividad(mon_salvaje, mon_batalla) ;
        estado_batalla(mon_batalla, mon_salvaje) ;
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
        do 
            switch(tecla){
                case '1': {
                    
                    if (rand() % 100 + 1 <= 10) MC = 1.5 ;
                    else MC = 1.0 ;
                    dano_recibido = ceil((mon_salvaje->damage_actual * ef_mon_jugador * MC) - (mon_salvaje->defense_actual * defensa_salvaje)) ;
                    mon_salvaje->hp_actual -= dano_recibido ; // Falta factor random de 0.9-1.1
                    printf("%s le quita %d de vida a %s \n", mon_batalla->apodo, dano_recibido, mon_salvaje->apodo) ;
                    esperar_enter() ;
                    break ;
                }
                case '2' : {
                    if (rand() % 100 + 1 <= 50) defensa_mon = 0.5 ; 
                    // se podria hacer que la defensa solo dure una cantidad de turnos...
                    break ;
                }
                case '3' : {
                    Objeto * obj = gestionar_inventario(jugador) ;
                    
                    if (obj != NULL) {
                        if (obj->cantidad == 0) {
                            printf("No tienes ese objeto...") ;
                            break ;
                        }
                        if (!strcmp(obj->nombre, "MonBall")) {
                            float PC = ((float)(mon_salvaje->stats_base.hp_base - mon_salvaje->hp_actual) / mon_salvaje->stats_base.hp_base) * 100;
                            float tiro = rand() % 100 + 1 ;
                            if (tiro <= PC) {
                                list_pushBack(jugador->equipo_mon, mon_salvaje) ;
                                printf("%s ha sido capturado! \n", mon_salvaje->apodo) ;
                                obj->cantidad -= 1 ;
                                esperar_enter() ;
                                return 1 ;
                            }
                            else {
                                printf("Has fallado el tiro... \n") ;
                            }
                        }
                        else if (!strcmp(obj->nombre, "Pocion")){
                            mon_batalla->hp_actual += 4 ;
                            if (mon_batalla->hp_actual > mon_batalla->stats_base.hp_base < mon_batalla->hp_actual){
                            }
                            printf("%s recupera algo de vida! \n", mon_batalla->apodo) ;
                        }
                        obj_ocupado = 0 ;
                        esperar_enter() ;
                    }
                    break ;
                }
                case '4' :
                    return 2 ;
                    break ;
                default :
                    printf("Opcion no valida \n") ;

            esperar_enter() ;
        }
        while (tecla != '1' && tecla != '2' && tecla !='4' && obj_ocupado) ;
        if (tecla == '4'){
            printf("Huyes de la batalla \n") ;
            esperar_enter() ;
            break ;

        } 
        if (mon_salvaje->hp_actual <= 0){
            printf("Mon fue derrotado! \n") ;
            esperar_enter() ;
            return 1 ;
        }
        if (rand() % 100 + 1 <= 10) MC = 1.5 ;
        else MC = 1.0 ;
        dano_recibido = ceil((mon_salvaje->damage_actual * ef_mon_salvaje * MC) - (mon_salvaje->defense_actual * defensa_salvaje)) ;
        mon_batalla->hp_actual -= dano_recibido ; // Falta factor random de 0.9-1.1
        printf("%s le quita %d de vida a %s \n", mon_salvaje->apodo, dano_recibido, mon_batalla->apodo) ;
        if (mon_batalla->hp_actual <= 0) {
            mon_batalla = list_next(equipo) ;
            printf("%s ha sido derrotado...\n") ;
            if (mon_batalla != NULL){
                printf("Sacas al combate a %s", mon_batalla->apodo) ; 
            }
            else {
                printf("No te quedan mas mon, has perdido... \n") ;
                esperar_enter() ;
                free(mon_salvaje) ;
                return 0 ;
            }
        }
        esperar_enter() ;
        
    }

    return 1 ;
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
                    if (rand() % 100 + 1 <= 50) defensa_mon = 0.5 ; 
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

            
            esperar_enter() ;
        }
        while (tecla != '1' && tecla != '2' && tecla !='4' && obj_ocupado) ;
        if (tecla == '4'){
            printf("Huyes de la batalla \n") ;
            esperar_enter() ;
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
                esperar_enter() ;
                return 0 ;
            }
        }
        esperar_enter() ;
        
    }

    return 1 ;
}


// --- //
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
        } else {
            printf("\nRespuesta incorrecta. Intenta de nuevo.\n");
        }
    } while (opcion != 0);
}