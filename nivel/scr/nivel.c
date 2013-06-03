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

void libero_memoria(t_h_personaje *t_personaje, t_param_nivel *param_nivel);
void sig_handler(int signo);

int main(void) {
	t_param_nivel param_nivel;
	int sck_plat;
	pthread_t escucho_personaje_th;
	t_h_personaje *t_personaje;
	fd_set readfds;
	int i, tipo;
	char *buffer;


	param_nivel = leer_nivel_config();

	t_personaje = malloc(sizeof(t_h_personaje));
	t_personaje->nomb_nivel = param_nivel.nom_nivel;
	t_personaje->pueto = param_nivel.PUERTO;

	inicializo_pantalla();
	recusos_pantalla(param_nivel.recusos);

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
				buffer = recv_variable(*(t_personaje->sck_personaje), &tipo);

				free(buffer);

			}
		}
	}

	pthread_join(escucho_personaje_th, NULL );

	libero_memoria(t_personaje, &param_nivel);

	nivel_gui_terminar();

	signal(SIGTERM, sig_handler);

	return EXIT_SUCCESS;

}

void libero_memoria(t_h_personaje *t_personaje, t_param_nivel *param_nivel) {
	libero_recursos_pantalla(param_nivel->recusos);

	free(t_personaje->nomb_nivel);
	free(t_personaje->readfds);
	free(t_personaje->sck_personaje);
	free(t_personaje);

}

void sig_handler(int signo) {
	log_in_disk_niv(LOG_LEVEL_INFO, "capturando la señal");
	if (signo == SIGTERM)
		nivel_gui_terminar();

}
