#ifndef AMBIENTE_CONFIG_H_
#define AMBIENTE_CONFIG_H_
#define PATH_CONFIG "resources/properties.cfg"
#define PATH_CONFIG_NIVEL "/home/utnso/git/tp-20131c-gaturro/nivel/resources/properties.cfg"
#include <commons/collections/list.h>

#define  val_pos_recurso(rows, cols,x,y) (((x<=rows && y<= cols) && (x>=1 && y>=1) && (x!=1 || y!=1)) ? 1 : 0)


typedef struct t_param_plat {
	char** planificador_nivel;
	int PUERTO;
} t_param_plat;

/**
 * Leo la configuracion del planificador
 */
typedef struct t_param_plan {
	int PUERTO;
} t_param_plan;

typedef struct {
	char NOMBRE[20];
	char SIMBOLO;
	char cantidad; //considero que no tengo mas 255 de un recurso
	char posX;
	char posY;
} t_recusos;

typedef struct {
	char *IP;
	int PUERTO_PLATAFORMA;
	int PUERTO;
	char *nom_nivel;
	t_list *recusos;
} t_param_nivel;

// para el personaje

typedef struct {
	char *nivel;
	char **recurso;
} t_recursos;


typedef struct {
	char *NOMBRE;
	char SMBOLO;
	char **PLAN_NIVELES;
	t_list *RECURSOS;
} t_param_persoje;


t_param_plat leer_archivo_plataforma_config();


t_param_nivel leer_nivel_config(int rows, int cols);

t_param_persoje leer_personaje_config();

#endif /* AMBIENTE_CONFIG_H_ */
