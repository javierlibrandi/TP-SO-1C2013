/*
 * planificador_thr.c
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <mario_para_todos/ambiente_config.h>
#include <commons/config.h>
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <commons/collections/list.h>
#include "planificador_thr.h"
#include <commons/collections/list.h>

static void *hilos_parlante_thr(t_h_parlante *h_parlante);

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;
	pthread_t parlante_thr;
	t_h_parlante *h_parlante;
	t_list *list_parlantes;

	int sck_server, new_fd;

	struct t_param_plan param_planificador; //si no declaro la variable como  "struct t_param_plan" tengo problemas para resolver el nombre
	char *des_nivel;

	des_nivel = h_planificador->desc_nivel;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			des_nivel);

	list_parlantes = list_create(); //creo lista de hilos

	//leo el archivo de configuracion para el hilo orquestador
	param_planificador = leer_archivo_plan_config(des_nivel);
	h_planificador->puerto = param_planificador.PUERTO;

	if ((sck_server = Abre_Socket_Inet(param_planificador.PUERTO)) == -1) {

		log_in_disk_plan(LOG_LEVEL_ERROR,
				"Error al abrir la conexion para el planificador de nivel",
				des_nivel);
		exit(1);
	}

	log_in_disk_plan(LOG_LEVEL_TRACE, "servidor escuchando %s", des_nivel);

	while (1) {
		if ((new_fd = Acepta_Conexion_Cliente(sck_server)) == -1) {
			log_in_disk_plan(LOG_LEVEL_ERROR,
					"error en Acepta_Conexion_Cliente %s", des_nivel);
			exit(1);
		}

		h_parlante = malloc(sizeof(h_parlante)); //recervo la memoria para almacenar el nuevo hilo
		h_parlante->desc_nivel = des_nivel; //agrego la des del nivel
		h_parlante->parlante_thr = new_fd;
		/**
		 * creo los hilos parlante
		 */
		pthread_create(&parlante_thr, NULL, (void*) hilos_parlante_thr,
				 h_parlante);


		list_add(list_parlantes, h_parlante); //agrego el nuevo hilo a la lista

	}
	//cierro el socket que escucha para no aceptar nuevas conexiones.
	//Como estoy en un while infinito no tiene sentido lo pogo como ejempo
	close(sck_server);

	return 0;
}

static void *hilos_parlante_thr(t_h_parlante *h_parlante) {
	char msj[1024], rec[5];
	int enviados;

	while (1) {

		strcpy(msj, "Hello, world!\n");
		enviados = Escribe_Socket(h_parlante->parlante_thr, msj, strlen(msj));
		if (enviados == -1) {
			perror("send");
			log_in_disk_plan(LOG_LEVEL_ERROR, "error en send %s", h_parlante->desc_nivel);
			exit(1);
		}

		enviados = Lee_Socket(h_parlante->parlante_thr, rec, 5);
		log_in_disk_plan(LOG_LEVEL_TRACE,
				"mensaje recivido %s de un len %d \n ", rec, enviados);

		if (strncmp(rec, "fin", 3) == 0) {
			close(h_parlante->parlante_thr); //cierro el socket con el que estoy hablando
			break; //salgo del segundo while para atender otra conexion
		}
	}

	return 0;

}


