/**
 * Javier Abell�n, 20 Junio 2000
 *
 * Funciones de lectura y escritura de la librer�a de sockets.
 *
 * MODIFICACIONES:
 * 4 de Septiembre de 2003. A�adidas funciones Lee_Socket_Udp(), 
 *    Escribe_Socket_Udp() y Dame_Direccion_Udp()
 */
#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/socket.h>

/** Lee Datos de tama�o Longitud de un socket cuyo descriptor es fd.
 * Devuelve el numero de bytes leidos o -1 si ha habido error */
int Lee_Socket (int fd, void *Datos, int Longitud);

/** Envia Datos de tama�o Longitud por el socket cuyo descriptor es fd.
 * Devuelve el n�mero de bytes escritos o -1 si ha habido error. */
int Escribe_Socket (int fd, void *Datos, int Longitud);

/**
 * Leo primero la cabecera y luego el mensaje.
 * Devolviendo solo el mensaje en el puntero a void buffer
 * quien lo llama debe castiar el puntero al tipo de dato esperado
 */
void *recv_variable(const int socketReceptor, int *tipo);

void fd_mensaje(const int socket, const int header_mensaje, const char *msj,
		int *env);

void elimino_sck_lista(int sck, fd_set *readfds);
#endif
