/*
 * personaje_thr.c
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/entorno.h>
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/grabar.h>
#include "personaje_thr.h"
#include <semaphore.h>
#include "../manjo_pantalla/pantalla.h"
#include "../manjo_pantalla/nivel_p.h"
#include "../manjo_pantalla/tad_items.h"
#include <string.h>
#include <commons/string.h>

void *escucho_personaje(void *p) {
	int sck, new_sck;
	t_h_personaje *t_personaje = (t_h_personaje*) p;
	char ip_cliente[16];
	char *buffer = NULL;
	int tipo, tot_enviados, tipo_mensaje;
	char **mensaje;
	log_in_disk_niv(LOG_LEVEL_TRACE,
			"Ecucho conexiones de los personajes en el puerto %d \t soy el nivel %s ",
			t_personaje->pueto, t_personaje->nomb_nivel);

	sck = Abre_Socket_Inet(t_personaje->pueto);
	t_personaje->sck_personaje = 0;

	for (;;) {

		new_sck = Acepta_Conexion_Cliente(sck, ip_cliente);

		if (new_sck == -1) {
			log_in_disk_niv(LOG_LEVEL_ERROR,
					"Error con la conexion del personaje %d  ", new_sck);
			exit(1);
		}

		buffer = recv_variable(new_sck, &tipo);

		mensaje = string_split(buffer, ";");

		log_in_disk_niv(LOG_LEVEL_TRACE,
				"1er mensaje recibido en el nivel. Tipo de mensaje %d ", tipo);

		switch (tipo) {

		case P_TO_N_INICIAR_NIVEL:

			log_in_disk_niv(LOG_LEVEL_TRACE,
					"Nivel recibe solicitud de inicio en su mapa.");

			pthread_mutex_lock(t_personaje->s_personaje_recursos);
			personaje_pantalla(mensaje[1][0], 1, 1, t_personaje->ListaItemss);
			add_personaje_lista(mensaje[1][0], mensaje[0], new_sck,
					t_personaje);
			pthread_mutex_unlock(t_personaje->s_personaje_recursos);

			tipo_mensaje = OK;

			if (new_sck > t_personaje->sck_personaje) {
				t_personaje->sck_personaje = new_sck;
			}
			FD_SET(new_sck, t_personaje->readfds);

			log_in_disk_niv(LOG_LEVEL_TRACE,
					"Acepto la conexion del personaje en el socket %d  ",
					new_sck);
			pthread_mutex_unlock(t_personaje->s_personaje_conectado);
			usleep(500000);

			fd_mensaje(new_sck, tipo_mensaje, "Nivel iniciado.", &tot_enviados);

			log_in_disk_niv(LOG_LEVEL_INFO,
					"****** El personaje %s inicia el nivel ******",
					mensaje[0]);

			break;
		}

		free(buffer);

	}

}

void add_personaje_lista(char id_personaje, char *nombre_personaje, int i,
		t_h_personaje *t_personaje) {

	t_lista_personaje *list_personajes;

	log_in_disk_niv(LOG_LEVEL_TRACE, "funcion add_personaje_lista ");

	list_personajes = malloc(sizeof(t_lista_personaje));
	list_personajes->id_personaje = id_personaje;
	list_personajes->nombre_personaje = malloc(strlen(nombre_personaje) + 1);
	list_personajes->nombre_personaje = nombre_personaje;
	list_personajes->sokc = i;
	list_personajes->l_recursos_optenidos = list_create();
	list_personajes->bloquedo = true;
	list_personajes->proximo_recurso = NULL;
	list_add(t_personaje->l_personajes, list_personajes);

	log_in_disk_niv(LOG_LEVEL_TRACE,
			"Agrego a la lista el personaje con  id_personaje %c el nombre %s socket %d",
			list_personajes->id_personaje, list_personajes->nombre_personaje,
			list_personajes->sokc);
}
