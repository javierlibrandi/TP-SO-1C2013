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

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;

	void *buffer = NULL;
	int tipo;
	int i;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			h_planificador->desc_nivel);

	for (;;) {
		if (select(*(h_planificador->sock) + 1, h_planificador->readfds, NULL,
				NULL, NULL ) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i <= *(h_planificador->sock); i++) {
			if (FD_ISSET(i, h_planificador->readfds)) {
				buffer = recv_variable(*(h_planificador->sock), &tipo);
				log_in_disk_plan(LOG_LEVEL_TRACE,
						"salida del segundo llamado %s nivel %s \n", (char*) buffer,h_planificador->desc_nivel);

				free(buffer);


			}
		}
	}
	//cierro el socket que escucha para no aceptar nuevas conexiones.
	//Como estoy en un while infinito no tiene sentido lo pogo como ejempo

	pthread_exit(EXIT_SUCCESS);
}

