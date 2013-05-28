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

 typedef struct{
	char* nombre;
	t_list recursosRestantes;
 }Nivel;

 typedef struct{
	char *nombre;
	char simbolo;
	t_list nivelesRestantes;
	int vidas;
	char *ip_orquestador;
	int puerto_orquestador;
 }Personaje;

 typedef struct{
	 char* ip_nivel;
	 int puerto_nivel;
 }InfoProxNivel;

 typedef struct{
	 char* nombrePersonaje;
	 char* proxNivel;
 }PersonajeNivel;

 typedef struct {
 	int *descriptor;
 	fd_set *readfds;
 } t_listenerPersonaje;


/*Primera función que llama el main. Informa al usuario el personaje asignado.
* Inicializa la estructura de un personaje con los datos de un archivo de configuración*/
 Personaje* nuevoPersonaje(char* nombrePersonaje);

 //Se conecta y realiza el handshake con el orquestador. Devuelve el descriptor.
 int conectarOrquestador(Personaje* personaje);

/*El personaje determina el próx nivel a completar.
 *y le solicita ubicación (ip-puerto) del nivel al orquestador*/
 InfoProxNivel consultarProximoNivel(int descriptor, Personaje* personaje);

/* El personaje se conecta al nivel y planificador y espera su turno para jugar*/
void iniciarNivel(Personaje* personaje, InfoProxNivel infoNivel);

// Esta función va a escuchar al planificador y al nivel
int listenerPersonaje (int descriptorNivel, int descriptorPlan);

/* Ciclo de juego: conectarse y desconectarse del orquestador, niveles y planificadores mientras no se complete el plan de niveles*/
//jugar();

/* El personaje se fija cuál es el próximo recurso a conseguir. Le solicita al Nivel las coordenadas x-y del mismo. */
//solicitarUbicacionRecurso();

/* El personaje determina si debe seguir moviéndose para conseguir el recurso o si llegó a destino y debe solicitar una instancia al Nivel*/
//evaluarPosicion();

/* Se moverá una posición en un eje para acercarse al próximo recurso. */
//realizarMovimiento();

/* Realizará los movimientos necesarios y evaluará su posición hasta llegar al recurso. */
//llegarADestino();

/* El personaje espera el mensaje del planificador para poder moverse. */
//esperarTurno();

/* El personaje solicita adjudicarse un recurso. El nivel deberá descontarlo de sus recursos disponibles.*/
//solicitarInstanciaRecurso();

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
