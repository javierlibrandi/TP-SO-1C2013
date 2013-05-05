/**
 * Javier Abell�n. 14 Abril 2003
 *
 * Funciones para que un servidor puede abrir sockets para atender un servicio y aceptar
 * conexiones de un cliente.
 */
#ifndef _SOCKET_SERVIDOR_H
#define _SOCKET_SERVIDOR_H
#define BACKLOG 10 // Cuántas conexiones pendientes se mantienen en cola

/**
 * Abre un socket INET para atender al servicio cuyo nombre es Servicio.
 * El Servicio debe estar dado de alta en /etc/services como tcp.
 * Devuelve el descriptor del socket que atiende a ese servicio o -1 si ha habido error.
 */
int Abre_Socket_Inet (int puerto);

/**
 * Acepta un cliente para un socket INET.
 * Devuelve el descriptor de la conexi�n con el cliente o -1 si ha habido error.
 */
int Acepta_Conexion_Cliente (int sck_server);
/**
 * Leo primero la cabecera y luego el mensaje.
 * Devolviendo solo el mensaje en el puntero a void buffer
 * quien lo llama debe castiar el puntero al tipo de dato esperado
 */
int recv_variable(int socketReceptor, void* buffer);

#endif
