/*
 * lib_personaje.h
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#ifndef PERSONAJE_THR_H_
#define PERSONAJE_THR_H_

#include <semaphore.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include "../manjo_pantalla/nivel_p.h"

typedef struct {
	char *nomb_nivel;
	int pueto;
	int sck_personaje;  // Socket mas alto (para el selet)
	fd_set *readfds;
	pthread_mutex_t *s_personaje_conectado;
	t_list *l_personajes;
	ITEM_NIVEL** ListaItemss;
	pthread_mutex_t *s_personaje_recursos; //uso el semaforo para asigunar liberar recursos y verificar el interbloqueo
} t_h_personaje;

typedef struct {
	char* nombre_personaje;
	char id_personaje;
	int sokc;
	struct h_t_recusos *proximo_recurso; //recurso actual del personaje
	t_list *l_recursos_optenidos; //recursos optenidos por el personaje
	bool bloquedo;
	bool recusos_sumados; //utilizo en la segunda pasadea (otnego_vector_diponibles) del algoritmo de interbloqueo
	int posX;
	int posY;
} t_lista_personaje;

void *escucho_personaje(void *p);
void add_personaje_lista(char id_personaje, char *nombre_personaje, int i,
		t_h_personaje *t_personaje);

#endif /* LIB_PERSONAJE_H_ */
