/*
 * FileDescriptors.h
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */

//Se definen los tipos de header para los mensajes
typedef enum {
	SALUDO_PERSONAJE,
	PUERTO_PLANIFICADOR,
	PUETO_NIVEL
} t_header_mensaje;


/**
 * el cambio en los tipos lo saco del ejemplo
 * https://github.com/sisoputnfrba/so-test-sockets/blob/master/recv_variable/recv_variable.c
 */
typedef struct t_header{
	t_header_mensaje  t_header_mensaje; //catiar ante de enviar a int8_t
	int16_t payLoadLength;
} t_header;

typedef struct {
	char *payLoad;
} t_msj;
