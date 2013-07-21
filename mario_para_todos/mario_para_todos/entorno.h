/*
 * entorno.h
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */

#ifndef ENTORNO_H_
#define ENTORNO_H_
#include <pthread.h>
#include <commons/collections/list.h>
#include "ambiente_config.h"
#include <stdlib.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_PLATAFORMA_LOG "log/tracer.log"
#define PATH_CONFIG "resources/properties.cfg"
//#define PATH_PLATAFORMA_LOG_NIVEL "/home/utnso/Dropbox/tp-20131c-gaturro/nivel/log/tracer.log"
//#define PATH_CONFIG_NIVEL "/home/utnso/Dropbox/tp-20131c-gaturro/nivel/resources/properties.cfg"
//#define PATH_CONFIG_INOTIFY "/home/utnso/Dropbox/tp-20131c-gaturro/plataforma/resources/"
#define PATH_PLATAFORMA_LOG_NIVEL "/home/utnso/git/tp-20131c-gaturro/nivel/log/tracer.log"
#define PATH_CONFIG_NIVEL "/home/utnso/git/tp-20131c-gaturro/nivel/resources/properties.cfg"
#define PATH_CONFIG_INOTIFY "/home/utnso/git/tp-20131c-gaturro/plataforma/resources/"
#define B_DIBUJAR false


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
	char prox_recurso;
}t_personaje; // re subido

typedef struct{
	char* nombre;
	//t_recusos* recursos;
	char **recursos;
}Nivel;

typedef struct{
	 int x;
	 int y;
} Posicion;

typedef struct{
	char *nombre;
	char simbolo;
	t_list *niveles;
	int nivelActual;
	int vidas;
	char *ip_orquestador;
	int puerto_orquestador;
	int sockNivel;
	int sockPlanif;
	Nivel infoNivel;
	char recursoActual;
	int indexRecurso;
	Posicion posProxRecurso;
	Posicion posActual;
	bool finNivel;
	int vidasIniciales;
}Personaje;


#endif /* ENTORNO_H_ */
