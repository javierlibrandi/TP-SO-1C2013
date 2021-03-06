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
char* determinarProxNivel(Personaje* personaje);

t_link_element* obtenerNodo(t_list* self, int index);

//Le solicita al Nivel las coordenadas x-y del proximo nivel a completar
void solicitarUbicacionRecurso(Personaje* personaje);

/* Realizará los movimientos necesarios y evaluará su posición hasta llegar al recurso. */
//void llegarARecurso(Posicion posicionRecurso, Posicion posicionActual);

/* El personaje determina si debe seguir moviéndose para conseguir el recurso o si llegó a destino y debe solicitar una instancia al Nivel*/
bool evaluarPosicion(Posicion posActual, Posicion posRecurso);

/* Se moverá una posición en un eje para acercarse al próximo recurso. */
void moverse(Personaje* personaje);

char* determinarProxRecurso(Personaje* personaje);

/* El personaje solicita adjudicarse un recurso. El nivel deberá descontarlo de sus recursos disponibles.
 * devuelve 0 si no hay disponibles y 1 si se logró adjudicar */
bool solicitarInstanciaRecurso(Personaje *personaje);

bool objetivoNivelCumplido(Personaje* personaje);

bool planDeNivelesCumplido(Personaje* personaje);

void salirNivelPorObjCumplido(Personaje *personaje);

void salirPlanifPorMuerte(Personaje *personaje);

void salirNivelPorMuerte(Personaje *personaje);

void reiniciarNivel(Personaje *personaje);

void reiniciarListaRecursos(Personaje *personaje);

void reiniciarPlanDeNiveles(Personaje *personaje);

void reiniciarPlanDeNiveles2(Personaje *personaje);

bool conocePosicionRecurso(char recursoActual);

t_recusos  *recursos_nivel(t_list *recursos,char *nivel);

void controlarConexion_recv(char *buffer);


#endif /* FUNCIONES_PERSONAJE_H_ */
