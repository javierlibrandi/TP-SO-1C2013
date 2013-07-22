#ifndef AMBIENTE_CONFIG_H_
#define AMBIENTE_CONFIG_H_

#include <commons/collections/list.h>
#include "entorno.h"
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

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
	char cantidad;/*considero que no tengo mas 255 de un recurso*/
	char posX;
	char posY;
	char **RECURSOS; //lo usa el personaje no el nivel OJO!!!
	struct h_t_recusos  *ref_recuso; //referencia al reucuso que usa el personaje para devolver los recursos lo uso en add_recurso_personaje y liberar_recursos
	char recursos_disponibles; //campo auximilar para el interbloqueo
} t_recusos;

typedef struct h_t_param_nivel {
	char *IP;
	int PUERTO_PLATAFORMA;
	int PUERTO;
	char *nom_nivel;
	t_list *recusos;
	unsigned long int TiempoChequeoDeadlock;
	int Recovery;
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
	int *sock; //socket por el cual se comunica, es creado por la plataforma y pasado al planificador /// Es el socket mas alto para saber hasta donde recorre el select.
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
	int sck_planificador; //guardo el socked del planificador para poder diferencialo de los personajes //Es el que se usa para comunicarse con el nivel.
	bool error_nivel;	//Es una bandera para que el el planificador sepa si tiene que matar el hilo. Se pone en false cuando se crea el planificador y la cambio el orquestador si hubo error con algun nivel.
} t_h_planificador;

typedef struct{
	t_list *prj_listo;
	t_list *prj_bloquedo;
}t_estados;

typedef struct {
	int *sock;
	//int sock_nivel; //esta chanchada es pq nunca conecte el nivel al orquetador y es un lio y no se me ocurre como hacerlo bien :S
	fd_set *readfds;
	t_estados *lista_estados;
	t_list *planificadores;
	t_list *l_listos;
	t_list *l_bloquedos;
	t_list *l_errores;
	t_list *l_nuevos;
	pthread_mutex_t *s_lista_plani;
	pthread_mutex_t *s_listos;
	pthread_mutex_t *s_bloquedos;
	pthread_mutex_t *s_errores;

} t_h_orquestadro;




t_param_plat leer_archivo_plataforma_config();

struct h_t_param_nivel leer_nivel_config(int rows, int cols);

t_param_persoje leer_personaje_config();

void lock_listas_plantaforma(t_h_planificador *h_planificador);

void un_lock_listas_plataforma(t_h_planificador *h_planificador);

int mover_personaje_lista(int sck,t_list *origen, t_list *destino);

void lock_listas_plantaforma_orq(t_h_orquestadro *h_orq);

void un_lock_listas_plataforma_orq(t_h_orquestadro *h_orq);

#endif /* AMBIENTE_CONFIG_H_ */
