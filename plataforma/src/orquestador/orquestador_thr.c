/*
 * orquestador_thr.c
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/grabar.h>
#include <commons/config.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include "orquestador_thr.h"
#include <commons/collections/list.h>
#include <pthread.h>

void *orequestador_thr(void* p) {
	t_h_orquestadro *t_h_orq = (t_h_orquestadro *) p;
	void *buffer = NULL;
	int i;
	int tipo;
	//char* personaje;


	for (;;) {
		if (select(*(t_h_orq->sock) + 1, t_h_orq->readfds, NULL,
				NULL, NULL ) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}


		for (i = 0; i <= *(t_h_orq->sock); i++) {
			if (FD_ISSET(i, t_h_orq->readfds)) {
				buffer = recv_variable(i,&tipo); // *(t_h_orq->sock) Para mi es i el 1er parametro del rec por que el socket que me respondio tiene ese valor.
				free(buffer);


			}
		}


	}
	pthread_exit(EXIT_SUCCESS);
}
