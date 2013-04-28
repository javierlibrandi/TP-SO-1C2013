/*
 * planificador_thr.h
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */

#ifndef PLANIFICADOR_THR_H_
#define PLANIFICADOR_THR_H_

#include <pthread.h>

typedef struct{
	pthread_t planificador_thr; //descriptor del hilo
	char *desc_nivel; //nombre del nivel al que reprecenta el hilo
}t_h_planificador;


void *planificador_nivel_thr(void *p);

#endif /* PLANIFICADOR_THR_H_ */
