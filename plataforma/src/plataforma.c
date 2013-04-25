/*
 ============================================================================
 Name        : mario.c
 Author      : javier
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "biblioteca/ambiente_config.h"
#include <mario_para_todos/prueba/prueba.h>


int main(void) {
	t_param_orq param_orquestador;
	t_param_plat param_plataforma;


	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();

	//leo el archivo de configuracion para el hilo orquestador
	param_orquestador = leer_archivo_orquestador_config();

	hola_javi();

	return EXIT_SUCCESS;
}
