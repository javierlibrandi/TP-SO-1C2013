#define PATH_CONFIG "resources/config_orquestador.cfg"



typedef struct {
	char* IP;
	char* PUERTO;
} t_param_orq;

typedef struct {
	char** planificador_nivel;

} t_param_plat;

t_param_orq leer_archivo_orquestador_config();

t_param_plat leer_archivo_plataforma_config();
