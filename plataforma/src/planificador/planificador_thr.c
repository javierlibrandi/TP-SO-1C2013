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

static void *hilo_parlante_thr(t_h_parlante *h_parlante);

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;

	void *buffer = NULL;
	int tipo;

	struct t_param_plan param_planificador; //si no declaro la variable como  "struct t_param_plan" tengo problemas para resolver el nombre
	char *des_nivel;

	des_nivel = h_planificador->desc_nivel;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			des_nivel);

	//leo el archivo de configuracion para el hilo orquestador
	param_planificador = leer_archivo_plan_config(des_nivel);


	log_in_disk_plan(LOG_LEVEL_TRACE, "servidor escuchando %s", des_nivel);

	while (1) {

		buffer = recv_variable(h_planificador->sock, &tipo);
		printf("salida del segundo llamado %s",(char*)buffer);
	}
	//cierro el socket que escucha para no aceptar nuevas conexiones.
	//Como estoy en un while infinito no tiene sentido lo pogo como ejempo


	return 0;
}

static void *hilo_parlante_thr(t_h_parlante *h_parlante) {

	char buffer[25];

	while (1) {

		if (recv_variable(h_parlante->sock, buffer) == -1) {
			log_in_disk_plan(LOG_LEVEL_ERROR,
					"error el recibir mensaje del planificador %s",
					h_parlante->desc_nivel);
			exit(-1);
		}

		log_in_disk_plan(LOG_LEVEL_TRACE, "mensaje recivido %s  \n ",
				h_parlante->desc_nivel);
	}

	close(h_parlante->sock);

	return 0;
}



