/*
 * main_personaje.c
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/ambiente_config.h>
#include "funciones_personaje.h"
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/grabar.h>

int main(void) {

	char nombre_per[10] = "Jaz";
	Personaje *personaje = NULL;
	int descriptor, tipo, bytes_enviados;
	InfoProxNivel infoNivel;
	char *buffer, mensajeFinJuego[max_len];

	//puts("Elija el nombre para su personaje:");
	//printf( "\nHas elegido: \"%s\"\n", gets(nombre_per) );

	personaje = nuevoPersonaje(nombre_per);

	while (!planDeNivelesCumplido(personaje)) {

		descriptor = conectarOrquestador(personaje);

		infoNivel = consultarProximoNivel(descriptor, personaje);

		//harcodeo ip y puerto del nivel para probar esta función
		/*infoNivel.nombre_nivel="nivel2";
		 infoNivel.ip_nivel="localhost";
		 infoNivel.puerto_nivel=5002;*/

		iniciarNivel(personaje, infoNivel);

		//mientras no se complete el nivel
		while (!objetivoNivelCumplido(personaje)) {

			//Espero y recibo notificación de movimiento permitido
			log_in_disk_per(LOG_LEVEL_INFO,
					"Esperando turno para jugar del planificador del %s",
					personaje->nivelActual);

			buffer = recv_variable(personaje->sockPlanif, &tipo);
			if (tipo == ERROR) {
				log_in_disk_per(LOG_LEVEL_ERROR,
						"Mensaje de error del planificador: %s", buffer);
				exit(EXIT_FAILURE);
			}

			if (tipo == PL_TO_P_TURNO) {
				ejecutarTurno(personaje);
			}

			if (tipo == PL_TO_P_MUERTE) {
				//me puede mandar el mensaje de muerte mientras estoy ejecutando el turno?
				log_in_disk_per(LOG_LEVEL_INFO,
						"El personaje ha perdido una vida.");
				log_in_disk_per(LOG_LEVEL_INFO, "MUERTE POR DEADLOCK");
				personaje->vidas--;

				log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes:%d",
						personaje->vidas);

				if (personaje->vidas > 0) {
					reiniciarNivel();
					log_in_disk_per(LOG_LEVEL_INFO,
							"¡El personaje debe reiniciar el nivel!");
				} else {
					reiniciarPlanDeNiveles();
					log_in_disk_per(LOG_LEVEL_INFO,
							"¡El personaje debe reiniciar el juego!");
					//pensar más adelante como implementar esta función
				}
			}

		}//fin del while "Mientras haya recursos pendientes para conseguir en el nivel"

		log_in_disk_per(LOG_LEVEL_INFO, "¡Objetivo del nivel cumplido!");

		//Se informa al nivel y planificador del objetivo cumplido y se cierra la conexión con los mismos
		salirDelNivel(personaje->sockNivel, personaje->sockPlanif);

	}	// fin de while "Mientras haya niveles que completar"

	log_in_disk_per(LOG_LEVEL_INFO, "¡Plan de Niveles completo!");
	log_in_disk_per(LOG_LEVEL_INFO, "El personaje %s ha ganado :)", personaje->nombre);

	//Se informa al orquestador que se terminó el plan de niveles
	 sprintf(mensajeFinJuego, "%s;fin del juego", personaje->nombre);

	fd_mensaje(personaje->sockPlanif, P_TO_P_JUEGO_GANADO,
					mensajeFinJuego, &bytes_enviados);

	return EXIT_SUCCESS;
}
