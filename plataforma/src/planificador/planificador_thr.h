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
	int *sock; //socket por el cual se comunica, es creado por la plataforma y pasado al planificador
	fd_set *readfds; // puntero al fd_set para agregarlo en el select() dentro del hilo
	char ip[16];
	char puerto[6];
	t_list *lista_planificadores; //tengo una referencia a los planificadores para sacarlo de la lista si es necesario
	t_list *l_listos;
	t_list *l_bloquedos;
	t_list *l_errores;
	pthread_mutex_t *s_lista_plani;
	pthread_mutex_t *s_listos;
	pthread_mutex_t *s_bloquedos;
	pthread_mutex_t *s_errores;
	int segundos_espera;
	int *cuantum;
	int sck_planificador; //guardo el socked del planificador para poder diferencialo de los personajes
} t_h_planificador;

typedef struct t_h_parlante{
	char *desc_nivel;
	pthread_t sock;
}t_h_parlante;

void *planificador_nivel_thr(void *p);

#endif /* PLANIFICADOR_THR_H_ */
