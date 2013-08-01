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
	int tot_enviados, tipo, indice;
	char * buffer;
	memcpy(&h_interbloqueo, (t_h_interbloqueo*) p, sizeof(t_h_interbloqueo));
	memcpy(&t_personaje, &h_interbloqueo.t_personaje, sizeof(t_h_personaje));
	memcpy(&param_nivel, &h_interbloqueo.param_nivel,
			sizeof(struct h_t_param_nivel));
	t_lista_personaje * un_personaje_aux;
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

	/*	if (cantidad_interbloquedos(t_personaje.l_personajes,
				&personaje_bloquedos) != 0) {

			log_in_disk_niv(LOG_LEVEL_INFO,
					"#######################INTER_BLOQUEO  SE DETECTO UN INTERBLOQUEO EN EL NIVEL: %s. Y LOS PERSONAJES QUE PARTICIPAN DEL MISMO SON: %s.  #######INTER_BLOQUEO",
					param_nivel.nom_nivel, personaje_bloquedos);

			if (param_nivel.Recovery) {
				//TODO envio mensaje personajes interbloquedos para que se elija a la visticm
				fd_mensaje(t_personaje.sck_orquestador, N_TO_O_RECOVERY,
						personaje_bloquedos, &tot_enviados);

				buffer = recv_variable(t_personaje.sck_orquestador, &tipo);

				if (tipo == O_TO_N_MUERTE) {
					un_personaje_aux = busca_personaje_simbolo(buffer[0],
							t_personaje.l_personajes, &indice);

					buffer = recv_variable(un_personaje_aux->sokc, &tipo);

					if (!strcmp(buffer, Leido_error)) {
						pthread_mutex_lock(&s_personaje_recursos);
						elimino_personaje_lista_nivel(i,
								t_personaje->l_personajes, ListaItems);
						elimino_sck_lista(i, t_personaje->readfds);
						pthread_mutex_unlock(&s_personaje_recursos);
					}

					log_in_disk_niv(LOG_LEVEL_INFO,
							"#######################INTER_BLOQUEO Se recibió un tipo de mensaje %d: %s #######################INTER_BLOQUEO",
							tipo, buffer);

					switch (tipo)
					mensaje = string_split(buffer, ";");
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
						nivel_gui_dibujar(ListaItems);
					}

					recursos_personaje = "";
					recursos_personaje = listarRecursosPersonaje(
							nodo_lista_personaje->l_recursos_optenidos);
					tipo_mensaje = OK;

					fd_mensaje(i, tipo_mensaje, "Saliste del nivel.",
							&tot_enviados);
					elimino_personaje_lista_nivel(i, t_personaje->l_personajes,
							ListaItems);
					elimino_sck_lista(i, t_personaje->readfds);
					pthread_mutex_unlock(&s_personaje_recursos);

					desbloquear_Personajes(recursos_personaje, buffer,
							t_personaje, param_nivel, sck_plat);

					log_in_disk_niv(LOG_LEVEL_INFO,
							"Se eliminó el personaje del nivel. Se liberan sus recursos.");
					//TODO Listar recursos

					break;

				}

				//Recibir la victima, e iniciar el desbloqueo informando al orquestador.
				pthread_mutex_unlock(t_personaje.s_personaje_recursos);
			}

		} else {
			pthread_mutex_unlock(t_personaje.s_personaje_recursos);
		}

		//free(personaje_bloquedos);
		free(buffer);
		personaje_bloquedos = NULL;
	}
	return NULL ;
} */

} }  //BORRAR ESTAS 2 llaves cuando descomento lo de arriba.

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
						l_personaje->proximo_recurso->cantidad,
						l_personaje->proximo_recurso->NOMBRE);
				marcados++;
			} else {

				if (l_personaje->proximo_recurso->posX != l_personaje->posX
						|| l_personaje->proximo_recurso->posY
								!= l_personaje->posY) { //si el proximo recurso del personaje es 0 pero el personaje no llego al recurso por lo tanto no esta bloqueado
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

	for (cont_personajes = 0; cont_personajes < tot_perosnajes;
			cont_personajes++) {
		l_personaje = list_get(personajes, cont_personajes);

		if (l_personaje->bloquedo) {
			tot_interbloquedos++;

			if (personajes_bloquedos == NULL ) {
				string_append(personajes_bloquedos, &l_personaje->id_personaje);
			} else {
				string_append(personajes_bloquedos, ";");
				string_append(personajes_bloquedos, &l_personaje->id_personaje);
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
