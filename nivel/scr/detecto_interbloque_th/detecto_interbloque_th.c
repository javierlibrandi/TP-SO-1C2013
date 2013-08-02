/*
 * detecto_interbloque_th.c
 *
 *  Created on: 15/07/2013
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/entorno.h>
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/grabar.h>
#include <semaphore.h>
#include "detecto_interbloque_th.h"
#include "../escuchar_personaje/personaje_thr.h"
#include <stdbool.h>
#include <commons/string.h>
#include <unistd.h>
#include "../conect_plataforma/conect_plataforma.h"
#include <pthread.h>
#include <semaphore.h>
#include "../manjo_pantalla/pantalla.h"
#include "../manjo_pantalla/nivel_p.h"
#include "../manjo_pantalla/tad_items.h"

int marcar_personajes_s_recursos(t_list *personajes);
void otnego_vector_diponibles(t_list *recursos, t_list *personajes);
int marchar_personaje_c_recursos(t_list *personajes);
int cantidad_interbloquedos(t_list *personajes, char **personaje_bloquedos);

void *detecto_interbloque(void *p) {
	//int sck_orq;
	t_h_interbloqueo h_interbloqueo;
	t_h_personaje t_personaje;
	struct h_t_param_nivel param_nivel;
	char *personaje_bloquedos = NULL;
	int tot_enviados, tipo, indice, tipo_mensaje;
	char * buffer;
	char* recursos_personaje;
	char** mensaje;
	memcpy(&h_interbloqueo, (t_h_interbloqueo*) p, sizeof(t_h_interbloqueo));
	memcpy(&t_personaje, &h_interbloqueo.t_personaje, sizeof(t_h_personaje));
	memcpy(&param_nivel, &h_interbloqueo.param_nivel,
			sizeof(struct h_t_param_nivel));
	t_lista_personaje * nodo_lista_personaje;
	//conecxion con el orquestador, solo si tengo el valor Recovery = 1
//	if (param_nivel.Recovery) {
//		sck_orq = con_pla_nival(param_nivel.IP, param_nivel.PUERTO_PLATAFORMA,
//				param_nivel.nom_nivel, param_nivel.PUERTO);
//	}
	log_in_disk_niv(LOG_LEVEL_INFO,
			"####INTERBLOQUEO	Hilo pra la deteccion de interbloqueo del nivel %s levantado",
			param_nivel.nom_nivel);
	for (;;) {

		usleep(param_nivel.TiempoChequeoDeadlock);
		pthread_mutex_lock(t_personaje.s_personaje_recursos);

		log_in_disk_niv(LOG_LEVEL_INFO,
				"#######INTER_BLOQUEO	Comienza el algoritmo de deteccion de interbloqueo del nivel: %s	#######INTER_BLOQUEO",
				param_nivel.nom_nivel);

		if (marcar_personajes_s_recursos(t_personaje.l_personajes) != 0) { // paso 1
			otnego_vector_diponibles(param_nivel.recusos, NULL ); //paso 2

			while (marchar_personaje_c_recursos(t_personaje.l_personajes) != 0) { //paso  3 miestra sea distinto de 0

				otnego_vector_diponibles(param_nivel.recusos,
						t_personaje.l_personajes); //paso 4
			}
		}
		//
		if (cantidad_interbloquedos(t_personaje.l_personajes,
				&personaje_bloquedos) != 0) {

			log_in_disk_niv(LOG_LEVEL_INFO,
					"#######################INTER_BLOQUEO  SE DETECTO UN INTERBLOQUEO EN EL NIVEL: %s. Y LOS PERSONAJES QUE PARTICIPAN DEL MISMO SON: %s .  #######INTER_BLOQUEO",
					param_nivel.nom_nivel, personaje_bloquedos);

			if (param_nivel.Recovery) {
				//TODO envio mensaje personajes interbloquedos para que se elija a la visticm

				fd_mensaje(t_personaje.sck_orquestador, N_TO_O_RECOVERY,
						personaje_bloquedos, &tot_enviados);

				buffer = recv_variable(t_personaje.sck_orquestador, &tipo);

				if (tipo == O_TO_N_MUERTE) {
					nodo_lista_personaje = busca_personaje_simbolo(buffer[0],
							t_personaje.l_personajes, &indice);

					buffer = recv_variable(nodo_lista_personaje->sokc, &tipo);

					if (!strcmp(buffer, Leido_error)) {

						elimino_personaje_lista_nivel(
								nodo_lista_personaje->sokc,
								t_personaje.l_personajes,
								*(t_personaje.ListaItemss));
						elimino_sck_lista(nodo_lista_personaje->sokc,
								t_personaje.readfds);
					}

					log_in_disk_niv(LOG_LEVEL_INFO,
							"#######################INTER_BLOQUEO Se recibió un tipo de mensaje %d: %s #######################INTER_BLOQUEO",
							tipo, buffer);

					switch (tipo) {
					mensaje = string_split(buffer, ";");

				case P_TO_N_SALIR:

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se recibió un mensaje P_TO_N_SALIR");

					log_in_disk_niv(LOG_LEVEL_INFO,
							"****** El personaje %c sale del nivel por muerte. ******",
							mensaje[0][0]);

//					nodo_lista_personaje = busco_personaje(i,
//							t_personaje->l_personajes, &pos);

					if (B_DIBUJAR) {
						BorrarItem(t_personaje.ListaItemss,
								nodo_lista_personaje->id_personaje);
						nivel_gui_dibujar(t_personaje.ListaItemsss);
					}

					recursos_personaje = string_new();
					recursos_personaje = listarRecursosPersonaje(
							nodo_lista_personaje->l_recursos_optenidos);
					tipo_mensaje = OK;

					fd_mensaje(nodo_lista_personaje->sokc, tipo_mensaje,
							"Saliste del nivel.", &tot_enviados);
					elimino_personaje_lista_nivel(nodo_lista_personaje->sokc,
							t_personaje.l_personajes, t_personaje.ListaItemsss);
					elimino_sck_lista(nodo_lista_personaje->sokc,
							t_personaje.readfds);

					desbloquear_Personajes(recursos_personaje, buffer,
							&t_personaje, param_nivel,
							t_personaje.sck_orquestador);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se eliminó el personaje del nivel. Se liberan sus recursos.");
					//TODO Listar recursos

					break;

					//TODO AGregar el case del salir. y controlar semaforos.

					}

					//Recibir la victima, e iniciar el desbloqueo informando al orquestador.

				}

			}

		}
		pthread_mutex_unlock(t_personaje.s_personaje_recursos);
		//free(personaje_bloquedos);
		free(buffer);
		personaje_bloquedos = string_new();
	}
	return NULL ;
}

//} }  //BORRAR ESTAS 2 llaves cuando descomento lo de arriba.

/**
 * Inicializo las estructuras
 * 1.- se marca cada proceso que tenga una fila de la matriz de Asiganacion completamente a cerro
 */
int marcar_personajes_s_recursos(t_list *personajes) {
	int tot_personajes = list_size(personajes);
	int cont;
	t_lista_personaje *l_personaje;

	log_in_disk_niv(LOG_LEVEL_INFO,
			"#######INTER_BLOQUEO  marcar_personajes_s_recursos total de personajes %d   #######INTER_BLOQUEO",
			tot_personajes);

	log_in_disk_niv(LOG_LEVEL_DEBUG,
			"#######INTER_BLOQUEO#####	PASO 1.- se marca cada proceso que tenga una fila de la matriz de Asiganacion completamente a cero.###########INTER_BLOQUEO");

	for (cont = 0; cont < tot_personajes; cont++) {
		l_personaje = (t_lista_personaje*) list_get(personajes, cont);

		if (list_is_empty(l_personaje->l_recursos_optenidos)) {
			l_personaje->bloquedo = false;
			log_in_disk_niv(LOG_LEVEL_TRACE,
					"##########INTER_BLOQUEO  El personaje %c no es candidato para el interbloqueo  ##########INTER_BLOQUEO",
					l_personaje->id_personaje);

		} else {
			l_personaje->bloquedo = true;
			l_personaje->recusos_sumados = false;

			log_in_disk_niv(LOG_LEVEL_TRACE,
					"##########INTER_BLOQUEO    El personaje %c es candidato para el interbloqueo  ##########INTER_BLOQUEO  ",
					l_personaje->id_personaje);

		}
	}
	return tot_personajes;
}

/**
 *
 * 2.- obtengo el vertor de recursos disponible
 *
 */

void otnego_vector_diponibles(t_list *recursos, t_list *personajes) {
	int tot_recursos = list_size(recursos);
	int tot_personajes, tot_personajes_personaje;
	int cont_recursos, cont_personajes, cont_recursos_personaje;
	t_recusos *l_recurso = NULL, *l_recursos_personaje = NULL;
	t_lista_personaje *l_personaje = NULL;
	struct h_t_recusos *aux_recurso;

	for (cont_recursos = 0; cont_recursos < tot_recursos; cont_recursos++) {
		l_recurso = list_get(recursos, cont_recursos);

		if (personajes == NULL ) { //si no paso la lista de personajes inicializo el vertor
			l_recurso->recursos_disponibles = l_recurso->cantidad;
		} else {
			tot_personajes = list_size(personajes);
			for (cont_personajes = 0; cont_personajes < tot_personajes;
					cont_personajes++) { //recorro todos los personajes
				l_personaje = list_get(personajes, cont_personajes);
				if (!l_personaje->bloquedo && !l_personaje->recusos_sumados) { //los personajes que no estan bloquedos ya que si terminan pueden devolver los recursos los sumo

					tot_personajes_personaje = list_size(
							l_personaje->l_recursos_optenidos); //lista que tiene el personaje

					for (cont_recursos_personaje = 0;
							cont_recursos_personaje < tot_personajes_personaje;
							cont_recursos_personaje++) { // recorro todos los recursos del presonaje

						l_recursos_personaje = list_get(recursos,
								cont_recursos_personaje); // obtengo el primer recurso del personaje
						char id = l_recursos_personaje->SIMBOLO;
						aux_recurso = busco_recurso(id, recursos); // bueco en la lista de recurso por el ID del recuros del personaje

						aux_recurso->recursos_disponibles =
								aux_recurso->recursos_disponibles
										+ l_recursos_personaje->cantidad; //suma la cantidad de recuros que tiene el personaje
						l_personaje->recusos_sumados = true; //lo pongo en true para no sumar 2 vesces los recursos de un personaje
					}
				}
			}

		}
	}
}

/**
 *
 * 3.- marco los procesos que pueden que pueden contunuar con sus peticiones
 *
 */
int marchar_personaje_c_recursos(t_list *personajes) {
	int tot_personajes = list_size(personajes);
	int cont, marcados = 0;
	t_lista_personaje *l_personaje;
	int difX, difY;

	log_in_disk_niv(LOG_LEVEL_INFO,
			"marchar_personaje_c_recursos total de personajes %d",
			tot_personajes);

	log_in_disk_niv(LOG_LEVEL_DEBUG,
			"################INTER_BLOQUEO---PASO3.- marco los procesos que pueden que pueden contunuar con sus peticiones  ########################INTER_BLOQUE");

	for (cont = 0; cont < tot_personajes; cont++) {
		l_personaje = (t_lista_personaje*) list_get(personajes, cont);

		if (l_personaje->bloquedo) {

			if (l_personaje->proximo_recurso->recursos_disponibles > 0) { //si la cantidad del vertor de disponible del proximo recurso del personaje es mayor a 0 por lo tanto el personaje no esta bloquedo
				l_personaje->bloquedo = false;
				log_in_disk_niv(LOG_LEVEL_TRACE,
						"El personaje %c no es candidato para el interbloqueo, exiten %d recurso %s para cumplir su solicitud",
						l_personaje->id_personaje,
						l_personaje->proximo_recurso->cantidad,
						l_personaje->proximo_recurso->NOMBRE);
				marcados++;
			} else {
				difX = l_personaje->proximo_recurso->posX - l_personaje->posX;
				difY = l_personaje->proximo_recurso->posY - l_personaje->posY;
				if ((!(difX == 0 && difY == 0))
						&& l_personaje->proximo_recurso == NULL ) { //si el proximo recurso del personaje es 0 pero el personaje no llego al recurso por lo tanto no esta bloqueado
					l_personaje->bloquedo = false;
					marcados++;

					log_in_disk_niv(LOG_LEVEL_TRACE,
							"El personaje %c no es candidato para el interbloqueo, exiten %d recurso %s para cumplir su solicitud pero el personaje no llego a la posicion del recurso",
							l_personaje->proximo_recurso->cantidad,
							l_personaje->proximo_recurso->NOMBRE);
				}
			}
		}
	}
	return marcados;
}

//devuevo la cantida de personaje loquedos
int cantidad_interbloquedos(t_list *personajes, char **personajes_bloquedos) {
	int cont_personajes;
	int tot_perosnajes = list_size(personajes);
	int tot_interbloquedos = 0;
	t_lista_personaje *l_personaje;
	char respuesta_recu_aux[10] = ("");
	*personajes_bloquedos = string_new();
	for (cont_personajes = 0; cont_personajes < tot_perosnajes;
			cont_personajes++) {
		l_personaje = list_get(personajes, cont_personajes);

		if (l_personaje->bloquedo) {
			tot_interbloquedos++;

			if (!strcmp(*personajes_bloquedos, "")) {
				//if (*personajes_bloquedos == ("")) {
				sprintf(respuesta_recu_aux, "%c", l_personaje->id_personaje);
				//string_append(&respuesta_recursos, respuesta_recu_aux);
				string_append(personajes_bloquedos, respuesta_recu_aux);
			} else {
				sprintf(respuesta_recu_aux, ";%c", l_personaje->id_personaje);
				//string_append(personajes_bloquedos, ";");
				string_append(personajes_bloquedos, respuesta_recu_aux);
			}
		}
	}
	return tot_interbloquedos;
}

/**
 * Busco el recurso por el id, y lo devuelvo en caso de no exister el recurso en la lista devuelvo null
 */
struct h_t_recusos *busco_recurso(char id, t_list *recusos) {
	int i, tot_elementos;
//struct h_t_recusos *recurso = NULL;
	struct h_t_recusos *recurso_aux = NULL;

	log_in_disk_niv(LOG_LEVEL_TRACE, "Busco el recurso %c", id);

	tot_elementos = list_size(recusos);

	for (i = 0; i < tot_elementos; i++) {
		recurso_aux = (struct h_t_recusos*) list_get(recusos, i);

		if (id == recurso_aux->SIMBOLO) {
			log_in_disk_niv(LOG_LEVEL_INFO,
					"encuentro el recurso buscado: %c, en la posicion %d de la lista de recursos",
					recurso_aux->SIMBOLO, i);

			return recurso_aux;
		}
	}

	return NULL ;
}

////////////////////////////

void elimino_personaje_lista_nivel(int sck, t_list *l_personajes,
		ITEM_NIVEL *item) {
	int pos;

	t_lista_personaje *personaje;

	busco_personaje(sck, l_personajes, &pos);
	personaje = (t_lista_personaje *) list_remove(l_personajes, pos);
	liberar_memoria(personaje, item);

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

void desbloquear_Personajes(char * recursos_personaje, char *buffer,
		t_h_personaje * t_personaje, struct h_t_param_nivel param_nivel,
		int sck_plat) {

	int cont_msj, tot_enviados, pos, tipo;
	//int sck_plat = t_personaje->sck_personaje;
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

		log_in_disk_niv(LOG_LEVEL_INFO,
				"Se recibieron los siguientes recursos asignados desde el orquestador: %s .",
				buffer);
		mensaje = string_split(buffer, ";");

		for (cont_msj = 0; mensaje[cont_msj] != '\0'; cont_msj++) {

			log_in_disk_niv(LOG_LEVEL_INFO,
					"Se recibio el recurso asignado desde el orquestador a un personaje (personaje,Recurso): %s",
					mensaje[cont_msj]);

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
				restarRecurso(t_personaje->ListaItemsss,
						nodo_lista_personaje->proximo_recurso->SIMBOLO);
			}

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
				"Se recibieron los recursos asignados desde el orquestador pero no se desbloqueó ningún personaje");

		break;
	}

}

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

void liberar_memoria(t_lista_personaje *personaje, ITEM_NIVEL *item) {
	liberar_recursos(personaje->l_recursos_optenidos, item);
	free(personaje->nombre_personaje);
	//free(personaje->proximo_recurso);
	free(personaje);
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

		recurso_aux->ref_recuso->cantidad = recurso_aux->ref_recuso->cantidad
				+ recurso_aux->cantidad;

		//si esta puesta la forma grafica y la tantidad de recursos es mayor o
		//sumo recurosos a la pantalla
		while (B_DIBUJAR && recurso_aux->cantidad--) {
			sumarRecurso(item, recurso_aux->SIMBOLO);
		}
	}
	list_destroy(recursos_otenido); //TODO Destruir los nodos de la lista!
}
