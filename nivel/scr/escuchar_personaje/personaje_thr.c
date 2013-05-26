/*
 * personaje_thr.c
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/entorno.h>
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/grabar.h>
#include "personaje_thr.h"

void *escucho_personaje(void *p) {
	int sck, new_sck, tipo;
	void *buffer = NULL;
	t_h_personaje *t_personaje = (t_h_personaje*) p;

	log_in_disk_niv(LOG_LEVEL_TRACE,
			"Ecucho conexiones de los personajes en el puerto %d \t soy el nivel %s ",
			t_personaje->pueto, t_personaje->nomb_nivel);

	sck = Abre_Socket_Inet(t_personaje->pueto);

	for (;;) {
		new_sck = Acepta_Conexion_Cliente(sck);
		buffer = recv_variable(new_sck, &tipo);
	}
}

