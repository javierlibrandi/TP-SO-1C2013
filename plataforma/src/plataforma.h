/*
 * plataforma.h
 *
 *  Created on: 21/05/2013
 *      Author: utnso
 */

#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_
#include <commons/collections/list.h>
#include <pthread.h>



t_personaje *busca_personaje_simbolo_pla(char id, t_list *l_personajes,
		int *indice_personaje);

bool existe_personaje(const char *nombre_personaje, char simbolo,
		t_list *list_personaje);
t_personaje *busca_personaje_skc(int sck, t_list *l_listos,
		int *indice_personaje);


#endif /* PLATAFORMA_H_ */
