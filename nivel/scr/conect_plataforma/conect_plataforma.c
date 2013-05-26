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

	memset(t_send.mensaje, '\0', 20);
	strcpy(t_send.mensaje, nombre_nivel);

	t_send.header_mensaje = N_TO_O_SALUDO;
	t_send.payLoadLength = sizeof(t_send.mensaje);

	sck = Abre_Conexion_Inet(ip, puerto);

	Escribe_Socket(sck, &t_send, sizeof(struct t_send));

	buffer = recv_variable(sck, &tipo);
	if (tipo == ERROR) {
		log_in_disk_niv(LOG_LEVEL_ERROR, "%s", (char*) buffer);
		exit(EXIT_FAILURE);

	}
	log_in_disk_niv(LOG_LEVEL_INFO, "%s", (char*) buffer);

	Escribe_Socket(sck, &t_send, sizeof(struct t_send));

	return sck;
}
