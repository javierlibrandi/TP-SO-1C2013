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

} t_h_interbloqueo;

void liberar_recursos(t_list *recursos_otenido, ITEM_NIVEL *item);

void liberar_memoria(t_lista_personaje *personaje, ITEM_NIVEL *item);

t_lista_personaje *busco_personaje(int sck, t_list *l_personajes, int *i);

void imprmir_recursos_nivel(t_list * recursos);

void add_recurso_personaje(t_list *l_recursos_optenidos,
		struct h_t_recusos *recurso_actual);

void desbloquear_Personajes(char * recursos_personaje, char *buffer,
		t_h_personaje * t_personaje, struct h_t_param_nivel param_nivel,
		int sck_plat);

char* listarRecursosPersonaje(t_list * lista_Recursos);
t_lista_personaje *busca_personaje_simbolo(char id, t_list *l_personajes,
		int *indice_personaje);
void elimino_personaje_lista_nivel(int sck, t_list *l_personajes,
		ITEM_NIVEL *item);

void *detecto_interbloque(void *p);
struct h_t_recusos *busco_recurso(char id, t_list *recusos);


#endif /* DETECTO_INTERBLOQUE_TH_H_ */
