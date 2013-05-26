/*
 * lib_personaje.h
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#ifndef PERSONAJE_THR_H_
#define PERSONAJE_THR_H_

typedef struct{
	char *nomb_nivel;
	int pueto;
	int *sck_personaje;
	fd_set *readfds;
	fd_set *exceptfds;
} t_h_personaje;

void *escucho_personaje(void *p);

#endif /* LIB_PERSONAJE_H_ */
