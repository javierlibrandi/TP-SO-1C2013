/*
 * nivel.c
 *
 *  Created on: 08/05/2013
 *      Author: utnso
 *      cambio
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <commons/string.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/ambiente_config.h>
#include "conect_plataforma/conect_plataforma.h"
#include "escuchar_personaje/personaje_thr.h"
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include "manjo_pantalla/pantalla.h"
#include <signal.h>
#include <mario_para_todos/entorno.h>
#include "manjo_pantalla/nivel_p.h"
#include <semaphore.h>
#include <commons/collections/list.h>
#include "manjo_pantalla/tad_items.h"
#include "detecto_interbloque_th/detecto_interbloque_th.h"
#include <pthread.h>

void desbloquear_Personajes(char * recursos_personaje, char *buffer, int tipo,
		t_h_personaje * t_personaje, struct h_t_param_nivel param_nivel);

t_lista_personaje *busco_personaje(int sck, t_list *l_personajes, int *i);
void add_recurso_personaje(t_list *l_recursos_optenidos,
		struct h_t_recusos *recurso_actual);
void elimino_personaje_lista_nivel(int sck, t_list *l_personajes,
		ITEM_NIVEL *item);
void liberar_memoria(t_lista_personaje *personaje, ITEM_NIVEL *item);
void liberar_recursos(t_list *recursos_otenido, ITEM_NIVEL *item);
char* listarRecursosPersonaje(t_list * lista_Recursos);
t_lista_personaje *busca_personaje_simbolo(char id, t_list *l_personajes,
		int *indice_personaje);
void imprimir_recursos(t_list * lista_Recursos);

//void libero_memoria(t_h_personaje *t_personaje, struct t_param_nivel *param_nivel);
void sig_handler(int signo);
void imprmir_recursos_nivel(t_list * recursos);
static pthread_mutex_t s_personaje_conectado = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_personaje_recursos = PTHREAD_MUTEX_INITIALIZER;
ITEM_NIVEL *ListaItems = NULL;

int main(void) {
	struct h_t_param_nivel param_nivel;
	int sck_plat;
	pthread_t escucho_personaje_th, detecto_interbloque_th;
	t_h_personaje * t_personaje = malloc(sizeof(t_h_personaje));
	t_h_interbloqueo h_interbloqueo;
	fd_set readfds;
	int i, tipo, rows, cols, posX, posY;
	char *buffer;
	rows = 37;
	cols = 167;
	char **mensaje;
	int tot_enviados, cont_msj;
	char *aux_mensaje;
	struct h_t_recusos *recurso;
	//ITEM_NIVEL *ListaItems = NULL;
	t_lista_personaje *nodo_lista_personaje;
	int tipo_mensaje, catidad_recursos;
	char *nombre_recurso;
	char *recursos_personaje;
	int pos; //la posicion en la lista de un personaje
	int cant_presonajes_conectados = 0;
	int j;
	t_lista_personaje * un_per;
	t_lista_personaje personaje_aux;

	if (B_DIBUJAR) {
		inicializo_pantalla();
		nivel_gui_get_area_nivel(&rows, &cols);
		log_in_disk_niv(LOG_LEVEL_INFO, "rows %d cols %d", rows, cols);

	}
	param_nivel = leer_nivel_config(rows, cols);

	t_personaje = malloc(sizeof(t_h_personaje));
	t_personaje->nomb_nivel = param_nivel.nom_nivel;
	t_personaje->pueto = param_nivel.PUERTO;
	t_personaje->l_personajes = list_create();
	t_personaje->s_personaje_recursos = &s_personaje_recursos;
	recusos_pantalla(param_nivel.recusos, &ListaItems);

	if (B_DIBUJAR) {
		nivel_gui_dibujar(ListaItems);
	}

//conecxion con el planificador
	sck_plat = con_pla_nival(param_nivel.IP, param_nivel.PUERTO_PLATAFORMA,
			param_nivel.nom_nivel, param_nivel.PUERTO);

	FD_ZERO(&readfds);
	FD_SET(sck_plat, &readfds);
	t_personaje->readfds = &readfds;
	t_personaje->sck_personaje = sck_plat;
	t_personaje->s_personaje_conectado = &s_personaje_conectado;
	t_personaje->ListaItemss = &ListaItems;
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 50;

	pthread_mutex_lock(&s_personaje_conectado);

	//creo el hilo que va a escuchar conexiones del personaje
	pthread_create(&escucho_personaje_th, NULL, (void*) escucho_personaje,
			(void*) t_personaje);
	//	pthread_mutex_lock(&s_personaje_conectado);

	//setteo las estructura para pasar al hilo
	memcpy(&h_interbloqueo.t_personaje, t_personaje, sizeof(t_h_personaje));
	h_interbloqueo.param_nivel = param_nivel;
	//creo el hilo para la deteccion de interbloqueo
	//pthread_create(&detecto_interbloque_th, NULL, (void*) detecto_interbloque,
	//	(void*) &h_interbloqueo);
	imprmir_recursos_nivel(param_nivel.recusos);

	for (;;) {

		FD_ZERO(t_personaje->readfds);
		FD_SET(sck_plat, t_personaje->readfds);
		cant_presonajes_conectados = list_size(t_personaje->l_personajes);

		for (j = 0; j < cant_presonajes_conectados; j++) {
			un_per = list_get(t_personaje->l_personajes, j);
			FD_SET(un_per->sokc, t_personaje->readfds);

		}

		if (select(t_personaje->sck_personaje + 1, t_personaje->readfds, NULL,
				NULL, &tv) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i <= t_personaje->sck_personaje; i++) {
			if (FD_ISSET(i, t_personaje->readfds)) {

				buffer = recv_variable(i, &tipo);
				if (!strcmp(buffer, Leido_error)) {
					pthread_mutex_lock(&s_personaje_recursos);
					elimino_personaje_lista_nivel(i, t_personaje->l_personajes,
							ListaItems);
					elimino_sck_lista(i, t_personaje->readfds);
					pthread_mutex_unlock(&s_personaje_recursos);
				}

				log_in_disk_niv(LOG_LEVEL_INFO,
						"****** Se recibió un tipo de mensaje %d: %s ******",
						tipo, buffer);

				mensaje = string_split(buffer, ";");
				switch (tipo) {
				case P_TO_N_UBIC_RECURSO:

					pthread_mutex_lock(&s_personaje_recursos);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);
					log_in_disk_niv(LOG_LEVEL_INFO,
							"%s solicita ubicación del recurso %c ",
							nodo_lista_personaje->nombre_personaje,
							mensaje[0][0]);
					//para pruebas
					//imprmir_recursos_nivel(param_nivel.recusos);
					recurso = busco_recurso(mensaje[0][0], param_nivel.recusos);
					//para pruebas
					//imprmir_recursos_nivel(param_nivel.recusos);
					nodo_lista_personaje->proximo_recurso = recurso; //lo relaciono con el proximo recuros que tiene que obtener
					pthread_mutex_unlock(&s_personaje_recursos);

					log_in_disk_niv(LOG_LEVEL_INFO, "encontro recurso: %c ",
							recurso->SIMBOLO);

					aux_mensaje = string_from_format("%d;%d", recurso->posX,
							recurso->posY);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"despues de string from format");

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se enviaron coordenadas: (%d,%d)", recurso->posX,
							recurso->posY);

					fd_mensaje(i, N_TO_P_UBIC_RECURSO, aux_mensaje,
							&tot_enviados);
					//busco el personaje que me solicita el recurso

					break;
				case P_TO_N_MOVIMIENTO:
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Nivel recibe solicitud de movimiento");

					posX = atoi(mensaje[3]);
					posY = atoi(mensaje[4]);
					pthread_mutex_lock(&s_personaje_recursos);
					MoverPersonaje(ListaItems, mensaje[0][0], posX, posY);
					//la informacion del la posicion la necesito para determinar el interbloqueo
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);
					nodo_lista_personaje->posX = posX;
					nodo_lista_personaje->posX = posY;
					pthread_mutex_unlock(&s_personaje_recursos);
					if (B_DIBUJAR) {
						nivel_gui_dibujar(ListaItems);
					}

					aux_mensaje = "te movi";
					fd_mensaje(i, N_TO_P_MOVIDO, aux_mensaje, &tot_enviados);
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se envió confirmación de movimiento a %s a (%d,%d)",
							nodo_lista_personaje->nombre_personaje, posX, posY);

					break;

				case P_TO_N_OBJ_CUMPLIDO:

					recursos_personaje = "";
					pthread_mutex_lock(&s_personaje_recursos);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					recursos_personaje = listarRecursosPersonaje(
							nodo_lista_personaje->l_recursos_optenidos);
					pthread_mutex_unlock(&s_personaje_recursos);

					personaje_aux.id_personaje =
							nodo_lista_personaje->id_personaje;
					personaje_aux.nombre_personaje = malloc(
							strlen(nodo_lista_personaje->nombre_personaje) + 1);
					strcpy(personaje_aux.nombre_personaje,
							nodo_lista_personaje->nombre_personaje);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Notificación de objetivo de nivel cumplido de %s",
							nodo_lista_personaje->nombre_personaje);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"El personaje %s ha completado el nivel y libera estos recursos: %s. Se informa al orquestador.",
							nodo_lista_personaje->nombre_personaje,
							recursos_personaje);

					fd_mensaje(i, OK, "Fin de nivel", &tot_enviados);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se envió OK de fin de nivel al personaje");
					//La funcion el "elimino_personaje_lista_nivel" ya suma los recursos que se liberaron y tambien actualiza los recursos en pantalla.
					pthread_mutex_lock(&s_personaje_recursos);
					elimino_personaje_lista_nivel(i, t_personaje->l_personajes,
							ListaItems);
					pthread_mutex_unlock(&s_personaje_recursos);
					elimino_sck_lista(i, t_personaje->readfds);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Los recursos del nivel: %s despues de liberar el personaje: %s %c son los siguietes: (proximo log)",
							param_nivel.nom_nivel,
							personaje_aux.nombre_personaje,
							personaje_aux.id_personaje);

					imprmir_recursos_nivel(param_nivel.recusos);
					//Informo al orquestador los recursos liberados

//					desbloquear_Personajes(recursos_personaje, buffer, tipo,
//							t_personaje, param_nivel);

					fd_mensaje(sck_plat, N_TO_O_PERSONAJE_TERMINO_NIVEL,
							recursos_personaje, &tot_enviados); // Respuesta_PErsonaje = "cantidad_tipos_recurso;Recurso1,Cantidad1;Recurso2,Cantidad2"

					//Espero la respuesta del orquestador con los recursos que asigno
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se espera respuesta del orquestador con los recursos asignados...");

					buffer = recv_variable(sck_plat, &tipo);

					if (!strcmp(buffer, Leido_error)) {
						log_in_disk_niv(LOG_LEVEL_ERROR,
								"Hubo un error en la lectura del socket de la plataforma.");

						exit(EXIT_FAILURE);
					}

					switch (tipo) {

					case O_TO_N_ASIGNAR_RECURSOS:
						mensaje = string_split(buffer, ";");

						log_in_disk_niv(LOG_LEVEL_INFO,
								"Se recibieron los siguientes recursos asignados desde el orquestador: %s .",
								buffer);

						for (cont_msj = 0; mensaje[cont_msj] != '\0';
								cont_msj++) {

							log_in_disk_niv(LOG_LEVEL_INFO,
									"Se recibio el recurso asignado desde el orquestador a un personaje (personaje,Recurso): %s",
									mensaje[cont_msj]);
							pthread_mutex_lock(&s_personaje_recursos);
							nodo_lista_personaje = busca_personaje_simbolo(
									mensaje[cont_msj][0],
									t_personaje->l_personajes, &pos);

							recurso = busco_recurso(mensaje[cont_msj][2],
									nodo_lista_personaje->l_recursos_optenidos);

							if (recurso != NULL ) { //agreo a la lista de recursos asignados al personaje
								recurso->cantidad++; //si esta en la lista le agrego una instancia el recurso que ya tiene el personaje
							} else {
								add_recurso_personaje(
										nodo_lista_personaje->l_recursos_optenidos,
										nodo_lista_personaje->proximo_recurso);

							}
							//TODO MArcar el personaje para que no se tenga en cuenta en el desbloqueo por que ya no tiene proximo recurso. (Hasta que vuevla a pedir uno).
							//Recursos despues de la asignacion del orquestador:
							recurso = busco_recurso(mensaje[cont_msj][2],
									param_nivel.recusos);
							//Resto el recurso asignado
							recurso->cantidad--;
							//Resto el recurso asignado en la pantalla
							if (B_DIBUJAR) {
								restarRecurso(ListaItems,
										nodo_lista_personaje->proximo_recurso->SIMBOLO);
							}
							pthread_mutex_unlock(&s_personaje_recursos);

							log_in_disk_niv(LOG_LEVEL_INFO,
									"el orquestador asigno el siguiente recurso (%c --> %c) ",
									nodo_lista_personaje->id_personaje,
									recurso->SIMBOLO);

							//imprmir_recursos_nivel(param_nivel.recusos);

// Comenteado para pruebas *** //
//							fd_mensaje(nodo_lista_personaje->sokc,
//									N_TO_P_PROXIMO_RECURSO, buffer,
//									&tot_enviados);
//
//							buffer = recv_variable(nodo_lista_personaje->sokc,
//									&tipo);
//
//							busco_recurso(buffer[0], param_nivel.recusos);
//							nodo_lista_personaje->proximo_recurso = recurso;

						}
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Los recursos del nivel despues de las asignaciones del orquestador son: (Proximo_Log)");
						imprmir_recursos_nivel(param_nivel.recusos);

						break;

					case O_TO_N_ASIGNAR_RECURSOS_null:
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Se recibieron los recursos asignados desde el orquestador pero no se desbloqueo ningun personaje");

//						elimino_personaje_lista_nivel(i,
//								t_personaje->l_personajes, ListaItems);

						//liberar_memoria(nodo_lista_personaje); // ya se libera la memoria adentro de la funcion anterior (elimino_personaje_lista_nivel)
//						elimino_sck_lista(i, t_personaje->readfds);

						break;
					}

					if (B_DIBUJAR) {
						pthread_mutex_lock(&s_personaje_recursos);
						BorrarItem(ListaItems,
								nodo_lista_personaje->id_personaje);
						nivel_gui_dibujar(ListaItems);
						pthread_mutex_unlock(&s_personaje_recursos);
					}

					//free(recursos_personaje);
					imprimir_recursos(param_nivel.recusos);
					free(personaje_aux.nombre_personaje);
					break;

				case P_TO_N_REINICIAR_NIVEL:
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se recibió un mensaje P_TO_N_REINICIAR_NIVEL");

					log_in_disk_niv(LOG_LEVEL_INFO,
							"****** El personaje %c reinicia el nivel. ******",
							mensaje[0][0]);

					pthread_mutex_lock(&s_personaje_recursos);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					if (B_DIBUJAR) {
						BorrarItem(&ListaItems,
								nodo_lista_personaje->id_personaje);
					}
					imprmir_recursos_nivel(param_nivel.recusos);

					liberar_recursos(nodo_lista_personaje->l_recursos_optenidos,
							ListaItems);

					imprmir_recursos_nivel(param_nivel.recusos);

					log_in_disk_niv(LOG_LEVEL_INFO, "Liberó recursos.");
					//Desbloquear personajes

					nodo_lista_personaje->l_recursos_optenidos = list_create();

					personaje_pantalla(mensaje[0][0], 1, 1, &ListaItems);

					add_personaje_lista(mensaje[0][0], mensaje[0], i,
							t_personaje);

					pthread_mutex_unlock(&s_personaje_recursos);
					tipo_mensaje = OK;

					fd_mensaje(i, tipo_mensaje, "Nivel re-iniciado.",
							&tot_enviados);
					break;

//					log_in_disk_niv(LOG_LEVEL_INFO,
//							"El personaje %c debe reiniciar el nivel.",
//							mensaje[0][0]);
//					log_in_disk_niv(LOG_LEVEL_INFO,
//							"Se lo reubica en la posición (1,1) del mapa y se liberan sus recursos.");
//
//					MoverPersonaje(ListaItems, mensaje[0][0], 1, 1);
//
//					//Liberar recursos y desbloquear personajes
//
//					break;

				case P_TO_N_SOLIC_RECURSO:

					log_in_disk_niv(LOG_LEVEL_INFO,
							"****** Nivel recibe solicitud de instancia de recurso ******");

					log_in_disk_niv(LOG_LEVEL_INFO,
							"El personaje solicita un recurso");
					pthread_mutex_lock(&s_personaje_recursos);

					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					catidad_recursos =
							nodo_lista_personaje->proximo_recurso->cantidad;
					nombre_recurso =
							nodo_lista_personaje->proximo_recurso->NOMBRE;

					log_in_disk_niv(LOG_LEVEL_INFO,
							"El personaje %s solicita un recurso del tipo %s. La cantidad actual es de %d.",
							nodo_lista_personaje->nombre_personaje,
							nombre_recurso, catidad_recursos);

					if (catidad_recursos > 0) { //si tengo recursos se los doy

						log_in_disk_niv(LOG_LEVEL_INFO,
								"Hay instancias disponibles de %s",
								nombre_recurso);

						catidad_recursos--;
						tipo_mensaje = N_TO_P_RECURSO_OK;
						fd_mensaje(i, tipo_mensaje,
								"LLEGASTE AL RECURSO, EN HORA BUENA!!!",
								&tot_enviados);
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Se envió N_TO_P_RECURSO_OK");

						log_in_disk_niv(LOG_LEVEL_INFO,
								"****** El recurso %s es entregado al personaje %c ******",
								nombre_recurso,
								nodo_lista_personaje->id_personaje);
						//para pruebas
						imprmir_recursos_nivel(param_nivel.recusos);
						recurso = busco_recurso(
								nodo_lista_personaje->proximo_recurso->SIMBOLO,
								nodo_lista_personaje->l_recursos_optenidos);

						log_in_disk_niv(LOG_LEVEL_INFO,
								"Cantidad de recursos después de asignar uno al personaje: %d",
								catidad_recursos);
						imprmir_recursos_nivel(param_nivel.recusos);

						if (B_DIBUJAR) {

							restarRecurso(ListaItems,
									nodo_lista_personaje->proximo_recurso->SIMBOLO);
						}
						if (recurso != NULL ) { //agrego a la lista de recursos asignados al personaje
							recurso->cantidad++; //si esta en la lista le agrego una instancia el recurso que ya tiene el personaje
						} else {
							add_recurso_personaje(
									nodo_lista_personaje->l_recursos_optenidos,
									nodo_lista_personaje->proximo_recurso);
						}

					} else { // en caso de no tener bloqueo al personaje
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Actualmente no hay instancias disponibles de %s",
								nombre_recurso);

						tipo_mensaje = N_TO_P_RECURSO_ERROR;

						fd_mensaje(i, tipo_mensaje,
								"No hay instancias disponibles. Tendrás que esperar :P!!!",
								&tot_enviados);
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Se envió N_TO_P_RECURSO_ERROR");

					}

					nodo_lista_personaje->proximo_recurso->cantidad =
							catidad_recursos;
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Cantidad de recursos después de la solicitud: %d",
							nodo_lista_personaje->proximo_recurso->cantidad);

					pthread_mutex_unlock(&s_personaje_recursos);
					break;

				case P_TO_N_SALIR:
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se recibió un mensaje P_TO_N_SALIR");

					log_in_disk_niv(LOG_LEVEL_INFO,
							"****** El personaje %c sale del nivel por muerte. ******",
							mensaje[0][0]);

					pthread_mutex_lock(&s_personaje_recursos);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					if (B_DIBUJAR) {
						BorrarItem(&ListaItems,
								nodo_lista_personaje->id_personaje);
					}
					pthread_mutex_lock(&s_personaje_recursos);
					elimino_personaje_lista_nivel(i, t_personaje->l_personajes,
							ListaItems);
					pthread_mutex_unlock(&s_personaje_recursos);
					elimino_sck_lista(i, t_personaje->readfds);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se eliminó el personaje del nivel. Se liberan sus recursos.");
					//TODO Listar recursos
					//TODO Desbloquear personajes

					pthread_mutex_unlock(&s_personaje_recursos);
					tipo_mensaje = OK;

					fd_mensaje(i, tipo_mensaje, "Saliste del nivel.",
							&tot_enviados);
					break;

				default:
					log_in_disk_plat(LOG_LEVEL_ERROR,
							"opcion en el switch no implementada", tipo);
					//exit(1);
				}

				if (B_DIBUJAR) {
					nivel_gui_dibujar(ListaItems);
				}
				free(buffer);

			}

		}
//		FD_ZERO(t_personaje->readfds);
//		FD_SET(sck_plat, t_personaje->readfds);
//		cant_presonajes_conectados = list_size(t_personaje->l_personajes);
//
//		for (j = 0; j < cant_presonajes_conectados; j++) {
//			un_per = list_get(t_personaje->l_personajes, j);
//			FD_SET(un_per->sokc, t_personaje->readfds);
//
//		}
	}

	pthread_join(escucho_personaje_th, NULL );

	//ero_memoria(t_personaje, &param_nivel);

	//nivel_gui_terminar();

	signal(SIGTERM, sig_handler);

	return EXIT_SUCCESS;

}

void desbloquear_Personajes(char * recursos_personaje, char *buffer, int tipo,
		t_h_personaje * t_personaje, struct h_t_param_nivel param_nivel) {

	int cont_msj, tot_enviados, pos;
	int sck_plat = t_personaje->sck_personaje;
	char ** mensaje;
	t_lista_personaje * nodo_lista_personaje;
	struct h_t_recusos *recurso;

	fd_mensaje(sck_plat, N_TO_O_PERSONAJE_TERMINO_NIVEL, recursos_personaje,
			&tot_enviados); // Respuesta_PErsonaje = "cantidad_tipos_recurso;Recurso1,Cantidad1;Recurso2,Cantidad2"

	//Espero la respuesta del orquestador con los recursos que asigno
	log_in_disk_niv(LOG_LEVEL_INFO,
			"Se espera respuesta del orquestador con los recursos asignados...");

	buffer = recv_variable(sck_plat, &tipo);

	if (!strcmp(buffer, Leido_error)) {
		log_in_disk_niv(LOG_LEVEL_ERROR,
				"Hubo un error en la lectura del socket de la plataforma.");

		exit(EXIT_FAILURE);
	}

	switch (tipo) {

	case O_TO_N_ASIGNAR_RECURSOS:
		mensaje = string_split(buffer, ";");

		log_in_disk_niv(LOG_LEVEL_INFO,
				"Se recibieron los siguientes recursos asignados desde el orquestador: %s .",
				buffer);

		for (cont_msj = 0; mensaje[cont_msj] != '\0'; cont_msj++) {

			log_in_disk_niv(LOG_LEVEL_INFO,
					"Se recibio el recurso asignado desde el orquestador a un personaje (personaje,Recurso): %s",
					mensaje[cont_msj]);
			pthread_mutex_lock(&s_personaje_recursos);
			nodo_lista_personaje = busca_personaje_simbolo(mensaje[cont_msj][0],
					t_personaje->l_personajes, &pos);

			recurso = busco_recurso(mensaje[cont_msj][2],
					nodo_lista_personaje->l_recursos_optenidos);

			if (recurso != NULL ) { //agreo a la lista de recursos asignados al personaje
				recurso->cantidad++; //si esta en la lista le agrego una instancia el recurso que ya tiene el personaje
			} else {
				add_recurso_personaje(
						nodo_lista_personaje->l_recursos_optenidos,
						nodo_lista_personaje->proximo_recurso);

			}
			//TODO MArcar el personaje para que no se tenga en cuenta en el desbloqueo por que ya no tiene proximo recurso. (Hasta que vuevla a pedir uno).
			//Recursos despues de la asignacion del orquestador:
			recurso = busco_recurso(mensaje[cont_msj][2], param_nivel.recusos);
			//Resto el recurso asignado
			recurso->cantidad--;
			//Resto el recurso asignado en la pantalla
			if (B_DIBUJAR) {
				restarRecurso(ListaItems,
						nodo_lista_personaje->proximo_recurso->SIMBOLO);
			}
			pthread_mutex_unlock(&s_personaje_recursos);

			log_in_disk_niv(LOG_LEVEL_INFO,
					"el orquestador asigno el siguiente recurso (%c --> %c) ",
					nodo_lista_personaje->id_personaje, recurso->SIMBOLO);

			//imprmir_recursos_nivel(param_nivel.recusos);

// Comenteado para pruebas *** //
//							fd_mensaje(nodo_lista_personaje->sokc,
//									N_TO_P_PROXIMO_RECURSO, buffer,
//									&tot_enviados);
//
//							buffer = recv_variable(nodo_lista_personaje->sokc,
//									&tipo);
//
//							busco_recurso(buffer[0], param_nivel.recusos);
//							nodo_lista_personaje->proximo_recurso = recurso;

		}
		log_in_disk_niv(LOG_LEVEL_INFO,
				"Los recursos del nivel despues de las asignaciones del orquestador son: (Proximo_Log)");
		imprmir_recursos_nivel(param_nivel.recusos);

		break;

	case O_TO_N_ASIGNAR_RECURSOS_null:
		log_in_disk_niv(LOG_LEVEL_INFO,
				"Se recibieron los recursos asignados desde el orquestador pero no se desbloqueo ningun personaje");

//						elimino_personaje_lista_nivel(i,
//								t_personaje->l_personajes, ListaItems);

		//liberar_memoria(nodo_lista_personaje); // ya se libera la memoria adentro de la funcion anterior (elimino_personaje_lista_nivel)
//						elimino_sck_lista(i, t_personaje->readfds);

		break;
	}

	if (B_DIBUJAR) {
		pthread_mutex_lock(&s_personaje_recursos);
		BorrarItem(&ListaItems, nodo_lista_personaje->id_personaje);
		nivel_gui_dibujar(ListaItems);
		pthread_mutex_unlock(&s_personaje_recursos);
	}

	//free(recursos_personaje);
	imprimir_recursos(param_nivel.recusos);
	//free(personaje_aux.nombre_personaje);

}

//void libero_memoria(t_h_personaje *t_personaje, struct t_param_nivel *param_nivel) {
//	libero_recursos_pantalla(param_nivel->recusos);
//
//	free(t_personaje->nomb_nivel);
//	free(t_personaje->readfds);
//	free(t_personaje->sck_personaje);
//	free(t_personaje);
//
//}

void sig_handler(int signo) {
	log_in_disk_niv(LOG_LEVEL_INFO, "capturando la señal");
	//if (signo == SIGTERM)
	//nivel_gui_terminar();

}
/**
 * el 3er parametro en la posicion en la lista donde se encontro el personaje
 */
t_lista_personaje *busco_personaje(int sck, t_list *l_personajes, int *i) {

	t_lista_personaje *personaje;
	int total_personajes;
	int j;

	log_in_disk_niv(LOG_LEVEL_INFO, "busco_personaje");
	total_personajes = list_size(l_personajes);
	for (j = 0; j < total_personajes; j++) {

		personaje = (t_lista_personaje*) list_get(l_personajes, j);
		log_in_disk_niv(LOG_LEVEL_INFO, "personaje comparado %c",
				personaje->id_personaje);

		if (personaje->sokc == sck) {
			log_in_disk_niv(LOG_LEVEL_INFO, "personaje encontrado %c",
					personaje->id_personaje);
			break;	//salgo de el while
		}

	}

	log_in_disk_niv(LOG_LEVEL_INFO,

	"devuelvo el personaje %c sock del personaje %d", personaje->id_personaje,
			personaje->sokc);

	*i = j;
	return personaje;
}

/**
 * Si el recurso no esta en la lista de los optenidos por elpersonaje se lo agrego con el valor 1
 * param 1 lista de recirsos del personaje
 * param 2 recurso actual del personaje
 */
void add_recurso_personaje(t_list *l_recursos_optenidos,
		struct h_t_recusos *recurso_actual) {
	struct h_t_recusos *recurso = malloc(sizeof(struct h_t_recusos));
	log_in_disk_niv(LOG_LEVEL_TRACE,
			"add_recurso_personaje agrego el recurso %c",
			recurso_actual->SIMBOLO);

	memcpy(recurso, recurso_actual, sizeof(struct h_t_recusos));

	recurso->cantidad = 1;
	recurso->ref_recuso = recurso_actual;

	list_add(l_recursos_optenidos, recurso);

}

void elimino_personaje_lista_nivel(int sck, t_list *l_personajes,
		ITEM_NIVEL *item) {
	int pos;

	t_lista_personaje *personaje;

	busco_personaje(sck, l_personajes, &pos);
	personaje = (t_lista_personaje *) list_remove(l_personajes, pos);
	liberar_memoria(personaje, item);

}

void liberar_memoria(t_lista_personaje *personaje, ITEM_NIVEL *item) {
	liberar_recursos(personaje->l_recursos_optenidos, item);
	free(personaje->nombre_personaje);
	//free(personaje->proximo_recurso);
}

void liberar_recursos(t_list *recursos_otenido, ITEM_NIVEL *item) {
	int total_recursos;
	int i;
	t_recusos *recurso_aux;

	total_recursos = list_size(recursos_otenido);
	log_in_disk_niv(LOG_LEVEL_TRACE, "Total recursos obtenidos: %d",
			total_recursos);
	for (i = 0; i < total_recursos; i++) {
		recurso_aux = (t_recusos *) list_get(recursos_otenido, i);


		recurso_aux->ref_recuso->cantidad = recurso_aux->ref_recuso->cantidad + recurso_aux->cantidad;

		//si esta puesta la forma grafica y la tantidad de recursos es mayor o
		//sumo recurosos a la pantalla
		while (B_DIBUJAR && recurso_aux->cantidad--) {
			sumarRecurso(item, recurso_aux->SIMBOLO);
		}
	}
	list_destroy(recursos_otenido); //TODO Destruir los nodos de la lista!
}

//Recibe una lista de recursos de tipo t_recusos y devuelve un string de tipo. EJ:"cantidadRecursos;simbolo1,cantidad1;simbolo2,cantidad2.."

char* listarRecursosPersonaje(t_list * lista_Recursos) {

	int cantidadRecu = list_size(lista_Recursos);
	int i;
	char* recursos;
	char recursosAux[5] = ("");
	t_recusos *recurso_aux;
	recursos = malloc(2 * sizeof(char));

	sprintf(recursos, "%d", cantidadRecu);
	for (i = 0; i < cantidadRecu; i++) {
		recurso_aux = list_get(lista_Recursos, i);
		sprintf(recursosAux, ";%c,%d", recurso_aux->SIMBOLO,
				recurso_aux->cantidad);
		string_append(&recursos, recursosAux);

	}
	log_in_disk_niv(LOG_LEVEL_TRACE, "Los recursos a devolver son: %s ",
			recursos);

	return recursos;

}

t_lista_personaje *busca_personaje_simbolo(char id, t_list *l_personajes,
		int *indice_personaje) {
	int count;
	int total_personajes = list_size(l_personajes);
	t_lista_personaje *per;

	log_in_disk_niv(LOG_LEVEL_TRACE, "busca_personaje_simbolo: %c", id);

	for (count = 0; count < total_personajes; count++) {
		per = list_get(l_personajes, count);

		if (per->id_personaje == id) {

			log_in_disk_niv(LOG_LEVEL_TRACE, "Retorno el personaje %s",
					per->nombre_personaje);

			*indice_personaje = count;
			return per;
		}
	}

	return NULL ;
}

void imprmir_recursos_nivel(t_list * recursos) {

	int cant_elemmm = 0;
	t_recusos * recuss;
	int j;
	cant_elemmm = list_size(recursos);
	for (j = 0; j < cant_elemmm; j++) {

		recuss = list_get(recursos, j);
		log_in_disk_niv(LOG_LEVEL_TRACE,

		"Recursos del nivel: (indice--> Recurso-->Cantidad:) %d --> %c --> %d",
				j, recuss->SIMBOLO, recuss->cantidad);
	}

}

void imprimir_recursos(t_list * lista_Recursos) {
	int tot_recusos = list_size(lista_Recursos);
	int count;
	struct h_t_recusos *recurso;

	for (count = 0; count < tot_recusos; count++) {
		recurso = (struct h_t_recusos *) list_get(lista_Recursos, count);
		log_in_disk_niv(LOG_LEVEL_TRACE,
				"El recurso está en la posición %d de la lista con el id %c y la cantidad %d",
				count, recurso->SIMBOLO, recurso->cantidad);
	}

}

