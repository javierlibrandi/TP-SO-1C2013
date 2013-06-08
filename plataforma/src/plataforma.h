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
	char * simbolo;
}t_personaje; // re subido




#endif /* PLATAFORMA_H_ */
