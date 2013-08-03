/*
 * ambiente_config.c

 *
 *  Created on: 18/04/2013
 *      Author: javier
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "ambiente_config.h"
#include "grabar.h"
#include <commons/string.h>
#include <commons/collections/list.h>
#include <string.h>
#include <pthread.h>
#include "entorno.h"
#include <unistd.h>
#include "comunicacion/Socket_Cliente.h"
#include "comunicacion/Socket.h"
static void impimir_lista(t_list *lista);

t_param_plat leer_archivo_plataforma_config() {
	t_config* config;
	t_param_plat param;

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion plataforma en el %s ",
			PATH_CONFIG_PLATAFORMA);

	config = config_create(PATH_CONFIG_PLATAFORMA);

	param.PUERTO = config_get_int_value(config, "PUERTO");

	param.SEGUNDOS_ESPERA = config_get_double_value(config, "SEGUNDOS_ESPERA");

	param.CUANTUM = config_get_int_value(config, "CUANTUM");

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"Parametros planificador puerto %d segundos de planificacions %.2f cuantum %d ",
			param.PUERTO, param.SEGUNDOS_ESPERA, param.CUANTUM);

	//param.planificador_nivel = config_get_array_value(config, "planDeNiveles");

	//solo entro en el loop si el mivel de log es LOG_LEVEL_TRACE
//	if (LOG_LEVEL_TRACE == 0) {
//		int i = 0;
//		while (param.planificador_nivel[i] != '\0') {
//			log_in_disk_plat(LOG_LEVEL_TRACE, "Niveles de planificador %s ",
//					param.planificador_nivel[i]);
//			i++;
//		}
//	}

	return param;

}

t_param_plan leer_archivo_plan_config(char *nivel) {
	t_config* config;
	t_param_plan param;

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion panificador nivel en el path %s ",
			PATH_CONFIG_PLATAFORMA);

	config = config_create(PATH_CONFIG_PLATAFORMA);

	param.PUERTO = config_get_int_value(config, nivel);

	log_in_disk_plat(LOG_LEVEL_TRACE,
			"el nivel  %s  tiene asignado el puerto %d", nivel, param.PUERTO);

	return param;
}

struct h_t_param_nivel leer_nivel_config(int rows, int cols) {
	t_config* config;
	struct h_t_param_nivel param;
	t_recusos *list_recursos; //declaro el tipo nodo
	char *aux_str;
	int i, j;
	char **aux;
	char *aux_char;

	log_in_disk_niv(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion del nivel en el %s ",
			PATH_CONFIG_NIVEL);

	log_in_disk_niv(LOG_LEVEL_TRACE, "limites de pantalla x = %d ,y = %d", rows,
			cols);
	param.recusos = list_create(); //creo lista de hilos
	config = config_create(PATH_CONFIG_NIVEL);

	//param.nom_nivel = nivel; //copio el nombre del nivel que pase por parametro en el main

	aux_char = config_get_string_value(config, "PLATAFORMA");

	aux = string_split(aux_char, ":");
	param.IP = aux[0];
	param.PUERTO_PLATAFORMA = atoi(aux[1]);

	param.PUERTO = config_get_int_value(config, "PUERTO");
	param.nom_nivel = config_get_string_value(config, "NOMBRE");

	i = 1;

	aux_str = string_from_format("%s%d", "CAJA", i);

	while ((aux = config_get_array_value(config, aux_str)) != NULL ) {

		list_recursos = malloc(sizeof(t_recusos)); //creo el nodo
		j = 0;

		strcpy(list_recursos->NOMBRE, aux[j++]);
		if (!strcmp(list_recursos->NOMBRE, "Fin")) {
			break; //salgo del for cuando encuentro la caja de fin
		}
		list_recursos->SIMBOLO = aux[j++][0];
		list_recursos->cantidad = atoi(aux[j++]);
		list_recursos->posX = atoi(aux[j++]);
		list_recursos->posY = atoi(aux[j]);

		if (val_pos_recurso(rows, cols, list_recursos->posX,
				list_recursos->posY)) {

			log_in_disk_niv(LOG_LEVEL_TRACE,
					"Cargo a la lista la caja %s que contiene el recurso %s con el simbolo %c y la candidad %d en la poscion [x,y] [%d][%d] ",
					aux_str, list_recursos->NOMBRE, list_recursos->SIMBOLO,
					list_recursos->cantidad, list_recursos->posX,
					list_recursos->posY);

			list_add(param.recusos, list_recursos); //agrego el nuevo nodo

		} else {
			log_in_disk_niv(LOG_LEVEL_ERROR,
					"Cargo a la lista la caja %s que contiene el recurso %s con el simbolo %c y la candidad %d en la poscion [x,y] [%d][%d] ES INVELIDA!!",
					aux_str, list_recursos->NOMBRE, list_recursos->SIMBOLO,
					list_recursos->cantidad, list_recursos->posX,
					list_recursos->posY);

		}

		aux_str = string_from_format("%s%d", "CAJA", ++i);
	}

	param.TiempoChequeoDeadlock = config_get_int_value(config,
			"TiempoChequeoDeadlock");
	param.Recovery = config_get_int_value(config, "RECOVERY");

	return param;

}

int leer_archivo_plataforma_config_cuantum(){
	t_config* config = config_create(PATH_CONFIG_PLATAFORMA);
	return config_get_int_value(config, "CUANTUM");

}
t_param_persoje leer_personaje_config() {
	t_config* config;
	t_param_persoje param;
	t_recusos *list_recursos; //declaro el tipo nodo
	char *aux_char;
	char *aux_str;
	char **aux;
	int i;

	log_in_disk_per(LOG_LEVEL_TRACE,
			"comienzo lectura archivo de configuracion del personaje en el %s ",
			PATH_CONFIG_PERSONAJE);

	param.RECURSOS = list_create(); //creo lista de recursos
	config = config_create(PATH_CONFIG_PERSONAJE);

	//param.nom_nivel = nivel; //copio el nombre del nivel que pase por parametro en el main
	param.NOMBRE = config_get_string_value(config, "NOMBRE");

	aux_char = config_get_string_value(config, "SIMBOLO");

	param.SMBOLO = aux_char[0];

	param.PLAN_NIVELES = config_get_array_value(config, "PLANDENIVELES");

	param.VIDAS = config_get_int_value(config, "VIDAS");

	aux_char = config_get_string_value(config, "PLATAFORMA");

	aux = string_split(aux_char, ":");
	param.IP = aux[0];
	param.PUERTO_PLATAFORMA = atoi(aux[1]);

	i = 0;
	while (param.PLAN_NIVELES[i] != '\0') {

		list_recursos = malloc(sizeof(t_recusos));
		//sprintf(aux_str, "OBJ[%s]", param.PLAN_NIVELES[i]);
		aux_str = string_from_format("OBJ[%s]", param.PLAN_NIVELES[i]);

		log_in_disk_per(LOG_LEVEL_TRACE, "Niveles de planificador %s ",
				param.PLAN_NIVELES[i]);

		list_recursos->RECURSOS = config_get_array_value(config, aux_str);
		strcpy(list_recursos->NOMBRE, param.PLAN_NIVELES[i]);
		list_add(param.RECURSOS, list_recursos); //agrego el nuevo nodo

		log_in_disk_per(LOG_LEVEL_TRACE,
				"Niveles de planificador %s con los recursos %s",
				param.PLAN_NIVELES[i],
				config_get_string_value(config, aux_str));
		i++;
	}

	return param;

}

void lock_listas_plantaforma(t_h_planificador *h_planificador) {
	pthread_mutex_lock(h_planificador->s_listos);
	pthread_mutex_lock(h_planificador->s_bloquedos);
	pthread_mutex_lock(h_planificador->s_errores);
	pthread_mutex_lock(h_planificador->s_koopa);

}

void un_lock_listas_plataforma(t_h_planificador *h_planificador) {
	pthread_mutex_unlock(h_planificador->s_listos);
	pthread_mutex_unlock(h_planificador->s_bloquedos);
	pthread_mutex_unlock(h_planificador->s_errores);
	pthread_mutex_unlock(h_planificador->s_koopa);
}

void lock_listas_plantaforma_orq(t_h_orquestadro *h_orq) {
	pthread_mutex_lock(h_orq->s_listos);
	pthread_mutex_lock(h_orq->s_bloquedos);
	pthread_mutex_lock(h_orq->s_errores);
	pthread_mutex_lock(h_orq->s_koopa);
	pthread_mutex_lock(h_orq->s_nuevos);
	pthread_mutex_lock(h_orq->s_deadlock);

}

void un_lock_listas_plataforma_orq(t_h_orquestadro *h_orq) {
	pthread_mutex_unlock(h_orq->s_listos);
	pthread_mutex_unlock(h_orq->s_bloquedos);
	pthread_mutex_unlock(h_orq->s_errores);
	pthread_mutex_unlock(h_orq->s_koopa);
	pthread_mutex_unlock(h_orq->s_nuevos);
	pthread_mutex_unlock(h_orq->s_deadlock);
}

/**
 * @Muevo un personaje de una lista a otra
 * de la llista, lo busco por socket
 *
 */
int mover_personaje_lista(int sck, t_list *origen, t_list *destino) {
	int i, cant_elementos;
	cant_elementos = list_size(origen);
	t_personaje *perso;

	log_in_disk_plat(LOG_LEVEL_INFO, "mover_personaje_lista");

	for (i = 0; i < cant_elementos; i++) {
		perso = (t_personaje*) list_get(origen, i);

		if (perso->sck == sck) {
			list_add(destino, list_remove(origen, i));
			return 1;
		}
	}
	return 0;
}

void imprimir_listas(void *estruc, char tipo_estruc) {
	t_h_planificador *h_struc_p = NULL;
	t_h_orquestadro *h_struc_o = NULL;

	switch (tipo_estruc) {
	case 'o':
	case 'O':
		log_in_disk_plat(LOG_LEVEL_INFO,
				">>>>>>Impimo la lista de personajes ORQUESTADOR<<<<<<<<<<<<");
		h_struc_o = (t_h_orquestadro*) estruc;
		log_in_disk_plat(LOG_LEVEL_INFO, "LISTOS");
		impimir_lista(h_struc_o->l_listos);
		log_in_disk_plat(LOG_LEVEL_INFO, "BLOQUEADOS");
		impimir_lista(h_struc_o->l_bloquedos);
		log_in_disk_plat(LOG_LEVEL_INFO, "ERRORES");
		impimir_lista(h_struc_o->l_errores);
		log_in_disk_plat(LOG_LEVEL_INFO, "NUEVOS");
		impimir_lista(h_struc_o->l_nuevos);
		log_in_disk_plat(LOG_LEVEL_INFO, "EPERANDO POR KOOPA");
		impimir_lista(h_struc_o->l_koopa);

		break;
	case 'p':
	case 'P':
		log_in_disk_plat(LOG_LEVEL_INFO,
				">>>>>>Impimo la lista de personajes PANIFICADOR<<<<<<<<<<<<");
		h_struc_p = (t_h_planificador*) estruc;
		log_in_disk_plat(LOG_LEVEL_INFO, "LISTOS");
		impimir_lista(h_struc_p->l_listos);
		log_in_disk_plat(LOG_LEVEL_INFO, "BLOQUEADOS");
		impimir_lista(h_struc_p->l_bloquedos);
		log_in_disk_plat(LOG_LEVEL_INFO, "ERRORES");
		impimir_lista(h_struc_p->l_errores);
		log_in_disk_plat(LOG_LEVEL_INFO, "NUEVOS");
		log_in_disk_plat(LOG_LEVEL_INFO, "EPERANDO POR KOOPA");
		impimir_lista(h_struc_p->l_koopa);
		break;
	}

}

static void impimir_lista(t_list *lista) {
	int count;
	int total_personajes;
	t_personaje *per;

	if (list_size(lista) > 0) {
		total_personajes = list_size(lista);
		for (count = 0; count < total_personajes; count++) {

			per = list_get(lista, count);

			log_in_disk_plat(LOG_LEVEL_INFO, "Nombre personaje %s nivel %s",
					per->nombre, per->nivel);

		}
	}

}

int val_pos_recurso(int rows, int cols, int x, int y) {
	return (((y <= rows && x <= cols) && (x >= 1 && y >= 1)
			&& (x != 2 || y != 2)) ? 1 : 0);
}




void tabla_a_koopa(t_h_planificador *h_planificador) {

	int pasadas = 0;
	int sck;
	//Si nadie espera por Koopa salgo
	pthread_mutex_lock(h_planificador->s_koopa);
	if (list_is_empty(h_planificador->l_koopa)) {
		pthread_mutex_unlock(h_planificador->s_koopa);
		return;
	}

	pthread_mutex_unlock(h_planificador->s_koopa);

	while (VALIDAR_KOOPA != pasadas) {
		log_in_disk_plan(LOG_LEVEL_TRACE, "Se va a ejecutar Koopa.");

		//TODO PONEMOS LA LISTA DE NUEVOS?????
		//pthread_mutex_lock(h_planificador->s_listos);
		pthread_mutex_lock(h_planificador->s_bloquedos);

		//si las lista tiene personajes SALGO
		if (!list_is_empty(h_planificador->l_listos)
				|| !list_is_empty(h_planificador->l_bloquedos)) {
			pthread_mutex_unlock(h_planificador->s_bloquedos);
			return;
		}

		//pthread_mutex_unlock(h_planificador->s_listos);
		pthread_mutex_unlock(h_planificador->s_bloquedos);
		sleep(ESPERA_POR_KOOPA); //Epero un tiempo por koopa si las listas vacias HAY TABLA PARA KOOPA

		pasadas++;
	}
//	sck = Abre_Conexion_Inet("localhost", 5000);
//		int byteRecibidos;
//
//		fd_mensaje(sck,5000," ",&byteRecibidos);


		//buffer = recv_variable(sck, &tipo);

execlp(PATH_KOOPA, PATH_KOOPA, FILE_KOOPA, (const char *) NULL );


		exit(EXIT_SUCCESS);

}
void koopita_querido(){
	execlp(PATH_KOOPA, PATH_KOOPA, FILE_KOOPA, (const char *) NULL );
	exit(EXIT_SUCCESS);
}

/**
 * muevo los personajes que coinciden con el nivel a la lista de errores
 * parametros
 * 1)descripcion del nivel
 * 2)lista que quiero recorrer
 * 3)lista de errores
 */
void mover_personaje_errores_por_nivel(char *desc_nivel, t_list *lista_auxiliar,
		t_list *l_errores) {

	int tot, i;
	t_personaje *per;

	tot = list_size(lista_auxiliar);

	for (i = 0; i < tot; i++) {
		per = list_get(lista_auxiliar, i);

		if (!strcmp(per->nivel, desc_nivel)) {
			mover_personaje_lista(per->sck, lista_auxiliar, l_errores);
		}
	}

}

