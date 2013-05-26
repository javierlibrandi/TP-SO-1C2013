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

void libero_memoria(t_h_personaje *t_personaje);

int main(int argc, char *argv[], char *env[]) {
	t_param_nivel param_nivel;
	int sck_plat;
	pthread_t escucho_personaje_th;
	t_h_personaje *t_personaje;


	param_nivel = leer_nivel_config(argv[1]);

	//conecxion con el planificador
	sck_plat = con_pla_nival(param_nivel.IP, param_nivel.PUERTO_PLATAFORMA, param_nivel.nom_nivel);

	t_personaje = malloc(sizeof(t_h_personaje));
	t_personaje->nomb_nivel=param_nivel.nom_nivel;
	t_personaje->pueto= param_nivel.PUERTO;

	//creo el hilo que va a escuchar conexiones del personaje
	pthread_create(&escucho_personaje_th, NULL, (void*) escucho_personaje,
			(void*) t_personaje);

	pthread_join(escucho_personaje_th, NULL);

	libero_memoria(t_personaje);

	return EXIT_SUCCESS;

}

void libero_memoria(t_h_personaje *t_personaje){
	free(t_personaje->nomb_nivel);
	free(t_personaje);
}
