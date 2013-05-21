/*
 * orquestador_thr.c
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/grabar.h>
#include <commons/config.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>

#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close

void *orequestador_thr() {


	//leo el archivo de configuracion para el hilo orquestador

	log_in_disk_orq(LOG_LEVEL_DEBUG, "se crea el orquestador");
	///perfecto



}
