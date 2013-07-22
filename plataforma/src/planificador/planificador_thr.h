/*
 * planificador_thr.h
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */

#ifndef PLANIFICADOR_THR_H_
#define PLANIFICADOR_THR_H_

#include <pthread.h>
#include <commons/collections/list.h>
#include <mario_para_todos/ambiente_config.h>


typedef struct t_h_parlante{
	char *desc_nivel;
	pthread_t sock;
}t_h_parlante;

void *planificador_nivel_thr(void *p);
void eliminar_planificador(int sck, t_list *list_planifidores);

#endif /* PLANIFICADOR_THR_H_ */
