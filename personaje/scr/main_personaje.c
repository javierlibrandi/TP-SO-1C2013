/*
 * main_personaje.c
 *
 *  Created on: 25/05/2013
 *      Author: Jazmin
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/ambiente_config.h>
#include "funciones_personaje.h"
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/entorno.h>

void manejador_signal(int signal);

Personaje *personaje;
//bool flagReiniciarNivel, flagReiniciarJuego;

void manejador_signal(int signal) {
	switch (signal) {
	case SIGUSR1:
		log_in_disk_per(LOG_LEVEL_INFO,
				"[SEÑAL] Se ha recibido una vida por señal para el personaje");
		personaje->vidas++;
		log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes para %s: %d",
				personaje->nombre, personaje->vidas);

		break;
	case SIGTERM:
		log_in_disk_per(LOG_LEVEL_INFO,
				"[SEÑAL] Se ha perdido una vida por señal para el personaje");
		personaje->vidas--;

		log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes para %s: %d",
				personaje->nombre, personaje->vidas);

		if (personaje->vidas > 0) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Se han perdido todos los recursos conseguidos del %s. ",
					personaje->infoNivel.nombre);
			reiniciarNivel(personaje);
		} else {
			log_in_disk_per(LOG_LEVEL_INFO,
					"****** EL PERSONAJE HA MUERTO POR SEÑAL******");
			log_in_disk_per(LOG_LEVEL_INFO,
					"Se han perdido todos los recursos y niveles cumplidos. ");

			personaje->muerto = true;

			if (personaje->bloqueado) {
				salirNivelPorMuerte(personaje);
				//personaje->bloqueado = false;
			}
		}

		break;
	}
	return;
}

int main(void) {

	int descriptor, tipo, bytes_enviados;
	InfoProxNivel InfoProxNivel;
	char *buffer, mensajeFinJuego[max_len];

	//flagReiniciarJuego = false;
	//flagReiniciarNivel = false;

//ver el criterio para crear personajes.
	personaje = nuevoPersonaje();

//pthread_create(&listener, NULL, (void*) listenerPersonaje, (void*) personaje);

	log_in_disk_per(LOG_LEVEL_INFO, "****** COMIENZA EL JUEGO PARA %s ******",
			personaje->nombre);

	while (!planDeNivelesCumplido(personaje)) {

		descriptor = conectarOrquestador(personaje);

		InfoProxNivel = consultarProximoNivel(descriptor, personaje);

		iniciarNivel(personaje, InfoProxNivel);

		log_in_disk_per(LOG_LEVEL_INFO, "****** INICIO NIVEL ******");

		//mientras no se complete el nivel y el personaje tenga vidas
		while (!objetivoNivelCumplido(personaje) && personaje->vidas > 0) {
			//Espero y recibo notificación de movimiento permitido
			log_in_disk_per(LOG_LEVEL_INFO,
					"Esperando turno para jugar del planificador del %s",
					personaje->infoNivel.nombre);

			buffer = recv_variable(personaje->sockPlanif, &tipo);

			controlarConexion_recv(buffer);

			switch (tipo) {

			case ERROR:
				log_in_disk_per(LOG_LEVEL_ERROR,
						"Mensaje de error del planificador: %s", buffer);
				exit(EXIT_FAILURE);
				break;

				//Espero y recibo muerte del nivel.
			case PL_TO_P_MUERTE:
				log_in_disk_per(LOG_LEVEL_INFO,
						"Se ha perdido una vida! El personaje fue elegido víctima por deadlock.");
				personaje->vidas--;
				log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes: %d",
						personaje->vidas);

				if (personaje->vidas > 0) {
					log_in_disk_per(LOG_LEVEL_INFO,
							"Se han perdido todos los recursos conseguidos del %s. ",
							personaje->infoNivel.nombre);
					reiniciarNivel(personaje);

				} else {
					log_in_disk_per(LOG_LEVEL_INFO,
							"****** EL PERSONAJE HA MUERTO POR DEADLOCK******");
					log_in_disk_per(LOG_LEVEL_INFO,
							"Se han perdido todos los recursos y niveles cumplidos. ",
							personaje->infoNivel.nombre);
					personaje->muerto = true;
					salirNivelPorMuerte(personaje);

					buffer = recv_variable(personaje->sockPlanif, &tipo);
					if(tipo == PL_TO_P_TURNO){
						log_in_disk_per(LOG_LEVEL_INFO, "lalalalala" );
						salirPlanifPorMuerte(personaje);
						//reiniciarPlanDeNiveles2(personaje);
					}

				}

				break;

			case PL_TO_P_TURNO:

				if (personaje->muerto) {
					salirPlanifPorMuerte(personaje);

					if (!personaje->bloqueado) {
						salirNivelPorMuerte(personaje);
						personaje->bloqueado = false;
					}
					personaje->muerto = false;
					break;
				}

				log_in_disk_per(LOG_LEVEL_INFO, "****** TURNO PARA %s ******",
						personaje->nombre);
				ejecutarTurno(personaje);
				break;

			default:
				log_in_disk_per(LOG_LEVEL_INFO,
						"Opción del switch personaje no implementada. Buffer: %s.Tipo: %d",
						buffer, tipo);
				exit(EXIT_FAILURE);
				break;
			}

			//Aviso si las señales no pudieron capturarse
			if (signal(SIGUSR1, manejador_signal) == SIG_ERR )
				log_in_disk_per(LOG_LEVEL_INFO,
						"[SEÑAL]No se pudo capturar la señal SIGUSR1 para sumar una vida.");

			if (signal(SIGTERM, manejador_signal) == SIG_ERR )
				log_in_disk_per(LOG_LEVEL_INFO,
						"[SEÑAL]No se pudo capturar la señal SIGTERM para restar una vida");

		} //fin del while "Mientras haya recursos pendientes para conseguir en el nivel "

		personaje->finNivel = false;

		if (personaje->vidas > 0) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"****** ¡OBJETIVO DE %s CUMPLIDO! ******",
					personaje->infoNivel.nombre);

			if (personaje->nivelActual == -3) {

				personaje->nivelActual = -2;
				log_in_disk_per(LOG_LEVEL_INFO,
						"Se acaba de completar el último nivel.");
			}

			salirNivelPorObjCumplido(personaje);
		}else{
			reiniciarPlanDeNiveles2(personaje);
		}

		//Se informa al nivel y planificador del objetivo cumplido/muerte y se cierra la conexión con los mismos
	}	// fin de while "Mientras haya niveles que completar"

//Objetivo de último nivel cumplido
	log_in_disk_per(LOG_LEVEL_INFO, "****** ¡PLAN DE NIVELES CUMPLIDO! ******");
	log_in_disk_per(LOG_LEVEL_INFO, "El personaje %s ha ganado :)",
			personaje->nombre);

//Se informa al orquestador que se terminó el plan de niveles
	sprintf(mensajeFinJuego, "%c;fin del juego", personaje->simbolo);

	fd_mensaje(personaje->sockPlanif, P_TO_PL_JUEGO_GANADO, mensajeFinJuego,
			&bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_PL_JUEGO_GANADO");

		log_in_disk_per(LOG_LEVEL_ERROR,
				"Planificador cerró la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);
	}

	log_in_disk_per(LOG_LEVEL_INFO,
			"El personaje %s está ansioso por matar a Koopa!",
			personaje->nombre);

	tipo = 0;
	while (tipo != PL_TO_P_MATAR_KOOPA) {

		buffer = recv_variable(personaje->sockPlanif, &tipo);

		if (tipo == PL_TO_P_MATAR_KOOPA) {
			//log_in_disk_per(LOG_LEVEL_INFO, "Se venció a Koopa!");
			log_in_disk_per(LOG_LEVEL_INFO,
					"Termina exitosamente el proceso personaje.");
			close(personaje->sockPlanif);

			return EXIT_SUCCESS;
		} else {
			//log_in_disk_per(LOG_LEVEL_INFO, "No se recibió un mensaje esperado. Tipo:%d. Buffer:%s", tipo, buffer);
			log_in_disk_per(LOG_LEVEL_INFO, "Espero por Koopa...");

			sleep(1);
		}
	}
}
