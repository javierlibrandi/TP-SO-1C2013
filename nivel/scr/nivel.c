/*
 * nivel.c
 *
 *  Created on: 08/05/2013
 *      Author: utnso
 *      cambio
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/ambiente_config.h>
#include "conect_plataforma/conect_plataforma.h"


int main() {
	t_param_nivel param_nivel;
	int sck_nivel;

	param_nivel = leer_nivel_config("preuba");

	//sck_nivel = con_pla_nival(param_nivel.IP, param_nivel.PUERTO,"nombre_nivel");

	return EXIT_SUCCESS;

}
