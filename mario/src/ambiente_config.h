#define PATH_PLATAFORMA_CONFIG "resources/config_orquestador.cfg"



typedef struct {
	char* IP;
	char* PUERTO;
} t_param;

t_param leer_archivo_orquestador_config();
