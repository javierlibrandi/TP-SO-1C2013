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
#include "bibliteca/ambiente_config.h"
#include <scr/prueba/prueba.h>




int main(void) {
	t_param_orq param_orquestador;
	t_param_plat param_plataforma;

	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();

	//leo el archivo de configuracion para el hilo orquestador
	param_orquestador = leer_archivo_orquestador_config();
hola_javi();

	/**
	 * Creo el hiloquestador
	 */
//	pthread_create(&tabla_thr[0], NULL, (void *) orquestador_thr, (void *)param_orquestador );

	/**
	 * creo los hilos planificadores de nivel
	 *
	 */

//	for(int i=1;param_plataforma.planificador_nivel[i]!='\0';i++){
//		pthread_create(&tabla_thr[0], NULL, (void *) planificador_thr,(void *)param_palnificador );
//	}

	return EXIT_SUCCESS;
}
