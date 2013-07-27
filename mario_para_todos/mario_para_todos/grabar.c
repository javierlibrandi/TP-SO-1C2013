/*
 * grabar.c
 *
 *  Created on: 19/04/2013
 *      Author: javier
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include "grabar.h"
#include <stdbool.h>
#include <sys/types.h>
#include <commons/temporal.h>
#include <commons/error.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/process.h>
#include "entorno.h"

static void grabar_log(t_log_level nivel, t_log* logger,
		const char* message_template, va_list list_arguments);

/** funcio privada */
static void grabar_log(t_log_level nivel, t_log* logger,
		const char* message_template, va_list list_arguments) {
	char *message = string_from_vformat(message_template, list_arguments);
	switch (nivel) {
	case LOG_LEVEL_TRACE:
		log_trace(logger, message);
		break;
	case LOG_LEVEL_DEBUG:
		log_debug(logger, message);
		break;
	case LOG_LEVEL_INFO:
		log_info(logger, message);
		break;
	case LOG_LEVEL_WARNING:
		log_warning(logger, message);
		break;
	default:
		log_error(logger, message);
	}
}

static void iniciar_log(char* file) {
	FILE *fichero;
	fichero = fopen(file, "w");
	fclose(fichero);
}

/**Grabo log orquestador*/
void log_in_disk_orq(t_log_level nivel, const char* format, ...) {

	t_log* logger;

	logger = log_create(PATH_PLATAFORMA_LOG, "ORQUESTADOR", true, LOG_NIVEL);

	va_list arguments;
	va_start(arguments, format);

	grabar_log(nivel, logger, format, arguments);

	va_end(arguments);

	log_destroy(logger);
}

void log_in_disk_plat(t_log_level nivel, const char* format, ...) {
	static bool borrar = true;
	t_log* logger;

	if (borrar)
		iniciar_log(PATH_PLATAFORMA_LOG);
	borrar = false;

	logger = log_create(PATH_PLATAFORMA_LOG, "PLATAFORMA", true, LOG_NIVEL);

	va_list arguments;
	va_start(arguments, format);

	grabar_log(nivel, logger, format, arguments);

	va_end(arguments);

	log_destroy(logger);
}

void log_in_disk_plan(t_log_level nivel, const char* format, ...) {

	t_log* logger;
	logger = log_create(PATH_PLATAFORMA_LOG, "PLANIFICADOR", true, LOG_NIVEL);

	va_list arguments;
	va_start(arguments, format);

	grabar_log(nivel, logger, format, arguments);

	va_end(arguments);

	log_destroy(logger);
}

void log_in_disk_niv(t_log_level nivel, const char* format, ...) {

	t_log* logger;
	logger = log_create(PATH_PLATAFORMA_LOG_NIVEL, "NIVEL", !B_DIBUJAR,
			LOG_NIVEL);

	va_list arguments;
	va_start(arguments, format);

	grabar_log(nivel, logger, format, arguments);

	va_end(arguments);

	log_destroy(logger);
}

void log_in_disk_per(t_log_level nivel, const char* format, ...) {

	t_log* logger;
	logger = log_create(PATH_PERSONAJE_LOG, "PERSONAJE", true, LOG_NIVEL);

	va_list arguments;
	va_start(arguments, format);

	grabar_log(nivel, logger, format, arguments);

	va_end(arguments);

	log_destroy(logger);
}

void log_in_disk_mensajes(t_log_level nivel, const char* format, ...) {

	t_log* logger;
	logger = log_create(PATH_PERSONAJE_LOG, "MENSAJE", false, LOG_NIVEL);

	va_list arguments;
	va_start(arguments, format);

	grabar_log(nivel, logger, format, arguments);

	va_end(arguments);

	log_destroy(logger);
}
