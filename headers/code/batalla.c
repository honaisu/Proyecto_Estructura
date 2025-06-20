#include "../batalla.h"
#include "../jugador.h"
#include "../prints.h"

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

}

#define MON_VIVO(mon) mon->hp_actual > 0

int batalla_pokemon_salvaje(Entrenador *jugador, Mon *mon_salvaje){
    List *equipo = jugador->equipo_mon ;
    if (equipo == NULL) { printf("No tienes Mones!"); return 0; }
    Mon *mon_batalla = list_first(equipo) ;
    bool jugador_en_pie = true ;
    while (MON_VIVO(mon_batalla)) {
        estado_batalla(mon_batalla, mon_salvaje) ;
        menu_batalla_salvaje() ;
        char tecla;
        do {
            esperar_tecla(&tecla);

            switch(tecla){
                case 1: {
                    efectuar_ataque(mon_batalla, mon_salvaje);
                    esperar_enter() ;
                    break ;
                }
                case 2 : {
                    if (rand() % 100 + 1 <= 50) mon_batalla->defense_actual = 0.5 ; 
                    // se podria hacer que la defensa solo dure una cantidad de turnos...
                    break ;
                }
                case 3 : {
                    // esperando a implementacion de inventario
                    // monballs podrian ser infinitas?
                    break ;
                }
                case 4 : {
                    // va a huir si o si? se podria implementar que sea una probabilidad como en los juegos
                    break ;
                }
                default :
                    printf("Opcion no valida \n") ;
            }
        } while (tecla != '1' && tecla != '2' && tecla !='4') ;
        
        if (tecla == '4'){
            printf("Huyes de la batalla \n") ;
            esperar_enter() ;
            break ;
        } 
        if (mon_salvaje->hp_actual <= 0){
            printf("Mon fue derrotado! \n") ;
            esperar_enter() ;
        }
        /*
        if (rand() % 100 + 1 <= 10) MC = 1.5 ;
        else MC = 1.0 ;
        dano_recibido = (mon_salvaje->damage_actual * ef_mon_salvaje * MC) - (mon_salvaje->defense_actual * defensa_salvaje) ;
        mon_batalla->hp_actual -= dano_recibido ; // Falta factor random de 0.9-1.1
        printf("%s le quita %d de vida a %s \n", mon_salvaje->apodo, dano_recibido, mon_batalla ) ;
        if (mon_batalla->hp_actual <= 0) {
            mon_batalla = list_next(equipo) ;
            printf("%s ha sido derrotado...\n", mon_batalla->apodo) ;
            if (mon_batalla != NULL){
                printf("Sacas al combate a %s", mon_batalla->apodo) ; 
            }
            else {
                printf("No te quedan mas mon, has perdido...") ;
                esperar_enter() ;
                return 0 ;
            }
        }
        */
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