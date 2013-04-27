/*
 * FileDescriptors.h
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */

//Se definen los tipos de header para
typedef enum {
	PERSONAJE
} t_header_mensaje;

typedef struct {
	char t_header_mensaje;
	char payLoadLength[2];
	void * payLoad[128];
} t_trama;

