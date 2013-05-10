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


typedef struct {
	pthread_t planificador_thr; //descriptor del hilo
	char *desc_nivel; //nombre del nivel al que reprecenta el hilo
	int sock; //socket por el cual se comunica, es creado por la plataforma y pasado al planificador
} t_h_planificador;

typedef struct t_h_parlante{
	char *desc_nivel;
	pthread_t sock;
}t_h_parlante;

void *planificador_nivel_thr(void *p);

#endif /* PLANIFICADOR_THR_H_ */
