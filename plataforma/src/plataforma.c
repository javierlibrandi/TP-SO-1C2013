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
<<<<<<< HEAD
#include <string.h> //para funciones de cadena como strcpy
=======
#include <string.h> //para funcione de cadena como strcpy
>>>>>>> refs/remotes/origin/master
#include <mario_para_todos/grabar.h>

void libero_memoria(t_list *list_plataforma);
void creo_hilos_planificador(char *desc_nivel, t_list *list_plataforma,
		int sock);
void escucho_conexiones(const t_param_plat param_plataforma,
		t_list *list_plataforma);
void join_orquestador(t_list *list_plataforma); //pthread_join de los hilos orquestadores
<<<<<<< HEAD
bool existe_nivel(char *desc_nivel, t_list *list_plataforma);

=======
>>>>>>> refs/remotes/origin/master

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
<<<<<<< HEAD
			if (!existe_nivel(buffer, list_plataforma)) {
				creo_hilos_planificador(buffer, list_plataforma, new_sck);
				}
=======
			creo_hilos_planificador(buffer, list_plataforma, new_sck);
>>>>>>> refs/remotes/origin/master
			break;
		default:
			log_in_disk_plat(LOG_LEVEL_ERROR,
					"opcion en el switch no implementada", puerto);
			exit(1);
		}
		free(buffer);
	}
<<<<<<< HEAD
}

/////////////////////////////////////////////////////////////////////
///					creo_hilos_planificador						////
////////////////////////////////////////////////////////////////////

void creo_hilos_planificador(char *desc_nivel, t_list *list_plataforma,
		int sock) {

	pthread_t planificador_pthread;
	t_h_planificador *h_planificador;
	fd_set readfds;

	log_in_disk_plat(LOG_LEVEL_TRACE, "creo el planificador %s", desc_nivel);

	h_planificador = malloc(sizeof(t_h_planificador)); //recervo la memoria para almacenar el nuevo hilo

	h_planificador->desc_nivel = malloc(strlen(desc_nivel));
	strcpy(h_planificador->desc_nivel, desc_nivel); //agrego la des del nivel

	///////configuro el select() que despues voy a usar en el hilo////////
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	/*	IMPORTANTE QUE SEA UN PUNTERO ASI LO VEO EN EL SELECT() DEL HILO
	 CUANDO LO MODIFICO EN LA PLATAFORMA.
	 TAMBIEN HAGO UNA COPIA DE MEMORIA PARA NO PERDER EL VALOR.
	 NO OLVIDARCE DE HACER UN FREE EN libero_memoria
	 */
	h_planificador->sock = malloc(sizeof(int));
	memcpy(h_planificador->sock, &sock, sizeof(int));
	h_planificador->readfds = malloc(sizeof(fd_set));
	memcpy(h_planificador->readfds, &readfds, sizeof(fd_set));
	///////fin configuro el select() que despues voy a usar en el hilo////////

	//creo los hilos planificador
	pthread_create(&planificador_pthread, NULL, (void*) planificador_nivel_thr,
			(void*) h_planificador);

	h_planificador->planificador_thr = planificador_pthread;

	//agrego el nuevo hilo a la lista y muestro cuanto elemnetos tengo
	log_in_disk_plat(LOG_LEVEL_TRACE, "Elementos en la lista plataforma %d",
			list_add(list_plataforma, h_planificador));

}

/////////////////////////////////////////////////////////////////////
///					      libero_memoria						////
////////////////////////////////////////////////////////////////////

void libero_memoria(t_list *list_plataforma) {
	int index;
	index = 0;

	void _list_elements(t_h_planificador *h_planificador) {
		free(h_planificador->desc_nivel);
		free(h_planificador->readfds);
		free(h_planificador->sock);
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

=======
>>>>>>> refs/remotes/origin/master
}

/////////////////////////////////////////////////////////////////////
////					creo_hilos_planificador					////
////////////////////////////////////////////////////////////////////

void creo_hilos_planificador(char *desc_nivel, t_list *list_plataforma,
		int sock) {

	pthread_t planificador_pthread;
	t_h_planificador *h_planificador;
	fd_set readfds;

	log_in_disk_plat(LOG_LEVEL_TRACE, "creo el planificador %s", desc_nivel);

	h_planificador = malloc(sizeof(t_h_planificador)); //recervo la memoria para almacenar el nuevo hilo

	h_planificador->desc_nivel = malloc(strlen(desc_nivel));
	strcpy(h_planificador->desc_nivel, desc_nivel); //agrego la des del nivel

	///////configuro el select() que despues voy a usar en el hilo////////
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	h_planificador->sock = &sock; //IMPORTANTE QUE SEA UN PUNTERO ASI LO VEO EN EL SELECT() DEL HILO CUANDO LO MODIFICO EN LA PLATAFORMA
	h_planificador->readfds = &readfds;
	///////fin configuro el select() que despues voy a usar en el hilo////////

	/**
	 * creo los hilos planificador
	 */
	pthread_create(&planificador_pthread, NULL, (void*) planificador_nivel_thr,
			(void*) h_planificador);

	h_planificador->planificador_thr = planificador_pthread;

	//agrego el nuevo hilo a la lista
	log_in_disk_plat(LOG_LEVEL_TRACE, "Elementos en la lista plataforma %d",
			list_add(list_plataforma, h_planificador));
}

/////////////////////////////////////////////////////////////////////
///					      libero_memoria						////
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
///					      existe_nivel							////
////////////////////////////////////////////////////////////////////

bool existe_nivel(char *desc_nivel, t_list *list_plataforma) {

	if (list_is_empty(list_plataforma) == 1) {
		return false;
	}

	bool _list_elements(t_h_planificador *h_planificador) {

		if (!strcmp(h_planificador->desc_nivel, desc_nivel)) {
			log_in_disk_plat(LOG_LEVEL_WARNING,
					"Ya se declaro un nivel con el nombre %s", desc_nivel);
			return true;
		}
		return false;

	}

	return (bool*) list_find(list_plataforma, (void*) _list_elements);

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

