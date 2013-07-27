/*
 * planificador_thr.c
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/ambiente_config.h>
#include <commons/config.h>
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <commons/collections/list.h>
#include "planificador_thr.h"
#include <commons/collections/list.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/entorno.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>
#include "../planificador/planificador_thr.h"

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <mario_para_todos/entorno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static t_personaje *planifico_personaje(t_h_planificador *h_planificador,
		int *index);
static void mover_personaje(t_personaje *personaje,
		t_h_planificador *h_planificador);
void eliminar_personaje_termino_nivel(int sck, t_list *l_listos);
t_personaje *busca_personaje_skc(int sck, t_list *l_listos,
		int *indice_personaje);
void liberar_memoria_personaje(t_personaje *personaje);
void * hilo_planificador(void * p);

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;

	void *buffer = NULL;
	int tipo, sck;
	struct timeval tv;
	pthread_t _planificador_t_h;
	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			h_planificador->desc_nivel);

	tv.tv_sec = h_planificador->segundos_espera;
	tv.tv_usec = 0;
	h_planificador->error_nivel = false; //Inicializo al bandera del error en el planificador.

	pthread_create(&_planificador_t_h, NULL, (void*) hilo_planificador,
			(void*) p);

	for (;;) {

		if (h_planificador->error_nivel) {

			pthread_exit((void *) "Se desconecto el planificador"); // Si la bandera esta en true, es por que hubo error y hay que matar al hilo.
		}

		if (select(*(h_planificador->sock) + 1, h_planificador->readfds, NULL,
				NULL, &tv) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (sck = 0; sck <= *(h_planificador->sock); sck++) {

			if (FD_ISSET(sck, h_planificador->readfds)) {
				buffer = recv_variable(sck, &tipo);

				if (!strcmp(buffer, Leido_error)) {

					log_in_disk_plan(LOG_LEVEL_ERROR,
							"%s lo saco de la lista  ", Leido_error);

//					log_in_disk_plan(LOG_LEVEL_ERROR,
//												"Error en la coneccion con el personaje: %c con el planificador de nivel: %s  "); // TODO Agregar el nombre del personaje que tuvo el error. 1ro buscarlo en la lista por socket.

					lock_listas_plantaforma(h_planificador);
					mover_personaje_lista(sck, h_planificador->l_listos,
							h_planificador->l_errores);
					mover_personaje_lista(sck, h_planificador->l_bloquedos,
							h_planificador->l_errores);
					imprimir_listas(h_planificador, 'p');
					un_lock_listas_plataforma(h_planificador);

					elimino_sck_lista(sck, h_planificador->readfds);
					free(buffer);

				} else {

					log_in_disk_plan(LOG_LEVEL_TRACE, "TIPO %d", tipo);
					switch (tipo) {

					case P_TO_PL_OBJ_CUMPLIDO: //cuando el nivel esta complido saco el personaje de las listas

						log_in_disk_plan(LOG_LEVEL_TRACE, "TIPO %d", tipo);

						lock_listas_plantaforma(h_planificador);

						eliminar_personaje_termino_nivel(sck,
								h_planificador->l_listos);
						elimino_sck_lista(sck, h_planificador->readfds);
						imprimir_listas(h_planificador, 'p');
						un_lock_listas_plataforma(h_planificador);

						free(buffer); // TODO Donde se hace el malloc??

						// Aca se responde a los mensajes que reciba el planificador.
						log_in_disk_plan(LOG_LEVEL_TRACE,
								"salida del segundo llamado %s nivel %s \n",
								buffer, h_planificador->desc_nivel);
						break;

					}
					tv.tv_sec = h_planificador->segundos_espera;

				}

			}
		}
	}

//cierro el socket que escucha para no aceptar nuevas conexiones.
//Como estoy en un while infinito no tiene sentido lo pogo como ejempo

	pthread_exit(EXIT_SUCCESS);
}

void eliminar_planificador(int sck, t_list *list_planificadores) {
	int index = 0;
	t_h_planificador *h_planificador;

	bool _list_elements(t_h_planificador *h_planificador) {

//		if (*(h_planificador->sock) == sck) {
		if (h_planificador->sck_planificador == sck) { //Este es el socket que hay que comparar.
			log_in_disk_plan(LOG_LEVEL_INFO,
					"Elimino el planificador %s de la lista",
					h_planificador->desc_nivel);

			return true;
		}
		index++;
		return false;

	}

	if (list_find(list_planificadores, (void*) _list_elements)) {
		h_planificador = list_get(list_planificadores, index);
		list_remove(list_planificadores, index);

		free(h_planificador->desc_nivel);
		free(h_planificador->sock);
		free(h_planificador);
	}
}

static t_personaje *planifico_personaje(t_h_planificador *h_planificador,
		int *index) {

	int total_elementos;
	int aux;
	t_personaje *personaje;
	int index_aux = *index;

	//log_in_disk_plat(LOG_LEVEL_INFO, "planifico_nivel");

	total_elementos = list_size(h_planificador->l_listos);
	if (total_elementos > 0) {
//si me pase del ultimo elemento me posicione en el primero y recorro hasta el ultimo, esto puede ser porque se elimino algun personaje
		if (index_aux >= total_elementos) {
			index_aux= 0;
		}

		log_in_disk_plan(LOG_LEVEL_INFO, "Indice en la planificacion %d",
								*index);
		aux = total_elementos;

//doy una vuelta completa al buffer y si no encuentro ningun personaje retorno null
		//while (aux != index)
		while (aux > index_aux) {

			//obtengo de a uno los personajes
			personaje = (t_personaje*) list_get(h_planificador->l_listos,
					index_aux);

			index_aux++;

			//log_in_disk_plat(LOG_LEVEL_INFO, "Personaje elijido para la planificacion %c, nivel del personaje %s, indice %d",personaje->simbolo,personaje->nivel,*index);

			if (!strcmp(h_planificador->desc_nivel, personaje->nivel)) {
//				log_in_disk_plat(LOG_LEVEL_INFO, "Personaje planificado %s",
//						personaje->nombre);
				*index = index_aux;
				return personaje;
			}
//		else if (index > total_elementos) {
//			index = 0;
//		}
		}
	}
	*index = index_aux;
	return NULL ;
}

static void mover_personaje(t_personaje *personaje,
		t_h_planificador *h_planificador) {
	int byteEnviados;
	char *buffer;
	int tipo;
	int movimientos_realizados = 0;
	bool personaje_bloqueado = false;
	int sock_aux;
//permito mover al personaje mientras el cuantun no llegue a 0
	while (*(h_planificador->cuantum) >= ++movimientos_realizados //TODO Revisar condicion del ciclo.
	&& !personaje_bloqueado) {

		log_in_disk_plan(LOG_LEVEL_INFO,
				"Permito el movimiento del personaje %s cantidad de movimientos realizados por el personaje %d",
				personaje->nombre, movimientos_realizados);

		fd_mensaje(personaje->sck, PL_TO_P_TURNO, "Movimiento permitido",
				&byteEnviados);

		buffer = recv_variable(personaje->sck, &tipo);

		switch (tipo) {

		case P_TO_PL_JUEGO_GANADO: //cuando el nivel esta complido saco el personaje de las listas

			log_in_disk_plan(LOG_LEVEL_TRACE,
					"el personaje:  %s a completado su plan de niveles. ",
					personaje->nombre);

			lock_listas_plantaforma(h_planificador);

			imprimir_listas(h_planificador, 'p');

			mover_personaje_lista(personaje->sck, h_planificador->l_listos,
					h_planificador->l_koopa);
			un_lock_listas_plataforma(h_planificador);
			personaje_bloqueado = true;
			fd_mensaje(personaje->sck, PL_TO_P_MATAR_KOOPA, "vacio", &byteEnviados);
			close (personaje->sck);
			//TODO Ejecutar koopa si las listas estan vacias.
			//TODO Libera la lista de koopa, o eliminarlos personajes en lugar de moverlos a koopa.
			break;

		case P_TO_PL_OBJ_CUMPLIDO: //cuando el nivel esta complido saco el personaje de las listas

			log_in_disk_plan(LOG_LEVEL_TRACE, "TIPO %d", tipo);

//			fd_mensaje(personaje->sck, OK, "Me alegro pos vos!!!!",
//					&byteEnviados);

			lock_listas_plantaforma(h_planificador);
			sock_aux = personaje->sck;
			eliminar_personaje_termino_nivel(personaje->sck,
					h_planificador->l_listos);

			//elimino_sck_lista(personaje->sck, h_planificador->readfds);
			imprimir_listas(h_planificador, 'p');
			un_lock_listas_plataforma(h_planificador);

			fd_mensaje(sock_aux, OK, "Me alegro pos vos!!!!", &byteEnviados);

			close(personaje->sck);
			personaje_bloqueado = true;

			break;
			//TODO AGregar mensaje de que el personaje gano el juego!!

		case P_TO_PL_RECURSO_CONSEGUIDO:
			log_in_disk_plan(LOG_LEVEL_TRACE,
					"Se recibió el mensaje P_TO_PL_RECURSO_CONSEGUIDO");

			//PLANIFICAR EL SIGUIENTE PERSONAJE DE LISTOS
			break;

		case P_TO_PL_BLOQUEO:

			log_in_disk_plan(LOG_LEVEL_TRACE,
					"Se recibió el mensaje P_TO_N_BLOQUEO");

			lock_listas_plantaforma(h_planificador);

			mover_personaje_lista(personaje->sck, h_planificador->l_listos,
					h_planificador->l_bloquedos);
			imprimir_listas(h_planificador, 'p');
			un_lock_listas_plataforma(h_planificador);

			personaje_bloqueado = true;
			//personaje->prox_recurso = buffer[0];
			break;
		}

		if (!strcmp(buffer, Leido_error)) {
			lock_listas_plantaforma(h_planificador);

			mover_personaje_lista(personaje->sck, h_planificador->l_listos,
					h_planificador->l_errores);
			imprimir_listas(h_planificador, 'p');

			un_lock_listas_plataforma(h_planificador);
			elimino_sck_lista(personaje->sck, h_planificador->readfds); //creo que este el esl socket que tengo que eliminar

			personaje_bloqueado = true;

		}
		free(buffer);
		sleep(h_planificador->segundos_espera);
	}
	personaje_bloqueado = false;
}

void eliminar_personaje_termino_nivel(int sck, t_list *l_listos) {
	t_personaje *personaje;
	int indice_personaje;

	log_in_disk_plan(LOG_LEVEL_INFO, "eliminar_personaje_termino_nivel");

	busca_personaje_skc(sck, l_listos, &indice_personaje);

	personaje = list_remove(l_listos, indice_personaje);

	liberar_memoria_personaje(personaje);
}

void liberar_memoria_personaje(t_personaje *personaje) {
	log_in_disk_plan(LOG_LEVEL_INFO, "liberar_memoria_personaje personaje %s",
			personaje->nombre);
	free(personaje->nivel);
	free(personaje->nombre);
	free(personaje);

}

void * hilo_planificador(void * p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;
	t_personaje *personaje;
	static int index = 0;
	for (;;) {

		sleep(h_planificador->segundos_espera);
		pthread_mutex_lock(h_planificador->s_listos);
		personaje = planifico_personaje(h_planificador, &index);

		pthread_mutex_unlock(h_planificador->s_listos);

		//si el personaje no es nulo muevo el personaje
		if (personaje) {
			mover_personaje(personaje, h_planificador);
		}
	}
}

