/*
 * entorno.h
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */

#ifndef ENTORNO_H_
#define ENTORNO_H_
#include <pthread.h>

#define PATH_PLATAFORMA_LOG "log/tracer.log"
#define PATH_PLATAFORMA_LOG_NIVEL "/home/utnso/git/tp-20131c-gaturro/nivel/log/tracer.log"

#define LOG_NIVEL LOG_LEVEL_TRACE
//		niveles de log
//		LOG_LEVEL_TRACE,
//		LOG_LEVEL_DEBUG,
//		LOG_LEVEL_INFO,
//		LOG_LEVEL_WARNING,
//		LOG_LEVEL_ERROR

#define  Leido_error "Error en la lectura del socket......"

typedef struct{
	char * nombre;
	char * nivel;
	char simbolo;
	unsigned long int sec_entrada;
	int sck;
}t_personaje; // re subido

#endif /* ENTORNO_H_ */
