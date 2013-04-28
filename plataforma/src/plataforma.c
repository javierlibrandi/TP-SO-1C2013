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
#include <mario_para_todos/ambiente_config.h>
#include "orquestador/orquestador_thr.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include "planificador/planificador_thr.h"

int main(void) {
	t_param_orq param_orquestador;
	t_param_plat param_plataforma;
	pthread_t orquestador_thr, planificador_thr;
	t_list *list_plataforma;
	int i;
	t_h_planificador *h_planificador;

	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();

	//leo el archivo de configuracion para el hilo orquestador
	param_orquestador = leer_archivo_orquestador_config();

	/**
	 * creo el hilo orquetador
	 */
	pthread_create(&orquestador_thr, NULL, (void *) orequestador_thr, NULL );

	list_plataforma = list_create(); //creo lista de hilos
	for (i = 0; param_plataforma.planificador_nivel[i] != '\0'; i++) {


		h_planificador =  malloc(sizeof(h_planificador)); //recervo la memoria para almacenar el nuevo hilo
		/**
		 * creo los hilos planificador
		 */
		pthread_create(&planificador_thr, NULL, (void *) planificador_nivel_thr,
				param_plataforma.planificador_nivel[i]);

		h_planificador->desc_nivel=param_plataforma.planificador_nivel[i];
		h_planificador->planificador_thr = planificador_thr;
		list_add(list_plataforma, h_planificador); //agrego el nuevo hilo a la lista

	}

	pthread_join(orquestador_thr, NULL );

	int index = 0;

	void _list_elements(t_h_planificador *h_planificador) {
		pthread_join(h_planificador->planificador_thr, NULL );
		index++;
	}

	list_iterate(list_plataforma, (void*) _list_elements);
	list_destroy(list_plataforma);
	puts("fin\n");
	return EXIT_SUCCESS;
}
