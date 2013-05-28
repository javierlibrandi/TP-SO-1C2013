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

#define IP "192.168.1.1"
#define PUERTO 6000

//Por ahora se inicializa el personaje harcodeado. Falta hacer funcionar la lectura del arch de configuración y el uso de listas.
Personaje* nuevoPersonaje(char* nombrePersonaje)
{
Personaje* personaje = NULL;

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
personaje->ip_orquestador = "192.168.0.100";
personaje->puerto_orquestador = 5000;
//personaje->nivelesRestantes= null;

log_in_disk_per(LOG_LEVEL_INFO, "EL personaje creado es %s", personaje->nombre);

return  personaje;
}

int conectarOrquestador(Personaje* personaje){

	int descriptor, tipo;
	struct t_send mensaje;
	int bytes_enviados;
	void *buffer = NULL;

	// Establezco conexión con el orquestador
	log_in_disk_per(LOG_LEVEL_INFO, "%s", "Conectándose a IP: ", personaje->ip_orquestador  );
	log_in_disk_per(LOG_LEVEL_INFO, "%s", "Conectándose a PUERTO: ", personaje->puerto_orquestador  );

	descriptor=Abre_Conexion_Inet(personaje->ip_orquestador, personaje->puerto_orquestador);

	if(descriptor==-1){
		log_in_disk_per(LOG_LEVEL_ERROR, "%s", "Hubo un error al conectarse al orquestador");

	}else{
		log_in_disk_per(LOG_LEVEL_INFO, "%s", "Conexión exitosa con el orquestador");
		}

		// Envía SALUDO_PERSONAJE.
		memset(mensaje.mensaje, '\0', 20);
		strcpy(mensaje.mensaje, "Hola soy un personaje");

		mensaje.header_mensaje= P_TO_P_SALUDO;
		mensaje.payLoadLength = sizeof(mensaje.mensaje);

		bytes_enviados= Escribe_Socket(descriptor, &mensaje, sizeof(mensaje));

		if(bytes_enviados == -1)
			log_in_disk_per(LOG_LEVEL_ERROR, "%s", "Hubo un error al enviar el mensaje SALUDO_PERSONAJE");
			//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

		//Recibo OK del orquestador

		buffer = recv_variable(descriptor, &tipo);
			if (tipo == ERROR) {
				log_in_disk_per(LOG_LEVEL_ERROR, "%s", (char*) buffer);
				exit(EXIT_FAILURE);}

			if (tipo == OK)
				log_in_disk_per(LOG_LEVEL_INFO, "%s", "Se recibió OK del orquestador.");

free(buffer);
return descriptor;
}

//Ver como crear la estructura InfoProxNivel en esta función con la respuesta del orquestador.
InfoProxNivel consultarProximoNivel(int descriptor, Personaje* personaje){

	struct t_send mensaje;
	int tipo, bytes_enviados;
	InfoProxNivel info;
	void *buffer = NULL;

	//Creo estructura PersonajeNivel para enviársela al Orquestador en P_TO_O_PROX_NIVEL
	//Se fija en su lista de plan de niveles, cuál es el próximo nivel a completar.

	//Ver como enviar estructura PersonajeNivel en mensaje
	//pers_nivel.nombrePersonaje=personaje->nombre;
	//void *list_get(t_list *, int index);
	//nivel= list_get(personaje->nivelesRestantes, 1);
	//harcodeo
	//pers_nivel.proxNivel="Nivel1";

		//por ahora harcodeo separando por ";"
		memset(mensaje.mensaje, '\0', 20);
		strcpy(mensaje.mensaje, "Mario;Nivel1");

		mensaje.header_mensaje= P_TO_O_PROX_NIVEL;
		mensaje.payLoadLength = sizeof(mensaje.mensaje);

		bytes_enviados= Escribe_Socket(descriptor, &mensaje, sizeof(mensaje));

		if(bytes_enviados == -1){
			log_in_disk_per(LOG_LEVEL_ERROR, "%s", "Hubo un error al enviar el mensaje P_TO_O_PROX_NIVEL");
		}

		//Recibo ip/puerto del nivel 0_TO_P_UBIC_NIVEL del tipo InfoProxNivel. Ver con Mati.
		buffer = recv_variable(descriptor, &tipo);
				if (tipo == ERROR) {
					log_in_disk_per(LOG_LEVEL_ERROR, "%s", (char*) buffer);
					exit(EXIT_FAILURE);}

				if (tipo == O_TO_P_UBIC_NIVEL)
					log_in_disk_per(LOG_LEVEL_INFO, "%s", "Se recibió información del orquestador.");

	//ver como extraer ip y puerto del mensaje recibido en buffer
	//Por ahora harcodeo
	info.ip_nivel=IP;
	info.puerto_nivel=PUERTO;

	log_in_disk_per(LOG_LEVEL_ERROR, "%s", "EL personaje se desconecta del orquestador.");
	close(descriptor);
	free(buffer);
	return info;

};

void iniciarNivel(Personaje* personaje, InfoProxNivel infoNivel){
	int descriptorNiv, descriptorPlan, tipoN, tipoP;
	int bytes_enviados_niv, bytes_enviados_pl;
	struct t_send mensaje1, mensaje2;
	void* bufferNiv = NULL;
	void* bufferPla = NULL;


	descriptorNiv=Abre_Conexion_Inet(infoNivel.ip_nivel, infoNivel.puerto_nivel);

		if(descriptorNiv==-1){
			log_in_disk_per(LOG_LEVEL_ERROR, "Hubo un error al conectarse al Nivel");
		}else{
			log_in_disk_per(LOG_LEVEL_INFO, "Conexión exitosa con Nivel");}

	descriptorPlan=Abre_Conexion_Inet(personaje->ip_orquestador, personaje->puerto_orquestador);

		if(descriptorPlan==-1){
			log_in_disk_per(LOG_LEVEL_ERROR, "Hubo un error al conectarse al planificador del nivel");
		}else{
			log_in_disk_per(LOG_LEVEL_INFO, "Conexión exitosa con planificador del nivel");}


	// Envía a Nivel P_TO_N_INICIAR_NIVEL
	memset(mensaje1.mensaje, '\0', 20);
	//Ver con Maxi que necesita que le envíe y como me responde.
	strcpy(mensaje1.mensaje, "Hola Nivel. Quiero jugar en tu mapa");

	mensaje1.header_mensaje= P_TO_N_INICIAR_NIVEL;
	mensaje1.payLoadLength = sizeof(mensaje1.mensaje);

	bytes_enviados_niv= Escribe_Socket(descriptorNiv, &mensaje1, sizeof(mensaje1));

		if(bytes_enviados_niv == -1)
			log_in_disk_per(LOG_LEVEL_ERROR, "Hubo un error al enviar el mensaje P_TO_N_INICIAR_NIVEL");
			//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

	// Envía a Plataforma P_TO_PL_INICIAR_NIVEL para que lo asocie al planificador del nivel
	memset(mensaje2.mensaje, '\0', 20);
	//Ver con Javi que necesita que le envíe y como me responde.
	strcpy(mensaje2.mensaje, "Hola Plataforma. Quiero jugar en Nivel X");

	mensaje2.header_mensaje= P_TO_PL_INICIAR_NIVEL;
	mensaje2.payLoadLength = sizeof(mensaje2.mensaje);

	bytes_enviados_pl= Escribe_Socket(descriptorPlan, &mensaje2, sizeof(mensaje2));

		if(bytes_enviados_pl == -1)
			log_in_disk_per(LOG_LEVEL_ERROR, "Hubo un error al enviar el mensaje P_TO_PL_INICIAR_NIVEL");
			//HACER CICLO PARA VOLVER A MANDAR MENSAJE si falla

	//Recibe OK de nivel y plataforma

	bufferNiv = recv_variable(descriptorNiv, &tipoN);
		if (tipoN == ERROR) {
			log_in_disk_per(LOG_LEVEL_ERROR, "%s", (char*) bufferNiv);
			exit(EXIT_FAILURE);}

		if (tipoN == OK)
			//(LOG_LEVEL_INFO, "Se recibió OK del nivel.");

	bufferPla = recv_variable(descriptorNiv, &tipoP);
		if (tipoP == ERROR) {
			log_in_disk_per(LOG_LEVEL_ERROR, "%s", (char*) bufferPla);
			exit(EXIT_FAILURE);}

		if (tipoP == OK)
			//(LOG_LEVEL_INFO, "Se recibió OK del planificador.");
			puts("Se recibió OK del planificador.");

	//VER SI HAY QUE VOLVER A MANDAR MENSAJES CON MÁS INFO

		free(bufferNiv);
		free(bufferPla);
}

//¿crear función con select que escuche al planif y al nivel?
int listenerPersonaje (int descriptorNiv, int descriptorPlan){

	int N=1;
	int descriptores[N], tipo, result, j, i;
	int maxDescriptor=0;
	fd_set readset;
	void *buffer=NULL;

	//Limpio lista de sockets
	FD_ZERO(&readset);

	//Inicializo vector de sockets
	descriptores[0]=descriptorNiv;
	descriptores[1]=descriptorPlan;

	//Agrego los dos sockets de Nivel y Planif a la lista y busco máximo valor
	for (j=0; j<N; j++) {
	   FD_SET(descriptores[j], &readset);
	   maxDescriptor = (maxDescriptor>descriptores[j])?maxDescriptor:descriptores[j];
	}

	// Me fijo si en alguno llegaron mensajes
	result = select(maxDescriptor+1, &readset, NULL, NULL, NULL);

	if (result == -1) {
		perror("select");
		exit(EXIT_FAILURE);	}
	else {
	   for (i=0; i<N; i++) {
	      if (FD_ISSET(descriptores[i], &readset)) {
	         buffer = recv_variable(descriptores[i], &tipo);

	         switch (tipo){
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



