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
#include <commons/string.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <sys/socket.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <commons/collections/list.h>

//Por ahora se inicializa el personaje harcodeado. Falta hacer funcionar la lectura del arch de configuración y el uso de listas.
Personaje* nuevoPersonaje() {
	Personaje* personaje = malloc(sizeof(Personaje));
	//lo pongo para el ejemplo
	int i, k;
	t_recusos *recursos;
	char *aux_nivel;
	fd_set fd_set_aux;

	// lo pongo para el ejemplo

	log_in_disk_per(LOG_LEVEL_INFO, "Voy a crear al personaje");

	t_param_persoje param = leer_personaje_config();

	personaje->nombre = param.NOMBRE;
	personaje->simbolo = param.SMBOLO;
	personaje->vidas = param.VIDAS;
	personaje->ip_orquestador = param.IP;
	personaje->puerto_orquestador = param.PUERTO_PLATAFORMA;
	personaje->niveles = param.RECURSOS;
	FD_ZERO(&fd_set_aux);
    personaje->listaSelect= &fd_set_aux;

	personaje->nivelActual = -1;
	personaje->recursoActual = -1;

	log_in_disk_per(LOG_LEVEL_INFO,
			"El personaje creado es %s, identificado con el caracter %c, y con %d vidas.",
			personaje->nombre, personaje->simbolo, personaje->vidas);

	for (i = 0; param.PLAN_NIVELES[i] != '\0'; i++) { //recorro todos los niveles
		log_in_disk_per(LOG_LEVEL_INFO, "Estoy en el nivel %s",
				param.PLAN_NIVELES[i]);

		aux_nivel = param.PLAN_NIVELES[i];

		recursos = recursos_nivel(param.RECURSOS, aux_nivel);

		for (k = 0; recursos->RECURSOS[k] != '\0'; k++) { //recorro todos los recuros del nivel

			log_in_disk_per(LOG_LEVEL_INFO, "Nombre recurso %s ",
					recursos->RECURSOS[k]);

		}

	}

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
	log_in_disk_per(LOG_LEVEL_INFO, "PUERTO: %d",
			personaje->puerto_orquestador);

	descriptor = Abre_Conexion_Inet(personaje->ip_orquestador,
			personaje->puerto_orquestador);

	if (descriptor == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR, "%s",
				"Hubo un error al conectarse al orquestador");

	} else {
		log_in_disk_per(LOG_LEVEL_INFO, "%s",
				"Conexión exitosa con el orquestador");
	}

//Armo mensaje P_TO_P_SALUDO con "nombrePersonaje;simbolo;nivel"

	nivel = determinarProxNivel(personaje->niveles, personaje->nivelActual);
	sprintf(mensaje, "%s;%c;%s", personaje->nombre, personaje->simbolo, nivel);

	log_in_disk_per(LOG_LEVEL_INFO, "Envío primer mensaje de saludo.");
	fd_mensaje(descriptor, P_TO_P_SALUDO, mensaje, &bytes_enviados);

	if (bytes_enviados == -1)
		log_in_disk_per(LOG_LEVEL_ERROR, "%s",
				"Hubo un error al enviar el mensaje P_TO_P_SALUDO");
//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

//Recibo OK del orquestador

	buffer = recv_variable(descriptor, &tipo);

	if (tipo == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del orquestador: %s",
				buffer);
		exit(EXIT_FAILURE);
	}

	if (tipo == OK)
		log_in_disk_per(LOG_LEVEL_INFO, "Respuesta del orquestador: %s",
				buffer);

	free(buffer);
	return descriptor;
}

char* determinarProxNivel(t_list* niveles, int nivelActual) {
	char * proxNivel;

//Se fija el primero de la lista de NivelesPendientes
//Por ahora harcodeo
//proxNivel = "nivel2";

	nivelActual++;

	if (list_size(niveles) > nivelActual) {
		proxNivel = list_get(niveles, nivelActual);
	} else {
		// Error, porque no hay más niveles para leer
	}

	return proxNivel;
}

InfoProxNivel consultarProximoNivel(int descriptor, Personaje* personaje) {

	int tipo, bytes_enviados;
	InfoProxNivel infoNivel;
	char *buffer;
	char **aux_msj;
	char mensaje[max_len];

//Creo string con nombre y nivel para enviar al Orquestador en P_TO_O_PROX_NIVEL
//Se fija en su lista de plan de niveles, cuál es el próximo nivel a completar.
//infoNivel.nombre_nivel = "nivel2";
	infoNivel.nombre_nivel = determinarProxNivel(personaje->niveles,
			personaje->nivelActual);

	log_in_disk_per(LOG_LEVEL_INFO,
			"Voy a enviar solicitud de ubicación del %s",
			infoNivel.nombre_nivel, "al orquestador");

	sprintf(mensaje, "%s;%s", personaje->nombre, infoNivel.nombre_nivel);

	fd_mensaje(descriptor, P_TO_O_PROX_NIVEL, mensaje, &bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR, "%s",
				"Hubo un error al enviar el mensaje P_TO_O_PROX_NIVEL");
		exit(EXIT_FAILURE);
	}

//Recibo ip/puerto del nivel en un string separados por ;
	log_in_disk_per(LOG_LEVEL_INFO, "Esperando respuesta del orquestador");
	buffer = recv_variable(descriptor, &tipo);
	if (tipo == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "%s", buffer);
		exit(EXIT_FAILURE);
	}

	if (tipo == O_TO_P_UBIC_NIVEL) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se recibió información del orquestador:%s", buffer);

//ver como extraer ip y puerto del mensaje recibido en buffer
//Por ahora harcodeo

		aux_msj = string_split(buffer, ";");

		//strcpy(infoNivel.ip_nivel, aux_msj[0]);
		infoNivel.ip_nivel = aux_msj[0];
		infoNivel.puerto_nivel = atoi(aux_msj[1]);
	}

//infoNivel.ip_nivel="localhost";
//infoNivel.puerto_nivel = 5002;

	log_in_disk_per(LOG_LEVEL_ERROR, "%s",
			"EL personaje se desconecta del orquestador.");
	close(descriptor);

	free(aux_msj);
	free(buffer);

	return infoNivel;

}

void iniciarNivel(Personaje* personaje, InfoProxNivel infoNivel) {
	int descriptorNiv, descriptorPlan, tipoN, tipoP;
	int bytes_enviados_niv, bytes_enviados_pl;
//char **aux_msj1, **aux_msj2;
	char mensaje1[max_len], mensaje2[max_len];
	char *bufferNiv, *bufferPla;

	log_in_disk_per(LOG_LEVEL_INFO, "Me voy a conectar con %s",
			infoNivel.nombre_nivel);
	log_in_disk_per(LOG_LEVEL_INFO, "IP:%s", infoNivel.ip_nivel);
	log_in_disk_per(LOG_LEVEL_INFO, "PUERTO:%d", infoNivel.puerto_nivel);

	descriptorNiv = Abre_Conexion_Inet(infoNivel.ip_nivel,
			infoNivel.puerto_nivel);

	if (descriptorNiv == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Hubo un error al conectarse al %s",
				infoNivel.nombre_nivel);
	} else {
		log_in_disk_per(LOG_LEVEL_INFO, "Conexión exitosa con %s",
				infoNivel.nombre_nivel);
	}

	log_in_disk_per(LOG_LEVEL_INFO,
			"Me voy a conectar con plataforma para que me asocie al planificador del %s",
			infoNivel.nombre_nivel);
	descriptorPlan = Abre_Conexion_Inet(personaje->ip_orquestador,
			personaje->puerto_orquestador);

	if (descriptorPlan == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al conectarse al planificador del nivel");
	} else {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Conexión exitosa con planificador del %s",
				infoNivel.nombre_nivel);
	}

// Envía a Nivel P_TO_N_INICIAR_NIVEL "nombrePersonaje, símbolo"
	sprintf(mensaje1, "%s;%c", personaje->nombre, personaje->simbolo);

	fd_mensaje(descriptorNiv, P_TO_N_INICIAR_NIVEL, mensaje1,
			&bytes_enviados_niv);

	if (bytes_enviados_niv == -1)
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_INICIAR_NIVEL");
//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

// Envía a Plataforma P_TO_PL_INICIAR_NIVEL para que lo asocie al planificador del nivel "nombre;nivelNro"
	sprintf(mensaje2, "%s;%s", personaje->nombre, infoNivel.nombre_nivel);

	fd_mensaje(descriptorPlan, P_TO_PL_INICIAR_NIVEL, mensaje2,
			&bytes_enviados_pl);

	if (bytes_enviados_pl == -1)
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_PL_INICIAR_NIVEL");
//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

//Recibe OK de nivel y plataforma

	log_in_disk_per(LOG_LEVEL_INFO,
			"Espero OKEYS del nivel y de su planificador...");
	bufferPla = recv_variable(descriptorPlan, &tipoP);
	if (tipoP == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del planificador:%s",
				bufferPla);
		exit(EXIT_FAILURE);
	}

	if (tipoP == OK) {
		log_in_disk_per(LOG_LEVEL_INFO, "Se recibió OK del planificador.");
		personaje->sockPlanif = descriptorPlan;
		//usar semaforos para acceder a listaSelect
		FD_SET(descriptorPlan, personaje->listaSelect);
	}

	if (tipoP != OK && tipoP != ERROR) {
		log_in_disk_per(LOG_LEVEL_INFO, "No se recibió un mensaje esperado %s:",
				bufferPla);
		exit(EXIT_FAILURE);
	}

	bufferNiv = recv_variable(descriptorNiv, &tipoN);
	if (tipoN == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del Nivel:%s",
				bufferNiv);
		exit(EXIT_FAILURE);
	}

	if (tipoN == OK) {
		log_in_disk_per(LOG_LEVEL_INFO, "Se recibió OK del %s",
				infoNivel.nombre_nivel);
		personaje->sockNivel = descriptorNiv;
		personaje->infoNivel.nombre = infoNivel.nombre_nivel;
		FD_SET(descriptorNiv, personaje->listaSelect);
	}

	if (tipoN != OK && tipoN != ERROR) {
		log_in_disk_per(LOG_LEVEL_INFO, "No se recibió un mensaje esperado %s:",
				bufferNiv);
		exit(EXIT_FAILURE);
	}

	free(bufferNiv);
	free(bufferPla);
}

//¿crear función con select que escuche al planif y al nivel?

char determinarProxRecurso(Nivel infoNivel, int recursoActual) {

	char proxRecurso;

//Fijarse en la lista de nivelesPendientes, en el nivel correspondiente el primer recurso a conseguir
//Por ahora harcodeo
//proxRecurso = 'F';

	recursoActual++;

	if (list_size(infoNivel.recursos) > recursoActual) {
		proxRecurso = (char) (list_get(infoNivel.recursos, recursoActual));
	} else {
		// Error, porque no hay más niveles para leer
	}

	return proxRecurso;
}

//el personaje se fija cuál es el próximo recurso a conseguir. Le solicita al Nivel las coordenadas x-y del mismo. */
void solicitarUbicacionRecurso(Personaje* personaje) {

	char recurso;
	int bytes_enviados, tipo;
	char *buffer;
	char **aux_msj;
	char mensaje[max_len];

// Envía a Nivel P_TO_N_UBIC_RECURSO para pedir coordenadas del próximo recurso requerido "recurso"

	recurso = determinarProxRecurso(personaje->infoNivel,
			personaje->recursoActual);
//Ver si sirve el campo recursoActual
	personaje->recursoActual = recurso;

	sprintf(mensaje, "%c", recurso);

	log_in_disk_per(LOG_LEVEL_INFO, "Necesito la ubicación del recurso %s",
			recurso);

	fd_mensaje(personaje->sockNivel, P_TO_N_UBIC_RECURSO, mensaje,
			&bytes_enviados);

	if (bytes_enviados == -1)
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_UBIC_RECURSO");

//Recibe coordenadas del recurso

	log_in_disk_per(LOG_LEVEL_INFO, "Espero respuesta del Nivel...");

	buffer = recv_variable(personaje->sockNivel, &tipo);

	if (tipo == ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del Nivel:%s",
				buffer);
		exit(EXIT_FAILURE);
	}

	if (tipo == N_TO_P_UBIC_RECURSO) {
		log_in_disk_per(LOG_LEVEL_INFO, "Se recibió la información %s:",
				buffer);

		aux_msj = string_split(buffer, ";");

		personaje->posProxRecurso.x = atoi(aux_msj[1]);
		personaje->posProxRecurso.y = atoi(aux_msj[2]);

	}

	if (tipo != N_TO_P_UBIC_RECURSO && tipo != ERROR) {
		log_in_disk_per(LOG_LEVEL_INFO, "No se recibió un mensaje esperado %s:",
				buffer);
		exit(EXIT_FAILURE);
	}

	free(buffer);
	free(aux_msj);
}

//el personaje actúa ante una notificación de movimiento permitido por parte del planificador del nivel
/*Las acciones posibles en 1 quantum son:
 * moverse()
 * solicitarUbicacionNivel() y moverse()
 * moverse() y solicitarInstanciaRecurso()
 * moverse() y solicitarInstanciaRecurso() y notificarBloqueo()
 */
void ejecutarTurno(Personaje *personaje) {

//int tipo;
	int bytes_enviados, bytes_enviados2, recursoAdjudicado;
//char *buffer;
	char *mensajeFinTurno, *mensajeBloqueo;

//Armo mensaje de turno cumplido y bloqueo. Ver con Planif. qué info mandar
	mensajeFinTurno = personaje->nombre;
	mensajeBloqueo = personaje->nombre;

//Espero y recibo notificación de movimiento permitido

	log_in_disk_per(LOG_LEVEL_INFO,
			"Se recibió notificación de movimiento permitido");

//Si no sabemos las coordenadas del próximo recurso a conseguir preguntamos y luego nos movemos en 1 quantum
	if (!conocePosicionRecurso(personaje->recursoActual)) {

		log_in_disk_per(LOG_LEVEL_INFO,
				"Desconozco la ubicación del recurso que necesito. Solicito coordenadas al nivel");
		solicitarUbicacionRecurso(personaje);

		log_in_disk_per(LOG_LEVEL_INFO,
				"Me muevo una posición en dirección del recurso");
		moverse(personaje);

		log_in_disk_per(LOG_LEVEL_INFO,
				"Evalúo si llegué a la posición destino del recurso.");

		if (evaluarPosicion(personaje->posActual, personaje->posProxRecurso)) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Llegué al recurso. Solicito una instancia del mismo.");

			recursoAdjudicado = solicitarInstanciaRecurso(personaje);
			if (recursoAdjudicado) {
				//Se adjudicó el recurso, agregar a la lista de recursos del personaje
				log_in_disk_per(LOG_LEVEL_INFO,
						"Recurso adjudicado al personaje.");

			} else {
				//notificarBloqueo:
				log_in_disk_per(LOG_LEVEL_INFO,
						"Nivel informa que no hay instancias disponibles del recurso solicitado.");
				log_in_disk_per(LOG_LEVEL_INFO,
						"Envío notificación de bloqueo al planificador.");

				fd_mensaje(personaje->sockPlanif, P_TO_N_BLOQUEO,
						mensajeBloqueo, &bytes_enviados2);
			}
		}

		//no se llegó al recurso todavía. Enviar mensaje de turno cumplido
		log_in_disk_per(LOG_LEVEL_INFO,
				"No se llegó a la posición del recurso aún. Envío notificación de turno cumplido al planificador.");

		fd_mensaje(personaje->sockPlanif, P_TO_PL_TURNO_CUMPLIDO,
				mensajeFinTurno, &bytes_enviados);

	} else {
		//si ya conocemos la ubic del recurso
		log_in_disk_per(LOG_LEVEL_INFO,
				"Me muevo una posición en dirección del recurso");
		moverse(personaje);

		log_in_disk_per(LOG_LEVEL_INFO,
				"Evalúo si llegué a la posición destino del recurso.");

		if (evaluarPosicion(personaje->posActual, personaje->posProxRecurso)) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Llegué al recurso. Solicito una instancia del mismo.");

			recursoAdjudicado = solicitarInstanciaRecurso(
					personaje->recursoActual);
			if (recursoAdjudicado) {
				//Se adjudicó el recurso, agregar a la lista de recursos del personaje
				log_in_disk_per(LOG_LEVEL_INFO,
						"Recurso adjudicado al personaje.");

			} else {
				//notificarBloqueo:
				log_in_disk_per(LOG_LEVEL_INFO,
						"Nivel informa que no hay instancias disponibles del recurso solicitado.");
				log_in_disk_per(LOG_LEVEL_INFO,
						"Envío notificación de bloqueo al planificador.");

				fd_mensaje(personaje->sockPlanif, P_TO_N_BLOQUEO,
						mensajeBloqueo, &bytes_enviados2);
			}
		}

		//no se llegó al recurso todavía. Enviar mensaje de turno cumplido
		log_in_disk_per(LOG_LEVEL_INFO,
				"No se llegó a la posición del recurso aún. Envío notificación de turno cumplido al planificador.");

		fd_mensaje(personaje->sockPlanif, P_TO_PL_TURNO_CUMPLIDO,
				mensajeFinTurno, &bytes_enviados);
	}

}

//Avisa y se desconecta del planificador y del nivel
void salirDelNivel(int sockNivel, int sockPlanif) {

	char *mensajeFinNivel, *mensajeFinNivelP;
	int bytes_enviados, bytes_enviados1;

	mensajeFinNivel = "Bye Nivel";
	mensajeFinNivelP = "Bye Planificador";

	log_in_disk_per(LOG_LEVEL_INFO,
			"Me desconecto del nivel y su planificador.");

	fd_mensaje(sockNivel, P_TO_N_OBJ_CUMPLIDO, mensajeFinNivel,
			&bytes_enviados);

	fd_mensaje(sockPlanif, P_TO_PL_OBJ_CUMPLIDO, mensajeFinNivelP,
			&bytes_enviados1);

	close(sockNivel);
	close(sockPlanif);
}

//Devuelve TRUE si la lista de recursos del nivel pendientes es null.
int objetivoNivelCumplido(Personaje* personaje) {

//if (personaje->niveles.recursosRestantes == NULL )
//harcodeo para que por ahora me devuelva siempre 0 :P
	if (2 > 3)
		return 1;
	else {
		return 0;
	}
}

//Devuelve TRUE si la lista de niveles pendientes es null.
int planDeNivelesCumplido(t_list * niveles) {

//if (niveles == NULL )
//harcodeo para que por ahora me devuelva siempre 0 :P
	if (2 > 3)
		return 1;
	else {
		return 0;
	}

}

//Avisa al nivel para que lo reinicie en el mapa y libere los recursos. Reinicia los recursos pendientes. VER
void reiniciarNivel(Personaje *personaje) {

	char mensaje[max_len];
	int bytes_enviados;

//Envío mensaje a nivel del tipo P_TO_N_REINICIAR_NIVEL. "simbolo"
	sprintf(mensaje, "%c", personaje->simbolo);

	log_in_disk_per(LOG_LEVEL_INFO, "Reiniciando nivel... Aviso al Nivel");

	fd_mensaje(personaje->sockNivel, P_TO_N_REINICIAR_NIVEL, mensaje,
			&bytes_enviados);
}

//Avisa al nivel y planificador para que lo eliminen y se desconecta de ambos. Reinicia su plan de niveles. VER
void reiniciarPlanDeNiveles(Personaje *personaje) {

	char mensajeNiv[max_len], mensajePl[max_len];
	int bytes_enviados, bytes_enviados1;

	log_in_disk_per(LOG_LEVEL_INFO, "Reiniciando plan de niveles...");

//Envío mensaje a nivel del tipo P_TO_N_SALIR "simbolo"
	sprintf(mensajeNiv, "%c", personaje->simbolo);

	log_in_disk_per(LOG_LEVEL_INFO, "Aviso al Nivel y cierro la conexión.");
	fd_mensaje(personaje->sockNivel, P_TO_N_SALIR, mensajeNiv, &bytes_enviados);

	close(personaje->sockNivel);

//Envío mensaje a orquestador/planficador del tipo P_TO_O_SALIR "simbolo"
	sprintf(mensajePl, "%c", personaje->simbolo);

	log_in_disk_per(LOG_LEVEL_INFO, "Aviso al Planificador");
	fd_mensaje(personaje->sockPlanif, P_TO_O_REINICIAR_JUEGO, mensajePl,
			&bytes_enviados1);

	close(personaje->sockPlanif);
}

//Devuelve 1 si la posicion actual del personaje es igual al del recurso que necesita, y 0 en caso contrario
int evaluarPosicion(Posicion posicionActual, Posicion posicionRecurso) {

	if (posicionActual.x == posicionRecurso.x
			&& posicionActual.y == posicionRecurso.y)
			//Harcodeo para que devuelva 1 para probar solicitarInstanciaRecurso()
			//Harcodeo para que devuelve 0 para probar seguir moviéndose
			//if (2 > 1)
					{
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se llegó a la posición del recurso que se necesita.");
		return 1;
	} else {
		log_in_disk_per(LOG_LEVEL_INFO,
				"No se llegó aún a la posición del recurso que se necesita.");
		return 0;
	}

}

//Se moverá una posición en un eje para acercarse al próximo recurso.
void moverse(Personaje* personaje) {

	char mensajeMovimiento[max_len];
	int bytes_enviados, flag_y, tipo;
	Posicion nuevaPosicion;
	char* buffer;

	log_in_disk_per(LOG_LEVEL_INFO,
			"Me muevo una posición y le aviso al nivel.");

	log_in_disk_per(LOG_LEVEL_INFO, "Mi posición actual: (%d, %d)",
			personaje->posActual.x, personaje->posActual.y);

//Ver si hay una mejor forma que estos if anidados :P

//Primero me muevo por los x. Cuando llego a x le doy la condición para moverse a y.
	if (personaje->posActual.x == personaje->posProxRecurso.x) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"El personaje está en la coordenada x del recurso deseado");
		flag_y = 1;
	}

	if (personaje->posActual.x < personaje->posProxRecurso.x) {
		nuevaPosicion.x = (personaje->posActual.x + 1);
		nuevaPosicion.y = (personaje->posActual.y);
	} else {
		if (personaje->posActual.x > personaje->posProxRecurso.x) {
			nuevaPosicion.x = (personaje->posActual.x - 1);
			nuevaPosicion.y = (personaje->posActual.y);
		}
	}

	if (flag_y == 1) {
		if (personaje->posActual.y < personaje->posProxRecurso.y) {
			nuevaPosicion.y = (personaje->posActual.y + 1);
			nuevaPosicion.x = (personaje->posActual.x);
		} else {
			if (personaje->posActual.x > personaje->posProxRecurso.x) {
				nuevaPosicion.y = (personaje->posActual.y - 1);
				nuevaPosicion.x = (personaje->posActual.x);
			}
		}
	}

	log_in_disk_per(LOG_LEVEL_INFO,
			"Mi nueva posición a pedir al nivel: (%d, %d)", nuevaPosicion.x,
			nuevaPosicion.y);

//Envío mensaje a nivel del tipo P_TO_N_MOVIMIENTO. "simbolo;xActual;yActual;xNuevo;yNuevo"
	sprintf(mensajeMovimiento, "%c;%d;%d;%d;%d", personaje->simbolo,
			personaje->posActual.x, personaje->posActual.y, nuevaPosicion.x,
			nuevaPosicion.y);

	fd_mensaje(personaje->sockNivel, P_TO_N_MOVIMIENTO, mensajeMovimiento,
			&bytes_enviados);

//Espero confirmación del nivel N_TO_P_MOVIDO

	log_in_disk_per(LOG_LEVEL_INFO,
			"Espero confirmación de movimiento del Nivel...");

	buffer = recv_variable(personaje->sockNivel, &tipo);

	if (tipo == N_TO_P_MOVIDO) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Se movió a la posición solicitada:%s",
				buffer);
		personaje->posActual.x = nuevaPosicion.x;
		personaje->posActual.y = nuevaPosicion.y;
		free(buffer);
	}

	if (tipo == ERROR) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Hubo un error al mover el personaje. Respuesta del Nivel: %s",
				buffer);
		//hacer ciclo para volver a mandar solicitud de movimiento si falla
		free(buffer);
	}
}

int solicitarInstanciaRecurso(Personaje *personaje) {

	char mensaje[max_len];
	int bytes_enviados, tipo;
	char *buffer;

//Envío mensaje a nivel del tipo P_TO_N_SOLIC_RECURSO. "simbolo;recurso"
	sprintf(mensaje, "%c;%c", personaje->simbolo, personaje->recursoActual);

	log_in_disk_per(LOG_LEVEL_INFO, "Solicito una instancia de %s al Nivel",
			personaje->recursoActual);

	fd_mensaje(personaje->sockNivel, P_TO_N_SOLIC_RECURSO, mensaje,
			&bytes_enviados);

//Recibe respuesta del Nivel si se adjudicó o no el recurso

	log_in_disk_per(LOG_LEVEL_INFO, "Espero respuesta del Nivel...");

	buffer = recv_variable(personaje->sockNivel, &tipo);

	if (tipo == N_TO_P_RECURSO_ERROR) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"No hay instancias disponibles del recurso:%s", buffer);
		free(buffer);
		return 0;
	}

	if (tipo == N_TO_P_RECURSO_OK) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Hay instancias disponibles. Se adjudicó correctamente el recurso: %s",
				buffer);
		//eliminar de la lista de recursos el adjudicado. Apuntar al sgte recurso.
		personaje->recursoActual = '-';
		free(buffer);
		return 1;
	}

	/*if (tipo != N_TO_P_RECURSO_OK && tipo != N_TO_P_RECURSO_ERROR) {
	 log_in_disk_per(LOG_LEVEL_INFO, "No se recibió un mensaje esperado: %s",
	 buffer);
	 exit(EXIT_FAILURE);
	 }*/
	//TODO fata este return lo agrege para que no me de un error
	return 1;
}

int conocePosicionRecurso(char recursoActual) {

	if (recursoActual == '-') {
		log_in_disk_per(LOG_LEVEL_INFO,
				"No tengo asignado un recurso actual. No conozco la posición del próximo recurso.");
		return 0;
	} else {
		return 1;
	}
}

t_recusos *recursos_nivel(t_list *recursos, char *nivel) {
	log_in_disk_per(LOG_LEVEL_TRACE,
			"func recursos_nivel busco los recursos del nivel: %s", nivel);

	bool _list_elements(t_recusos *list_recursos) {

		log_in_disk_orq(LOG_LEVEL_TRACE, "El elemento %s lo comparo con %s",
				list_recursos->NOMBRE, nivel);

		if (!strcmp(list_recursos->NOMBRE, nivel)) {

			log_in_disk_per(LOG_LEVEL_TRACE,
					"devuelvo el los recursos del nivel");

			return true;

		} else {
			return false;
		}
	}

	return (t_recusos*) list_find(recursos, (void*) _list_elements);

}
