/*
 * grabar.h
 *
 *  Created on: 19/04/2013
 *      Author: javier
 */

#ifndef GRABAR_H_
#define GRABAR_H_

#include <commons/log.h>
#define PATH_PLATAFORMA_LOG "log/tracer.orquestador.log"
#define LOG_NIVEL LOG_LEVEL_TRACE
//		niveles de log
//		LOG_LEVEL_TRACE,
//		LOG_LEVEL_DEBUG,
//		LOG_LEVEL_INFO,
//		LOG_LEVEL_WARNING,
//		LOG_LEVEL_ERROR


/**Grabo log orquestador*/
void log_in_disk_orq(t_log_level nivel,const char* format, ... );

/**Grabo log plataforma*/
void log_in_disk_plat(t_log_level nivel,const char* format,...);

/**Grabo log planificador*/
void log_in_disk_plan(t_log_level nivel,const char* format, ...);

/**Grabo log nivel*/
void log_in_disk_niv(t_log_level nivel,const char* format, ...);

/**Grabo log personaje*/
void log_in_disk_per(t_log_level nivel,const char* format, ...);

#endif /* GRABAR_H_ */
