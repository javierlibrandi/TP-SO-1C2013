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

static void eliminar_nodo(int sck,  t_list *list_planifidores);

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;

	void *buffer = NULL;
	int tipo,sck;

//	char *hostname;
//	int size;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			h_planificador->desc_nivel);

	for (;;) {
		if (select(*(h_planificador->sock) + 1, h_planificador->readfds, NULL,
				NULL, NULL ) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (sck = 0; sck <= *(h_planificador->sock); sck++) {

			if (FD_ISSET(sck, h_planificador->readfds)) {

				buffer = recv_variable(sck, &tipo);

				if (!strcmp(buffer, Leido_error)) {

					elimino_sck_lista(sck, h_planificador->readfds);

					pthread_mutex_lock(h_planificador->s_lista_plani);
					eliminar_nodo(sck,h_planificador->lista_planificadores);
					pthread_mutex_unlock(h_planificador->s_lista_plani);

					log_in_disk_plan(LOG_LEVEL_ERROR,
												"%s lo saco de la lista  ", Leido_error);

					pthread_exit(EXIT_FAILURE);//solo si se desconecta el planificador

				} else {

					log_in_disk_plan(LOG_LEVEL_TRACE,
							"salida del segundo llamado %s nivel %s \n",
							 buffer, h_planificador->desc_nivel);
					switch (tipo) {
						case P_TO_N_BLOQUEO:

							break;
						default:
							log_in_disk_plan(LOG_LEVEL_INFO, "busco personaje listo para moverce");

							break;
					}

				}
				free(buffer);

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

	if (list_find(list_planificadores, (void*) _list_elements)){
		h_planificador = list_get(list_planificadores,index);
		list_remove(list_planificadores, index);

		free(h_planificador->desc_nivel);
		free(h_planificador->sock);
		free(h_planificador);
	}
}
