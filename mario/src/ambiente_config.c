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



t_param leer_archivo_orquestador_config() {
	t_config* config;
	t_param p_orquestador;


	log_in_disk_orq(LOG_LEVEL_TRACE,"comienzo lectura archivo de configuracion %s ",PATH_PLATAFORMA_CONFIG);

	config = config_create(PATH_PLATAFORMA_CONFIG);

	p_orquestador.IP = config_get_string_value(config, "IP");
	p_orquestador.PUERTO  = config_get_string_value(config, "PUERTO");


	log_in_disk_orq(LOG_LEVEL_TRACE,"ip %s puerto %s", p_orquestador.IP, p_orquestador.PUERTO );


	return p_orquestador;
}

