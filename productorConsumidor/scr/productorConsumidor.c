/*
 * productorConsumidor.c
 *
 *  Created on: 20/04/2013
 *      Author: utnso
 */

/**
 * aca explica como se configuira el eclipse para usar hilos
 * http://blog.asteriosk.gr/2009/02/15/adding-pthread-to-eclipse-for-using-posix-threads/
 */

#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

#define BUFFER_SIZE 100

typedef struct {
	unsigned int cont;
	char productod_thr[20];
} t_nodo;

void* productor_thr(void* p);
void* consumidor_thr();
static t_nodo* create_nodo_thr(unsigned int cont, char *productod_thr);

/**
 * declaro variables globales
 */
unsigned int nodo = 0;
t_nodo* nodo_thr = NULL;
t_queue* queue;

int buffer[BUFFER_SIZE]; // puntero para el identificador de los semáforo
sem_t *sem_cont, *sem_free;

int main(void) {
	int i;
	pthread_t tabla_thr[200];
	queue = queue_create();
	int res;

	printf("Creando semáforos .....\n");
	/* semáforo del contador de productos sino lo crea inicializado(0)*/
	if ((sem_cont = sem_open("/sem_cont", O_CREAT, 0644, 0)) == (sem_t *) -1)
		perror("Error creando semáforo 1");
	/* semáforo del espacio libre y sino lo crea inicializado (BUFFER_SIZE)*/
	if ((sem_free = sem_open("/sem_free", O_CREAT, 0644, BUFFER_SIZE))
			== (sem_t *) -1)
		perror("Error creando semáforo 2");

	// creamos los threads consumidores
	printf("Creando threads consumidores...\n");
	pthread_create(&tabla_thr[2], NULL, (void *) consumidor_thr, NULL );

	// creamos los threads productor
	printf("Creando threads productores...\n");
	pthread_create(&tabla_thr[0], NULL, (void*) productor_thr,
			(void*) "Productor 1");
	pthread_create(&tabla_thr[1], NULL, (void*) productor_thr,
			(void*) "Productor 2");

	for (i = 0; i < 2; i++) {
		pthread_join(tabla_thr[i], NULL );
		printf("El thread %d devolvio el valor %d\n", i, res);
	}

	/* libero semáforos */
	sem_close(sem_cont);
	sem_close(sem_free);
	return EXIT_SUCCESS;

}

/**
 * creo el nodo
 */
static t_nodo* create_nodo_thr(unsigned int cont, char *productod_thr) {
	t_nodo *new = malloc(sizeof(t_nodo));
	new->cont = cont;
	strcpy(new->productod_thr, productod_thr);
	return new;
}

/**
 * funcion hilo productor
 */
void* productor_thr(void* p) {
	int val;
	int entrada;
	char* hilo_productor = (char*) p;

	for (;;) {
		nodo_thr = create_nodo_thr(nodo++, hilo_productor);
		printf("incremento del contador a %d por el hilo %s \n", nodo_thr->cont,
				nodo_thr->productod_thr);

		/**
		 * zona critica
		 */

		sem_wait(sem_free); /* espera que haya espacio en el buffer y decrementa */
		queue_push(queue, nodo_thr);
		sem_post(sem_cont); /* incrementa el contador del semáforo */

		sem_getvalue(sem_cont, &val); // valor del contador del semáforo
		printf("Soy el Productor: entrada=%d, Estado %d productos\n", entrada,
				val);

		sleep(10);
	}

	pthread_exit(EXIT_SUCCESS);
}

void* consumidor_thr() {

	for (;;) {
		/**
		 * zona critica
		 */
		sem_wait(sem_cont); /* espera datos en el buffer (contador>0) y dec.*/
		t_nodo *aux = (t_nodo*) queue_pop(queue);
		sem_post(sem_free); /* incrementa el contador de espacio */

		int sem_wait(sem_t *sem);
		if (aux != NULL ) {
			printf("El nodo consumido es el %d constuido por el hilo %s \n",
					aux->cont, aux->productod_thr);
		} else {
			printf("Se trata de sacar un elemento de un lista vacia \n");
			exit(1);
		}
		sleep(1);
	}
	pthread_exit(EXIT_SUCCESS);
}
