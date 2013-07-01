/*
 * funciones_personaje.h
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#ifndef FUNCIONES_PERSONAJE_H_
#define FUNCIONES_PERSONAJE_H_

#include <commons/collections/list.h>
//#include select.h
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/entorno.h>

 typedef struct{
	 char* nombre_nivel;
	 char* ip_nivel;
	 int puerto_nivel;
 }InfoProxNivel;

 typedef struct {
 	int *descriptor;
 	fd_set *readfds;
 } t_listenerPersonaje;



/*Primera función que llama el main. Informa al usuario el personaje asignado.
* Inicializa la estructura de un personaje con los datos de un archivo de configuración*/
 Personaje* nuevoPersonaje();

 //Se conecta y realiza el handshake con el orquestador. Devuelve el descriptor.
 int conectarOrquestador(Personaje* personaje);

/*El personaje determina el próx nivel a completar.
 *y le solicita ubicación (ip-puerto) del nivel al orquestador*/
 InfoProxNivel consultarProximoNivel(int descriptor, Personaje* personaje);

/* El personaje se conecta al nivel y planificador y espera su turno para jugar*/
void iniciarNivel(Personaje* personaje, InfoProxNivel infoNivel);

// Esta función va a escuchar al planificador y al nivel

//el personaje actúa ante una notificación de movimiento permitido por parte del planificador del nivel
void ejecutarTurno(Personaje *personaje);

//devuelve el próximo nivel a completar
char* determinarProxNivel(t_list* niveles, int nivelActual);

//Le solicita al Nivel las coordenadas x-y del proximo nivel a completar
void solicitarUbicacionRecurso(Personaje* personaje);

/* Realizará los movimientos necesarios y evaluará su posición hasta llegar al recurso. */
//void llegarARecurso(Posicion posicionRecurso, Posicion posicionActual);

/* El personaje determina si debe seguir moviéndose para conseguir el recurso o si llegó a destino y debe solicitar una instancia al Nivel*/
int evaluarPosicion(Posicion posActual, Posicion posRecurso);

/* Se moverá una posición en un eje para acercarse al próximo recurso. */
void moverse(Personaje* personaje);

char determinarProxRecurso(Nivel infoNivel, int recursoActual);

/* El personaje solicita adjudicarse un recurso. El nivel deberá descontarlo de sus recursos disponibles.
 * devuelve 0 si no hay disponibles y 1 si se logró adjudicar */
int solicitarInstanciaRecurso();

int objetivoNivelCumplido(Personaje* personaje);

int planDeNivelesCumplido(t_list *niveles);

void salirDelNivel(int sockNivel, int sockPlanif);

void reiniciarNivel(Personaje *personaje);

void reiniciarPlanDeNiveles(Personaje *personaje);

int conocePosicionRecurso(char recursoActual);

t_recusos  *recursos_nivel(t_list *recursos,char *nivel);

/* En caso que no exisitiera un recurso disponible al momento de solicitar una instancia a Nivel, el personaje deberá notificar su bloqueo y esperar
 *  a que el mismo se libere. */
//notificarBloqueo();

/* El personaje evaluará en su lista de recursos pendientes si completó el nivel.*/
//evaluarObjetivo();

/* En caso que la lista de recursos pendientes sea null, el personaje deberá notificar al nivel/planificador, desconectarse del mismo para que se
 *  habiliten los recursos que tenía asignados. */
//notificarObjetivoCumplido();

/* EL personaje le notificará al planificador del nivel que pudo cumplir su turno */
//notificarTurnoCumplido();

/* EL personaje informa al nivel que murió por interbloqueo o por señal SIGTERM*/
//notificarMuerte(Motivo);

/* AL recibir la señal SIGUSR1 el personaje deberá incrementar sus vidas en 1 */
//incrementarVida();

/* AL recibir la señal SIGTERM o la orden del planificador(por deadlock) el personaje deberá descontar sus vidas en 1*/
//descontarVida();

/* EN caso que el personaje muera, evaluará si tiene vidas disponibles y solicitará reiniciar el nivel*/
//reiniciarNivel();

/* En caso que el personaje muera, evaluará si sus vidas disponibles = 0 y solicitará reiniciar el juego.
 * Se reiniciará su plan de niveles respecto a su archivo de configuración inicial y se volverá a conectar al orquestador */
//reiniciarJuego();

/* EL personaje determina al fin de cada nivel, si siguen existiendo niveles por completar*/
//evaluarJuegoCompleto();

/* Completado su plan de niveles, el personaje notifica al orquestador*/
//notificarJuegoCompletoOrquestador();


#endif /* FUNCIONES_PERSONAJE_H_ */
