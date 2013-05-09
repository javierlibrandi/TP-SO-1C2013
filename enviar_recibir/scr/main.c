#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> //para funcione de cadena como strcpy
#include <unistd.h> //para el close
#include "FileDescriptors.h"
#include "Socket_Cliente.h"
#include "Socket_Servidor.h"
#include "Socket.h"

typedef struct {
	char nombre[20];
	int edad;
} persona_t;

void send_thr(void);
void reciv_thr(void);

///PROBANDO EL GIT!!
int main(void) {

	pthread_t send_thr_t, reciv_thr_t;

	pthread_create(&send_thr_t, NULL, (void *) send_thr, NULL );
	pthread_create(&reciv_thr_t, NULL, (void *) reciv_thr, NULL );

	pthread_join(send_thr_t, NULL );
	pthread_join(reciv_thr_t, NULL );

	return EXIT_SUCCESS;
}

void send_thr() {
	persona_t persona;
	struct t_send send_t;
	int sck;
	memset(send_t.mensaje, '\0', 20);

	sleep(2); //me aseguro que el servidor esta levantado

	strcpy(persona.nombre, "tomys");

	send_t.header_mensaje = PUERTO_PLANIFICADOR;

	send_t.payLoadLength = strlen(persona.nombre);

	memcpy(send_t.mensaje, persona.nombre, strlen(persona.nombre));

	sck = Abre_Conexion_Inet("127.0.0.1", 5000);

	Escribe_Socket(sck, &send_t, sizeof(struct t_send));

}
void reciv_thr() {

	int sck_server, new_fd;
		char* msj;

	sck_server = Abre_Socket_Inet(5000);
	new_fd = Acepta_Conexion_Cliente(sck_server);
	printf("el largo de mesaje es de %d\n",recv_variable(new_fd, &msj));
	printf("Mi nombre es %s\n", (char*) msj);
	exit(0);
}
