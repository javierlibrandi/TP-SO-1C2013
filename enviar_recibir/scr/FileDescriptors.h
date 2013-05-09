/*
 * FileDescriptors.h
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 *///
//
#ifndef FILEDESCRIPTORS_H_
#define FILEDESCRIPTORS_H_

#define SALUDO_PERSONAJE 1
#define PUERTO_PLANIFICADOR 2
#define PUETO_NIVEL 3
#define NOMBRE_PERSONAJE 4
#define EDAD_PERSONAJE 101

/**
 * el cambio en los tipos lo saco del ejemplo
 * https://github.com/sisoputnfrba/so-test-sockets/blob/master/recv_variable/recv_variable.c
 */
typedef struct {
	int8_t header_mensaje;
	int16_t payLoadLength;
} t_header;

typedef struct {
	void *payLoad;
} t_msj;

/**
 * el cambio en los tipos lo saco del ejemplo
 * https://github.com/sisoputnfrba/so-test-sockets/blob/master/recv_variable/recv_variable.c
 */
typedef struct t_send{
	int8_t header_mensaje;
	int16_t payLoadLength;
	int edad;
	char mensaje[20];

} t_send;

#endif /* FILEDESCRIPTORS_H_*/
