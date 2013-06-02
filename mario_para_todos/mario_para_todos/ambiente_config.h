#ifndef AMBIENTE_CONFIG_H_
#define AMBIENTE_CONFIG_H_
#define PATH_CONFIG "resources/properties.cfg"
#include <commons/collections/list.h>

#define  val_pos_recurso(x,y) ((x<=80 && y<=80) ? 1 : 0)

//typedef struct t_param_orq{
//	char* IP;
//	int PUERTO;
//}t_param_orq;

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

//struct t_param_orq leer_archivo_orquestador_config();

t_param_plat leer_archivo_plataforma_config();

//t_param_plan leer_archivo_plan_config(char *nivel);

t_param_nivel leer_nivel_config(char *nivel);

#endif /* AMBIENTE_CONFIG_H_ */