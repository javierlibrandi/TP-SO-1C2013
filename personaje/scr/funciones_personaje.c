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
#include <stdbool.h>
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

	// lo pongo para el ejemplo

	log_in_disk_per(LOG_LEVEL_INFO, "Voy a crear al personaje");
	t_param_persoje param = leer_personaje_config();

	personaje->nombre = param.NOMBRE;
	personaje->simbolo = param.SMBOLO;
	personaje->vidas = param.VIDAS;
	personaje->vidasIniciales = param.VIDAS;
	personaje->ip_orquestador = param.IP;
	personaje->puerto_orquestador = param.PUERTO_PLATAFORMA;

	personaje->nivelActual = -1;
	personaje->indexRecurso = -1;
	personaje->recursoActual = '-';
	personaje->finNivel = false;
	personaje->bloqueado = false;

	log_in_disk_per(LOG_LEVEL_INFO,
			"El personaje creado es %s, identificado con el caracter %c, y con %d vidas.",
			personaje->nombre, personaje->simbolo, personaje->vidas);

	for (i = 0; param.PLAN_NIVELES[i] != '\0'; i++) { //recorro todos los niveles
		log_in_disk_per(LOG_LEVEL_INFO, "Estoy en el %s",
				param.PLAN_NIVELES[i]);

		aux_nivel = param.PLAN_NIVELES[i];

		recursos = recursos_nivel(param.RECURSOS, aux_nivel);

		for (k = 0; recursos->RECURSOS[k] != '\0'; k++) { //recorro todos los recuros del nivel

			log_in_disk_per(LOG_LEVEL_INFO, "Nombre: %s ",
					recursos->RECURSOS[k]);

		}

	}

	personaje->niveles = param.RECURSOS;

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

	nivel = determinarProxNivel(personaje);
	personaje->infoNivel.nombre = nivel;
	sprintf(mensaje, "%s;%c;%s", personaje->nombre, personaje->simbolo, nivel);

	log_in_disk_per(LOG_LEVEL_INFO, "Envío primer mensaje de saludo.");
	fd_mensaje(descriptor, P_TO_P_SALUDO, mensaje, &bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_P_SALUDO");
		log_in_disk_per(LOG_LEVEL_ERROR, "%s",
				"Plataforma cerró la conexión. El proceso personaje va a terminar.");
	}

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

	if (tipo != ERROR && tipo != OK) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"No se recibió un mensaje esperado: %s", buffer);
		exit(EXIT_FAILURE);
	}

	free(buffer);
	return descriptor;
}

char* determinarProxNivel(Personaje *personaje) {
	char *proxNivel;
	t_link_element *nodoNivelProx;
	int nivAct;

	personaje->nivelActual++;
	nivAct = personaje->nivelActual;

	/*log_in_disk_per(LOG_LEVEL_INFO,
	 "nivel actual: %d, tamaño lista niveles: %d",
	 personaje->nivelActual, list_size(personaje->niveles));*/

	if (list_size(personaje->niveles) > nivAct) {
		proxNivel = list_get(personaje->niveles, nivAct);
		//log_in_disk_per(LOG_LEVEL_INFO, "ProxNivel -> %s", proxNivel);

	}

	nodoNivelProx = obtenerNodo(personaje->niveles, nivAct + 1);
	//log_in_disk_per(LOG_LEVEL_INFO, "despues de obtener nodo");

	if (nodoNivelProx == NULL ) {
		//if ((obtenerNodo(personaje.niveles, personaje.nivelActual))->next == NULL){
		log_in_disk_per(LOG_LEVEL_INFO,
				"Último nivel para completar el juego. var nivelActual");
		personaje->nivelActual = -3;
		log_in_disk_per(LOG_LEVEL_INFO, "var nivelActual: %d",
				personaje->nivelActual);
	}

	return proxNivel;

}

t_link_element* obtenerNodo(t_list* self, int index) {
	int cont = 0;

	if ((self->elements_count > index) && (index >= 0)) {
		t_link_element *element = self->head;
		while (cont < index) {
			element = element->next;
			cont++;
		}
		return element;
	}
	return NULL ;

}

InfoProxNivel consultarProximoNivel(int descriptor, Personaje* personaje) {

	int tipo, bytes_enviados;
	InfoProxNivel infoNivel;
	char *buffer;
	char **aux_msj;
	char mensaje[max_len];

//Creo string con nombre y nivel para enviar al Orquestador en P_TO_O_PROX_NIVEL
//Se fija en su lista de plan de niveles, cuál es el próximo nivel a completar.
	//infoNivel.nombre_nivel = determinarProxNivel(personaje);
	infoNivel.nombre_nivel = personaje->infoNivel.nombre;
	log_in_disk_per(LOG_LEVEL_INFO, "Próximo Nivel -> %s",
			infoNivel.nombre_nivel);
	log_in_disk_per(LOG_LEVEL_INFO,
			"Voy a enviar solicitud de ubicación del %s al orquestador",
			infoNivel.nombre_nivel);

	sprintf(mensaje, "%s;%s", personaje->nombre, infoNivel.nombre_nivel);

	fd_mensaje(descriptor, P_TO_O_PROX_NIVEL, mensaje, &bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_O_PROX_NIVEL");
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Orquestador cerró la conexión. El proceso personaje va a terminar");

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

	log_in_disk_per(LOG_LEVEL_ERROR, "%s",
			"EL personaje se desconecta del orquestador.");
	close(descriptor);

	free(aux_msj);
	free(buffer);

	return infoNivel;

}

void iniciarNivel(Personaje* personaje, InfoProxNivel infoNivel) {
	int descriptorPlan, tipoP, descriptorNiv, tipoN;

	int bytes_enviados_niv, bytes_enviados_pl;
	char mensaje1[max_len], mensaje2[max_len];
	char *bufferPla, *bufferNiv;
	t_recusos *recursos;
	int k;

	log_in_disk_per(LOG_LEVEL_INFO, "Me voy a conectar con %s",
			infoNivel.nombre_nivel);
	log_in_disk_per(LOG_LEVEL_INFO, "IP:%s", infoNivel.ip_nivel);
	log_in_disk_per(LOG_LEVEL_INFO, "PUERTO:%d", infoNivel.puerto_nivel);

	descriptorNiv = Abre_Conexion_Inet(infoNivel.ip_nivel,
			infoNivel.puerto_nivel);

	if (descriptorNiv == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR, "Hubo un error al conectarse al %s",
				infoNivel.nombre_nivel);
		exit(1);
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
		exit(EXIT_FAILURE);

	} else {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Conexión exitosa con planificador del %s",
				infoNivel.nombre_nivel);
	}

// Envía a Nivel P_TO_N_INICIAR_NIVEL "nombrePersonaje, símbolo"
	sprintf(mensaje1, "%s;%c", personaje->nombre, personaje->simbolo);

	fd_mensaje(descriptorNiv, P_TO_N_INICIAR_NIVEL, mensaje1,
			&bytes_enviados_niv);

	if (bytes_enviados_niv == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_INICIAR_NIVEL");
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Nivel cerró la conexión. El proceso personaje va a terminar.");

		exit(EXIT_FAILURE);
	}

//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

// Envía a Plataforma P_TO_PL_INICIAR_NIVEL para que lo asocie al planificador del nivel "nombre;nivelNro"
	sprintf(mensaje2, "%s;%s", personaje->nombre, infoNivel.nombre_nivel);

	fd_mensaje(descriptorPlan, P_TO_PL_INICIAR_NIVEL, mensaje2,
			&bytes_enviados_pl);

	if (bytes_enviados_pl == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_PL_INICIAR_NIVEL");
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Planificador cerró la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);

	}
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

		//fd_mensaje(personaje->sockPlanif, OK, mensaje2a, &bytes_enviados_pla);
//		if (bytes_enviados_pla == -1) {
//				log_in_disk_per(LOG_LEVEL_ERROR,
//						"Hubo un error al enviar el mensaje OK");
//				log_in_disk_per(LOG_LEVEL_ERROR,
//						"Planificador cerró la conexión. El proceso personaje va a terminar.");
//				exit(EXIT_FAILURE);
//
//			}

	}

	if (tipoP != OK && tipoP != ERROR) {
		log_in_disk_per(LOG_LEVEL_INFO, "No se recibió un mensaje esperado %s:",
				bufferPla);
		exit(EXIT_FAILURE);
	}
	tipoN = 0;
	while (tipoN != OK) {
		bufferNiv = recv_variable(descriptorNiv, &tipoN);
		if (tipoN == ERROR) {
			log_in_disk_per(LOG_LEVEL_ERROR, "Falló. Respuesta del Nivel:%s",
					bufferNiv);
			exit(EXIT_FAILURE);
		}

		if (tipoN == OK) {
			log_in_disk_per(LOG_LEVEL_INFO, "Se recibió OK del %s",
					infoNivel.nombre_nivel);
			personaje->infoNivel.nombre = infoNivel.nombre_nivel;
			personaje->sockNivel = descriptorNiv;
			personaje->posActual.x = 1;
			personaje->posActual.y = 1;

			log_in_disk_per(LOG_LEVEL_INFO, "Inicializo recursos del %s",
					infoNivel.nombre_nivel);
			recursos = recursos_nivel(personaje->niveles,
					infoNivel.nombre_nivel);

			for (k = 0; recursos->RECURSOS[k] != '\0'; k++) { //recorro todos los recuros del nivel

				log_in_disk_per(LOG_LEVEL_INFO, "Recurso %d: %s ", k,
						recursos->RECURSOS[k]);

			}
			personaje->infoNivel.recursos = recursos->RECURSOS;
		}

		if (tipoN != OK && tipoN != ERROR) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"No se recibió un mensaje esperado. Tipo:%d Buffer:%s",
					tipoN, bufferNiv);
			//exit(EXIT_FAILURE);
		}

	}
	free(bufferNiv);
	free(bufferPla);
}

//¿crear función con select que escuche al planif y al nivel?

// ******* 21/7 REEMPLAZAR TODA LA FUNCIÓN determinarProxRecurso en REPO
char* determinarProxRecurso(Personaje* personaje) {

	char* proxRecurso;
	char** prox;
	int index, k, cont;
	cont = 0;

	for (k = 0; personaje->infoNivel.recursos[k] != '\0'; k++) {
		cont++;
	}

	//log_in_disk_per(LOG_LEVEL_INFO, "Tamaño lista recursos: %d", cont);

	personaje->indexRecurso++;
	index = personaje->indexRecurso;
	prox = personaje->infoNivel.recursos;

	if (prox[index][0] != '\0') {
		proxRecurso = personaje->infoNivel.recursos[index];
	}

	if (cont == index + 1) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Último recurso para completar el nivel.");
		personaje->indexRecurso = -1;

	}

	return proxRecurso;

}

//el personaje se fija cuál es el próximo recurso a conseguir. Le solicita al Nivel las coordenadas x-y del mismo. */
void solicitarUbicacionRecurso(Personaje* personaje) {

	char* recurso;
	int bytes_enviados, tipo;
	char *buffer;
	char **aux_msj;
	char mensaje[max_len];

// Envía a Nivel P_TO_N_UBIC_RECURSO para pedir coordenadas del próximo recurso requerido "recurso"

	recurso = determinarProxRecurso(personaje);

	sprintf(mensaje, "%s", recurso);

	log_in_disk_per(LOG_LEVEL_INFO, "Necesito la ubicación del recurso %s",
			recurso);

	fd_mensaje(personaje->sockNivel, P_TO_N_UBIC_RECURSO, mensaje,
			&bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_UBIC_RECURSO");
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Nivel ha cerrado la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);

	}

//Recibe coordenadas del recurso

	log_in_disk_per(LOG_LEVEL_INFO, "Espero respuesta del Nivel...");

	tipo = 0;
	while (tipo != N_TO_P_UBIC_RECURSO && tipo != ERROR) {
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

			personaje->posProxRecurso.x = atoi(aux_msj[0]);
			personaje->posProxRecurso.y = atoi(aux_msj[1]);

			personaje->recursoActual = *recurso;
		}

		if (tipo != N_TO_P_UBIC_RECURSO && tipo != ERROR) {
			log_in_disk_per(LOG_LEVEL_ERROR,
					" UbicRecurso: No se recibió un mensaje esperado. Tipo:%d Buffer:%s",
					tipo, buffer);
			//exit(EXIT_FAILURE);
		}
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

//***** REEMPLAZAR FUNCIÓN ejecutarTurno en REPO 21/7
void ejecutarTurno(Personaje *personaje) {
	int bytes_enviados, bytes_enviados1, bytes_enviados2, recursoAdjudicado;
	char *mensajeFinTurno;
	char mensajeBloqueo[max_len];

	//Me fijo si el anterior turno estuve bloqueado par asignarme el recurso correspondiente.
	if (personaje->bloqueado && personaje->indexRecurso == -1) {
		//Entra por acá cuando justo el recurso por el que se bloqueó era el último para completar el nivel
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se da como asignado el recurso por el que había quedado bloqueado. Index: %d",
				personaje->indexRecurso);
		personaje->recursoActual = '-';
		personaje->finNivel = true;
		personaje->bloqueado = false;
		return;

	} else {
		if (personaje->bloqueado) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Se da como asignado el recurso por el que había quedado bloqueado. Index: %d",
					personaje->indexRecurso);
			personaje->recursoActual = '-';
			personaje->bloqueado = false;
		}

	}

//Armo mensaje de turno cumplido y bloqueo. Ver con Planif. qué info mandar
	mensajeFinTurno = personaje->nombre;

	sprintf(mensajeBloqueo, "%c", personaje->recursoActual);

//Espero y recibo notificación de movimiento permitido

	log_in_disk_per(LOG_LEVEL_INFO,
			"Se recibió notificación de movimiento permitido");

//Si no sabemos las coordenadas del próximo recurso a conseguir preguntamos y luego nos movemos en 1 quantum
	if (!conocePosicionRecurso(personaje->recursoActual)) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Desconozco la ubicación del recurso que necesito. Solicito coordenadas al %s",
				personaje->infoNivel.nombre);
		solicitarUbicacionRecurso(personaje);
	}

	log_in_disk_per(LOG_LEVEL_INFO,
			"Me muevo una posición en dirección del recurso");
	moverse(personaje);

	log_in_disk_per(LOG_LEVEL_INFO,
			"Evalúo si llegué a la posición destino del recurso.");

	if (evaluarPosicion(personaje->posActual, personaje->posProxRecurso)) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Llegué al recurso. Solicito una instancia de %c.",
				personaje->recursoActual);

		recursoAdjudicado = solicitarInstanciaRecurso(personaje);
		if (recursoAdjudicado) {
			//Se adjudicó el recurso, agregar a la lista de recursos del personaje
			log_in_disk_per(LOG_LEVEL_INFO,
					"****** RECURSO %c CONSEGUIDO ******",
					personaje->recursoActual);
			fd_mensaje(personaje->sockPlanif, P_TO_PL_RECURSO_CONSEGUIDO,
					mensajeFinTurno, &bytes_enviados);

			if (bytes_enviados == -1) {
				log_in_disk_per(LOG_LEVEL_TRACE,
						"Hubo un error al enviar el mensaje P_TO_PL_TURNO_CUMPLIDO");
				log_in_disk_per(LOG_LEVEL_TRACE,
						"Planificador cerró la conexión. El proceso personaje va a terminar.");
				exit(EXIT_FAILURE);
			}

		} else {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Nivel informa que no hay instancias disponibles de %c",
					personaje->recursoActual);
			log_in_disk_per(LOG_LEVEL_INFO,
					"Envío notificación de bloqueo al planificador.");

			fd_mensaje(personaje->sockPlanif, P_TO_PL_BLOQUEO, mensajeBloqueo,
					&bytes_enviados1);

			if (bytes_enviados1 == -1) {
				log_in_disk_per(LOG_LEVEL_TRACE,
						"Hubo un error al enviar el mensaje P_TO_N_BLOQUEO");
				log_in_disk_per(LOG_LEVEL_TRACE,
						"Nivel cerró la conexión. El proceso personaje va a terminar.");
				exit(EXIT_FAILURE);
			}
			personaje->bloqueado = true;
		}
	} else {

		//no se llegó al recurso todavía. Enviar mensaje de turno cumplido
		log_in_disk_per(LOG_LEVEL_INFO,
				"Envío notificación de turno cumplido al planificador.");

		fd_mensaje(personaje->sockPlanif, P_TO_PL_TURNO_CUMPLIDO,
				mensajeFinTurno, &bytes_enviados2);

		if (bytes_enviados2 == -1) {
			log_in_disk_per(LOG_LEVEL_TRACE,
					"Hubo un error al enviar el mensaje P_TO_PL_TURNO_CUMPLIDO");
			log_in_disk_per(LOG_LEVEL_TRACE,
					"Planificador cerró la conexión. El proceso personaje va a terminar.");
			exit(EXIT_FAILURE);
		}
	}
}

//Avisa y se desconecta del planificador y del nivel
void salirDelNivel(Personaje *personaje) {
	char *mensajeFinNivel, *mensajeFinNivelP;
	int vidas, sockNivel, sockPlanif;
	int bytes_enviados, bytes_enviados1, bytes_enviados3, bytes_enviados4, tipo;

	vidas = personaje->vidas;
	sockNivel = personaje->sockNivel;
	sockPlanif = personaje->sockPlanif;

	mensajeFinNivel = "Bye Nivel";
	mensajeFinNivelP = "Bye Planificador";

	log_in_disk_per(LOG_LEVEL_INFO,
			"Me desconecto del planificador.");

	if (personaje->nivelActual != -2) {
		//Si no es el último nivel
		fd_mensaje(sockPlanif, P_TO_PL_OBJ_CUMPLIDO, mensajeFinNivelP,
				&bytes_enviados1);

		if (bytes_enviados1 == -1) {
			log_in_disk_per(LOG_LEVEL_ERROR,
					"Hubo un error al enviar el mensaje P_TO_PL_OBJ_CUMPLIDO");

			log_in_disk_per(LOG_LEVEL_ERROR,
					"Planificador cerró la conexión. El proceso personaje va a terminar.");
			exit(EXIT_FAILURE);
		}
		//Espero OK del planificador de finalización de nivel.
		tipo = 0;
		while (tipo != OK) {

			recv_variable(sockPlanif, &tipo);

			if (tipo == OK) {
				log_in_disk_per(LOG_LEVEL_ERROR,
						"Se recibió OK de finalización de nivel del planificador.");
				log_in_disk_per(LOG_LEVEL_INFO,
						"Cierro socket del planificador");

				close(sockPlanif);
			} else {
				log_in_disk_per(LOG_LEVEL_ERROR,
						"Ok de Obj Cumplido de PLanif: No se recibió un mensaje esperado. Tipo:%d ", tipo);
				sleep(3);

			}

		}
	} // fin IF si no es último nivel

	log_in_disk_per(LOG_LEVEL_INFO,
				"Me desconecto del nivel.");

	fd_mensaje(sockNivel, P_TO_N_OBJ_CUMPLIDO, mensajeFinNivel,
			&bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_OBJ_CUMPLIDO");

		log_in_disk_per(LOG_LEVEL_ERROR,
				"Nivel cerró la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);
	}

	tipo = 0;
	while (tipo != OK) {
		recv_variable(sockNivel, &tipo);

		if (tipo == OK) {
			log_in_disk_per(LOG_LEVEL_ERROR,
					"Se recibió OK de finalización de nivel.");
			log_in_disk_per(LOG_LEVEL_INFO, "Cierro socket del nivel");
			close(sockNivel);

		} else {
			log_in_disk_per(LOG_LEVEL_ERROR,
					"No se recibió un mensaje esperado. Tipo:%d ", tipo);
			sleep(1);

		}

	}

}

// se debe inicializar nuevamente la lista de recursos de determinado nivel. Debe ser igual a la del arch de configuración.
void reiniciarListaRecursos(Personaje *personaje) {
	log_in_disk_per(LOG_LEVEL_INFO, "Reiniciando recursos a conseguir del %s",
			personaje->infoNivel.nombre);

	personaje->recursoActual = '-';
	personaje->indexRecurso = -1;

}

bool objetivoNivelCumplido(Personaje* personaje) {

	if (personaje->finNivel) {
		return true;
	} else {
		return false;
	}
}

//REEMPLAZAR EN REPO 21/7
bool planDeNivelesCumplido(Personaje* personaje) {

	if (personaje->nivelActual == -2)
		return true;
	else {
		return false;
	}

}

//Avisa al nivel para que lo reinicie en el mapa y libere los recursos. Reinicia los recursos pendientes.
//REEMPLAZAR TODA ESTA FUNCIÓN EN REPO 21/7
void reiniciarNivel(Personaje *personaje) {

	char mensaje[max_len];
	int bytes_enviados;

//Envío mensaje a nivel del tipo P_TO_N_REINICIAR_NIVEL. "simbolo"
	sprintf(mensaje, "%c", personaje->simbolo);

	log_in_disk_per(LOG_LEVEL_INFO, "****** REINICIANDO %s ******",
			personaje->infoNivel.nombre);

	fd_mensaje(personaje->sockNivel, P_TO_N_REINICIAR_NIVEL, mensaje,
			&bytes_enviados);

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar el mensaje P_TO_N_REINICIAR_NIVEL");

		log_in_disk_per(LOG_LEVEL_ERROR,
				"Nivel cerró la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);
	}

	reiniciarListaRecursos(personaje);

	personaje->posActual.x = 1;
	personaje->posActual.y = 1;
	personaje->bloqueado = false;
	log_in_disk_per(LOG_LEVEL_INFO,
			"Vuelvo a empezar en posición (1,1) del mapa.");

}

//Reinicia sus vidas y plan de niveles.
//REEMPLAZAR TODA ESTA FUNCIÓN EN REPO 21/7
void reiniciarPlanDeNiveles(Personaje *personaje) {

	log_in_disk_per(LOG_LEVEL_INFO,
			"****** REINICIANDO PLAN DE NIVELES ******");

	personaje->nivelActual = -1;
	personaje->recursoActual = '-';
	personaje->indexRecurso = -1;
	personaje->finNivel = false;
	personaje->bloqueado = false;

}

//Devuelve 1 si la posicion actual del personaje es igual al del recurso que necesita, y 0 en caso contrario
bool evaluarPosicion(Posicion posicionActual, Posicion posicionRecurso) {

	if (posicionActual.x == posicionRecurso.x
			&& posicionActual.y == posicionRecurso.y) {
		log_in_disk_per(LOG_LEVEL_INFO,
				"Se llegó a la posición del recurso que se necesita.");
		return true;
	} else {
		log_in_disk_per(LOG_LEVEL_INFO,
				"No se llegó aún a la posición del recurso que se necesita.");
		return false;
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
			if (personaje->posActual.y > personaje->posProxRecurso.y) {
				nuevaPosicion.y = (personaje->posActual.y - 1);
				nuevaPosicion.x = (personaje->posActual.x);
			}
		}
	}

	log_in_disk_per(LOG_LEVEL_INFO,
			"soy %s --->Mi nueva posición a pedir al nivel: (%d, %d)",
			personaje->nombre, nuevaPosicion.x, nuevaPosicion.y);

//Envío mensaje a nivel del tipo P_TO_N_MOVIMIENTO. "simbolo;xActual;yActual;xNuevo;yNuevo"
	sprintf(mensajeMovimiento, "%c;%d;%d;%d;%d", personaje->simbolo,
			personaje->posActual.x, personaje->posActual.y, nuevaPosicion.x,
			nuevaPosicion.y);

	fd_mensaje(personaje->sockNivel, P_TO_N_MOVIMIENTO, mensajeMovimiento,
			&bytes_enviados);

	log_in_disk_per(LOG_LEVEL_INFO,
			"Espero confirmación de movimiento del Nivel...");

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Nivel cerró la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);
	}

	tipo = 0;
	while (tipo != N_TO_P_MOVIDO) {
		buffer = recv_variable(personaje->sockNivel, &tipo);

		if (tipo == N_TO_P_MOVIDO) {
			log_in_disk_per(LOG_LEVEL_ERROR,
					"Se movió a la posición solicitada:%s", buffer);
			personaje->posActual.x = nuevaPosicion.x;
			personaje->posActual.y = nuevaPosicion.y;
		}

		if (tipo == ERROR) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Hubo un error al mover el personaje. Respuesta del Nivel: %s",
					buffer);
		}
		if (tipo != ERROR && tipo != N_TO_P_MOVIDO) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"No se recibió un mensaje esperado. Tipo: %d. Buffer:%s",
					tipo, buffer);
		}

	}
	free(buffer);
}

// ******** 21/7 REEMPLAZAR TODA FUNCIÓN solicitarInstanciaRecurso 21/7 EN REPO
bool solicitarInstanciaRecurso(Personaje *personaje) {

	char mensaje[max_len];
	int bytes_enviados, tipo;
	char *buffer;

//Envío mensaje a nivel del tipo P_TO_N_SOLIC_RECURSO. "simbolo;recurso"
//sprintf(mensaje, "%c;%c", personaje->simbolo, personaje->recursoActual);
	sprintf(mensaje, "%s", "Quiero una instancia");

	log_in_disk_per(LOG_LEVEL_INFO, "Solicito una instancia al Nivel");

	fd_mensaje(personaje->sockNivel, P_TO_N_SOLIC_RECURSO, mensaje,
			&bytes_enviados);

//Recibe respuesta del Nivel si se adjudicó o no el recurso

	log_in_disk_per(LOG_LEVEL_INFO, "Espero respuesta del Nivel...");

	if (bytes_enviados == -1) {
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Hubo un error al enviar P_TO_N_SOLIC_RECURSO.");
		log_in_disk_per(LOG_LEVEL_ERROR,
				"Nivel cerró la conexión. El proceso personaje va a terminar.");
		exit(EXIT_FAILURE);
	}
	tipo = 0;
	while (tipo != N_TO_P_RECURSO_OK && tipo != N_TO_P_RECURSO_ERROR) {
		buffer = recv_variable(personaje->sockNivel, &tipo);

		if (tipo == N_TO_P_RECURSO_ERROR) {
			log_in_disk_per(LOG_LEVEL_ERROR,
					"No hay instancias disponibles del recurso:%s", buffer);
			//Lo adjudico para el siguiente turno pero quedo bloqueado

			personaje->bloqueado = true;
			return false;
		}

		if (tipo == N_TO_P_RECURSO_OK) {
			log_in_disk_per(LOG_LEVEL_INFO,
					"Hay instancias disponibles. Se adjudicó correctamente el recurso: %s",
					buffer);
			//eliminar de la lista de recursos el adjudicado. Apuntar al sgte recurso.
			personaje->recursoActual = '-';
			log_in_disk_per(LOG_LEVEL_INFO,
					"Valor index: %d. Si es -1 deberia salir del nivel",
					personaje->indexRecurso);
			if (personaje->indexRecurso == -1) {
				personaje->finNivel = true;
			}
			return true;
		}

		if (tipo != N_TO_P_RECURSO_OK && tipo != N_TO_P_RECURSO_ERROR)
			log_in_disk_per(LOG_LEVEL_INFO,
					"No se recibió un mensaje esperado. Tipo:%d. Buffer:%s",
					tipo, buffer);
		//return false;
		//exit(EXIT_FAILURE);

	}

	free(buffer);

}

bool conocePosicionRecurso(char recursoActual) {

	if (recursoActual == '-') {
		log_in_disk_per(LOG_LEVEL_INFO,
				"No tengo asignado un recurso actual. No conozco la posición del próximo recurso.");
		return false;
	} else {
		return true;
	}
}

//***** REMPLAZAR esta función 21/7 en REPO
t_recusos *recursos_nivel(t_list *recursos, char *nivel) {
	log_in_disk_per(LOG_LEVEL_TRACE, "Busco los recursos del %s", nivel);

	bool _list_elements(t_recusos *list_recursos) {

		//log_in_disk_orq(LOG_LEVEL_TRACE, "El elemento %s lo comparo con %s",
		//		list_recursos->NOMBRE, nivel);

		if (!strcmp(list_recursos->NOMBRE, nivel)) {

			log_in_disk_per(LOG_LEVEL_TRACE, "Recursos del nivel:");

			return true;

		} else {
			return false;
		}
	}

	return (t_recusos*) list_find(recursos, (void*) _list_elements);

}
