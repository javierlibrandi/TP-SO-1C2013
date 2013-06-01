/*
 * conect_plataforma.c
 *
 *  Created on: 22/05/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/grabar.h>
int con_pla_nival(char *ip, int puerto, char *nombre_nivel) {
	int sck, tipo;
	struct t_send t_send;
	void *buffer = NULL;

	log_in_disk_niv(LOG_LEVEL_TRACE, "con_pla_nival del planificador %s ",
			nombre_nivel);

	sck = Abre_Conexion_Inet(ip, puerto);
	fd_mensaje(sck,N_TO_O_SALUDO,nombre_nivel);

	buffer = recv_variable(sck, &tipo);
	if (tipo == ERROR) {
		log_in_disk_niv(LOG_LEVEL_ERROR, "%s", (char*) buffer);
		exit(EXIT_FAILURE);

	}
	log_in_disk_niv(LOG_LEVEL_INFO, "%s", (char*) buffer);

	fd_mensaje(sck,N_TO_O_SALUDO,nombre_nivel);
	fd_mensaje(sck,N_TO_O_SALUDO,nombre_nivel);


	return sck;
}
