/*
 * nivel.c
 *
 *  Created on: 08/05/2013
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>

int main() {
	t_send send_t;

	int sck;
	memset(send_t.mensaje, '\0', 20 * sizeof(char));

	strcpy(send_t.mensaje, "Nivel1");
	send_t.header_mensaje = SALUDO_NIVEL;

	send_t.payLoadLength = sizeof(send_t.mensaje);

	//memcpy(send_t.mensaje, persona.nombre, strlen(persona.nombre));

	sck = Abre_Conexion_Inet("127.0.0.1", 5000);

	Escribe_Socket(sck, &send_t, sizeof(t_send));

	memset(send_t.mensaje, '\0', 20 * sizeof(char));
	strcpy(send_t.mensaje, "saraza");
	send_t.header_mensaje = SALUDO_NIVEL;

	send_t.payLoadLength = sizeof(send_t.mensaje);

	Escribe_Socket(sck, &send_t, sizeof(t_send));

	memset(send_t.mensaje, '\0', 20 * sizeof(char));

	strcpy(send_t.mensaje, "Nivel2");
	send_t.header_mensaje = SALUDO_NIVEL;

	send_t.payLoadLength = sizeof(send_t.mensaje);

	//memcpy(send_t.mensaje, persona.nombre, strlen(persona.nombre));


	sck = Abre_Conexion_Inet("127.0.0.1", 5000);

	Escribe_Socket(sck, &send_t, sizeof(t_send));

	memset(send_t.mensaje, '\0', 20 * sizeof(char));
	strcpy(send_t.mensaje, "ppwpwpwpw");
	send_t.header_mensaje = SALUDO_NIVEL;

	send_t.payLoadLength = sizeof(send_t.mensaje);

	Escribe_Socket(sck, &send_t, sizeof(t_send));
sleep(1200);
	return EXIT_SUCCESS;
}
