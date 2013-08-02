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

void imprimir_recursos(t_list * lista_Recursos);

//void libero_memoria(t_h_personaje *t_personaje, struct t_param_nivel *param_nivel);
void sig_handler(int signo);

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
	char mensajeMover[3];
	struct h_t_recusos *recurso;
	//ITEM_NIVEL *ListaItems = NULL;
	t_lista_personaje *nodo_lista_personaje;
	int tipo_mensaje, catidad_recursos;
	char *nombre_recurso, *buff;
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
	t_personaje->ListaItemsss = ListaItems;
	t_personaje->sck_orquestador = sck_plat;
	struct timeval tv;
	tv.tv_sec = 5;
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
//	pthread_create(&detecto_interbloque_th, NULL, (void*) detecto_interbloque,
//		(void*) &h_interbloqueo);
	imprmir_recursos_nivel(param_nivel.recusos);

	for (;;) {
		//recursos_personaje = malloc (sizeof(char) * 35);
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

					if (B_DIBUJAR) {
						pthread_mutex_lock(&s_personaje_recursos);
						BorrarItem(&ListaItems,
								nodo_lista_personaje->id_personaje);
						nivel_gui_dibujar(ListaItems);
						pthread_mutex_unlock(&s_personaje_recursos);
					}

					fd_mensaje(i, OK, "Fin de nivel", &tot_enviados);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se envió OK de fin de nivel al personaje");
					//La funcion el "elimino_personaje_lista_nivel" ya suma los recursos que se liberaron y tambien actualiza los recursos en pantalla.
					pthread_mutex_lock(&s_personaje_recursos);
					elimino_personaje_lista_nivel(i, t_personaje->l_personajes,
							ListaItems);
					elimino_sck_lista(i, t_personaje->readfds);
					pthread_mutex_unlock(&s_personaje_recursos);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Los recursos del nivel: %s despues de liberar el personaje: %s %c son los siguietes: (proximo log)",
							param_nivel.nom_nivel,
							personaje_aux.nombre_personaje,
							personaje_aux.id_personaje);

					imprmir_recursos_nivel(param_nivel.recusos);
					/*Informo al orquestador los recursos liberados y actualizo las instancias de los recursos dependiendo de la respuesta.
					 Ahora se hace en la funcion desbloquear_Personajes.	 	 */

					desbloquear_Personajes(recursos_personaje, buffer,
							t_personaje, param_nivel, sck_plat);

					//imprimir_recursos(param_nivel.recusos);
					free(personaje_aux.nombre_personaje);
					break;

				case P_TO_N_REINICIAR_NIVEL:

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se recibió un mensaje P_TO_N_REINICIAR_NIVEL");

					log_in_disk_niv(LOG_LEVEL_INFO,
							"****** El personaje %c reinicia el nivel. Se lo reubica en la posición (1,1) del mapa y se liberan sus recursos. ******",
							mensaje[0][0]);

					sprintf(mensajeMover, "%c", mensaje[0][0]);

					pthread_mutex_lock(&s_personaje_recursos);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					if (B_DIBUJAR) {

						BorrarItem(&ListaItems,
								nodo_lista_personaje->id_personaje);
						nivel_gui_dibujar(ListaItems);
						personaje_pantalla(mensaje[0][0], 1, 1, &ListaItems);
						nivel_gui_dibujar(ListaItems);
					}

					imprmir_recursos_nivel(param_nivel.recusos);
					recursos_personaje = "";

					recursos_personaje = listarRecursosPersonaje(
							nodo_lista_personaje->l_recursos_optenidos);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"el personaje: %s Liberó estos recursos: %s.",
							nodo_lista_personaje->nombre_personaje,
							recursos_personaje);

					liberar_recursos(nodo_lista_personaje->l_recursos_optenidos,
							ListaItems);
					nodo_lista_personaje->l_recursos_optenidos = list_create();

					imprmir_recursos_nivel(param_nivel.recusos);

					log_in_disk_niv(LOG_LEVEL_INFO, "Personaje bloqueado?: %s", mensaje[2]);

					if (mensaje[2][0] - '0') {
						fd_mensaje(t_personaje->sck_orquestador,
								N_TO_O_PERS_REINICIO, mensajeMover,
								&tot_enviados);
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Se envió mensaje N_TO_O_PERS_REINICIO");

						buffer = recv_variable(t_personaje->sck_orquestador, &tipo);
						if(tipo == OK){
							log_in_disk_niv(LOG_LEVEL_INFO, "Se recibió OK del orquestador de reinicio del personaje bloqueado.");
						}
					}

					desbloquear_Personajes(recursos_personaje, buffer,
							t_personaje, param_nivel, sck_plat);

					pthread_mutex_unlock(&s_personaje_recursos);

					tipo_mensaje = OK;

					fd_mensaje(i, tipo_mensaje, "Nivel re-iniciado.",
							&tot_enviados);
					break;

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

					sprintf(mensajeMover, "%c", mensaje[0][0]);

					pthread_mutex_lock(&s_personaje_recursos);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					if (B_DIBUJAR) {
						BorrarItem(&ListaItems,
								nodo_lista_personaje->id_personaje);
						nivel_gui_dibujar(ListaItems);
					}

					recursos_personaje = "";
					recursos_personaje = listarRecursosPersonaje(
							nodo_lista_personaje->l_recursos_optenidos);

					log_in_disk_niv(LOG_LEVEL_INFO, "Personaje bloqueado?: %s", mensaje[1]);

					if (mensaje[1][0] - '0') {
						fd_mensaje(t_personaje->sck_orquestador,
								N_TO_O_PERS_REINICIO, mensajeMover, &tot_enviados);
						log_in_disk_niv(LOG_LEVEL_INFO,
								"Se envió mensaje N_TO_O_PERS_REINICIO");
						buff = recv_variable(t_personaje->sck_orquestador, &tipo);

						if(tipo == OK){
							log_in_disk_niv(LOG_LEVEL_INFO, "El personaje salió del planificador.");
						}

					}

					//esperar ok del orquestador?
					fd_mensaje(i, OK, "Saliste del nivel.", &tot_enviados);
					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se envió OK de salida al personaje.");

					elimino_personaje_lista_nivel(i, t_personaje->l_personajes,
							ListaItems);
					elimino_sck_lista(i, t_personaje->readfds);

					desbloquear_Personajes(recursos_personaje, buffer,
							t_personaje, param_nivel, sck_plat);

					pthread_mutex_unlock(&s_personaje_recursos);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se eliminó el personaje del nivel. Se liberan sus recursos.");
					//TODO Listar recursos

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
		//free(recursos_personaje);
	}

	pthread_join(escucho_personaje_th, NULL );

	//ero_memoria(t_personaje, &param_nivel);

	//nivel_gui_terminar();

	signal(SIGTERM, sig_handler);

	return EXIT_SUCCESS;

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
 * Si el recurso no esta en la lista de los optenidos por elpersonaje se lo agrego con el valor 1
 * param 1 lista de recirsos del personaje
 * param 2 recurso actual del personaje
 */

//Recibe una lista de recursos de tipo t_recusos y devuelve un string de tipo. EJ:"cantidadRecursos;simbolo1,cantidad1;simbolo2,cantidad2.."
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

