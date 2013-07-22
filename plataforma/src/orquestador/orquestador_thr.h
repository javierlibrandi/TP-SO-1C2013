/*
 * horquestador_thr.h
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */

#ifndef ORQUESTADOR_THR_H_
#define ORQUESTADOR_THR_H_

#include "../plataforma.h"


/**
 * Este es el hilo que va a escucha a los personajes
 */
void *orequestador_thr(void *p);
bool busca_planificador(char *desc_nivel, t_list *list_plataforma, char * msj);
bool busca_planificador_socket(int sock, t_list *list_plataforma,
		t_h_planificador * planificador);
void buscar_bloqueados_recurso(char * recur, char * nivel, t_list* bloqueados,
		t_personaje *pers);
bool busca_planificador_2(char *desc_nivel, t_list *list_plataforma, char * msj,
		t_h_planificador* h_planificador);


#endif /* ORQUESTADOR_THR_H_ */
