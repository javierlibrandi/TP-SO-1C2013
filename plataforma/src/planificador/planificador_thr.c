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

void *planificador_nivel_thr(void *p) {
	char *des_nivel = (char *) p;
	int sck_server;

	log_in_disk_plan(LOG_LEVEL_DEBUG, "Creo el panificador del nivel %s ",
			des_nivel);
	char puerto[6];

	strcpy(puerto, "5021");

	sck_server = Abre_Socket_Inet(puerto);

	if (sck_server == -1) {

		log_in_disk_plan(LOG_LEVEL_ERROR,
				"Error al abrir la conexion para el planificador de nivel",
				des_nivel);
		exit(1);
	}

	log_in_disk_plan(LOG_LEVEL_TRACE, "servidor escuchando %s", des_nivel);

	while (1) {

	Acepta_Conexion_Cliente(sck_server);
	log_in_disk_plan(LOG_LEVEL_TRACE, "conexion aceptada %s", des_nivel);

}

return 0;
}
