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

void libero_memoria(t_list *list_plataforma);

t_list *menejo_hilos_planificador(t_h_planificador *h_planificador, t_param_plat param_plataforma);



int main(void) {

	t_param_plat param_plataforma;
	pthread_t orquestador_thr;
	t_h_planificador *h_planificador;
	t_list *list_plataforma;




	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();


	/**
	 * creo el hilo orquetador
	 */
	pthread_create(&orquestador_thr, NULL, (void *) orequestador_thr, NULL );

	list_plataforma = menejo_hilos_planificador(h_planificador,param_plataforma);
	pthread_join(orquestador_thr, NULL );



	libero_memoria(list_plataforma);
	list_destroy(list_plataforma);

	return EXIT_SUCCESS;
}

t_list *menejo_hilos_planificador(t_h_planificador *h_planificador,t_param_plat param_plataforma){
	t_list *list_plataforma;
	int index;
	pthread_t planificador_thr;

	list_plataforma = list_create(); //creo lista de hilos
		for (index = 0; param_plataforma.planificador_nivel[index] != '\0'; index++) {


			h_planificador =  malloc(sizeof(h_planificador)); //recervo la memoria para almacenar el nuevo hilo
			h_planificador->desc_nivel=param_plataforma.planificador_nivel[index];//agrego la des del nivel
			/**
			 * creo los hilos planificador
			 */
			pthread_create(&planificador_thr, NULL,(void*) planificador_nivel_thr,
					(void*)h_planificador);

			h_planificador->planificador_thr = planificador_thr;
			list_add(list_plataforma, h_planificador); //agrego el nuevo hilo a la lista

		}
		index = 0;

		void _list_elements(t_h_planificador *h_planificador) {

			pthread_join(h_planificador->planificador_thr, NULL );

			index++;
		}

		list_iterate(list_plataforma, (void*) _list_elements);

		return list_plataforma;

}

void libero_memoria(t_list *list_plataforma){
	int index;
	index = 0;

		void _list_elements(t_h_planificador *h_planificador) {
			free(h_planificador->desc_nivel);
			index++;
		}

		list_iterate(list_plataforma, (void*) _list_elements);

}
