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

#define MAXLEN 2000
#define IP "192.168.1.1"
#define PUERTO 5024

int main(void){

	int descriptor;
	char nombre_per[10];
	Personaje *personaje=NULL;
	t_send *mensaje, respuesta;
	int bytes_enviados, bytes_recibidos;
	PersonajeNivel pers_nivel;
	Nivel nivel;

	puts("Elija el nombre para su personaje:");
	printf( "\nHas elegido: \"%s\"\n", gets(nombre_per) );

	nuevoPersonaje(nombre_per, personaje);

	// Establezco conexión con el orquestador
	descriptor=Abre_Conexion_Inet(IP, PUERTO);

	if(descriptor==-1){
		puts("Hubo un error al conectarse al orquestador");

	}else{
		puts("Conexión exitosa con el orquestador");
	}

	// Envía SALUDO_PERSONAJE.
	mensaje.header_mensaje=1;
	mensaje.mensaje = "Hola soy un personaje ";
	mensaje.payLoadLength = strlen(mensaje.mensaje);

	bytes_enviados= Escribe_Socket(descriptor, &mensaje, sizeof(mensaje));

	if(bytes_enviados == -1){
			puts("Hubo un error al enviar el mensaje SALUDO_PERSONAJE");
	}

	//Recibo OK del orquestador

	bytes_recibidos= Lee_Socket(descriptor, (void*) respuesta, MAXLEN);

			if(bytes_recibidos == 0){
				puts("El orquestador cerró la conexión");
			}
			if(bytes_recibidos == -1)
				puts("Falló la respuesta del orquestador");
			else {
				puts("La respuesta del orquestador es la siguiente: ");
				puts(respuesta);}

	//Creo estructura Personajenivel para enviársela al Orquestador en P_TO_O_PROX_NIVEL
	pers_nivel.nombrePersonaje=personaje->nombre;
	//void *list_get(t_list *, int index);
	//nivel= list_get(personaje->nivelesRestantes, 1);
	//por ahora harcodeo
	pers_nivel.proxNivel="Nivel1";

	mensaje.header_mensaje=7;
	//mensaje.mensaje = pers_nivel;
	mensaje.payLoadLength = sizeof(mensaje.mensaje);

	bytes_enviados= Escribe_Socket(descriptor, &mensaje, sizeof(mensaje));

	if(bytes_enviados == -1){
		puts("Hubo un error al enviar el mensaje P_TO_O_PROX_NIVEL");
	}

	//Recibo ip/puerto del nivel 0_TO_P_UBIC_NIVEL

	bytes_recibidos= Lee_Socket(descriptor, (void*) respuesta, MAXLEN);

				if(bytes_recibidos == 0){
					puts("El orquestador cerró la conexión");
				}
				if(bytes_recibidos == -1)
					puts("Falló la respuesta del orquestador");
				else {
					puts("La respuesta del orquestador es la siguiente: ");
					puts(respuesta);}

close(descriptor);

   return 0;
}
