#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include "memoria.h"
#include <commons/string.h>

t_list* list_particiones;
int tamanioTotal;

t_memoria crear_memoria(int tamanio) {
	t_particion* particion;
	t_memoria segmento[tamanio];

	tamanioTotal = tamanio;

	// Crea el segmento de memoria a particionar

	printf("Voy a crear el segmento de memoria\n");
	memset(segmento, '\0', tamanio);
	printf("Cree el segmento de memoria\n");

	// Creo la lista de particiones

	printf("Voy a crear la lista de particiones\n");
	list_particiones = list_create();
	printf("Cree la lista de particiones\n");

	//Agrego la primer partición

	particion = malloc(sizeof(t_particion));

	printf("Inicializo la partición\n");
	particion->id = ' ';
	particion->inicio = 0;
	particion->tamanio = tamanio;
	particion->libre = true;
	particion->dato = segmento[particion->inicio];
	printf("La partición actual es la %c\n", particion->id);
	printf("La partición actual comienza en %d\n", particion->inicio);
	printf("La partición actual es de %d\n", particion->tamanio);
	printf("La partición acutal está libre? %d\n", particion->libre);

	printf("Agrego la partición inicial\n");

	list_add(list_particiones, particion);

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

	int i, indexParticionMayor, total;
	unsigned int ptrMemoria, ptrFinal; //ptr a la direccion de memoria
	t_particion* particionAux;
	t_particion particionMayor;
	t_particion* particion;
	t_particion* particionRestante;

	//particionAux = malloc(sizeof(t_particion)); //no hace falta recive un valor ya allocado en la lista
	particion = malloc(sizeof(t_particion));
	particionRestante = malloc(sizeof(t_particion));

	//Inicializo particionMayor con un tamaño de 0 para después saber si encontré otra mayor, o ninguna disponible
	particionMayor.tamanio = 0;

	printf("\n");
	printf("\n");
	printf("Inicia el almacenamiento de la partición\n");

	if (tamanio > tamanioTotal) {
		printf(
				"El tamaño de la partición supera el tamaño del segmento. Abortando...\n");
		return 0;
	}

	total = list_size(list_particiones);

	printf("Me guardé el tamaño total de mi lista, que es %d\n", total);

	printf(
			"Reviso la lista a ver si la nueva partición entra, o si está duplicada\n");

	particiones(segmento);

	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		if (particionAux->id == id) {
			printf("Ya existe una partición con ID = %c. Abortando...\n", id);
			return -1;
		}
	}

	printf("Estoy por entrar en el for este de mierda\n");
	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		printf("La partición actual es la %c\n", particionAux->id);
		printf("La partición actual comienza en %d\n", particionAux->inicio);
		printf("La partición actual es de %d\n", particionAux->tamanio);
		printf("La partición acutal está libre? %d\n", particionAux->libre);
		printf("Estoy adentro del for como un champion\n");
		printf("Verifico si la partición está libre\n");
		if (particionAux->libre == false) {
			printf("La partición %d con ID %c está ocupada\n", i,
					particionAux->id);
		} else {
			if (tamanio > particionAux->tamanio) {
				printf(
						"La partición %d con ID %c no es lo suficientemente grande (%d a almacenar vs. %d disponible)\n",
						i, particionAux->id, tamanio, particionAux->id);
			} else {
				printf("El tamaño de la partición actual es %d\n",
						particionAux->tamanio);
				printf("El tamaño de la partición actual es %d\n",
						particionMayor.tamanio);
				if (particionAux->tamanio > particionMayor.tamanio) {
					printf(
							"Encontré un espacio libre mayor que el más grande que conocía; me lo guardo\n");
					particionMayor = *particionAux;
					indexParticionMayor = i;
				}
			}
		}
	}

	printf("La partición actual es la %c\n", particionMayor.id);
	printf("La partición actual comienza en %d\n", particionMayor.inicio);
	printf("La partición actual es de %d\n", particionMayor.tamanio);
	printf("La partición acutal está libre? %d\n", particionMayor.libre);

	if (particionMayor.tamanio == 0) {
		printf(
				"O no había ninguna partición libre, o niguna tenía suficiente espacio para almacenar la nueva partición. Abortando...\n");
		return 0;
	}
	//Saco la partición anterior
	eliminar_particion(segmento, particionMayor.id);
	particiones(segmento);

	//Inicializo la particion
	printf("Voy a inicializar la partición para agregar a la lista\n");
	particion->id = id;
	particion->inicio = particionMayor.inicio;
	particion->tamanio = tamanio;
	particion->dato = particionMayor.dato;
	particion->libre = false;
	printf("Inicialicé la partición para agregar a la lista\n");

	//Agrego la partición a la lista
	//TODO: ver esto
	list_add_in_index(list_particiones, indexParticionMayor, particion);
	particiones(segmento);
	printf("Agregué la partición a la lista\n");

	//Grabo el dato en memoria la particion
	printf("Grabo el dato en memoria la particion\n");
	printf("-------------VALORES--------------\n");
	printf("Valor de inicio: %d\n", particion->inicio);
	printf("Valor de tamanio: %d\n", particion->tamanio);
	printf("Valor de contenido: %s\n", contenido);
	memcpy(&segmento[particion->inicio], contenido, particion->tamanio);
	printf("-------------VALORES--------------\n");
	printf("Valor de ptrMemoria: %p\n", ptrMemoria);
	printf("Valor de ptrFinal: %p\n", ptrFinal);
	printf("Valor final calculado: %p\n", ptrMemoria + tamanio);
	printf("\n");
	printf("Valor de Segmento: %p\n", &segmento);
	printf("Valor de particion inicio: %d\n", particion->inicio);

	//Inicializo la particion restante
	printf("Voy a inicializar la partición para agregar a la lista\n");
	particionRestante->id = ' ';
	particionRestante->inicio = particion->inicio + particion->tamanio;
	particionRestante->tamanio = particionMayor.tamanio - particion->tamanio;
	//particionRestante->dato = particionMayor.dato + particion->tamanio - 1;
	particionRestante->dato = NULL;
	particionRestante->libre = true;
	printf("Inicialicé la partición para agregar a la lista\n");

	//Agrego la partición restante a la lista
	list_add_in_index(list_particiones, indexParticionMayor + 1,
			particionRestante);
	particiones(segmento);
	printf("Agregué la partición a la lista\n");

	//Grabo el dato en memoria la particion restante
	printf("Grabo el dato en memoria la particion restante\n");
	printf("-------------VALORES--------------\n");
	printf("Valor de inicio: %d\n", particionRestante->inicio);
	printf("Valor de tamanio: %d\n", particionRestante->tamanio);
	printf("Valor de contenido: %s\n", contenido);
	memcpy(&segmento[particionRestante->inicio], contenido, particionRestante->tamanio);
	printf("-------------VALORES--------------\n");
	printf("Valor de ptrMemoria: %p\n", ptrMemoria);
	printf("Valor de Segmento: %p\n", &segmento);
	printf("Valor de particion inicio: %d\n", particionRestante->inicio);

	//Imprimo las particiones para chequear
	printf("\n");
	printf("Las particiones quedaron así:\n");
	printf("\n");
	printf("\n");
	particiones(segmento);
	//free(particionAux);//si hago esto elimino el ultimo nodo obtenido de la lista pero sigo teniendo la referencia en la lista PELIGROOOOO!!!!!
//	free(particion);
//	free(particionRestante);
	return 1;
}

int eliminar_particion(t_memoria segmento, char id) {
	/* Esta funcion elimina la particion dentro del segmento de memoria
	 correspondiente al identificador enviado como parametro. Devuelve el
	 valor numérico 1 en caso de exito y 0 en caso de no encontrar una
	 particion con dicho identificador */

	int i, index, total;
	t_particion* ptrAEliminar, *particionEliminada;

	total = list_particiones->elements_count;

	if (total == 1) {
		ptrAEliminar = list_get(list_particiones, 0);
		if (ptrAEliminar->id == id) {
			particionEliminada = list_remove(list_particiones, 0);
			free(particionEliminada->dato);
			free(particionEliminada);
			printf("Se eliminó la partición %c de la posición 0\n", id);
			return 1;
		} else {
			return 0;
		}
	} else {
		index = -1;
		for (i = 0; i < (list_particiones->elements_count); i++) {
			ptrAEliminar = list_get(list_particiones, i);
			if (ptrAEliminar->id == id)
				index = i;
		}
		if (index == -1) {
			printf("No se encontró la partición %c\n", id);
			return 0;
		} else {
			particionEliminada = list_remove(list_particiones, index);
			free(particionEliminada->dato);
			free(particionEliminada);
			printf("Se eliminó la partición %c de la posición %d\n", id, index);
			return 1;
		}
	}
	particiones(segmento);
	return 0;
}

void liberar_memoria(t_memoria segmento) {
// Esta funcion libera los recursos tomados en crear_memoria()
//	printf("Libero la memoria del segmento de memoria\n");
//	free(segmento);
	printf("Libero la memoria de la lista de particiones\n");
	libero_memoria(list_particiones);
//list_destroy_and_destroy_elements(list_particiones, element_destroyer);
	return;
}

t_list* particiones(t_memoria segmento) {
	/* Esta funcion devuelve una lista en el formato t_list de las
	 commonslibrary con la siguiente descripcion por cada particion */
	t_particion* particionAux;
	int i, total;

	printf("\n");
	printf("Segmento de memoria:\n");
	printf("---------------------------------------\n");

//TODO: ordenar particiones

	total = list_particiones->elements_count;

	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		if (((t_particion*) particionAux)->id == ' ') {
			printf("VACIO[%d:%d:%d]\n", particionAux->inicio,
					(particionAux->inicio + particionAux->tamanio - 1),
					particionAux->tamanio);
		} else {
			printf("%c[%d:%d:%d]:%s\n", particionAux->id, particionAux->inicio,
					(particionAux->inicio + particionAux->tamanio - 1),
					particionAux->tamanio, particionAux->dato);
		}
	}

	printf("---------------------------------------\n");
	printf("\n");
	return list_particiones;
}

void libero_memoria(t_particion* list_particiones) {
	int index;
	index = 0;

	void _list_elements(t_particion* particion) {
		free(particion->dato);
		free(particion);
		index++;
	}
	list_iterate(list_particiones, (void*) _list_elements);
}
