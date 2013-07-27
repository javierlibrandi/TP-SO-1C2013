#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include "memoria.h"
#include <commons/string.h>

t_list* list_particiones;
int tamanioTotal;

t_memoria crear_memoria(int tamanio) {
	t_memoria segmento;
	t_particion particion;
	void* particionAux;

	tamanioTotal = tamanio;

	// Crea el segmento de memoria a particionar

	printf("Voy a crear el segmento de memoria\n");
	segmento = malloc(tamanio);
	printf("Cree el segmento de memoria\n");

	// Creo la lista de particiones

	printf("Voy a crear la lista de particiones\n");
	list_particiones = list_create();
	printf("Cree la lista de particiones\n");

	//Agrego la primer partición

	printf("Inicializo la partición\n");
	particion.id = ' ';
	particion.inicio = 0;
	particion.tamanio = tamanio;
	particion.libre = true;
	particion.dato = segmento;

	particionAux = &particion;

	printf("Agrego la partición inicial\n");

	list_add(list_particiones, particionAux);

	printf("Agregué la partición inicial\n");

	printf("El segmento tiene ahora %d partciones\n",
			list_particiones->elements_count);
	particiones(segmento);

	return segmento;
}

int almacenar_particion(t_memoria segmento, char id, int tamanio,
		char* contenido) {
	/* Crea una particion dentro del segmento de memoria de tamaño,
	 identificador y contenido especificado. Devuelve el valor numerico -1
	 en caso de error (ej: tamaño de la particion mayor que el tamaño total
	 del segmento, id duplicado, etc.), 1 en caso de exito y 0 en caso de
	 no encontrar una particion libre lo suficientemente grande para
	 almacenar la solicitud. */

	int i, indexParticionMayor;
	void* particionAux, particionMayor, particion, particionRestante,
			ptrMemoria;

	//Inicializo particionMayor con un tamaño de 0 para después saber si encontré otra mayor, o ninguna disponible
	((t_particion*) particionMayor)->tamanio = 0;

	printf("\n");
	printf("\n");
	printf("Inicia el almacenamiento de la partción\n");

	if (tamanio > tamanioTotal) {
		printf(
				"El tamaño de la partición supera el tamaño del segmento. Abortando...\n");
		return 0;
	} else {
		for (i = 0; i < (list_particiones->elements_count); i++) {
			particionAux = list_get(list_particiones, i);
			if (((t_particion*) particionAux)->id == id) {
				printf("Ya existe una partición con ID = %c. Abortando...\n",
						id);
				return -1;
			} else {
				if (((t_particion*) particionAux)->libre == false) {
					printf("La partición %d con ID %c está ocupada\n", i,
							((t_particion*) particionAux)->id);
				} else {
					if (tamanio > ((t_particion*) particionAux)->tamanio) {
						printf(
								"La partición %d con ID %c no es lo suficientemente grande (%d a almacenar vs. %d disponible)\n",
								i, ((t_particion*) particionAux)->id, tamanio,
								((t_particion*) particionAux)->tamanio);
					} else {
						if (((t_particion*) particionAux)->tamanio
								> ((t_particion*) particionMayor)->tamanio) {
							particionMayor = &particionAux;
							indexParticionMayor = i;
						}
					}
				}
			}
		}
		if (((t_particion*) particionMayor)->tamanio == 0) {
			printf(
					"O no había ninguna partición libre, o niguna tenía suficiente espacio para almacenar la nueva partición. Abortando...\n");
			return 0;
		} else {
			//Saco la partición anterior

			//Inicializo la particion
			printf("Voy a inicializar la partición para agregar a la lista\n");
			((t_particion*) particion)->id = id;
			((t_particion*) particion)->inicio =
					((t_particion*) particionMayor)->inicio;
			((t_particion*) particion)->tamanio = tamanio;
			((t_particion*) particion)->dato =
					((t_particion*) particionMayor)->dato;
			((t_particion*) particion)->libre = false;
			printf("Inicialicé la partición para agregar a la lista\n");
			//Agrego la partición a la lista
			//TODO: ver esto
			list_add_in_index(list_particiones, indexParticionMayor, particion);
			printf("Agregué la partición a la lista\n");
			//Grabo el dato en memoria la particion
			printf("Grabo el dato en memoria la particion\n");
			&ptrMemoria = segmento + (((t_particion*) particion)->dato);
			*ptrMemoria = contenido;

			//Inicializo la particion restante
			printf("Voy a inicializar la partición para agregar a la lista\n");
			((t_particion*) particionRestante)->id = ' ';
			((t_particion*) particionRestante)->inicio =
					((t_particion*) particion)->inicio
							+ ((t_particion*) particion)->tamanio - 1;
			((t_particion*) particionRestante)->tamanio =
					((t_particion*) particionMayor)->tamanio
							- ((t_particion*) particion)->tamanio;
			((t_particion*) particionRestante)->dato =
					((t_particion*) particionMayor)->dato
							+ ((t_particion*) particion)->tamanio - 1;
			((t_particion*) particionRestante)->libre = true;
			printf("Inicialicé la partición para agregar a la lista\n");
			//Agrego la partición restante a la lista
			list_add_in_index(list_particiones, indexParticionMayor + 1,
					particionRestante);
			printf("Agregué la partición a la lista\n");
			//Grabo el dato en memoria la particion restante
			printf("Grabo el dato en memoria la particion restante\n");
			&ptrMemoria = segmento + (((t_particion*) particion)->dato);
			*ptrMemoria = contenido;

			//Imprimo las particiones para chequear
			printf("/n");
			printf("Las particiones quedaron así:");
			printf("/n");
			printf("/n");
			particiones(segmento);
			return 1;
		}
		return 0;
	}
}

int eliminar_particion(t_memoria segmento, char id) {
	/* Esta funcion elimina la particion dentro del segmento de memoria
	 correspondiente al identificador enviado como parametro. Devuelve el
	 valor numérico 1 en caso de exito y 0 en caso de no encontrar una
	 particion con dicho identificador */

	int i, index;
	void* ptrAEliminar;

	if (list_particiones->elements_count == 1) {
		ptrAEliminar = list_get(list_particiones, 0);
		if ((((t_particion*) ptrAEliminar)->id) == id) {
			list_remove(list_particiones, 0);
			printf("Se eliminó la partición %c de la posición 0", id);
			return 1;
		} else {
			return 0;
		}
	} else {
		index = -1;
		for (i = 0; i < (list_particiones->elements_count); i++) {
			ptrAEliminar = list_get(list_particiones, i);
			if ((((t_particion*) ptrAEliminar)->id) == id)
				index = i;
		}
		if (index == -1) {
			printf("No se encontró la partición %c", id);
			return 0;
		} else {
			list_remove(list_particiones, index);
			printf("Se eliminó la partición %c de la posición %d", id, index);
			return 1;
		}
	}
	particiones(segmento);
	return 0;
}

void liberar_memoria(t_memoria segmento) {
	// Esta funcion libera los recursos tomados en crear_memoria()
	printf("Libero la memoria del segmento de memoria");
	free(segmento);
	printf("Libero la memoria de la lista de particiones");
	libero_memoria(list_particiones);
	printf("Liberé toda la memoria");
	//list_destroy_and_destroy_elements(list_particiones, element_destroyer);
	return;
}

t_list* particiones(t_memoria segmento) {
	/* Esta funcion devuelve una lista en el formato t_list de las
	 commonslibrary con la siguiente descripcion por cada particion */
	void* particionAux;
	int i;

	printf("\n");
	printf("Segmento de memoria:\n");
	printf("---------------------------------------\n");

	//TODO: ordenar particiones
	for (i = 0; i < (list_particiones->elements_count); i++) {
		particionAux = list_get(list_particiones, i);
		if (((t_particion*) particionAux)->id == ' ') {
			printf("VACIO[%d:%d:%d]\n", ((t_particion*) particionAux)->inicio,
					(((t_particion*) particionAux)->inicio
							+ ((t_particion*) particionAux)->tamanio - 1),
					((t_particion*) particionAux)->tamanio);
		} else {
			printf("%c[%d:%d:%d]:%s\n", ((t_particion*) particionAux)->id,
					((t_particion*) particionAux)->inicio,
					(((t_particion*) particionAux)->inicio
							+ ((t_particion*) particionAux)->tamanio - 1),
					((t_particion*) particionAux)->tamanio,
					((t_particion*) particionAux)->dato);
		}
	}

	printf("---------------------------------------\n");
	printf("\n");
	return list_particiones;
}

void libero_memoria(t_list* list_particiones) {
	int index;
	index = 0;

	void _list_elements(t_list* particion) {
		free(((t_particion*) particion)->dato);
		free(particion);
		index++;
	}
	list_iterate(list_particiones, (void*) _list_elements);
}
