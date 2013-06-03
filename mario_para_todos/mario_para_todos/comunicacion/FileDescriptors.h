/*
 * FileDescriptors.h
 *
 *  Created on: Apr 27, 2013
 *      Author: utnso
 */



#ifndef FILEDESCRIPTORS_H_
#define FILEDESCRIPTORS_H_

#include <netinet/in.h>
#define max_len 1024

#define	P_TO_P_SALUDO  1// personaje saluda a la plataforma
#define	P_TO_O_PROX_NIVEL 16 //personaje solicita ip/puerto de su proximo nivel al orquestador "nombrePersonaje;nivelNro"
#define	O_TO_P_UBIC_NIVEL 2//plataforma le brinda datos sobre nivel "ipNivel;puertoNivel"
#define P_TO_N_INICIAR_NIVEL 3 //personaje se conecta a nivel para que lo ubique en mapa "nombrePersonaje;simbolo"
#define	P_TO_PL_INICIAR_NIVEL 4 //personaje se conecta a plataforma y solicita jugar en un determinado nivel "nombrePersonaje, nivelNro"
#define	P_TO_N_UBIC_RECURSO 5
#define	N_TO_P_UBIC_RECURSO 6
#define	PL_TO_P_TURNO 7
#define	P_TO_PL_TURNO_CUMPLIDO 8
#define	P_TO_N_SOLIC_RECURSO 9
#define	P_TO_N_BLOQUEO 10
#define	P_TO_N_MUERTE 11
#define	P_TO_N_OBJ_CUMPLIDO 12
#define	P_TO_N_REINICIAR_NIVEL 13
#define	P_TO_O_REINICIAR_JUEGO 14
#define N_TO_O_SALUDO 15
#define OK 100
#define ERROR 101

/**
 * el cambio en los tipos lo saco del ej
 * https://github.com/sisoputnfrba/so-test-sockets/blob/master/recv_variable/recv_variable.c
 */

typedef struct {
	int8_t header_mensaje;
	int16_t payLoadLength;
} t_header;

/**
 * el cambio en los tipos lo saco del ejemplo
 * https://github.com/sisoputnfrba/so-test-sockets/blob/master/recv_variable/recv_variable.c
 */
typedef struct t_send{
	int8_t header_mensaje;
	int16_t payLoadLength;
	char mensaje[max_len];
} t_send;


#endif /* FILEDESCRIPTORS_H_*/
