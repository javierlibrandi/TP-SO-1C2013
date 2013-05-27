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

	puts("Elija el nombre para su personaje:");
	printf( "\nHas elegido: \"%s\"\n", gets(nombre_per) );

	//personaje = nuevoPersonaje(nombre_per);
	
	descriptor = conectarOrquestador(personaje);
	
	infoNivel = consultarProximoNivel(descriptor, personaje);

	iniciarNivel(personaje, infoNivel); 
	

   return EXIT_SUCCESS;
}
