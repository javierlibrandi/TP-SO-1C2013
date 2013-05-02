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

void* planificador_nivel_thr(void *p) {
	t_h_planificador *h_planificador = (t_h_planificador *) p;
	int sck_server, new_fd, enviados;
	char msj[1024], rec[5];
	struct t_param_plan param_planificador; //si no declaro la variable como  "struct t_param_plan" tengo problemas para resolver el nombre
	char *des_nivel;

	des_nivel = h_planificador->desc_nivel;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
				des_nivel);

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

	while(1){
	if ((new_fd = Acepta_Conexion_Cliente(sck_server)) == -1) {
		log_in_disk_plan(LOG_LEVEL_ERROR, "error en Acepta_Conexion_Cliente %s",
				des_nivel);
		exit(1);
	}

	while (1) {


		strcpy(msj, "Hello, world!\n");
		enviados = Escribe_Socket(new_fd, msj, strlen(msj));
		if (enviados == -1) {
			perror("send");
			log_in_disk_plan(LOG_LEVEL_ERROR, "error en send %s", des_nivel);
			exit(1);
		}

		enviados = Lee_Socket(new_fd, rec, 5);
		log_in_disk_plan(LOG_LEVEL_TRACE, "mensaje recivido %s de un len %d \n ",
				rec, enviados);

		if (strncmp(rec,"fin",3)==0){
			close(new_fd);//cierro el socket con el que estoy hablando
			break; //salgo del segundo while para atender otra conexion
		}
	}
	}
	//cierro el socket que escucha para no aceptar nuevas conexiones.
	//Como estoy en un while infinito no tiene sentido lo pogo como ejempo
	close(sck_server);

	return 0;
}

