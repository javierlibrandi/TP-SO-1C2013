/*
 * orquestador_thr.c
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/grabar.h>
#include <commons/config.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include "orquestador_thr.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include <mario_para_todos/entorno.h>
#include "../planificador/planificador_thr.h"
#include <stdbool.h>

void *orequestador_thr(void* p) {
	t_h_orquestadro *t_h_orq = (t_h_orquestadro *) p;
	char *buffer;
	int i;
	int tipo;
	char **mensaje;
	//char *aux_char=NULL;
	int byteEnviados;
	char respuesta[100];
	t_h_planificador * h_planificador;

	/*//pongo el socket del nivel en el orquestador
	 if(*(t_h_orq->sock) < t_h_orq->sock_nivel){
	 *(t_h_orq->sock) = t_h_orq->sock_nivel;
	 }

	 FD_SET(t_h_orq->sock_nivel,t_h_orq->readfds); */
	FD_SET(0, t_h_orq->readfds); // pongo a escuhcar la entrada estardar para que no se vuelve loco cuando se caen todas las conexiones

	log_in_disk_orq(LOG_LEVEL_TRACE, "creo el orquestador");
	for (;;) {
		if (select(*(t_h_orq->sock) + 1, t_h_orq->readfds, NULL, NULL, NULL )
				== -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i <= *(t_h_orq->sock); i++) {
			if (FD_ISSET(i, t_h_orq->readfds)) {

				buffer = recv_variable(i, &tipo); // *(t_h_orq->sock) Para mi es i el 1er parametro del rec por que el socket que me respondio tiene ese valor.
				if (!strcmp(buffer, Leido_error)) {

					log_in_disk_orq(LOG_LEVEL_ERROR, "%s ", Leido_error);

					if (busca_planificador_socket(i, t_h_orq->planificadores, //Miro si el socket del error es el de algun nivel/planificador.
							h_planificador)) {

						log_in_disk_orq(LOG_LEVEL_ERROR,
								"Error en el socket del nivel: %s, se mata el hilo ",
								h_planificador->desc_nivel);
						//Saco el planificador de la lista de planificadores
						pthread_mutex_lock(h_planificador->s_lista_plani);
						eliminar_planificador(i, t_h_orq->planificadores);
						pthread_mutex_unlock(t_h_orq->s_lista_plani);
						h_planificador->error_nivel = true; //marco el error en la bandera para que el planificador mate el hilo.

					} else {
						//TODO Loquear que personaje es el que tuvo el error
						lock_listas_plantaforma_orq(t_h_orq);
						mover_personaje_lista(i, t_h_orq->l_listos,
								t_h_orq->l_errores);
						mover_personaje_lista(i, t_h_orq->l_bloquedos,
								t_h_orq->l_errores);
						un_lock_listas_plataforma_orq(t_h_orq);

					}
					elimino_sck_lista(i, t_h_orq->readfds);
				}

				mensaje = string_split(buffer, ";");

				switch (tipo) {
				case P_TO_O_PROX_NIVEL:

					pthread_mutex_lock(t_h_orq->s_lista_plani);

					if (busca_planificador(mensaje[1], t_h_orq->planificadores,
							respuesta)) {
						pthread_mutex_unlock(t_h_orq->s_lista_plani);

						fd_mensaje(i, O_TO_P_UBIC_NIVEL, respuesta,
								&byteEnviados);

						log_in_disk_orq(LOG_LEVEL_TRACE,
								"datos del nivel enviados: %s", respuesta);

//						fd_mensaje(i, O_TO_P_DESCONEXTAR_OREQUESTADOR, respuesta,&byteEnviados); //El personaje cierra la conexion si preguntar, esta perfeto aca tambien lo cierro

						elimino_sck_lista(i, t_h_orq->readfds);

						log_in_disk_orq(LOG_LEVEL_ERROR,
								"cierro la conexion con el personaje");

					} else {
						fd_mensaje(i, ERROR, respuesta, &byteEnviados);
					}

					break;

				default:
					log_in_disk_orq(LOG_LEVEL_ERROR,
							"opcion en el switch no implementada", tipo);
					//exit(1);
				}
				free(buffer);

			}
		}

	}
	pthread_exit(EXIT_SUCCESS);
}

bool busca_planificador(char *desc_nivel, t_list *list_plataforma, char * msj) {

	log_in_disk_orq(LOG_LEVEL_TRACE, "busco el planificador de nivel: %s \t",
			desc_nivel);

	if (list_is_empty(list_plataforma) == 1) {
		return false;
	}

	bool _list_elements(t_h_planificador *h_planificador) {

		if (string_equals_ignore_case(h_planificador->desc_nivel, desc_nivel)) {
			//if (!strcmp(string_to_lower (h_planificador->desc_nivel), desc_nivel)) {

			sprintf(msj, "%s;%s", h_planificador->ip, h_planificador->puerto);

			log_in_disk_orq(LOG_LEVEL_TRACE,
					"Los datos del nivel son ip: %s, puerto: %s ",
					h_planificador->ip, h_planificador->puerto);

			return true;

		} else {
			msj = "ERROR: Planificador de Nivel no encontrado";
			return false;
		}
	}

	return (bool*) list_find(list_plataforma, (void*) _list_elements);

}

bool busca_planificador_socket(int sock, t_list *list_plataforma,
		t_h_planificador * planificador) {

	log_in_disk_orq(LOG_LEVEL_TRACE,
			"busco si el socket del error es del nivel de algun planificador.: ");

	if (list_is_empty(list_plataforma) == 1) {
		return false;
	}

	bool _list_elements(t_h_planificador *h_planificador) {

		if (h_planificador->sck_planificador == sock) {

			log_in_disk_orq(LOG_LEVEL_TRACE,
					"El error es del socket del planificador del nivel %s",
					h_planificador->desc_nivel);
			planificador = h_planificador;
			return true;

		} else {

			return false;
		}
	}

	return (bool*) list_find(list_plataforma, (void*) _list_elements);

}
