/*
 * detecto_interbloque_th.h
 *
 *  Created on: 15/07/2013
 *      Author: utnso
 */

#ifndef DETECTO_INTERBLOQUE_TH_H_
#define DETECTO_INTERBLOQUE_TH_H_

#include "../escuchar_personaje/personaje_thr.h"
#include <pthread.h>
#include <semaphore.h>

typedef struct {
	t_h_personaje t_personaje;
	struct h_t_param_nivel param_nivel;

}t_h_interbloqueo;



void *detecto_interbloque(void *p);
struct h_t_recusos *busco_recurso(char id, t_list *recusos);


#endif /* DETECTO_INTERBLOQUE_TH_H_ */
