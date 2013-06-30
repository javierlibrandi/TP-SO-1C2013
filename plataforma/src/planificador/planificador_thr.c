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
#include <mario_para_todos/entorno.h>

static void eliminar_nodo(int sck, t_list *list_planifidores);
static t_personaje *planifico_personaje(t_h_planificador *h_planificador);
static void mover_personaje(t_personaje *personaje, int *cuantum, int espera);

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;
	t_personaje *personaje;
	void *buffer = NULL;
	int tipo, sck;
	struct timeval tv;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			h_planificador->desc_nivel);

	tv.tv_sec = h_planificador->segundos_espera;

	for (;;) {
		if (select(*(h_planificador->sock) + 1, h_planificador->readfds, NULL,
				NULL, &tv) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (sck = 0; sck <= *(h_planificador->sock); sck++) {

			if (FD_ISSET(sck, h_planificador->readfds)) {

				buffer = recv_variable(sck, &tipo);

				if (!strcmp(buffer, Leido_error)) {

					elimino_sck_lista(sck, h_planificador->readfds);

					log_in_disk_plan(LOG_LEVEL_ERROR,
							"%s lo saco de la lista  ", Leido_error);
					if (h_planificador->sck_planificador == sck) {
						log_in_disk_plan(LOG_LEVEL_ERROR,
								"El error esta en el planificador mato al hilo %s",
								h_planificador->desc_nivel);
						pthread_mutex_lock(h_planificador->s_lista_plani);
						eliminar_nodo(sck,
								h_planificador->lista_planificadores);
						pthread_mutex_unlock(h_planificador->s_lista_plani);
						pthread_exit((void *) "Se desconecto el planificador"); //solo si se desconecta el planificador
					}

					free(buffer);
				} else {

					log_in_disk_plan(LOG_LEVEL_TRACE,
							"salida del segundo llamado %s nivel %s \n", buffer,
							h_planificador->desc_nivel);

				}

			} else {
				//busco el siguiente personaje a planificar
//				log_in_disk_plan(LOG_LEVEL_INFO,
//						"busco personaje listo para moverce");

				pthread_mutex_lock(h_planificador->s_listos);
				personaje = planifico_personaje(h_planificador);
				pthread_mutex_unlock(h_planificador->s_listos);

				//si el personaje no es nulo muevo el personaje
				if (personaje) {
					mover_personaje(personaje, h_planificador->cuantum,
							h_planificador->segundos_espera);
				}
				tv.tv_sec = h_planificador->segundos_espera;
				break;
			}
		}

	}

//cierro el socket que escucha para no aceptar nuevas conexiones.
//Como estoy en un while infinito no tiene sentido lo pogo como ejempo

	pthread_exit(EXIT_SUCCESS);
}

static void eliminar_nodo(int sck, t_list *list_planificadores) {
	int index = 0;
	t_h_planificador *h_planificador;

	bool _list_elements(t_h_planificador *h_planificador) {

		if (*(h_planificador->sock) == sck) {
			log_in_disk_plat(LOG_LEVEL_INFO,
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

static t_personaje *planifico_personaje(t_h_planificador *h_planificador) {
	static int index = 0; //hago un buffer circular
	int total_elementos;
	int aux = index;
	t_personaje *personaje;

	log_in_disk_plat(LOG_LEVEL_INFO, "planifico_nivel");

	total_elementos = list_size(h_planificador->l_listos);

	//si me pase del ultimo elemento me posicione en el primero y recorro hasta el ultimo, esto puede ser porque se elimino algun personaje
	if (index > total_elementos) {
		index = 0;
	}
	aux = total_elementos;

	//doy una vuelta completa al buffer y si no encuentro ningun personaje retorno null
	while (aux != index) {
		{
			//obtengo de a uno los personajes
			personaje = (t_personaje*) list_get(h_planificador->l_listos,
					index++);

			if (!strcmp(h_planificador->desc_nivel, personaje->nivel)) {
				log_in_disk_plat(LOG_LEVEL_INFO, "Personaje planificado %s",personaje->nombre);
				return personaje;
			} else if (index > total_elementos) {
				index = 0;
			}
		}
	}

	return NULL ;
}

static void mover_personaje(t_personaje *personaje, int *cuantum, int espera) {
	int byteEnviados;
	char *buffer;
	int tipo;

	//permito mover al personaje mientras el cuantun no llegue a 0
	while (*cuantum--) {
		log_in_disk_plat(LOG_LEVEL_INFO,
				"Permito el movimiento del personaje %s", personaje->nombre);
		fd_mensaje(personaje->sck, PL_TO_P_TURNO, "Movimiento permitido",
				&byteEnviados);

		buffer = recv_variable(personaje->sck, &tipo);

		log_in_disk_plat(LOG_LEVEL_ERROR,
				"No esta implementado el switch para el mensaje %s que es del tipo %d",
				buffer, tipo);

//		switch (tipo) {
//		case P_TO_N_BLOQUEO:
//
//			break;
//		default:
//			log_in_disk_plan(LOG_LEVEL_INFO,
//					"busco personaje listo para moverce");
//
//			pthread_mutex_lock(h_planificador->s_listos);
//			personaje = planifico_nivel(h_planificador);
//			pthread_mutex_unlock(h_planificador->s_lista_plani);
//
//			//si el personaje no es nulo muevo el personaje
//			if (personaje) {
//				mover_personaje(personaje, h_planificador->cuantum);
//			}
//
//			break;
//		}
		sleep(espera);
	}
}
