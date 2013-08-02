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
#include <netdb.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include <mario_para_todos/entorno.h>
#include "../planificador/planificador_thr.h"
#include <stdbool.h>

void *orequestador_thr(void* p) {
	t_h_orquestadro *t_h_orq = (t_h_orquestadro *) p;
	char *buffer;
	char *respuesta_recursos;
	//char respuesta_recursos[200];
	int i, j, k, sock_aux;
	int tipo;
	char **mensaje;
	//char *aux_char=NULL;
	int byteEnviados;
	char respuesta[100];
	t_h_planificador * h_planificador = NULL;
	t_personaje* pers = NULL, *per_aux = NULL;
	int indice_personaje, cantidad_Recurso_Aux;
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	char respuesta_recu_aux[10] = ("");
	int indice;
	/*//pongo el socket del nivel en el orquestador
	 if(*(t_h_orq->sock) < t_h_orq->sock_nivel){
	 *(t_h_orq->sock) = t_h_orq->sock_nivel;
	 }

	 FD_SET(t_h_orq->sock_nivel,t_h_orq->readfds); */
	//FD_SET(0, t_h_orq->readfds); // pongo a escuhcar la entrada estardar para que no se vuelve loco cuando se caen todas las conexiones
	log_in_disk_orq(LOG_LEVEL_TRACE, "creo el orquestador");

	for (;;) {

		pthread_mutex_lock(t_h_orq->reads_select);
		pthread_mutex_lock(t_h_orq->s_lista_plani);
		pthread_mutex_lock(t_h_orq->s_nuevos);
		FD_ZERO(t_h_orq->readfds);
		FD_SET(0, t_h_orq->readfds);
		int cant_planificadores = list_size(t_h_orq->planificadores);
		int cant_personajes_nuevos = list_size(t_h_orq->l_nuevos);
		int l, m;
		t_h_planificador * plani_aux;
		for (l = 0; l < cant_planificadores; l++) {

			plani_aux = list_get(t_h_orq->planificadores, l);
			FD_SET(plani_aux->sck_planificador, t_h_orq->readfds);
		}
		for (m = 0; m < cant_personajes_nuevos; m++) {
			pers = list_get(t_h_orq->l_nuevos, m);
			if (!pers->listo_para_planificar) {
				FD_SET(pers->sck, t_h_orq->readfds);
			}

		}
		pthread_mutex_unlock(t_h_orq->reads_select);
		pthread_mutex_unlock(t_h_orq->s_lista_plani);
		pthread_mutex_unlock(t_h_orq->s_nuevos);

		//pthread_mutex_lock(t_h_orq->reads_select);

		if (select(*(t_h_orq->sock) + 1, t_h_orq->readfds, NULL, NULL, &tv)
				== -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}
		//pthread_mutex_unlock(t_h_orq->reads_select);

		for (i = 0; i <= *(t_h_orq->sock); i++) {

			if (FD_ISSET(i, t_h_orq->readfds)) {

				buffer = recv_variable(i, &tipo); // *(t_h_orq->sock) Para mi es i el 1er parametro del rec por que el socket que me respondio tiene ese valor.
				if (!strcmp(buffer, Leido_error)) {

					log_in_disk_orq(LOG_LEVEL_ERROR, "%s ", Leido_error);

					if (busca_planificador_socket(i, t_h_orq->planificadores, //Miro si el socket del error es el de algun nivel/planificador.
							&h_planificador)) {

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

						imprimir_listas(t_h_orq, 'o');

						un_lock_listas_plataforma_orq(t_h_orq);

					}
					elimino_sck_lista(i, t_h_orq->readfds);
				}

				mensaje = string_split(buffer, ";");
				log_in_disk_orq(LOG_LEVEL_ERROR, "rev tipo de mensaje %d",
						tipo);
				log_in_disk_orq(LOG_LEVEL_INFO, "mensaje recibido %s", buffer);
				sleep(1);

				switch (tipo) {

				case N_TO_O_RECOVERY:

					log_in_disk_orq(LOG_LEVEL_INFO,
							"Se ha detectado un interbloqueo.");
					//el nivel pasa lista de personajes interbloqueados.
					//Se elije el de secuencia más baja y se le envía mensaje de muerte

					pthread_mutex_lock(t_h_orq->s_bloquedos);

					per_aux = NULL;
					for (l = 0; mensaje[l] != '\0'; l++) {

						pers = busca_personaje_simbolo_pla(mensaje[l][0],
								t_h_orq->l_bloquedos, &indice);
						if (per_aux == NULL ) {
							per_aux = pers;
						} else {
							if (per_aux->sec_entrada > pers->sec_entrada) {
								per_aux = pers;
							}
						}

					}
					pthread_mutex_lock(t_h_orq->s_deadlock);
					//TODO sacar la lista de deadlock
					mover_personaje_lista(per_aux->sck, t_h_orq->l_bloquedos,
							t_h_orq->l_deadlock);
					pthread_mutex_unlock(t_h_orq->s_bloquedos);
					pthread_mutex_unlock(t_h_orq->s_deadlock);

					fd_mensaje(i, O_TO_N_MUERTE,
							string_from_format("%c", per_aux->simbolo),
							&byteEnviados);

					usleep(200000);

					fd_mensaje(per_aux->sck, PL_TO_P_MUERTE, "moriste",
							&byteEnviados);

					buffer = recv_variable(per_aux->sck, &tipo);

					if (!strcmp(buffer, Leido_error)) {

						log_in_disk_orq(LOG_LEVEL_ERROR, "%s ", Leido_error);

						log_in_disk_orq(LOG_LEVEL_ERROR,
								"Error en el socket del nivel: %s, se mata el hilo ",
								h_planificador->desc_nivel);
						//Saco el planificador de la lista de planificadores
						pthread_mutex_lock(h_planificador->s_lista_plani);
						eliminar_planificador(i, t_h_orq->planificadores);
						pthread_mutex_unlock(t_h_orq->s_lista_plani);
						h_planificador->error_nivel = true; //marco el error en la bandera para que el planificador mate el hilo.

					}

					if (tipo == P_TO_PL_SALIR) {
						// ELIMINAR EL PERSONAJE DE TODAS LAS LISTAS. ( BLOQUEADOS O DEADLOCK)
					}

					if (tipo == P_TO_O_REINICIAR_NIVEL) {
						// PONER EL PERSONAJE EN LA COLA DE LISTOS.
					}

//					if (tipo == OK) {
//						usleep(200000);
//						fd_mensaje(per_aux->sck, PL_TO_P_MUERTE, "moriste",
//								&byteEnviados);
//
//					}

					break;

					/*case N_TO_O_PERS_SALIR:

					 log_in_disk_orq(LOG_LEVEL_INFO,
					 "Nivel informa que un personaje bloqueado reinicia el plan de niveles.");

					 pers = busca_personaje_simbolo_pla(mensaje[0][0],
					 t_h_orq->l_bloquedos, &indice);

					 lock_listas_plantaforma_orq(t_h_orq);

					 //sock_aux = pers->sck;

					 eliminar_personaje_termino_nivel(pers->sck,
					 t_h_orq->l_bloquedos);

					 un_lock_listas_plataforma_orq(t_h_orq);

					 fd_mensaje(i, OK,
					 "Personaje salió del planificador del nivel",
					 &byteEnviados);

					 close(pers->sck);

					 break;*/

				case N_TO_O_PERS_REINICIO:

					log_in_disk_orq(LOG_LEVEL_INFO,
							"Nivel informa que un personaje bloqueado reinicia el mapa.");

					lock_listas_plantaforma_orq(t_h_orq);
					pers = busca_personaje_simbolo_pla(mensaje[0][0],
							t_h_orq->l_bloquedos, &indice);

					mover_personaje_lista(pers->sck, t_h_orq->l_bloquedos,
							t_h_orq->l_listos);

					un_lock_listas_plataforma_orq(t_h_orq);

					log_in_disk_orq(LOG_LEVEL_INFO,
							"Se movió el personaje a listos para planificar.");
					fd_mensaje(i, OK, "Personaje movido a listos.",
							&byteEnviados);

					break;
				case N_TO_O_PERSONAJE_TERMINO_NIVEL:

					log_in_disk_orq(LOG_LEVEL_ERROR,
							"El personaje termina el nivel y libera los recuros");

					pthread_mutex_lock(t_h_orq->s_lista_plani);
					busca_planificador_socket(i, t_h_orq->planificadores,
							&h_planificador); // necesitmos el nombre del nivel
					pthread_mutex_unlock(t_h_orq->s_lista_plani);
					//*****el mensaje recibido es de tipo = "cantidad_tipos_recurso;Recurso1,Cantidad1;Recurso2,Cantidad2"

					log_in_disk_orq(LOG_LEVEL_ERROR,
							"Los recursos recibidos del nivel: %s son: %s",
							h_planificador->desc_nivel, buffer);

					//respuesta_recursos = malloc(sizeof(respuesta_recu_aux));
					//respuesta_recursos = malloc(3 * sizeof(char));
					//respuesta_recursos = "";
					respuesta_recursos = string_new();
					for (j = 0; j < atoi(mensaje[0]); j++) {

						lock_listas_plantaforma_orq(t_h_orq);

						imprimir_listas(t_h_orq, 'o');

						buscar_bloqueados_recurso(mensaje[j + 1],
								h_planificador->desc_nivel,
								t_h_orq->l_bloquedos, &pers);

						cantidad_Recurso_Aux = (mensaje[j + 1][2] - '0');

						log_in_disk_orq(LOG_LEVEL_ERROR,
								"Se puede liberar el recurso %c con la cantidad %d",
								mensaje[j + 1][0], cantidad_Recurso_Aux);

						for (k = 0;
								(pers != NULL )&& (k < cantidad_Recurso_Aux); k++){
								log_in_disk_orq(LOG_LEVEL_ERROR,
										"Hay personajes para desbloquear");
								if (!strcmp(respuesta_recursos, "")) {

									sprintf(respuesta_recu_aux, "%c,%c", pers->simbolo, mensaje[j + 1][0]);
									string_append(&respuesta_recursos,respuesta_recu_aux);
								} else {
									sprintf(respuesta_recu_aux, ";%c,%c", pers->simbolo, mensaje[j + 1][0]);
									string_append(&respuesta_recursos,respuesta_recu_aux);
								}

								mover_personaje_lista(pers->sck,
										t_h_orq->l_bloquedos, t_h_orq->l_listos);

								imprimir_listas(t_h_orq,'o');

								buscar_bloqueados_recurso(mensaje[j + 1],
										h_planificador->desc_nivel,
										t_h_orq->l_bloquedos, &pers);

							}
						un_lock_listas_plataforma_orq(t_h_orq);

					}

					if (!strcmp(respuesta_recursos, "")) {
						fd_mensaje(i, O_TO_N_ASIGNAR_RECURSOS_null,
								"ESO YA LO HABLAMOS", &byteEnviados);
					} else {
						fd_mensaje(i, O_TO_N_ASIGNAR_RECURSOS,
								respuesta_recursos, &byteEnviados);
						log_in_disk_orq(LOG_LEVEL_INFO,
								"Lo recursos asignados son los siguientes (ej: personaje;recurso) %s",
								respuesta_recursos);
					}
					//free(respuesta_recursos);
					break;

				case P_TO_O_PROX_NIVEL:
					log_in_disk_orq(LOG_LEVEL_INFO,
							"El personaje %s, pide el nivel %s,", mensaje[0],
							mensaje[1]);

					pthread_mutex_lock(t_h_orq->s_lista_plani);

					if (busca_planificador_2(mensaje[1],
							t_h_orq->planificadores, respuesta,
							h_planificador)) {

						log_in_disk_orq(LOG_LEVEL_INFO, "%s", respuesta);

						log_in_disk_orq(LOG_LEVEL_INFO,
								"elimino el socket del personaje de los reads %s",
								respuesta);
						fd_mensaje(i, O_TO_P_UBIC_NIVEL, respuesta,
								&byteEnviados);

						//recv_variable(i,&tipo); //TODO Controlar el error. // Esperamos

						pthread_mutex_lock(t_h_orq->reads_select);
						pthread_mutex_lock(t_h_orq->s_nuevos);
						busca_personaje_skc(i, t_h_orq->l_nuevos,
								&indice_personaje)->listo_para_planificar =
								true;
						pthread_mutex_unlock(t_h_orq->s_nuevos);
						elimino_sck_lista(i, t_h_orq->readfds);

						pthread_mutex_unlock(t_h_orq->reads_select);

					} else {
						fd_mensaje(i, ERROR, respuesta, &byteEnviados);

					}
					pthread_mutex_unlock(t_h_orq->s_lista_plani);
					break;

				default:
					log_in_disk_orq(LOG_LEVEL_ERROR,
							"opcion en el switch no implementada", tipo);
					//exit(1);
				}
				free(buffer);
				//free(mensaje);
			}
		}

		//limppio la lista de socket y Agrego de nuevo el socket de los niveles y los personajes que estan en la lista de nuevos.

	} ///TODO no agregar los socket de personajes o niveles que dieron error.

	pthread_exit(EXIT_SUCCESS);
}

bool busca_planificador_2(char *desc_nivel, t_list *list_plataforma, char * msj,
		t_h_planificador* h_planificador) {

	log_in_disk_orq(LOG_LEVEL_TRACE, "Busco el planificador de nivel: %s \t",
			desc_nivel);
	struct hostent *dirIP;
	int cant_planificadores, i;
	//t_h_planificador * un_planificador;
	if (list_is_empty(list_plataforma)) {
		msj = "ERROR: Planificador de Nivel no encontrado";
		return false;
	}

	cant_planificadores = list_size(list_plataforma);

	for (i = 0; i < cant_planificadores; i++) {
		h_planificador = list_get(list_plataforma, i);

		if (string_equals_ignore_case(h_planificador->desc_nivel, desc_nivel)) {

			//PROBAR CON PLATAFORMA Y NIVEL EN UNA TERMINAL Y PERSONAJE EN OTRA
			if (!strcmp(h_planificador->ip, "127.0.0.1")) {
				log_in_disk_orq(LOG_LEVEL_INFO,
						"Se convierte localhost a ip pública");
				dirIP = gethostbyname(h_planificador->ip);
				sprintf(msj, "%s;%s", dirIP->h_name, h_planificador->puerto);
				log_in_disk_orq(LOG_LEVEL_TRACE,
						"Los datos del nivel son ip: %s, puerto: %s ",
						dirIP->h_name, h_planificador->puerto);
			} else {

				sprintf(msj, "%s;%s", h_planificador->ip,
						h_planificador->puerto);

				log_in_disk_orq(LOG_LEVEL_TRACE,
						"Los datos del nivel son ip: %s, puerto: %s ",
						h_planificador->ip, h_planificador->puerto);
			}

			return true;

		}

	}
	msj = "ERROR: Planificador de Nivel no encontrado";
	return false;

}
bool busca_planificador(char *desc_nivel, t_list *list_plataforma, char * msj) {

	log_in_disk_orq(LOG_LEVEL_TRACE, "busco el planificador de nivel: %s \t",
			desc_nivel);

//if (list_is_empty(list_plataforma) == 1) {
	if (list_is_empty(list_plataforma)) {
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
		t_h_planificador ** planificador) {

	log_in_disk_orq(LOG_LEVEL_TRACE, "busco planificador por socket.: ");

	if (list_is_empty(list_plataforma) == 1) {
		return false;
	}

	bool _list_elements(t_h_planificador *h_planificador) {

		if (h_planificador->sck_planificador == sock) {

			log_in_disk_orq(LOG_LEVEL_TRACE,
					"Planificador del nivel: %s Encontrado.",
					h_planificador->desc_nivel);
			*planificador = h_planificador;
			return true;

		} else {

			return false;
		}
	}

	return (bool*) list_find(list_plataforma, (void*) _list_elements);

}

//el 1er parametro de la funcion es un char* de tipo "recurso,cantidad" por ej: "F,3".
void buscar_bloqueados_recurso(char * recur, char * nivel, t_list* bloqueados,
		t_personaje **pers) {
	// recur Viene dado como: "recurso,cantidad"

	char recurso = recur[0];
	int total_bloqueados = list_size(bloqueados);
	int count;
	t_personaje *unper;

	for (count = 0; count < total_bloqueados; count++) {

		unper = list_get(bloqueados, count);

		if (!strcmp(unper->nivel, nivel) && (unper->prox_recurso == recurso)) {
			*pers = unper;
			return;
		}
	}
	*pers = NULL;
}

