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

//void libero_memoria(t_h_personaje *t_personaje, struct t_param_nivel *param_nivel);
void sig_handler(int signo);

int main(void) {
	struct h_t_param_nivel param_nivel;
	int sck_plat;
	pthread_t escucho_personaje_th;
	t_h_personaje *t_personaje = malloc(sizeof(t_h_personaje));
	fd_set readfds;
	int i, tipo, rows, cols;
	char *buffer;
	rows = 37;
	cols = 167;
	char **mensaje;
	int tot_enviados, cont_msj;
	char *aux_mensaje;
	struct h_t_recusos *recurso;
	ITEM_NIVEL *ListaItems = NULL;

	//inicializo_pantalla();
	//nivel_gui_get_area_nivel(&rows, &cols);

	param_nivel = leer_nivel_config(rows, cols);

	t_personaje = malloc(sizeof(t_h_personaje));
	t_personaje->nomb_nivel = param_nivel.nom_nivel;
	t_personaje->pueto = param_nivel.PUERTO;

	char asignado;

<<<<<<< HEAD
	//recusos_pantalla(param_nivel.recusos);
=======
	recusos_pantalla(param_nivel.recusos, &ListaItems);

	nivel_gui_dibujar(ListaItems);
>>>>>>> 3261073a7dbc9b6142e2af0ae1fd96c3538cace9

	//conecxion con el planificador
	sck_plat = con_pla_nival(param_nivel.IP, param_nivel.PUERTO_PLATAFORMA,
			param_nivel.nom_nivel, param_nivel.PUERTO);

	FD_ZERO(&readfds);
	FD_SET(sck_plat, &readfds);
	t_personaje->readfds = &readfds;
	t_personaje->sck_personaje = &sck_plat;

	//creo el hilo que va a escuchar conexiones del personaje
	pthread_create(&escucho_personaje_th, NULL, (void*) escucho_personaje,
			(void*) t_personaje);

	for (;;) {
		if (select(*(t_personaje->sck_personaje) + 1, t_personaje->readfds,
				NULL, NULL, NULL ) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i <= *(t_personaje->sck_personaje); i++) {
			if (FD_ISSET(i, t_personaje->readfds)) {

				buffer = recv_variable(i, &tipo); // *(t_h_orq->sock) Para mi es i el 1er parametro del rec por que el socket que me respondio tiene ese valor.
				if (!strcmp(buffer, Leido_error)) {

					elimino_sck_lista(i, t_personaje->readfds);
				}
				mensaje = string_split(buffer, ";");

				log_in_disk_niv(LOG_LEVEL_TRACE, "Tipo de mensaje %d ", tipo);

				for (cont_msj = 0; mensaje[cont_msj] != '/0'; cont_msj++) {
					log_in_disk_niv(LOG_LEVEL_TRACE, "mensaje %d contenido %s",
							cont_msj, mensaje[cont_msj]);

				}
				int iter = 1;
				bool seguir = true;

				switch (tipo) {
				case P_TO_N_UBIC_RECURSO:
					while (iter < list_size(param_nivel.recusos) && seguir) {
						recurso = (struct h_t_recusos*) list_get(
								param_nivel.recusos, iter);
						if (mensaje[1][0] == recurso->SIMBOLO) {

							aux_mensaje = string_from_format("%d;%d",
									recurso->posX, recurso->posY);

							//TODO falta el tipo
							fd_mensaje(i, N_TO_P_UBIC_RECURSO, aux_mensaje,
									&tot_enviados);
							free(aux_mensaje);
							seguir = false;
						}
						iter++;
					}
					break;
				case P_TO_N_MOVIMIENTO:
					while (iter < list_size(param_nivel.recusos) && seguir) {
						recurso = (struct t_recusos*) list_get(
								param_nivel.recusos, iter);

						if (mensaje[0][0] == recurso->SIMBOLO) {
							recurso->posX = atoi(mensaje[3]);
							recurso->posY = atoi(mensaje[4]);
							aux_mensaje = string_from_format("%d;%d",
									recurso->posX, recurso->posY);
							//TODO falta el tipo
							fd_mensaje(i, N_TO_P_MOVIDO, aux_mensaje,
									&tot_enviados);
							seguir = false;
							free(aux_mensaje);
						}
						iter++;
					}
					break;
				case P_TO_N_INICIAR_NIVEL:

					personaje_pantalla(mensaje[1][0], 1, 1, &ListaItems);
					//personaje_pantalla('k',15,15,&ListaItems,ListaItems);
					nivel_gui_dibujar(ListaItems);
					break;

				case P_TO_N_SOLIC_RECURSO:
					asignado = 'n';
					while (iter < list_size(param_nivel.recusos) && seguir) {
						recurso = (struct t_recusos*) list_get(
								param_nivel.recusos, iter);
						if (mensaje[3][0] == recurso->SIMBOLO) {
							if ((recurso->posX = atoi(mensaje[1]))
									&& (recurso->posY = atoi(mensaje[2]))) {
								if (recurso->cantidad >= 1) {
									aux_mensaje = string_from_format("%d;%d",
											recurso->posX, recurso->posY);
									//TODO falta el tipo
									fd_mensaje(i, N_TO_P_RECURSO_OK,
											aux_mensaje, &tot_enviados);
									free(aux_mensaje);
									asignado = 's';
									//TODO esto que es???
									//fd_mensaje(i, "definir mensaje ok");
									seguir = false;
								}
							}
						}
						iter++;
					}
					if (asignado != 's') {
						//TODO esto que es???
						//fd_mensaje(i, "error blabla");
					}
				}
				break;
//							switch (tipo) {
//							case P_TO_N_INICIAR_NIVEL:
//								//concatena
//								sprintf(buffer,"%s;%d","hola",5);
//								//func p mandar mensajes... (i ->puerto, "mensaje ; mensaje")
//								fd_mensaje(i,"sdsd;sds");
//
//
//
//
//							default:
//								;
//							}
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
