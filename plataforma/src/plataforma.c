/*
 ============================================================================
 Name        : mario.c
 Author      : javier
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para funciones de cadena como strcpy
#include <mario_para_todos/ambiente_config.h>
#include "orquestador/orquestador_thr.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include "planificador/planificador_thr.h"
#include  <commons/log.h>
#include <mario_para_todos/comunicacion/Socket_Cliente.h>
#include <mario_para_todos/comunicacion/Socket_Servidor.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <mario_para_todos/grabar.h>
#include "plataforma.h"
#include <mario_para_todos/comunicacion/FileDescriptors.h>
#include <mario_para_todos/comunicacion/Socket.h>
#include <commons/string.h>
#include <mario_para_todos/entorno.h>
#include <semaphore.h>
#include "inotify/inotify_thr.h"
#include <stdbool.h>

void libero_memoria(t_list *list_plataforma);
void creo_hilos_planificador(char *msj, t_list *list_plataforma, int sock,
		char ip_cliente[], t_h_orquestadro *h_orquestador, int segundos_espera,
		int *cuantum);
void escucho_conexiones(t_param_plat param_plataforma, t_list *list_plataforma,
		t_h_orquestadro *h_orquestador, pthread_t *orquestador_thr);
void join_orquestador(t_list *list_plataforma); //pthread_join de los hilos orquestadores
bool existe_nivel(const char *desc_nivel, t_list *list_plataforma);
void creo_personaje_lista(char crear_orquesador, int sock, char *aux_char,
		t_h_orquestadro* h_orquestador, int * existe_personaje);
bool existe_personaje(const char *nombre_personaje, char simbolo,
		t_list *list_personaje);
void agregar_personaje_planificador(int new_sck, t_h_orquestadro *h_orquestador,
		char *msj);
t_h_planificador *optener_nivel(char *desc_nivel, t_list *list_planificadores);
void agregar_sck_personaje(int sck, const char *nom_personaje, t_list *l_listos);

/* Declaración del objeto atributo */
pthread_attr_t attr;

static pthread_mutex_t s_lista_plani = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_listos = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_bloqueados = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_errores = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_nuevos = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_terminados = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_sock_orquestador = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t reads_orquestador = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_deadlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_koopa = PTHREAD_MUTEX_INITIALIZER;

int main(void) {

	t_param_plat param_plataforma;
	pthread_t inotify_pthread;

	pthread_t orquestador_thr;
	/* Inicialización del objeto atributo */
	pthread_attr_init(&attr);

	t_list *list_planificadores = list_create(); //creo lista de hilos

	t_h_orquestadro *h_orquestador = malloc(sizeof(t_h_orquestadro));

	//leo el archivo de configuracion para el hilo orquestador
	param_plataforma = leer_archivo_plataforma_config();

	h_orquestador->readfds = malloc(sizeof(fd_set));
	h_orquestador->sock = malloc(sizeof(int));
	h_orquestador->planificadores = list_planificadores;
	//punteros a semaforos
	h_orquestador->s_lista_plani = &s_lista_plani;
	h_orquestador->s_listos = &s_listos;
	h_orquestador->s_bloquedos = &s_bloqueados;
	h_orquestador->s_errores = &s_errores;
	h_orquestador->s_nuevos = &s_nuevos;
	h_orquestador->s_deadlock = &s_deadlock;

	h_orquestador->s_terminados = &s_terminados;

	h_orquestador->reads_select = &reads_orquestador;
	h_orquestador->s_sock_semaforo = &s_sock_orquestador;
	h_orquestador->s_koopa = &s_koopa;

	//listas
	h_orquestador->l_bloquedos = list_create(); //lista de personajes bloquedos
	h_orquestador->l_listos = list_create(); //lista de personajes listos
	h_orquestador->l_errores = list_create(); //lista de personajes que terminaron con error
	h_orquestador->l_nuevos = list_create(); //lista de personajes nuevos que no estan para lanificar.
	h_orquestador->l_terminados = list_create(); //pongo las visctimas que elige el orquestador
	h_orquestador->l_deadlock = list_create();
	h_orquestador->l_koopa = list_create();
	h_orquestador->readfds = malloc(sizeof(fd_set));
	FD_ZERO(h_orquestador->readfds);
	*(h_orquestador->sock) = 0;
	//creo los hilos para inotify
	pthread_create(&inotify_pthread, &attr, (void*) inotify_thr,
			(void*) &param_plataforma);

	escucho_conexiones(param_plataforma, list_planificadores, h_orquestador,
			&orquestador_thr);

	pthread_join(orquestador_thr, NULL );
	join_orquestador(list_planificadores);

	libero_memoria(list_planificadores);
	list_destroy(list_planificadores);

	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
///					  escucho_conexiones						////
////////////////////////////////////////////////////////////////////

void escucho_conexiones(t_param_plat param_plataforma,
		t_list *list_planificadores, t_h_orquestadro *h_orquestador,
		pthread_t *orquestador_thr) {

	int sck, new_sck;
	char *buffer = NULL;
	int puerto = param_plataforma.PUERTO;
	int tipo;
	sck = Abre_Socket_Inet(puerto);
	char ip_cliente[16];
	int byteEnviados;
	char solo_personaje = 'N';
	int bool_existe_personaje;

	log_in_disk_plat(LOG_LEVEL_TRACE, "escucho conexiones en el puerto %d",
			puerto);

	for (;;) {
		new_sck = Acepta_Conexion_Cliente(sck, ip_cliente);
		log_in_disk_plat(LOG_LEVEL_INFO,
				"Se me conecto el cliente con la ip %s", ip_cliente);

		buffer = recv_variable(new_sck, &tipo);

		switch (tipo) {
		case P_TO_P_SALUDO:
			log_in_disk_plat(LOG_LEVEL_TRACE, "Mensaje tip P_TO_P_SALUDO");
			if (solo_personaje == 'N') {
				//h_orquestador = creo_personaje_lista('N',new_sck, buffer);
				creo_personaje_lista(solo_personaje, new_sck, buffer,
						h_orquestador, &bool_existe_personaje);

				/**
				 * creo el hilo orquetador
				 */
				pthread_create(orquestador_thr, NULL, (void *) orequestador_thr,
						(void*) h_orquestador);
				//TODO CAmbiar para que el orquestador se cree en el Main. Es facil por que ahora la lista del select se inicializa en el main.
				solo_personaje = 'S';

			} else {
				creo_personaje_lista(solo_personaje, new_sck, buffer,
						h_orquestador, &bool_existe_personaje);
			}

			if (bool_existe_personaje == 0) {
				fd_mensaje(new_sck, ERROR,
						"Ya existe un personaje con ese nombre o simbolo",
						&byteEnviados);

			} else {

				pthread_mutex_lock(h_orquestador->reads_select);
				pthread_mutex_lock(h_orquestador->s_sock_semaforo);
				if (new_sck > *(h_orquestador->sock)) {
					*(h_orquestador->sock) = new_sck;
				}
				FD_SET(new_sck, h_orquestador->readfds); //Agrego el socket a la lista del select

				pthread_mutex_unlock(h_orquestador->reads_select);
				pthread_mutex_unlock(h_orquestador->s_sock_semaforo);
				sleep(1); //Darle un poco mas de tiempo
				fd_mensaje(new_sck, OK, "ok, personaje creado", &byteEnviados);

			}

			break;
		case N_TO_O_SALUDO: //creo el planificador del nivel
			log_in_disk_plat(LOG_LEVEL_TRACE, "Mensaje tip N_TO_O_SALUDO");
			if (!existe_nivel(buffer, list_planificadores)) {
				free(buffer);

				fd_mensaje(new_sck, OK, "Planificador creado", &byteEnviados);

				buffer = recv_variable(new_sck, &tipo);
				creo_hilos_planificador(buffer, list_planificadores, new_sck,
						ip_cliente, h_orquestador,
						param_plataforma.SEGUNDOS_ESPERA,
						&param_plataforma.CUANTUM);

				//Agrego el socket del nivel a la lista que escucha el orquestador
				pthread_mutex_lock(h_orquestador->reads_select);
				pthread_mutex_lock(h_orquestador->s_sock_semaforo);

				FD_ZERO(h_orquestador->readfds);
				FD_SET(new_sck, h_orquestador->readfds);

				if (new_sck > *(h_orquestador->sock)) {
					*(h_orquestador->sock) = new_sck;
				}
				pthread_mutex_unlock(h_orquestador->s_sock_semaforo);
				pthread_mutex_unlock(h_orquestador->reads_select);

			} else {
				fd_mensaje(new_sck, ERROR,
						"Ya hay un nivel con ese nombre dado de alta",
						&byteEnviados);
				//Hacer close del socket??
			}

			break;
		case P_TO_PL_INICIAR_NIVEL: //personaje se conecta a plataforma y solicita jugar en un determinado nivel "nombrePersonaje, nivelNro" Ej. "Mario;nivel2"

			log_in_disk_orq(LOG_LEVEL_TRACE,
					"Mensaje tip P_TO_PL_INICIAR_NIVEL");

			log_in_disk_plat(LOG_LEVEL_ERROR,
					"cantidad de listos: %d, cantidad de nuevos: %d",
					list_size(h_orquestador->l_listos),
					list_size(h_orquestador->l_nuevos));

			lock_listas_plantaforma_orq(h_orquestador);
			pthread_mutex_lock(&s_lista_plani);
			agregar_personaje_planificador(new_sck, h_orquestador, buffer);
			pthread_mutex_unlock(&s_lista_plani);
			un_lock_listas_plataforma_orq(h_orquestador);

			log_in_disk_plat(LOG_LEVEL_ERROR,
					"cantidad de listos: %d, cantidad de nuevos: %d",
					list_size(h_orquestador->l_listos),
					list_size(h_orquestador->l_nuevos));

			fd_mensaje(new_sck, OK, "Personaje planificado", &byteEnviados);

			if (byteEnviados == -1)
				log_in_disk_orq(LOG_LEVEL_TRACE,
						"Error al enviar mensaje a pesonaje");

			break;

		default:
			log_in_disk_plat(LOG_LEVEL_ERROR,
					"opcion en el switch no implementada", tipo);
			//exit(1);
		}

		free(buffer);
	}

}

/////////////////////////////////////////////////////////////////////
///					creo_hilos_planificador						////
////////////////////////////////////////////////////////////////////

void creo_hilos_planificador(char *msj, t_list *list_planificadores, int sock,
		char ip_cliente[], t_h_orquestadro *h_orquestador, int segundos_espera,
		int *cuantum) {

	pthread_t planificador_pthread;
	t_h_planificador *h_planificador = malloc(sizeof(t_h_planificador));
	fd_set readfds;
	char **aux_msj = string_split(msj, ";");
	int tot_elemntos;

	h_planificador = malloc(sizeof(t_h_planificador)); //recervo la memoria para almacenar el nuevo hilo

	//punteros para pasar informacion por referencia entre orquestador y planificador
	h_planificador->l_listos = h_orquestador->l_listos;
	h_planificador->l_bloquedos = h_orquestador->l_bloquedos;
	h_planificador->l_errores = h_orquestador->l_errores;
	h_planificador->l_deadlock = h_orquestador->l_deadlock;
	h_planificador->l_koopa = h_orquestador->l_koopa;

	h_planificador->s_koopa = h_orquestador->s_koopa;
	h_planificador->s_listos = h_orquestador->s_listos;
	h_planificador->s_deadlock = h_orquestador->s_deadlock;
	h_planificador->s_bloquedos = h_orquestador->s_bloquedos;
	h_planificador->s_errores = h_orquestador->s_errores;

	h_planificador->segundos_espera = segundos_espera;
	h_planificador->cuantum = cuantum;
	h_planificador->sck_planificador = sock; // guardo el socked del planificador para poder diferencialo de los personajes //En realidad es el socket del nivel.

///////configuro el select() que despues voy a usar en el hilo////////
	FD_ZERO(&readfds);
	// FD_SET(sock, &readfds);   // Lo comento por que ahora al nivel lo escucha el orquestador.
	/*	IMPORTANTE QUE SEA UN PUNTERO ASI LO VEO EN EL SELECT() DEL HILO
	 CUANDO LO MODIFICO EN LA PLATAFORMA.
	 TAMBIEN HAGO UNA COPIA DE MEMORIA PARA NO PERDER EL VALOR.
	 NO OLVIDARCE DE HACER UN FREE EN libero_memoria
	 */
	h_planificador->desc_nivel = malloc(strlen(aux_msj[0]));
	strcpy(h_planificador->desc_nivel, aux_msj[0]); //agrego la des del nivel
	h_planificador->sock = malloc(sizeof(int));
	//memcpy(h_planificador->sock, &sock, sizeof(int));  // Comentado por que ya no esta escuchando el socket del nivel.
	*(h_planificador->sock) = 0; //Arego el cero a la lista para que no quede vacia porque ya no esta el socket del nivel
	h_planificador->readfds = malloc(sizeof(fd_set));
	memcpy(h_planificador->readfds, &readfds, sizeof(fd_set));
	strcpy(h_planificador->ip, ip_cliente);
	strcpy(h_planificador->puerto, aux_msj[1]);
	h_planificador->s_lista_plani = &s_lista_plani;
	free(aux_msj);
///////fin configuro el select() que despues voy a usar en el hilo////////
	log_in_disk_plat(LOG_LEVEL_TRACE, "creo el planificador %s",
			h_planificador->desc_nivel);
//creo los hilos planificador
	pthread_create(&planificador_pthread, &attr, (void*) planificador_nivel_thr,
			(void*) h_planificador);

	h_planificador->planificador_thr = planificador_pthread;

	pthread_mutex_lock(&s_lista_plani);

	tot_elemntos = list_add(list_planificadores, h_planificador);
	h_planificador->lista_planificadores = list_planificadores;

	pthread_mutex_unlock(&s_lista_plani);

	log_in_disk_plat(LOG_LEVEL_INFO,
			"el total planificadores en la lista es de %d", tot_elemntos);

}

/////////////////////////////////////////////////////////////////////
///					      libero_memoria						////
////////////////////////////////////////////////////////////////////

void libero_memoria(t_list *list_plataforma) {
	int index;
	index = 0;

	void _list_elements(t_h_planificador *h_planificador) {
		free(h_planificador->desc_nivel);
		free(h_planificador->readfds);
		free(h_planificador->sock);
		free(h_planificador);
		index++;
	}

	list_iterate(list_plataforma, (void*) _list_elements);

}

/////////////////////////////////////////////////////////////////////
///					      join_orquestador						////
////////////////////////////////////////////////////////////////////

//pthread_join de los hilos orquestadores
void join_orquestador(t_list *list_plataforma) {
	int index = 0;

	void _list_elements(t_h_planificador *h_planificador) {

		pthread_join(h_planificador->planificador_thr, NULL );

		index++;
	}

	list_iterate(list_plataforma, (void*) _list_elements);

}

/////////////////////////////////////////////////////////////////////
///					      existe_nivel							////
////////////////////////////////////////////////////////////////////

bool existe_nivel(const char *desc_nivel, t_list *list_plataforma) {

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"dentro de la funcion existe_nivel nivel: %s \t", desc_nivel);

	if (list_is_empty(list_plataforma) == 1) {
		return false;
	}

	bool _list_elements(t_h_planificador *h_planificador) {

		if (!strcmp(h_planificador->desc_nivel, desc_nivel)) {
			log_in_disk_plat(LOG_LEVEL_WARNING,
					"El nivel  %s  se encuentra en la lista", desc_nivel);
			return true;
		}
		log_in_disk_plat(LOG_LEVEL_WARNING,
				"No se declaro un nivel con el nombre %s", desc_nivel);

		return false;

	}

	return (bool*) list_find(list_plataforma, (void*) _list_elements);

}
/**
 * Si crear_orquesador es N es el primer personaje y tengo que crear el hilo orquestador
 * si el crear_orquesador es S ya el orquestador esta creado
 */

void creo_personaje_lista(char crear_orquesador, int sock, char *aux_char,
		t_h_orquestadro* h_orquestador, int * existe_personaje_var) {

	t_personaje* nuevo_personaje;
	char **mensaje;

	//int byteEnviados;
	bool aux_existe_persosaje_listo;
	bool aux_existe_persojaje_bloquedo;
	bool aux_existe_persojaje_nuevo;
	static unsigned long int sec_personaje = 0;

	/* ESTO NO VA MAS POR QUE AHORA LA LISTA DE SOCKETS SE INICIALIZA AFUERA, EN EL MAIN.
	 * if (crear_orquesador == 'N') {

	 FD_ZERO(h_orquestador->readfds);
	 *(h_orquestador->sock) = 0;

	 } // A Partir de aca es codigo compartido para el caso S y N
	 */

	mensaje = string_split(aux_char, ";");

//	pthread_mutex_lock(h_orquestador->s_listos);
//	pthread_mutex_lock(h_orquestador->s_bloquedos);
//	pthread_mutex_lock(h_orquestador->s_nuevos);
	lock_listas_plantaforma_orq(h_orquestador);

	aux_existe_persosaje_listo = existe_personaje(mensaje[0], mensaje[1][0],
			h_orquestador->l_listos);
	aux_existe_persojaje_bloquedo = existe_personaje(mensaje[0], mensaje[1][0],
			h_orquestador->l_bloquedos);
	aux_existe_persojaje_nuevo = existe_personaje(mensaje[0], mensaje[1][0],
			h_orquestador->l_bloquedos);
	un_lock_listas_plataforma_orq(h_orquestador);
	//TODO Chequear la lista koopa!
//	pthread_mutex_unlock(h_orquestador->s_nuevos);
//	pthread_mutex_unlock(h_orquestador->s_bloquedos);
//	pthread_mutex_unlock(h_orquestador->s_listos);

	if (aux_existe_persosaje_listo || aux_existe_persojaje_bloquedo
			|| aux_existe_persojaje_nuevo) {

		log_in_disk_orq(LOG_LEVEL_TRACE,
				"Ya existe un personaje con este nombre o simbolo. nombre: %s, Simbolo: %c ",
				mensaje[0], mensaje[1][0]);

		*existe_personaje_var = 1;

		return;
	} else {

		//Creo el personaje
		nuevo_personaje = malloc(sizeof(t_personaje));
		nuevo_personaje->simbolo = mensaje[1][0];
		nuevo_personaje->nombre = malloc(strlen(mensaje[0]) + 1);
		strcpy(nuevo_personaje->nombre, mensaje[0]);
		nuevo_personaje->nivel = malloc(strlen(mensaje[2]) + 1);
		strcpy(nuevo_personaje->nivel, mensaje[2]);
		nuevo_personaje->listo_para_planificar = false;
		nuevo_personaje->sec_entrada = sec_personaje++; //creo una secuencia para seber cual es el personaje mas viejo y saber cual matar.
		nuevo_personaje->sck = sock;
		//nuevo_personaje->listo_para_planificar = false; //pongo al personane para que no se planifique hasta que pase los datos del nivel
		pthread_mutex_lock(h_orquestador->s_nuevos);
		list_add(h_orquestador->l_nuevos, nuevo_personaje); //Agrego el nuevo personaje a la cola de nuevos
		imprimir_listas(h_orquestador, 'o');

		pthread_mutex_unlock(h_orquestador->s_nuevos);

		log_in_disk_plat(LOG_LEVEL_TRACE,
				"creo el personaje %s de simbolo: %c y su nro de sec es: %d",
				nuevo_personaje->nombre, nuevo_personaje->simbolo,
				nuevo_personaje->sec_entrada);

		existe_personaje_var = 0;

	}
}

bool existe_personaje(const char *nombre_personaje, char simbolo,
		t_list *list_personaje) {

	int i;
	log_in_disk_orq(LOG_LEVEL_TRACE, "busco el personaje: %s \t",
			nombre_personaje);

	if (list_is_empty(list_personaje) != 0) {
		return false;
	}

	if (busca_personaje_simbolo_pla(simbolo, list_personaje, &i) == NULL ) {
		log_in_disk_orq(LOG_LEVEL_TRACE,
				"El siguiente personaje o simbolo ya existen personaje: %s, Simbolo: %c ",
				nombre_personaje, simbolo);

		return false;

	} else {
		log_in_disk_orq(LOG_LEVEL_TRACE,
				"El personaje :%s, con simbolo: %c sera creado ",
				nombre_personaje, simbolo);

		return true;

	}

}

void agregar_personaje_planificador(int sck, t_h_orquestadro *h_orquestador,
		char *msj) {

	char **aux_mjs = string_split(msj, ";");
	char *nom_personaje = aux_mjs[0];
	char *nom_nivel = aux_mjs[1];
	t_h_planificador *h_planificador = NULL;
//	int indice_aux;
//	Personaje* per_aux;
	log_in_disk_plat(LOG_LEVEL_TRACE,
			"agregar_personaje_planificador personaje: %s \t nivel: %s",
			nom_personaje, nom_nivel);

	h_planificador = optener_nivel(nom_nivel, h_orquestador->planificadores);

	if (h_planificador == NULL ) {
		log_in_disk_plat(LOG_LEVEL_ERROR,
				"Error al optener la estrucutra del planificador");
		exit(EXIT_FAILURE);
	}
	agregar_sck_personaje(sck, nom_personaje, h_orquestador->l_nuevos);
//comentado para que no se escuchen en el select del planificador
//	FD_SET(sck, h_planificador->readfds);
//	if (sck > *(h_planificador->sock)) {
//		*(h_planificador->sock) = sck;
//	}
	log_in_disk_plat(LOG_LEVEL_ERROR,
			"----->Muevo el personaje de NUEVO A LISTO<-----");

	mover_personaje_lista(sck, h_orquestador->l_nuevos,
			h_orquestador->l_listos);

	imprimir_listas(h_orquestador, 'o');

}

t_h_planificador *optener_nivel(char *desc_nivel, t_list *list_planificadores) {

	log_in_disk_orq(LOG_LEVEL_TRACE,
			"finc optener_nivel \t busco el nivel: %s \t", desc_nivel);

	bool _list_elements(t_h_planificador *h_planificador) {

		log_in_disk_orq(LOG_LEVEL_TRACE, "El elemento %s lo comparo con %s",
				h_planificador->desc_nivel, desc_nivel);

		if (!strcmp(h_planificador->desc_nivel, desc_nivel)) {

			log_in_disk_plan(LOG_LEVEL_TRACE, "devuelvo el planificador %s",
					h_planificador->desc_nivel);

			return true;

		} else {
			return false;
		}
	}

	return (t_h_planificador*) list_find(list_planificadores,
			(void*) _list_elements);

}
/**
 * Necesito agregar el socket al personaje para saber como comunicarme para planificarlo
 */
void agregar_sck_personaje(int sck, const char *nom_personaje, t_list *l_listos) {

	log_in_disk_orq(LOG_LEVEL_TRACE,
			"agregar_sck_personaje busco el personaje: %s \t", nom_personaje);

	bool _list_elements(t_personaje *personaje) {

		log_in_disk_orq(LOG_LEVEL_TRACE, "El elemento %s lo comparo con %s",
				personaje->nombre, nom_personaje);

		if (!strcmp(personaje->nombre, nom_personaje)) {

			log_in_disk_plan(LOG_LEVEL_TRACE,
					"Agrego el descriptor del socket al nivel");
			personaje->sck = sck;

			return true;

		} else {
			log_in_disk_plan(LOG_LEVEL_TRACE,
					"No econtre al personaje para agregar el socket");
			return false;
		}
	}

	list_find(l_listos, (void*) _list_elements);

}

t_personaje *busca_personaje_simbolo_pla(char id, t_list *l_personajes,
		int *indice_personaje) {
	int count;
	int total_personajes = list_size(l_personajes);
	t_personaje *per;

	log_in_disk_niv(LOG_LEVEL_INFO, "busca_personaje_simbolo: %c", id);

	for (count = 0; count < total_personajes; count++) {
		per = list_get(l_personajes, count);

		if (per->simbolo == id) {

			log_in_disk_niv(LOG_LEVEL_INFO, "Retorno el personaje %s",
					per->nombre);

			*indice_personaje = count;
			return per;
		}
	}

	return NULL ;
}

t_personaje *busca_personaje_skc(int sck, t_list *l_listo,
		int *indice_personaje) {
	int count;
	int total_personajes = list_size(l_listo);
	t_personaje *per;
	log_in_disk_plat(LOG_LEVEL_INFO, "busca_personaje_skc");
	for (count = 0; count < total_personajes; count++) {
		per = list_get(l_listo, count);
		if (per->sck == sck) {

			log_in_disk_plat(LOG_LEVEL_INFO, "Retorno el personaje %s",
					per->nombre);

			*indice_personaje = count;
			return per;
		}
	}
	*indice_personaje = -1;
	return NULL ;
}

