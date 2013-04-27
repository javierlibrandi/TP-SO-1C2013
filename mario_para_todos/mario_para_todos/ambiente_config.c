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

t_param_orq leer_archivo_orquestador_config() {
	t_config* config;
	t_param_orq param;

	log_in_disk_orq(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion %s ", PATH_CONFIG);

	config = config_create(PATH_CONFIG);

	param.IP = config_get_string_value(config, "IP");
	param.PUERTO = config_get_string_value(config, "PUERTO");

	log_in_disk_orq(LOG_LEVEL_TRACE, "ip %s puerto %s", param.IP, param.PUERTO);

	return param;
}

t_param_plat leer_archivo_plataforma_config() {
	t_config* config;
	t_param_plat param;

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion %s ", PATH_CONFIG);

	config = config_create(PATH_CONFIG);

	param.planificador_nivel = config_get_array_value(config, "planDeNiveles");

	//solo entro en el loop si el mivel de log es LOG_LEVEL_TRACE
	if (LOG_LEVEL_TRACE == 0) {
		int i = 0;
		while (param.planificador_nivel[i] != '\0') {
			log_in_disk_plat(LOG_LEVEL_TRACE, "Niveles de planificador %s ",
					param.planificador_nivel[i]);
			i++;
		}
	}

	return param;

}

