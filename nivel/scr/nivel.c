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
#include <pthread.h>
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

void add_personaje_lista(char id_personaje, char *nombre_personaje, int i,
		t_h_personaje *t_personaje);
t_lista_personaje *busco_personaje(int sck, t_list *l_personajes, int *i);
void add_recurso_personaje(t_list *l_recursos_optenidos,
		struct h_t_recusos *recurso_actual);
void elimino_personaje_lista_nivel(int sck, t_list *l_personajes);
void liberar_memoria(t_lista_personaje *personaje);
void liberar_recursos(t_list *recursos_otenido);
void listarRecursosPersonaje(t_list * lista_Recursos, char * recursos);

//void libero_memoria(t_h_personaje *t_personaje, struct t_param_nivel *param_nivel);
void sig_handler(int signo);

static pthread_mutex_t s_personaje_conectado = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_personaje_recursos = PTHREAD_MUTEX_INITIALIZER;

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
	ITEM_NIVEL *ListaItems = NULL;
	t_lista_personaje *nodo_lista_personaje;
	int tipo_mensaje, catidad_recursos;
	char *nombre_recurso;
	char *recursos_personaje;
	int pos; //la posicion en la lista de un personaje

	if (B_DIBUJAR) {
		inicializo_pantalla();
		nivel_gui_get_area_nivel(&rows, &cols);
	}
	param_nivel = leer_nivel_config(rows, cols);

	t_personaje = malloc(sizeof(t_h_personaje));
	t_personaje->nomb_nivel = param_nivel.nom_nivel;
	t_personaje->pueto = param_nivel.PUERTO;
	t_personaje->l_personajes = list_create();
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
	pthread_mutex_lock(&s_personaje_conectado);
	t_personaje->s_personaje_conectado = &s_personaje_conectado;

	//creo el hilo que va a escuchar conexiones del personaje
	pthread_create(&escucho_personaje_th, NULL, (void*) escucho_personaje,
			(void*) t_personaje);
	pthread_mutex_lock(&s_personaje_conectado);

	//setteo las estructura para pasar al hilo
	memcpy(&h_interbloqueo.t_personaje, t_personaje, sizeof(t_h_personaje));
	h_interbloqueo.param_nivel = param_nivel;
	//creo el hilo para la deteccion de interbloqueo
	//pthread_create(&detecto_interbloque_th, NULL, (void*) detecto_interbloque,
	//	(void*) &h_interbloqueo);

	for (;;) {

		if (select(t_personaje->sck_personaje + 1, t_personaje->readfds, NULL,
				NULL, NULL ) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i <= t_personaje->sck_personaje; i++) {
			if (FD_ISSET(i, t_personaje->readfds)) {

				buffer = recv_variable(i, &tipo); // *(t_h_orq->sock) Para mi es i el 1er parametro del rec por que el socket que me respondio tiene ese valor.
				if (!strcmp(buffer, Leido_error)) {

					elimino_personaje_lista_nivel(i, t_personaje->l_personajes);
					elimino_sck_lista(i, t_personaje->readfds);

				}
				mensaje = string_split(buffer, ";");

				log_in_disk_niv(LOG_LEVEL_TRACE, "Tipo de mensaje %d ", tipo);

				for (cont_msj = 0; mensaje[cont_msj] != '\0'; cont_msj++) {
					log_in_disk_niv(LOG_LEVEL_TRACE, "mensaje %d contenido %s",
							cont_msj, mensaje[cont_msj]);

				}

				switch (tipo) {
				case P_TO_N_UBIC_RECURSO:

					recurso = busco_recurso(mensaje[0][0], param_nivel.recusos);

					aux_mensaje = string_from_format("%d;%d", recurso->posX,
							recurso->posY);

					log_in_disk_niv(LOG_LEVEL_TRACE, "se envio recursos %d, %d",
							recurso->posX, recurso->posY);

					fd_mensaje(i, N_TO_P_UBIC_RECURSO, aux_mensaje,
							&tot_enviados);
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos); //busco el personaje que me solicita el recurso
					nodo_lista_personaje->proximo_recurso = recurso; //lo relaciono con el proximo recuros que tiene que obtener

					break;
				case P_TO_N_MOVIMIENTO:
					posX = atoi(mensaje[3]);
					posY = atoi(mensaje[4]);
					MoverPersonaje(ListaItems, mensaje[0][0], posX, posY);
					//la informacion del la posicion la necesito para determinar el interbloqueo
					pthread_mutex_lock(&s_personaje_recursos);
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

					break;

				case P_TO_N_OBJ_CUMPLIDO:

					recursos_personaje = "";
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);
					log_in_disk_niv(LOG_LEVEL_TRACE,
							"el personaje: %c a completado el nivel ",
							nodo_lista_personaje->id_personaje);

					listarRecursosPersonaje(
							nodo_lista_personaje->l_recursos_optenidos,
							recursos_personaje);

					log_in_disk_niv(LOG_LEVEL_TRACE,
							"el personaje: %c a completado el nivel y libera estos recursos: %s, ",
							nodo_lista_personaje->id_personaje,
							recursos_personaje);
					//Informo al orquestador los recursos liberados

					fd_mensaje(i, N_TO_O_PERSONAJE_TERMINO_NIVEL,
							recursos_personaje, &tot_enviados);
					//Espero la respuesta del orquestador con los recursos que asigno

					buffer = recv_variable(t_personaje->sck_personaje, &tipo);

					if (!strcmp(buffer, Leido_error)) {
						log_in_disk_niv(LOG_LEVEL_TRACE,
								"Hubo un error en la lectura del socket de la plataforma, se volvera a intentar.");

						buffer = recv_variable(t_personaje->sck_personaje,
								&tipo);
						//todo Volver a cotrolar error.
					}

					if (tipo == O_TO_N_ASIGNAR_RECURSOS) {

//						log_in_disk_niv (LOG_LEVEL_INFO,
//								"");

						//Recibir del nivel los recursos que se asignaron y a que personajes se le asignaron.

					}

					else {
						log_in_disk_niv(LOG_LEVEL_INFO,
								"No se recibió el mensaje esperado desde el orquestador con los recursos asignados %s:",
								buffer);
						exit(EXIT_FAILURE);
					}

					liberar_memoria(nodo_lista_personaje);

					free(recursos_personaje);

					break;

				case P_TO_N_INICIAR_NIVEL:

					personaje_pantalla(mensaje[1][0], 1, 1, &ListaItems);
					add_personaje_lista(mensaje[1][0], mensaje[0], i,
							t_personaje);
					break;

				case P_TO_N_SOLIC_RECURSO:

					fd_mensaje(i, N_TO_P_RECURSO_OK,
							"LEGASTE AL RECURSO, EN HORA BUENA!!!",
							&tot_enviados);

					pthread_mutex_lock(&s_personaje_recursos);
					log_in_disk_niv(LOG_LEVEL_INFO,
							"El pesonaje solicita un recurso");
					nodo_lista_personaje = busco_personaje(i,
							t_personaje->l_personajes, &pos);

					catidad_recursos =
							nodo_lista_personaje->proximo_recurso->cantidad;
					nombre_recurso =
							nodo_lista_personaje->proximo_recurso->NOMBRE;

					log_in_disk_niv(LOG_LEVEL_INFO,
							"El pesonaje solicita un recurso del tipo %s, la cantidad atual es de %d",
							nombre_recurso, catidad_recursos);

					if (catidad_recursos > 0) { //si tengo recursos se los doy

						catidad_recursos--;
						tipo_mensaje = N_TO_P_RECURSO_OK;

						/*fd_mensaje(i, tipo_mensaje,
								"LEGASTE AL RECURSO, EN HORA BUENA!!!",
								&tot_enviados);*/

						log_in_disk_niv(LOG_LEVEL_INFO,
								"El recurso entregado al personaje %c",
								nodo_lista_personaje->id_personaje);
						recurso = busco_recurso(
								nodo_lista_personaje->proximo_recurso->SIMBOLO,
								nodo_lista_personaje->l_recursos_optenidos);

						restarRecurso(ListaItems,
								nodo_lista_personaje->proximo_recurso->SIMBOLO);

						sleep(10);
						if (recurso != NULL ) { //agreo a la lista de recursos asignados al personaje
							recurso->cantidad++; //si esta en la lista le agrego una instancia el recurso que ya tiene el personaje
						} else {
							add_recurso_personaje(
									nodo_lista_personaje->l_recursos_optenidos,
									nodo_lista_personaje->proximo_recurso);

						}

					} else { // en caso de no tener bloqueo al personaje
						tipo_mensaje = N_TO_P_RECURSO_ERROR;
						log_in_disk_niv(LOG_LEVEL_INFO,
								"El recurso insuficientes para entregar el personaje %c",
								nodo_lista_personaje->id_personaje);
					}
					log_in_disk_niv(LOG_LEVEL_INFO, "Cantidad de recursos %d",
							catidad_recursos);

					nodo_lista_personaje->proximo_recurso->cantidad =
							catidad_recursos;

					/* fd_mensaje(i, tipo_mensaje,
					 "LEGASTE AL RECURSO, EN HORA BUENA!!!",
					 &tot_enviados);*/

					pthread_mutex_unlock(&s_personaje_recursos);
					break;
				}

				if (B_DIBUJAR) {
					nivel_gui_dibujar(ListaItems);
				}
				free(buffer);

			}
		}

	}

	pthread_join(escucho_personaje_th, NULL );

	//ero_memoria(t_personaje, &param_nivel);

	//nivel_gui_terminar();

	signal(SIGTERM, sig_handler);

	return EXIT_SUCCESS;

}

void add_personaje_lista(char id_personaje, char *nombre_personaje, int i,
		t_h_personaje *t_personaje) {

	t_lista_personaje *list_personajes;

	log_in_disk_niv(LOG_LEVEL_TRACE, "funcion add_personaje_lista ");

	list_personajes = malloc(sizeof(t_lista_personaje));
	list_personajes->id_personaje = id_personaje;
	list_personajes->nombre_personaje = nombre_personaje;
	list_personajes->sokc = i;
	list_personajes->l_recursos_optenidos = list_create();

	list_add(t_personaje->l_personajes, list_personajes);

	log_in_disk_niv(LOG_LEVEL_TRACE,
			"Agrego a la lista el personaje con  id_personaje %c el nombre %s socket %d",
			list_personajes->id_personaje, list_personajes->nombre_personaje,
			list_personajes->sokc);
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

	log_in_disk_niv(LOG_LEVEL_TRACE, "busco_personaje");
	total_personajes = list_size(l_personajes);
	for (j = 0; j < total_personajes; j++) {

		personaje = (t_lista_personaje*) list_get(l_personajes, j);
		log_in_disk_niv(LOG_LEVEL_TRACE, "personaje comparado %c",
				personaje->id_personaje);

		if (personaje->sokc == sck) {
			log_in_disk_niv(LOG_LEVEL_TRACE, "personaje encontrado %c",
					personaje->id_personaje);
			break;	//salgo de el while
		}

	}
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

void elimino_personaje_lista_nivel(int sck, t_list *l_personajes) {
	int pos;

	pthread_mutex_lock(&s_personaje_recursos);
	t_lista_personaje *personaje;
	busco_personaje(sck, l_personajes, &pos);
	personaje = (t_lista_personaje *) list_remove(l_personajes, pos);
	liberar_memoria(personaje);
	pthread_mutex_unlock(&s_personaje_recursos);
}

void liberar_memoria(t_lista_personaje *personaje) {
	liberar_recursos(personaje->l_recursos_optenidos);
	free(personaje->nombre_personaje);
	free(personaje->proximo_recurso);
}

void liberar_recursos(t_list *recursos_otenido) {
	int total_recursos;
	int i;
	t_recusos *recurso_aux;

	total_recursos = list_size(recursos_otenido);
	for (i = 0; i < total_recursos; i++) {
		recurso_aux = (t_recusos *) list_get(recursos_otenido, i);
		recurso_aux->ref_recuso->cantidad = +recurso_aux->cantidad;
	}
	list_destroy(recursos_otenido);
}

//Recibe una lista de recursos de tipo t_recusos y devuelve un string de tipo. EJ:"cantidadRecursos;simbolo1;cantidad1;simbolo2;cantidad2.."

void listarRecursosPersonaje(t_list * lista_Recursos, char * recursos) {

	int cantidadRecu = list_size(lista_Recursos);
	int i;
	char recursosAux[5] = ("");
	t_recusos *recurso_aux;
	recursos = malloc(2);

	sprintf(recursos, "%d", cantidadRecu);
	for (i = 0; i < cantidadRecu; i++) {
		recurso_aux = list_get(lista_Recursos, i);
		sprintf(recursosAux, "%c;%c;%c;%c;", ';', recurso_aux->SIMBOLO, ';',
				recurso_aux->cantidad);
		string_append(&recursos, recursosAux);

	}

}

