/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/grabar.h>
#include <commons/config.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include <mario_para_todos/entorno.h>
#include "funciones_personaje.h"

//void* listenerPersonaje ();

void* listenerPersonaje(void *per) {

	int N = 1;
	int descriptores[N], tipo, result, j, i;
	int maxDescriptor = 0;
	fd_set readset;
	void *buffer = NULL;
	Personaje* personaje = (Personaje*) per;

//Limpio lista de sockets
	//FD_ZERO(&readset);

//Inicializo vector de sockets
	descriptores[0] = personaje->sockNivel;
	descriptores[1] = personaje->sockPlanif;

//Agrego los dos sockets de Nivel y Planif a la lista y busco máximo valor
	for (j = 0; j < N; j++) {
		FD_SET(descriptores[j], personaje->listaSelect);
		maxDescriptor =
				(maxDescriptor > descriptores[j]) ?
						maxDescriptor : descriptores[j];
	}

// Me fijo si en alguno llegaron mensajes
	result = select(maxDescriptor + 1, personaje->listaSelect, NULL, NULL,
			NULL );

	if (result == -1) {
		perror("select");
		exit(EXIT_FAILURE);
	} else {
		for (i = 0; i < maxDescriptor; i++) {
			if (FD_ISSET(i, personaje->listaSelect)) {
				buffer = recv_variable(i, &tipo);

				if (!strcmp(buffer, Leido_error)) {

					elimino_sck_lista(i, personaje->listaSelect);
				}
			}
			switch (tipo) {
			case PL_TO_P_MUERTE:
				//me puede mandar el mensaje de muerte mientras estoy ejecutando el turno?
				log_in_disk_per(LOG_LEVEL_INFO,
						"El personaje ha perdido una vida.");
				log_in_disk_per(LOG_LEVEL_INFO, "MUERTE POR DEADLOCK");
				//USAR SEMAFOROS para acceder a vidas
				personaje->vidas--;

				log_in_disk_per(LOG_LEVEL_INFO, "Vidas restantes:%d",
						personaje->vidas);

				if (personaje->vidas > 0) {
					reiniciarNivel(personaje);
					log_in_disk_per(LOG_LEVEL_INFO,
							"¡El personaje debe reiniciar el nivel!");
				} else {
					reiniciarPlanDeNiveles(personaje);
					log_in_disk_per(LOG_LEVEL_INFO,
							"¡El personaje debe reiniciar el juego!");
					//pensar más adelante como implementar esta función
				}

			break;
			// ...
			default:
			break;

		}

		free(buffer);
	}
}
}
return EXIT_SUCCESS;
}
*/
