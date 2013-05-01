#ifndef AMBIENTE_CONFIG_H_
#define AMBIENTE_CONFIG_H_
#define PATH_CONFIG "resources/config_orquestador.cfg"


typedef struct t_param_orq{
	char* IP;
	int PUERTO;
}t_param_orq;

typedef struct t_param_plat{
	char** planificador_nivel;
} t_param_plat;

/**
 * Leo la configuracion del planificador
 */
typedef struct t_param_plan{
	int PUERTO;
}t_param_plan;


struct t_param_orq leer_archivo_orquestador_config();

t_param_plat leer_archivo_plataforma_config();

t_param_plan leer_archivo_plan_config(char *nivel);

#endif /* AMBIENTE_CONFIG_H_ */
