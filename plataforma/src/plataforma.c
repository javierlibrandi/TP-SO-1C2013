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
#include  <commons/log.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <string.h> //para funcione de cadena como strcpy
#include <mario_para_todos/grabar.h>

void libero_memoria(t_list *list_plataforma);
void creo_hilos_planificador(char *desc_nivel, t_list *list_plataforma,
		int sock);
void escucho_conexiones(const t_param_plat param_plataforma,
		t_list *list_plataforma);
void join_orquestador(t_list *list_plataforma); //pthread_join de los hilos orquestadores

int main(void) {

	t_param_plat param_plataforma;
	pthread_t orquestador_thr;
	t_list *list_plataforma = list_create(); //creo lista de hilos

	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();

	/**
	 * creo el hilo orquetador
	 */
	pthread_create(&orquestador_thr, NULL, (void *) orequestador_thr, NULL );

	escucho_conexiones(param_plataforma, list_plataforma);

	pthread_join(orquestador_thr, NULL );
	join_orquestador(list_plataforma);

	libero_memoria(list_plataforma);
	list_destroy(list_plataforma);

	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
///					  escucho_conexiones						////
////////////////////////////////////////////////////////////////////

void escucho_conexiones(const t_param_plat param_plataforma,
		t_list *list_plataforma) {
	int sck, new_sck;
	void *buffer = NULL;
	int puerto = param_plataforma.PUERTO;
	int tipo;

	log_in_disk_plat(LOG_LEVEL_TRACE, "escucho conexiones en el puerto %d",
			puerto);

	sck = Abre_Socket_Inet(puerto);

	for (;;) {
		new_sck = Acepta_Conexion_Cliente(sck);
		buffer = recv_variable(new_sck, &tipo);

		switch (tipo) {
		case SALUDO_PERSONAJE:
			//agrego del descirptor del socket para que hable con el oruqetador
		case SALUDO_NIVEL: //creo el planificador del nivel
			creo_hilos_planificador(buffer, list_plataforma, new_sck);
			break;
		default:
			log_in_disk_plat(LOG_LEVEL_ERROR,
					"opcion en el switch no implementada", puerto);
			exit(1);
		}
		free(buffer);
	}
}

/////////////////////////////////////////////////////////////////////
///					creo_hilos_planificador						////
////////////////////////////////////////////////////////////////////

void creo_hilos_planificador(char *desc_nivel, t_list *list_plataforma,
		int sock) {

	pthread_t planificador_thr;
	t_h_planificador *h_planificador;
	int tot_lista;

	log_in_disk_plat(LOG_LEVEL_TRACE, "creo el planificador %s", desc_nivel);

	h_planificador = malloc(sizeof(t_h_planificador)); //recervo la memoria para almacenar el nuevo hilo

	h_planificador->desc_nivel = malloc(strlen(desc_nivel));
	strcpy(h_planificador->desc_nivel, desc_nivel); //agrego la des del nivel


	h_planificador->sock = sock;

	/**
	 * creo los hilos planificador
	 */
	pthread_create(&planificador_thr, NULL, (void*) planificador_nivel_thr,
			(void*) h_planificador);

	h_planificador->planificador_thr = planificador_thr;

	tot_lista = list_add(list_plataforma, h_planificador); //agrego el nuevo hilo a la lista
	log_in_disk_plat(LOG_LEVEL_TRACE, "Elementos en la lista plataforma %d",
			tot_lista);
}

/////////////////////////////////////////////////////////////////////
///					      libero_memoria						////
////////////////////////////////////////////////////////////////////

void libero_memoria(t_list *list_plataforma) {
	int index;
	index = 0;

	void _list_elements(t_h_planificador *h_planificador) {
		free(h_planificador->desc_nivel);
		index++;
	}

	list_iterate(list_plataforma, (void*) _list_elements);

}

/////////////////////////////////////////////////////////////////////
///					      join_orquestador						////
////////////////////////////////////////////////////////////////////

//pthread_join de los hilos orquestadores
void join_orquestador(t_list *list_plataforma) {
	int index = 0;

	void _list_elements(t_h_planificador *h_planificador) {

		pthread_join(h_planificador->planificador_thr, NULL );

		index++;
	}

	list_iterate(list_plataforma, (void*) _list_elements);

}

