#ifndef AMBIENTE_CONFIG_H_
#define AMBIENTE_CONFIG_H_
#define PATH_CONFIG "resources/properties.cfg"
#define PATH_CONFIG_NIVEL "/home/utnso/git/tp-20131c-gaturro/nivel/resources/properties.cfg"
#include <commons/collections/list.h>
#include "entorno.h"

#define  val_pos_recurso(rows, cols,x,y) (((x<=rows && y<= cols) && (x>=1 && y>=1) && (x!=2 || y!=2)) ? 1 : 0)

typedef struct h_t_param_plat {
	char** planificador_nivel;
	int PUERTO;
	int SEGUNDOS_ESPERA;
	int CUANTUM;
} t_param_plat;

/**
 * Leo la configuracion del planificador
 */
typedef struct t_param_plan {
	int PUERTO;
} t_param_plan;

typedef struct h_t_recusos {
	char NOMBRE[20];
	char SIMBOLO;
	char cantidad; //considero que no tengo mas 255 de un recurso
	char posX;
	char posY;
	char **RECURSOS; //lo usa el personaje no el nivel OJO!!!
	struct h_t_recusos  *ref_recuso; //referencia al reucuso que usa el personaje para devolver los recursos lo uso en add_recurso_personaje y liberar_recursos
} t_recusos;

typedef struct h_t_param_nivel {
	char *IP;
	int PUERTO_PLATAFORMA;
	int PUERTO;
	char *nom_nivel;
	t_list *recusos;
} t_param_nivel;

// para el personaje

typedef struct {
	char *NOMBRE;
	char SMBOLO;
	char **PLAN_NIVELES;
	t_list *RECURSOS;
	int VIDAS;
	char *IP;
	int PUERTO_PLATAFORMA;
} t_param_persoje;

typedef struct {
	pthread_t planificador_thr; //descriptor del hilo
	char *desc_nivel; //nombre del nivel al que reprecenta el hilo
	int *sock; //socket por el cual se comunica, es creado por la plataforma y pasado al planificador
	fd_set *readfds; // puntero al fd_set para agregarlo en el select() dentro del hilo
	char ip[16];
	char puerto[6];
	t_list *lista_planificadores; //tengo una referencia a los planificadores para sacarlo de la lista si es necesario
	t_list *l_listos;
	t_list *l_bloquedos;
	t_list *l_errores;
	pthread_mutex_t *s_lista_plani; //esta no se para que la hice????
	pthread_mutex_t *s_listos; //personajes listos para planificar
	pthread_mutex_t *s_bloquedos; //personajes bloqueados
	pthread_mutex_t *s_errores; //personajes con errores
	int segundos_espera;
	int *cuantum;
	int sck_planificador; //guardo el socked del planificador para poder diferencialo de los personajes
} t_h_planificador;

t_param_plat leer_archivo_plataforma_config();

struct h_t_param_nivel leer_nivel_config(int rows, int cols);

t_param_persoje leer_personaje_config();

void lock_listas_plantaforma(t_h_planificador *h_planificador);

void un_lock_listas_plataforma(t_h_planificador *h_planificador);

int mover_personaje_lista(int sck,t_list *origen, t_list *destino);

#endif /* AMBIENTE_CONFIG_H_ */
