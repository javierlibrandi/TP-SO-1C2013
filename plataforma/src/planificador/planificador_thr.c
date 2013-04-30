/*
 * planificador_thr.c
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>

void *planificador_nivel_thr(void *p) {
	char *des_nivel = (char *) p;
	int sck_server, new_fd, enviados;
	char msj[1024], puerto[8],rec[5];



	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			des_nivel);


	strcpy(puerto,"5021");

	if ((sck_server = Abre_Socket_Inet(puerto)) == -1) {

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
		strcpy(msj,"Hello, world!\n");
		enviados=Escribe_Socket(new_fd, msj, strlen(msj));
		if (enviados == -1){
				perror("send");
				log_in_disk_plan(LOG_LEVEL_ERROR,
									"error en send %s", des_nivel);
				exit (1);
			}


		log_in_disk_plan(LOG_LEVEL_TRACE, "mensaje enviado %s de un len %d ", des_nivel,enviados);
		enviados=Lee_Socket(new_fd, rec, 5);
		log_in_disk_plan(LOG_LEVEL_TRACE, "mensaje recivido %s de un len %d ", rec,enviados);

	}

	return 0;
}
