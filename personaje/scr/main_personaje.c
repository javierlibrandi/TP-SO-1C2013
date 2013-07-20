/*
 * main_personaje.c
 *
 *  Created on: 25/05/2013
 *      Author: utnso
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
bool flagReiniciarNivel, flagReiniciarJuego;

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
		flagReiniciarNivel = true;
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se han perdido todos los recursos conseguidos. ");
		log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes para %s: %d",
				personaje->nombre, personaje->vidas);
		break;
	}
	return;
}

int main(void) {

	int descriptor, tipo, bytes_enviados;
	InfoProxNivel InfoProxNivel;
	char *buffer, mensajeFinJuego[max_len];

	flagReiniciarJuego = false;
	flagReiniciarNivel = false;

	//puts("Elija el nombre para su personaje:");
	//printf( "\nHas elegido: \"%s\"\n", gets(nombre_per) );

	//ver el criterio para crear personajes.
	personaje = nuevoPersonaje();

	//pthread_create(&listener, NULL, (void*) listenerPersonaje, (void*) personaje);

	log_in_disk_per(LOG_LEVEL_INFO, "**** COMIENZA EL JUEGO PARA %s ****",
			personaje->nombre);

	while (!planDeNivelesCumplido(personaje)) {

		if (flagReiniciarJuego) {
			reiniciarPlanDeNiveles(personaje);
			flagReiniciarJuego = false;
			flagReiniciarNivel = false;
		}
		descriptor = conectarOrquestador(personaje);

		InfoProxNivel = consultarProximoNivel(descriptor, personaje);

		iniciarNivel(personaje, InfoProxNivel);

		//mientras no se complete el nivel y el personaje tenga vidas
		while (!objetivoNivelCumplido(personaje) && personaje->vidas > 0) {

			if (flagReiniciarNivel) {
				reiniciarNivel(personaje);
				flagReiniciarNivel = false;
			}

			//Espero y recibo notificación de movimiento permitido
			log_in_disk_per(LOG_LEVEL_INFO,
					"Esperando turno para jugar del planificador del %s",
					personaje->infoNivel.nombre);

			buffer = recv_variable(personaje->sockPlanif, &tipo);
			if (tipo == ERROR) {
				log_in_disk_per(LOG_LEVEL_ERROR,
						"Mensaje de error del planificador: %s", buffer);
				exit(EXIT_FAILURE);
			}

			//Espero y recibo muerte. VER PORQUE ESTE MENSAJE DEBERÍA ENVIARLO EL NIVEL :S
			if (tipo == PL_TO_P_MUERTE) {
				log_in_disk_per(LOG_LEVEL_INFO,
						"Se ha perdido una vida! El personaje fue elegido víctima por deadlock.");
				personaje->vidas--;
				log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes: %d",
						personaje->vidas);
				flagReiniciarNivel = true;
				log_in_disk_per(LOG_LEVEL_INFO,
						"Se han perdido todos los recursos conseguidos. ");
			}

			if (tipo == PL_TO_P_TURNO) {
				log_in_disk_per(LOG_LEVEL_INFO,
						"****** TURNO PARA %s ******", personaje->nombre);
				ejecutarTurno(personaje);
			}

			//Aviso si las señales no pudieron capturarse
			if (signal(SIGUSR1, manejador_signal) == SIG_ERR )
				log_in_disk_per(LOG_LEVEL_INFO,
						"[SEÑAL]No se pudo capturar la señal SIGUSR1 para sumar una vida.");

			if (signal(SIGTERM, manejador_signal) == SIG_ERR )
				log_in_disk_per(LOG_LEVEL_INFO,
						"[SEÑAL]No se pudo capturar la señal SIGTERM para restar una vida");

		}//fin del while "Mientras haya recursos pendientes para conseguir en el nivel"

		if (personaje->vidas > 0) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"****** ¡OBJETIVO DE %s CUMPLIDO! ******",
					personaje->infoNivel.nombre);
		} else {
			personaje->nivelActual = -1;
			log_in_disk_per(LOG_LEVEL_INFO,
					"****** EL PERSONAJE HA MUERTO ******");
			flagReiniciarNivel = false;
			flagReiniciarJuego = true;
		}

		//Se informa al nivel y planificador del objetivo cumplido/muerte y se cierra la conexión con los mismos
		salirDelNivel(personaje->sockNivel, personaje->sockPlanif,
				personaje->vidas);

		if (personaje->nivelActual == -2) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Se acaba de completar el último nivel.");
		}

	}	// fin de while "Mientras haya niveles que completar"

	log_in_disk_per(LOG_LEVEL_INFO, "****** ¡PLAN DE NIVELES CUMPLIDO! ******");
	log_in_disk_per(LOG_LEVEL_INFO, "El personaje %s ha ganado :)",
			personaje->nombre);

	//Se informa al orquestador que se terminó el plan de niveles
	sprintf(mensajeFinJuego, "%c;fin del juego", personaje->simbolo);

	fd_mensaje(personaje->sockPlanif, P_TO_O_JUEGO_GANADO, mensajeFinJuego,
			&bytes_enviados);

	return EXIT_SUCCESS;
}
