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
#include "ambiente_config.h"


int main(void) {
	t_param param_orquestador;

	//leo el archivo de configuracion para el hilo orquestador
	param_orquestador = leer_archivo_orquestador_config();



	return EXIT_SUCCESS;
}
