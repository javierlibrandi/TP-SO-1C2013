/*
 * Javier Abell�n. 14 Abril 2003
 *
 * Funciones para que un cliente pueda abrir sockets con un servidor.
 */
#ifndef _SOCKET_CLIENTE_H
#define _SOCKET_CLIENTE_H

#include <sys/socket.h>

/**
 * Abre un socket UNIX con un servidor que est� en la misma m�quina y que atienda al
 * servicio de nombre Servicio. 
 */
int Abre_Conexion_Unix (char *Servicio);

/**
 * Abre un socket INET con un servidor que est� corriendo en Host_Servidor y que atienda
 * al servicio cuyo nombre es Servicio. 
 * Host_Servidor debe estar dado de alta en /etc/hosts.
 * Servicio debe estar dado de alta en /etc/services como tcp.
 */
int Abre_Conexion_Inet (char *Host_Servidor, int Servicio);

#endif
