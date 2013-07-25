/*
 * grabar.h
 *
 *  Created on: 19/04/2013
 *      Author: javier
 */

#ifndef GRABAR_H_
#define GRABAR_H_

#include <commons/log.h>


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

/**Grabo log pasaje de mensajes*/
void log_in_disk_mensajes(t_log_level nivel,const char* format, ...);

#endif /* GRABAR_H_ */
