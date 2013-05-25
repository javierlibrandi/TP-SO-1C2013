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
	char mensaje[1000], respuesta[2000];
	int bytes_enviados, bytes_recibidos;

	puts("Elija el nombre para su personaje:");
	printf( "\nHas elegido: \"%s\"\n", gets(nombre_per) );

	nuevoPersonaje(nombre_per, personaje);

	descriptor=Abre_Conexion_Inet(IP, PUERTO);

	if(descriptor==-1){
		puts("Hubo un error al conectarse al servidor");

	}else{
		puts("Conexión exitosa");
	}

	//Mantengo la conexión con el servidor
	while(1){
		printf("Ingrese mensaje a enviar al servidor:");
		scanf("%s", mensaje);


		bytes_enviados= Escribe_Socket(descriptor, &mensaje, strlen(mensaje));

		if(bytes_enviados == -1){
					puts("Hubo un error al enviar el mensaje");
				}

		/* Enviar sin usar biblioteca socket.h

		bytes_enviados= send(descriptor, mensaje, strlen(mensaje),0);

		if(bytes_enviados == strlen(mensaje)){
			puts("Se transmitió el mensaje entero con éxito");
		}
		if (bytes_enviados < 0){
			puts("Falló el envío");
		}else if(bytes_enviados < strlen(mensaje))
				puts("Se transmitió el mensaje parcialmente");

		Enviar sin usar biblioteca socket.h*/

		bytes_recibidos= Lee_Socket(descriptor, &respuesta, MAXLEN);

		if(bytes_recibidos == 0){
			puts("El servidor cerró la conexión");
		}
		if(bytes_recibidos == -1)
			puts("Falló la respuesta");
		else {
			puts("La respuesta del servidor es la siguiente: ");
			puts(respuesta);}

		/* Recibir sin usar librería socket.h

		bytes_recibidos= recv(descriptor, respuesta, MAXLEN,0);

			if(bytes_recibidos == 0){
				puts("El servidor cerró la conexión");
				}
			if(bytes_recibidos < 0)
					puts("Falló la respuesta");
			else{
				puts("La respuesta del servidor es la siguiente: ");
				puts(respuesta);}

		Recibir sin usar biblioteca socket.h*/
	}

close(descriptor);

   return 0;
}
