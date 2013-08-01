#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <commons/collections/list.h>
#include "memoria.h"
#include <commons/string.h>

t_list* list_particiones;
int tamanioTotal;

t_memoria crear_memoria(int tamanio) {
	t_particion* particion;
	t_memoria segmento;

	tamanioTotal = tamanio;

	segmento = (char*)(malloc(tamanio));

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
	particion->id = '\0';
	particion->inicio = 0;
	particion->tamanio = tamanio;
	particion->libre = true;
	particion->dato = &segmento[particion->inicio];

	printf("Agrego la partición inicial\n");

	list_add(list_particiones, particion);

	printf("Agregué la partición inicial\n");

	printf("El segmento tiene ahora %d partciones\n",
			list_particiones->elements_count);
	particiones(segmento);

	printf("%p", segmento);

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
	t_particion* particionAux;
	t_particion particionMayor;
	t_particion* particion;
	t_particion* particionRestante;
	t_particion* ptrAEliminar;

	printf("%p", segmento);

	//particionAux = malloc(sizeof(t_particion)); //no hace falta recive un valor ya allocado en la lista
	particion = malloc(sizeof(t_particion));
	particionRestante = malloc(sizeof(t_particion));

	printf("%p", segmento);

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

	printf("%p", segmento);

	particiones(segmento);

	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		if (particionAux->id == id) {
			printf("Ya existe una partición con ID = %c. Abortando...\n", id);
			return -1;
		}
	}

	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		printf("Verifico si la partición %c está libre\n", particionAux->id);
		if (particionAux->libre == false) {
			printf("La partición %c está ocupada\n", particionAux->id);
		} else {
			if (tamanio > particionAux->tamanio) {
				printf(
						"La partición %c no es lo suficientemente grande (%d a almacenar vs. %d disponible)\n", particionAux->id, tamanio, particionAux->tamanio);
			} else {
				printf("El tamaño de la partición actual es %d\n",
						particionAux->tamanio);
				printf("El tamaño de la mayor partición guardada es %d\n",
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

	if (particionMayor.tamanio == 0) {
		printf(
				"O no había ninguna partición libre, o niguna tenía suficiente espacio para almacenar la nueva partición. Abortando...\n");
		return 0;
	}

	//Saco la partición anterior
	ptrAEliminar = list_remove(list_particiones, indexParticionMayor);
	printf("Se eliminó la partición %c de la posición %d\n", ptrAEliminar->id, indexParticionMayor);
	//free(ptrAEliminar->dato);
	free(ptrAEliminar);

	particiones(segmento);

	//Inicializo la particion
	printf("Voy a inicializar la partición %c para agregar a la lista\n", id);
	particion->id = id;
	particion->inicio = particionMayor.inicio;
	particion->tamanio = tamanio;
	particion->dato = &segmento[particion->inicio];
	particion->libre = false;
	printf("Inicialicé la partición para agregar a la lista\n");

	//Agrego la partición a la lista
	list_add_in_index(list_particiones, indexParticionMayor, particion);
	printf("Agregué la partición a la lista\n");
	printf("Elements count: %d\n", list_particiones->elements_count);

	//Grabo el dato en memoria la particion
	printf("Grabo el dato en memoria la particion\n");
	memcpy(particion->dato, contenido, particion->tamanio);

	//Inicializo la particion restante
	printf("Voy a inicializar la partición restante para agregar a la lista\n");
	particionRestante->id = '\0';
	particionRestante->inicio = particion->inicio + particion->tamanio;
	particionRestante->tamanio = particionMayor.tamanio - particion->tamanio;
	particionRestante->dato = &segmento[particion->inicio + particion->tamanio];
	particionRestante->libre = true;
	printf("Inicialicé la partición restante para agregar a la lista\n");

	//Agrego la partición restante a la lista si tengo lugar
	if(particionRestante->inicio == tamanioTotal){
		printf("No tengo más lugar en mi segmento, así que no agregué partición restante a la lista\n");
		printf("Elements count: %d\n", list_particiones->elements_count);
	} else {
		list_add_in_index(list_particiones, indexParticionMayor, particionRestante);
		printf("Agregué la partición restante a la lista\n");
		printf("Elements count: %d\n", list_particiones->elements_count);
	}

	//Grabo el dato en memoria la particion restante
	//printf("Grabo el dato en memoria la particion restante\n");
	//memcpy(particionRestante->dato, contenido, particionRestante->tamanio);

	//Imprimo las particiones para chequear
	printf("\n");
	printf("Las particiones quedaron así:\n");
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
	t_particion* ptrAEliminar;

	total = list_particiones->elements_count;

	if (total == 1) {
		ptrAEliminar = list_get(list_particiones, 0);
		if (ptrAEliminar->id == id) {
//			particionEliminada = list_remove(list_particiones, 0);
//			free(particionEliminada->dato);
//			free(particionEliminada);
			ptrAEliminar = list_get(list_particiones, 0);
			ptrAEliminar->id = '\0';
			ptrAEliminar->libre = true;
			list_replace(list_particiones, 0, ptrAEliminar);
			printf("Se eliminó la partición %c de la posición 0, usando eliminar_particion\n", id);
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
//			particionEliminada = list_remove(list_particiones, index);
//			free(particionEliminada->dato);
//			free(particionEliminada);
			ptrAEliminar = list_get(list_particiones, index);
			ptrAEliminar->id = '\0';
			ptrAEliminar->libre = true;
			list_replace(list_particiones, index, ptrAEliminar);
			printf("Se eliminó la partición %c de la posición %d, usando eliminar_particion\n", id, index);
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
	t_list* list_segmento, *list_auxiliar;
	int i, n, totalLista;

	list_auxiliar = list_create();
	list_segmento = list_create();

	printf("Segmento de memoria:\n");
	printf("---------------------------------------\n");

	totalLista = list_particiones->elements_count;

	for (i = (totalLista-1); i >= 0; i--) {
		particionAux = list_get(list_particiones, i);
		list_add(list_auxiliar, particionAux);
	}

	list_segmento = list_take(list_auxiliar, totalLista);

	totalLista = list_segmento->elements_count;

	printf("%d", totalLista);

	for (i = 0; i < totalLista; i++) {
		particionAux = list_get(list_segmento, i);
		if (particionAux->libre == true) {
			printf("VACIO[%d:%d:%d]\n", particionAux->inicio,
					(particionAux->inicio + particionAux->tamanio - 1),
					particionAux->tamanio);
		} else {
			printf("%c[%d:%d:%d]:", particionAux->id, particionAux->inicio,
					(particionAux->inicio + particionAux->tamanio - 1),
					particionAux->tamanio);
			for (n = particionAux->inicio; n < (particionAux->tamanio + particionAux->inicio); n++) {
				printf("%c", segmento[n]);
			}
			printf("\n");
		}
	}

	printf("---------------------------------------\n");

	//TODO: Agregar tamaño total y blabla

	for(i=0; i< tamanioTotal; i++){
		printf("%c",segmento[i]);
	}
	printf("\n");

	list_destroy(list_auxiliar);

	return list_segmento;
}

void libero_memoria(t_list* list) {
	int index;
	index = 0;

	void _list_elements(t_particion* particion) {
		free(particion->dato);
		free(particion);
		index++;
	}

	list_iterate(list_particiones, (void*) _list_elements);
}
