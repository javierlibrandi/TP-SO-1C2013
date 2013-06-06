/*
 * funciones_personaje.c
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#include "funciones_personaje.h"
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/entorno.h>
#include <mario_para_todos/ambiente_config.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <sys/socket.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>

//Por ahora se inicializa el personaje harcodeado. Falta hacer funcionar la lectura del arch de configuración y el uso de listas.
Personaje* nuevoPersonaje(char* nombrePersonaje) {
	Personaje* personaje = malloc(sizeof(Personaje));

	log_in_disk_per(LOG_LEVEL_INFO, "Voy a crear a %s", nombrePersonaje);

//Esperar imple de pato  para levantar arch conf
	/*
	 archConf=leer_archivo_personaje_config();

	 personaje->nombre = archConf.nombre;
	 personaje->simbolo = archConf.simbolo;
	 personaje->vidas = archConf.vidas;
	 personaje->ipOrquestador = archconf.ipOrquestador;
	 //Niveles y objetivos???
	 */

//Por ahora harcodeo
	personaje->nombre = nombrePersonaje;
	personaje->simbolo = '@';
	personaje->vidas = 3;
	personaje->ip_orquestador = "localhost";
	personaje->puerto_orquestador = 5000;
//personaje->nivelesRestantes= null;

	log_in_disk_per(LOG_LEVEL_INFO, "EL personaje creado es %s",
			personaje->nombre);

	return personaje;
}

int conectarOrquestador(Personaje* personaje) {

	int descriptor, tipo;
	char* buffer = NULL;
	int bytes_enviados;
	char mensaje[max_len];
	char *nivel;


	// Establezco conexión con el orquestador
	log_in_disk_per(LOG_LEVEL_INFO, "Me conecto al orquestador");
	log_in_disk_per(LOG_LEVEL_INFO, "IP: %s", personaje->ip_orquestador);
	log_in_disk_per(LOG_LEVEL_INFO, "PUERTO: %d", personaje->puerto_orquestador);

	descriptor = Abre_Conexion_Inet(personaje->ip_orquestador, personaje->puerto_orquestador);

	if (descriptor == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR, "%s", "Hubo un error al conectarse al orquestador");

	} else {
		log_in_disk_per(LOG_LEVEL_INFO, "%s", "Conexión exitosa con el orquestador");
	}

	//Ver con Mati si enviar nivel aca o si no hace falta, ya que tengo que consultar y lo vuelvo a hacer en PROX_NIVEL
	//Sino agregar un campo a Personaje que guarde nivel actual
	nivel="nivel5";
	sprintf(mensaje, "%s;%s", personaje->nombre, nivel );

	log_in_disk_per(LOG_LEVEL_INFO, "Envío primer mensaje de saludo.");
	fd_mensaje(descriptor, P_TO_P_SALUDO, mensaje, &bytes_enviados);

	if (bytes_enviados == -1)
		log_in_disk_per(LOG_LEVEL_ERROR, "%s", "Hubo un error al enviar el mensaje P_TO_P_SALUDO");
	//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

	//Recibo OK del orquestador

	buffer = recv_variable(descriptor, &tipo);

	if (tipo == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del orquestador: %s", buffer);
		exit(EXIT_FAILURE);
	}

	if (tipo == OK)
		log_in_disk_per(LOG_LEVEL_INFO, "Respuesta del orquestador: %s", buffer);

	free(buffer);
	return descriptor;
}

//Ver como crear la estructura InfoProxNivel en esta función con la respuesta del orquestador.
InfoProxNivel consultarProximoNivel(int descriptor, Personaje* personaje) {

	int tipo, bytes_enviados;
	InfoProxNivel infoNivel;
	char *buffer;
	char **aux_msj;
	char mensaje[max_len];

	//Creo string con nombre y nivel para enviar al Orquestador en P_TO_O_PROX_NIVEL
	//Se fija en su lista de plan de niveles, cuál es el próximo nivel a completar.
	infoNivel.nombre_nivel = "nivel5";

	log_in_disk_per(LOG_LEVEL_INFO,
					"Voy a enviar solicitud de ubicación del %s", infoNivel.nombre_nivel, "al orquestador");

	sprintf(mensaje, "%s;%s", personaje->nombre, infoNivel.nombre_nivel);

	fd_mensaje(descriptor, P_TO_O_PROX_NIVEL, mensaje, &bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR, "%s",
				"Hubo un error al enviar el mensaje P_TO_O_PROX_NIVEL");
		exit(EXIT_FAILURE);
	}

	//Recibo ip/puerto del nivel en un string separados por ;
	log_in_disk_per(LOG_LEVEL_INFO,
						"Esperando respuesta del orquestador");
	buffer = recv_variable(descriptor, &tipo);
	if (tipo == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "%s", buffer);
		exit(EXIT_FAILURE);
	}

	if (tipo == O_TO_P_UBIC_NIVEL)
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se recibió información del orquestador:%s", buffer);

	//ver como extraer ip y puerto del mensaje recibido en buffer
	//Por ahora harcodeo

	aux_msj = string_split(buffer, ";");

	strcpy(infoNivel.ip_nivel, aux_msj[0]);
	infoNivel.puerto_nivel = atoi(aux_msj[1]);

	log_in_disk_per(LOG_LEVEL_ERROR, "%s",
			"EL personaje se desconecta del orquestador.");
	close(descriptor);

	free(aux_msj);
	free(buffer);

	return infoNivel;

}
;

void iniciarNivel(Personaje* personaje, InfoProxNivel infoNivel) {
	int descriptorNiv, descriptorPlan, tipoN, tipoP;
	int bytes_enviados_niv, bytes_enviados_pl;
	char mensaje1[max_len], mensaje2[max_len];
	char *bufferNiv, *bufferPla;

	log_in_disk_per(LOG_LEVEL_INFO, "Me voy a conectar con %s",
					infoNivel.nombre_nivel);
	log_in_disk_per(LOG_LEVEL_INFO, "IP:%s", infoNivel.ip_nivel);
	log_in_disk_per(LOG_LEVEL_INFO, "PUERTO:%d", infoNivel.puerto_nivel);

	descriptorNiv = Abre_Conexion_Inet(infoNivel.ip_nivel,
			infoNivel.puerto_nivel);

	if (descriptorNiv == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al conectarse al %s",
				infoNivel.nombre_nivel);
	} else {
		log_in_disk_per(LOG_LEVEL_INFO, "Conexión exitosa con %s",
				infoNivel.nombre_nivel);
	}

	log_in_disk_per(LOG_LEVEL_INFO,
			"Me voy a conectar con plataforma para que me asocie al planificador del %s", infoNivel.nombre_nivel);
	descriptorPlan = Abre_Conexion_Inet(personaje->ip_orquestador,
			personaje->puerto_orquestador);

	if (descriptorPlan == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al conectarse al planificador del nivel");
	} else {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Conexión exitosa con planificador del %s", infoNivel.nombre_nivel);
	}

	// Envía a Nivel P_TO_N_INICIAR_NIVEL "nombrePersonaje, símbolo"
	sprintf(mensaje1, "%s;%c", personaje->nombre, personaje->simbolo);


	fd_mensaje(descriptorNiv, P_TO_N_INICIAR_NIVEL, mensaje1, &bytes_enviados_niv);

	if (bytes_enviados_niv == -1)
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_INICIAR_NIVEL");
	//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

	// Envía a Plataforma P_TO_PL_INICIAR_NIVEL para que lo asocie al planificador del nivel "nombre;nivelNro"
	sprintf(mensaje2, "%s;%s", personaje->nombre, infoNivel.nombre_nivel);


	fd_mensaje(descriptorPlan, P_TO_PL_INICIAR_NIVEL, mensaje2, &bytes_enviados_pl);

	if (bytes_enviados_pl == -1)
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_PL_INICIAR_NIVEL");
	//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

//Recibe OK de nivel y plataforma

	log_in_disk_per(LOG_LEVEL_INFO, "Espero OKEYS del nivel y de su planificador...");

	bufferNiv = recv_variable(descriptorNiv, &tipoN);
	if (tipoN == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del Nivel:%s", bufferNiv);
		exit(EXIT_FAILURE);
	}

	if (tipoN == OK) {
		log_in_disk_per(LOG_LEVEL_INFO, "Se recibió OK del %s", infoNivel.nombre_nivel);
		personaje->sockNivel = descriptorNiv;
	}

	bufferPla = recv_variable(descriptorNiv, &tipoP);
	if (tipoP == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del planificador:%s", bufferPla);
		exit(EXIT_FAILURE);
	}

	if (tipoP == OK) {
		log_in_disk_per(LOG_LEVEL_INFO, "Se recibió OK del planificador.");
		personaje->sockPlanif = descriptorPlan;
	}
	//VER SI HAY QUE VOLVER A MANDAR MENSAJES CON MÁS INFO

	free(bufferNiv);
	free(bufferPla);
}

//¿crear función con select que escuche al planif y al nivel?
int listenerPersonaje(int descriptorNiv, int descriptorPlan) {

	int N = 1;
	int descriptores[N], tipo, result, j, i;
	int maxDescriptor = 0;
	fd_set readset;
	void *buffer = NULL;

	//Limpio lista de sockets
	FD_ZERO(&readset);

	//Inicializo vector de sockets
	descriptores[0] = descriptorNiv;
	descriptores[1] = descriptorPlan;

	//Agrego los dos sockets de Nivel y Planif a la lista y busco máximo valor
	for (j = 0; j < N; j++) {
		FD_SET(descriptores[j], &readset);
		maxDescriptor =
				(maxDescriptor > descriptores[j]) ?
						maxDescriptor : descriptores[j];
	}

	// Me fijo si en alguno llegaron mensajes
	result = select(maxDescriptor + 1, &readset, NULL, NULL, NULL );

	if (result == -1) {
		perror("select");
		exit(EXIT_FAILURE);
	} else {
		for (i = 0; i < N; i++) {
			if (FD_ISSET(descriptores[i], &readset)) {
				buffer = recv_variable(descriptores[i], &tipo);

				switch (tipo) {
				case ERROR:
					log_in_disk_per(LOG_LEVEL_ERROR, "%s", (char*) buffer);
					exit(EXIT_FAILURE);
					break;
				case PL_TO_P_TURNO:
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

//el personaje se fija cuál es el próximo recurso a conseguir. Le solicita al Nivel las coordenadas x-y del mismo. */
Posicion solicitarUbicacionRecurso(Personaje* personaje) {
	Posicion posicion;

	posicion.x = 0;
	posicion.y = 0;

	return posicion;

}

/* Realizará los movimientos necesarios y evaluará su posición hasta llegar al recurso. */
void llegarARecurso(Posicion posRecurso, Posicion posActual) {
	int xFinal, yFinal, xActual, yActual;

	xFinal = posRecurso.x;
	yFinal = posRecurso.y;
	xActual = posActual.x;
	yActual = posActual.y;

	//evaluamosx
	if (xFinal > xActual) {
		//xActual se va a incrementar hasta llegar a xFinal
	} else {
		//xActual se va a decrementar hasta llegar a xFinal
	}

	//evaluamosy
	if (yFinal > yActual) {
		//yActual se va a incrementar hasta llegar a yFinal
	} else {
		//yActual se va a decrementar hasta llegar a yFinal
	}

	while (evaluarPosicion(posActual, posRecurso) != 1) {

		//realizarMovimiento();

	}

}

int evaluarPosicion(Posicion posicionActual, Posicion posicionRecurso) {

	if (posicionActual.x == posicionRecurso.x
			&& posicionActual.y == posicionRecurso.y) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se llegó a la posición del recurso que se necesita.");
		//solicitarRecurso();
		return 1;
	} else {
		log_in_disk_per(LOG_LEVEL_INFO,
				"No se llegó aún a la posición del recurso que se necesita.");
	}
	return 0;
}

/* Se moverá una posición en un eje para acercarse al próximo recurso.
 void realizarMovimiento(Posicion posicion){

 }
 */

