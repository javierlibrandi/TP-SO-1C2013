/*
 * inotify_thr.c
 *
 *  Created on: 12/07/2013
 *      Author: utnso
 */
#include "inotify_thr.h"
#include <sys/inotify.h>
#include <mario_para_todos/entorno.h>
#include <mario_para_todos/ambiente_config.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <mario_para_todos/grabar.h>
#include <string.h>
#include <pthread.h>
// El tamaño de un evento es igual al tamaño de la estructura de inotify
// mas el tamaño maximo de nombre de archivo que nosotros soportemos
// en este caso el tamaño de nombre maximo que vamos a manejar es de 24
// caracteres. Esto es porque la estructura inotify_event tiene un array
// sin dimension ( Ver C-Talks I - ANSI C ).
#define EVENT_SIZE  ( sizeof (struct inotify_event) + strlen(PATH_CONFIG_INOTIFY) )

// El tamaño del buffer es igual a la cantidad maxima de eventos simultaneos
// que quiero manejar por el tamaño de cada uno de los eventos. En este caso
// Puedo manejar hasta 1024 eventos simultaneos.
#define BUF_LEN     ( 1024 * EVENT_SIZE )

void *inotify_thr(void* p) {
	t_param_plat *param_plataforma = (t_param_plat *) p;
	t_param_plat param; //lo uso pare leer todo el archivo de configuracion, despues me quedo con lo que necesito
	int offset;
	int watch_descriptor;
	char buffer[BUF_LEN];
	// Al inicializar inotify este nos devuelve un descriptor de archivo
	int file_descriptor = inotify_init();
	int length;
	int cuantum;
	// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
	// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
	// a sizeof( struct inotify_event ) + 24.
	struct inotify_event *event;

	log_in_disk_plat(LOG_LEVEL_TRACE, "inotify_thr");

	if (file_descriptor < 0) {
		perror("inotify_init");
	}

	// Creamos un monitor sobre un path indicando que eventos queremos escuchar
	watch_descriptor = inotify_add_watch(file_descriptor, PATH_CONFIG_INOTIFY,
			IN_MODIFY);
	for (;;) {
		// El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
		// para leer esta información el descriptor se lee como si fuera un archivo comun y corriente pero
		// la diferencia esta en que lo que leemos no es el contenido de un archivo sino la información
		// referente a los eventos ocurridos
		length = read(file_descriptor, buffer, BUF_LEN);
		if (length < 0) {
			perror("read");
		}

		offset = 0;

		// Luego del read buffer es un array de n posiciones donde cada posición contiene
		// un eventos ( inotify_event ) junto con el nombre de este.
		while (offset < length) {

			event = (struct inotify_event *) &buffer[offset];

			// El campo "len" nos indica la longitud del tamaño del nombre
			if (event->len) {
				// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
				// sea un archivo o un directorio
				if (event->mask & IN_MODIFY) {
					if (event->mask & IN_ISDIR) {
						printf("The directory %s was modified.\n", event->name);
					} else {
						*param_plataforma = leer_archivo_plataforma_config();
						//cuantum = leer_archivo_plataforma_config_cuantum();
						log_in_disk_plat(LOG_LEVEL_DEBUG, "Nuevo cuantum de %d",
								param_plataforma->CUANTUM);
//						log_in_disk_plat(LOG_LEVEL_DEBUG, "Nuevo cuantum de %d",
//								cuantum);
						//memcpy(&(param_plataforma->CUANTUM),&cuantum,sizeof(int));

					}
				}

			}
			offset += sizeof(struct inotify_event) + event->len;
		}

		log_in_disk_plat(LOG_LEVEL_DEBUG, "Nuevo cuantum de %d",
				param_plataforma->CUANTUM);
	}
	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);

	return EXIT_SUCCESS;
}
