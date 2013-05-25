/*
 * funciones_personaje.c
 *
 *  Created on: 25/05/2013
 *      Author: utnso
 */

#include "funciones_personaje.h"
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/entorno.h>
#include <mario_para_todos/ambiente_config.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket.h>
#define PUERTO_ORQ 5000
#define MAXLEN 2000

//Por ahora logean en consola. Implementar logs.

//Por ahora se inicializa el personaje harcodeado. Falta hacer funcionar la lectura del arch de configuración y el uso de listas.
void nuevoPersonaje(char* nombrePersonaje, Personaje* personaje)
{
printf("Voy a crear a %s", nombrePersonaje);
//puts("Voy a crear al personaje que esté en el path de resources xD");

/*t_param_pers archConf;

archConf=leer_archivo_personaje_config();

personaje->nombre = archConf.nombre;
personaje->simbolo = archConf.simbolo;
personaje->vidas = archConf.vidas;
personaje->ipOrquestador = archconf.ipOrquestador;
//Niveles y objetivos???
*/

//Prueba de logs
//log_in_disk_pers(LOG_LEVEL_TRACE, "Harcodeando valores en personaje %s");

personaje->nombre = nombrePersonaje;
personaje->simbolo = '@';
personaje->vidas = 3;
personaje->ip_orquestador = "192.168.0.100:5000";
//personaje->nivelesRestantes= null;


printf("EL personaje creado es %s", personaje->nombre);


};

//Ver como crear la estructura InfoProxNivel en esta función con la respuesta del orquestador.
t_msj consultarProximoNivel(Personaje* personaje){
	int descriptor;
	int bytes_enviados, bytes_enviados2, bytes_recibidos;
	t_header header;
	t_msj mensaje, respuesta;
	//int proxNivel;

	//Se fija en su lista de plan de niveles, cuál es el próximo nivel a completar
	//void *list_get(t_list *, int index);
	//Por ahora harcodeo
	//proxNivel=2;

	//Se conecta al orquestador. Por ahora defino PUERTO_ORQ. Ver como separar ip de puerto
	descriptor=Abre_Conexion_Inet(personaje->ip_orquestador, PUERTO_ORQ);
	if(descriptor==-1){
			puts("Hubo un error al conectarse al orquestador");
		}else{
			puts("Conexión exitosa");}

	//Crear mensaje de solicitud al orquestador. Fatan definir estructuras de los distintos tipo de mensajes, por ahora un string.
	//mensaje.payLoad=proxNivel;
	mensaje.payLoad="Necesito ubicación del Nivel/PLanificador 2";

	//Crear header de mensaje
	header.payLoadLength= sizeof(mensaje.payLoad);
	// Tipo 0 :P_TO_O_PROX_NIVEL
	header.t_header_mensaje=0;

	//Envía header y mensaje del tipo <P_TO_O_PROX_NIVEL> a orquestador
	bytes_enviados= Escribe_Socket(descriptor, (void*)&header, sizeof(header));
	bytes_enviados2= Escribe_Socket(descriptor, (void*)&mensaje, sizeof(mensaje));

	if(bytes_enviados == -1 || bytes_enviados2 == -1 ){
						puts("Hubo un error al enviar la solicitud de ubicación del proximo nivel");
					}

	//Recibe respuesta del tipo <O_TO_P_UBIC_NIVEL> del orquestador. Del tipo InfoProNivel
	bytes_recibidos= Lee_Socket(descriptor, &respuesta, MAXLEN);

			if(bytes_recibidos == 0)
				puts("El orquestador cerró la conexión");
			if(bytes_recibidos == -1)
				puts("Falló la respuesta");
			else
				return respuesta;
};

//Con la respuesta de ConsultarProximoNivel llenar la estructura InfoProxnivel
void iniciarNivel(Personaje* personaje, InfoProxNivel* infoNivel){
	int descriptorNiv, descriptorPlan;
	int bytes_enviados_niv, bytes_enviados_pl;
	//t_header P_TO_N_INICIAR_NIVEL, P_TO_PL_INICIAR_NIVEL;

	//Por ahora envío un string. Falta definir estructura con header y payLoad.
	char* P_TO_N_INICIAR_NIVEL, P_TO_PL_INICIAR_NIVEL;
	P_TO_N_INICIAR_NIVEL="Hola Nivel. Quiero jugar en tu mapa.";
	P_TO_N_INICIAR_NIVEL="Hola Planificador. Quiero jugar en tu nivel.";

	descriptorNiv=Abre_Conexion_Inet(infoNivel->ip_nivel, infoNivel->puerto_nivel);

	if(descriptorNiv==-1){
		puts("Hubo un error al conectarse al Nivel");
	}else{
		puts("Conexión exitosa con Nivel");}

	descriptorPlan=Abre_Conexion_Inet(infoNivel->ip_planif, infoNivel->puerto_planif);

	if(descriptorPlan==-1){
		puts("Hubo un error al conectarse al planificador del nivel");
	}else{
		puts("Conexión exitosa con planificador del nivel");}

	//Por ahora hago un solo send con el mensaje completo. Después definir si hay que hacer dos sends por cada
	// conexión. Uno por header y otro por payLoad.
	bytes_enviados_niv= Escribe_Socket(descriptorNiv, P_TO_N_INICIAR_NIVEL, sizeof(P_TO_N_INICIAR_NIVEL));
	bytes_enviados_pl= Escribe_Socket(descriptorPlan, P_TO_PL_INICIAR_NIVEL, sizeof(P_TO_PL_INICIAR_NIVEL));

	if(bytes_enviados_niv == -1 || bytes_enviados_pl == -1 ){
		puts("No se avisó al nivel o al planificador del arribo.");
						}
};



