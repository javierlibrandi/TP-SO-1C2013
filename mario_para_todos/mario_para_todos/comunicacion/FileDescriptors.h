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

#define	P_TO_P_SALUDO  1// personaje saluda a la plataforma "nombre_personaje;Simbolo;nivelNro"
#define	P_TO_O_PROX_NIVEL 2 //personaje solicita ip/puerto de su proximo nivel al orquestador "nombrePersonaje;nivelNro" Ej. "Mario;nivel2"
#define	O_TO_P_UBIC_NIVEL 3//plataforma le brinda datos sobre nivel "ipNivel;puertoNivel"
#define P_TO_N_INICIAR_NIVEL 4 //personaje se conecta a nivel para que lo ubique en mapa "nombrePersonaje;simbolo" Ej. "Mario;#"
#define	P_TO_PL_INICIAR_NIVEL 5 //personaje se conecta a plataforma y solicita jugar en un determinado nivel "nombrePersonaje, nivelNro" Ej. "Mario;nivel2"
#define	P_TO_N_UBIC_RECURSO 6 //personaje le solcita a nivel la ubicación de un recurso "recurso" Ej. 'F'
#define	N_TO_P_UBIC_RECURSO 7 //nivel le da las coordenadas del recurso solicitado "posX, posY" Ej. "14,21"
#define	PL_TO_P_TURNO 8
#define	P_TO_PL_TURNO_CUMPLIDO 9
#define	P_TO_N_SOLIC_RECURSO 10 //personaje solicita instancia de un recurso al nivel "simbolo;recurso" Ej. "mario;F"
#define	P_TO_PL_BLOQUEO 11 //el personaje se bloquea pq no tiene el recurso
#define	P_TO_N_MUERTE 12
#define	P_TO_N_OBJ_CUMPLIDO 13
#define P_TO_PL_OBJ_CUMPLIDO 14
#define	P_TO_N_REINICIAR_NIVEL 15 //personaje avisa a nivel para q reiniciar el nivel "simbolo"
#define	P_TO_O_REINICIAR_JUEGO 16 //personaje avisa a planif/orquestador para q reiniciar el plan de niveles "simbolo"
#define P_TO_PL_JUEGO_GANADO 17
#define N_TO_O_SALUDO 18
#define P_TO_N_MOVIMIENTO 19 //Personaje solicita moverse al nivel "simboloPersonaje;(xActual, yActual);(xNuevo, yNuevo)" Ej. "Mario;(5,30);(6,30)"
#define PL_TO_P_MUERTE 20
#define P_TO_N_SALIR 21
#define N_TO_P_RECURSO_OK 22
#define N_TO_P_RECURSO_ERROR 23
#define N_TO_P_MOVIDO 24
#define N_TO_O_PERSONAJE_TERMINO_NIVEL 25 // el nivel tiene que mandar la cantidad y el tipo de recusos liberados por el personaje que termino. EJ: "cantTiposRecurso;recurso1;cantidad;recurso2;cantidad;recurso3:cantidad"
#define O_TO_N_DESBLOQUEO_PERSONAJE 26 // INFORMAR LA CANTIDAD DE RECURSOS ASIGNADOS--???
#define N_TO_O_RECOVERY 27
#define O_TO_N_MUERTE 28 // INFORMAR QUE PERSONAJE MURIO. EJ: "MARIO"
#define P_TO_PL_SALIR 29
#define O_TO_N_ASIGNAR_RECURSOS 30
#define N_TO_P_PROXIMO_RECURSO 31
#define P_TO_PL_RECURSO_CONSEGUIDO 32
#define PL_TO_P_MATAR_KOOPA 33
#define O_TO_N_ASIGNAR_RECURSOS_null 34
#define N_TO_O_DEADLOCK 35
#define	P_TO_O_REINICIAR_NIVEL 36
#define N_TO_O_PERS_REINICIO 37
#define N_TO_O_PERS_SALIR 38
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
