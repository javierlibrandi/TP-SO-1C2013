/*
 * lib_personaje.h
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#ifndef PERSONAJE_THR_H_
#define PERSONAJE_THR_H_

#include <semaphore.h>

typedef struct{
	char *nomb_nivel;
	int pueto;
	int sck_personaje;
	fd_set *readfds;
	pthread_mutex_t *s_personaje_conectado;
	t_list *l_personajes;
} t_h_personaje;

typedef struct{
	char* nombre_personaje;
	char id_personaje;
	int	sokc;
}t_lista_personaje;

void *escucho_personaje(void *p);

#endif /* LIB_PERSONAJE_H_ */
