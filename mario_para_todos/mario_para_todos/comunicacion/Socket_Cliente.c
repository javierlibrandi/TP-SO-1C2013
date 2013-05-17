/* Javier Abell�n, 20 Junio 2000
 *
 * Funciones para abrir/establecer sockets de un cliente con un servidor.
 *
 * MODIFICACIONES:
 * 4 Septiembre 2003. A�adida funci�n Abre_Conexion_Udp()
 */

/*
 * Includes del sistema
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



/*
 * Conecta con un servidor remoto a traves de socket INET
 */
int Abre_Conexion_Inet(char *Host_Servidor, int puerto) {
	struct sockaddr_in Direccion;
	struct hostent *Host;
	int Descriptor;

	Host = gethostbyname(Host_Servidor);
	if (Host == NULL )
		return -1;

	Direccion.sin_family = AF_INET;
	Direccion.sin_addr.s_addr = ((struct in_addr *) (Host->h_addr))->s_addr;
	Direccion.sin_port = htons(puerto);

	Descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (Descriptor == -1)
		return -1;

	if (connect(Descriptor, (struct sockaddr *) &Direccion, sizeof(Direccion))
			== -1) {
		return -1;
	}

	return Descriptor;
}

