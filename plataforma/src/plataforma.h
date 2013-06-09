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

typedef struct{
	t_list *prj_listo;
	t_list *prj_bloquedo;
}t_estados;


typedef struct{
	char * nombre;
	char * nivel;
	char simbolo;
	unsigned long int sec_entrada;
}t_personaje; // re subido

bool existe_personaje(const char *nombre_personaje, char simbolo,
		t_list *list_personaje);



#endif /* PLATAFORMA_H_ */
