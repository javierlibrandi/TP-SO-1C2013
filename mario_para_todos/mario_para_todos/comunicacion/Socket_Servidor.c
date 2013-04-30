/*
 * Javier Abellan, 20 Jun 2000
 *
 * Funciones para la apertura de un socket servidor y la conexion con sus
 * clientes
 *
 * MODIFICACIONES:
 * 4 Septiembre 2003: A�adida funci�n Abre_Socket_Udp()
 */

/* Includes del sistema */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "Socket_Servidor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

/*
 * Se le pasa un socket de servidor y acepta en el una conexion de cliente.
 * devuelve el descriptor del socket del cliente o -1 si hay problemas.
 * Esta funcion vale para socket AF_INET o AF_UNIX.
 */
int Acepta_Conexion_Cliente(int sck_server) {
	socklen_t Longitud_Cliente;
	struct sockaddr Cliente;
	int new_fd; // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr; // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección delcliente
	int sin_size;
	struct sigaction sa;

	int Hijo;

	/*
	 * La llamada a la funcion accept requiere que el parametro
	 * Longitud_Cliente contenga inicialmente el tamano de la
	 * estructura Cliente que se le pase. A la vuelta de la
	 * funcion, esta variable contiene la longitud de la informacion
	 * util devuelta en Cliente
	 */
	sin_size = sizeof(struct sockaddr_in);
	new_fd = accept(sck_server, (struct sockaddr *) &their_addr, &sin_size);
	return new_fd;

}

/*
 * Abre un socket servidor de tipo AF_INET. Devuelve el descriptor
 *	del socket o -1 si hay probleamas
 * Se pasa como parametro el nombre del servicio. Debe estar dado
 * de alta en el fichero /etc/services
 */
int Abre_Socket_Inet(char *puerto) {
	int sockfd; // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr; // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección delcliente
	int sin_size;
	struct sigaction sa;
	int yes = 1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = AF_INET; // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(atoi(puerto)); // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	/*
	 * Se avisa al sistema que comience a atender llamadas de clientes
	 */
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		return -1;
	}

	/*
	 * Se devuelve el descriptor del socket servidor
	 */
	return sockfd;
}

