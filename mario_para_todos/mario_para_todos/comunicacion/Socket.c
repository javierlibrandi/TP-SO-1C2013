/*
 * Javier Abellan, 20 Jun 2000
 *
 * Funciones de lectura y escritura en sockets
 *
 * MODIFICACIONES:
 * 4 Septiembre 2003: A�adidas funciones para sockets UDP.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "Socket.h"
#include "FileDescriptors.h"
#include <stdlib.h>
#include <string.h>

/*
 * Lee datos del socket. Supone que se le pasa un buffer con hueco
 *	suficiente para los datos. Devuelve el numero de bytes leidos o
 * 0 si se cierra fichero o -1 si hay error.
 */
int Lee_Socket(int fd, void *Datos, int Longitud) {
	int Leido = 0;

	/*
	 * Comprobacion de que los parametros de entrada son correctos
	 */
	if ((fd == -1) || (Datos == NULL )|| (Longitud < 1)){
	fprintf(stderr,"error en validacion (fd == -1) %d || (Datos == NULL ) || (Longitud < 1) %d \n",fd,Longitud);
		return -1;
	}

	if ((Leido = recv(fd, Datos, Longitud, MSG_WAITALL)) == -1) {
		/*
		 * En caso de error, la variable errno nos indica el tipo
		 * de error.
		 * El error EINTR se produce si ha habido alguna
		 * interrupcion del sistema antes de leer ningun dato. No
		 * es un error realmente.
		 * El error EGAIN significa que el socket no esta disponible
		 * de momento, que lo intentemos dentro de un rato.
		 * Ambos errores se tratan con una espera de 100 microsegundos
		 * y se vuelve a intentar.
		 * El resto de los posibles errores provocan que salgamos de
		 * la funcion con error.
		 */
		switch (errno) {
		case EINTR:
		case EAGAIN:
			usleep(100);
			if (Lee_Socket(fd, Datos, Longitud) == -1) {
				break;
			}
			break;
		default:
			return -1;
		}
	}

	/*
	 * Se devuelve el total de los caracteres leidos
	 */
	return Leido;
}

/*
 * Escribe dato en el socket cliente. Devuelve numero de bytes escritos,
 * o -1 si hay error.
 */
int Escribe_Socket(int fd, void *Datos, int Longitud) {
	int Escrito = 0;

	/*
	 * Comprobacion de los parametros de entrada
	 */
	if ((fd == -1) || (Datos == NULL )|| (Longitud < 1))return -1;

	/*
	 * Bucle hasta que hayamos escrito todos los caracteres que nos han
	 * indicado.
	 */

	Escrito = send(fd, Datos, Longitud, 0);

	/*
	 * Devolvemos el total de caracteres leidos
	 */
	return Escrito;
}

void *recv_variable(const int socketReceptor, int *tipo) {

	t_header header;
	void *buffer;

// Primero: Recibir el header para saber cuando ocupa el payload.
	if (Lee_Socket(socketReceptor, &header, sizeof(header)) == -1) {
		perror("error al Lee_Socket recibe  header");
		exit(-1);
	}

	*tipo = (int) header.header_mensaje;
// Segundo: Alocar memoria suficiente para el payload.
	buffer = malloc(header.payLoadLength);

// Tercero: Recibir el payload.

	if (Lee_Socket(socketReceptor, buffer, header.payLoadLength) == -1) {
		perror("error al Lee_Socket receptor");
		exit(-1);
	}

	return buffer;
}

//informa si acepto o rechazo la conexion
void fd_mensaje(const int socket, const int header_mensaje, const char *msj,
		int *env) {
	t_send t_send;

	memset(t_send.mensaje, '\0', max_len);
	strcpy(t_send.mensaje, msj);
	t_send.header_mensaje = header_mensaje;
	t_send.payLoadLength = strlen(t_send.mensaje) + 1;

	*env = Escribe_Socket(socket, &t_send, sizeof(t_header) + t_send.payLoadLength);

}
