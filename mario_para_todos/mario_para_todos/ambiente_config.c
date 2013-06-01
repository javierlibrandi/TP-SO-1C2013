/*
 * ambiente_config.c

 *
 *  Created on: 18/04/2013
 *      Author: javier
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "ambiente_config.h"
#include "grabar.h"
#include <commons/string.h>
#include <commons/collections/list.h>
#include <string.h>

//struct t_param_orq leer_archivo_orquestador_config() {
//	t_config* config;
//	struct t_param_orq param;
//
//	log_in_disk_orq(LOG_LEVEL_TRACE,
//			"comienzo lectura archivo de configuracion orquestador en el path %s ",
//			PATH_CONFIG);
//
//	config = config_create(PATH_CONFIG);
//
//	//param.IP = config_get_string_value(config, "IP");
//	//param.PUERTO = config_get_int_value(config, "PUERTO");
//
//	//log_in_disk_orq(LOG_LEVEL_TRACE, "ip %s puerto %d", param.IP, param.PUERTO);
//
//	return param;
//}

t_param_plat leer_archivo_plataforma_config() {
	t_config* config;
	t_param_plat param;

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion plataforma en el %s ",
			PATH_CONFIG);

	config = config_create(PATH_CONFIG);

	param.PUERTO = config_get_int_value(config, "PUERTO");

	//param.planificador_nivel = config_get_array_value(config, "planDeNiveles");

	//solo entro en el loop si el mivel de log es LOG_LEVEL_TRACE
//	if (LOG_LEVEL_TRACE == 0) {
//		int i = 0;
//		while (param.planificador_nivel[i] != '\0') {
//			log_in_disk_plat(LOG_LEVEL_TRACE, "Niveles de planificador %s ",
//					param.planificador_nivel[i]);
//			i++;
//		}
//	}

	return param;

}

t_param_plan leer_archivo_plan_config(char *nivel) {
	t_config* config;
	t_param_plan param;

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion panificador nivel en el path %s ",
			PATH_CONFIG);

	config = config_create(PATH_CONFIG);

	param.PUERTO = config_get_int_value(config, nivel);

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"el nivel  %s  tiene asignado el puerto %d", nivel, param.PUERTO);

	return param;
}

t_param_nivel leer_nivel_config(char *nivel) {
	t_config* config;
	t_param_nivel param;
	t_recusos *list_recursos; //declaro el tipo nodo
	char filename[20];
	int i,j;


	char **aux;
	char *aux_char;

	log_in_disk_niv(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion del nivel en el %s ",
			PATH_CONFIG);

	param.recusos = list_create(); //creo lista de hilos
	config = config_create(PATH_CONFIG);

	param.nom_nivel = nivel; //copio el nombre del nivel que pase por parametro en el main

	aux_char = config_get_string_value(config, "PLATAFORMA");

	aux = string_split(aux_char, ":");
	param.IP = aux[0];
	param.PUERTO_PLATAFORMA = atoi(aux[1]);

	param.PUERTO = config_get_int_value(config, "PUERTO");


	for (i = 1; i < 255; i++) {

		sprintf(filename, "%s%d", "Caja", i);

		aux = config_get_array_value(config, filename);

		list_recursos = malloc(sizeof(t_recusos)); //creo el nodo
		j=0;

		strcpy(list_recursos->NOMBRE, aux[j++]);
		if (!strcmp(list_recursos->NOMBRE,"Fin")){
			break; //salgo del for cuando encuentro la caja de fin
		}
		list_recursos->SIMBOLO = aux[j++][0];
		list_recursos->cantidad = atoi(aux[j++]);
		list_recursos->posX = atoi(aux[j++]);
		list_recursos->posY = atoi(aux[j]);

		log_in_disk_niv(LOG_LEVEL_TRACE,
				"Cargo a la lista la caja %s que contiene el recurso %s con el simbolo %c y la candidad %d en la poscion [x,y] [%d][%d] ",
				filename,list_recursos->NOMBRE, list_recursos->SIMBOLO,
				list_recursos->cantidad, list_recursos->posX,
				list_recursos->posY);

		list_add(param.recusos, list_recursos); //agrego el nuevo nodo
	}

	return param;

}
