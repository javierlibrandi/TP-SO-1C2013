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

int main(void){
	
	char nombre_per[10];
	Personaje *personaje=NULL;
	int descriptor;
	InfoProxNivel infoNivel;
	Posicion posicionRecurso, posActual;

	//puts("Elija el nombre para su personaje:");
	//printf( "\nHas elegido: \"%s\"\n", gets(nombre_per) );

	nombre_per="Jaz";
	personaje = nuevoPersonaje(nombre_per);
	
	descriptor = conectarOrquestador(personaje);
	
	//infoNivel = consultarProximoNivel(descriptor, personaje);

	//harcodeo ip y puerto del nivel para probar esta función
	infoNivel.nombre_nivel="nivel2";
	infoNivel.ip_nivel="localhost";
	infoNivel.puerto_nivel=5002;

	iniciarNivel(personaje, infoNivel); 
	
	posicionRecurso = solicitarUbicacionRecurso(personaje);

	//harcodeo por ahora
	posActual.x=0;
	posActual.y=0;
	llegarARecurso(posicionRecurso, posActual);

   return EXIT_SUCCESS;
}
