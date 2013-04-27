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
#include "mario_para_todos/ambiente_config.h"
#include "orquestador/orquestador_thr.h"
#include <commons/collections/list.h>
#include <pthread.h>


int main(void) {
	t_param_orq param_orquestador;
	t_param_plat param_plataforma;
	pthread_t orquestador_thr;


	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();

	//leo el archivo de configuracion para el hilo orquestador
	param_orquestador = leer_archivo_orquestador_config();
	/**
	 * creo el hilo orquetador
	 */
	pthread_create(&orquestador_thr, NULL, (void *) orequestador_thr, NULL );

	pthread_join(orquestador_thr, NULL );

	return EXIT_SUCCESS;
}
