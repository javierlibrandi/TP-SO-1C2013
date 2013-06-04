/*
 * horquestador_thr.h
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */

#ifndef ORQUESTADOR_THR_H_
#define ORQUESTADOR_THR_H_

#include "../plataforma.h"

typedef struct {
	int *sock;
	fd_set *readfds;
	t_estados *lista_estados;
	t_list * planificadores;
	pthread_mutex_t *s_lista_plani;
} t_h_orquestadro;
/**
 * Este es el hilo que va a escucha a los personajes
 */
void *orequestador_thr(void *p);
bool busca_planificador(const char *desc_nivel, t_list *list_plataforma, char * msj);


#endif /* ORQUESTADOR_THR_H_ */
