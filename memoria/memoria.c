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
	memset(segmento, '\0', tamanio);

	// Creo la lista de particiones
	list_particiones = list_create();

	//Agrego la primer partición
	particion = malloc(sizeof(t_particion));

	particion->id = '\0';
	particion->inicio = 0;
	particion->tamanio = tamanio;
	particion->libre = true;
	particion->dato = &segmento[particion->inicio];

	list_add(list_particiones, particion);

	return segmento;
}

int almacenar_particion(t_memoria segmento, char id, int tamanio, char* contenido) {
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

	//particionAux = malloc(sizeof(t_particion)); //no hace falta recive un valor ya allocado en la lista
	particion = malloc(sizeof(t_particion));
	particionRestante = malloc(sizeof(t_particion));

	//Inicializo particionMayor con un tamaño de 0 para después saber si encontré otra mayor, o ninguna disponible
	particionMayor.tamanio = 0;

	if (tamanio > tamanioTotal) {
		//printf("El tamaño de la partición supera el tamaño del segmento. Abortando...\n");
		return 0;
	}

	total = list_size(list_particiones);

	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		if (particionAux->id == id) {
			//printf("Ya existe una partición con ID = %c. Abortando...\n", id);
			return -1;
		}
	}

	for (i = 0; i < total; i++) {
		particionAux = list_get(list_particiones, i);
		if (particionAux->libre == false) {
			//printf("La partición %c está ocupada\n", particionAux->id);
		} else {
			if (tamanio > particionAux->tamanio) {
				//printf("La partición %c no es lo suficientemente grande (%d a almacenar vs. %d disponible)\n", particionAux->id, tamanio, particionAux->tamanio);
			} else {
				//printf("El tamaño de la partición actual es %d\n", particionAux->tamanio);
				//printf("El tamaño de la mayor partición guardada es %d\n", particionMayor.tamanio);
				if (particionAux->tamanio > particionMayor.tamanio) {
					particionMayor = *particionAux;
					indexParticionMayor = i;
				}
			}
		}
	}

	if (particionMayor.tamanio == 0) {
		//printf("O no había ninguna partición libre, o niguna tenía suficiente espacio para almacenar la nueva partición. Abortando...\n");
		return 0;
	}

	//Saco la partición anterior
	ptrAEliminar = list_remove(list_particiones, indexParticionMayor);
	free(ptrAEliminar);

	//Inicializo la particion
	particion->id = id;
	particion->inicio = particionMayor.inicio;
	particion->tamanio = tamanio;
	particion->dato = &segmento[particion->inicio];
	particion->libre = false;

	//Agrego la partición a la lista
	list_add_in_index(list_particiones, indexParticionMayor, particion);

	//Grabo el dato en memoria la particion
	memcpy(particion->dato, contenido, particion->tamanio);

	//Inicializo la particion restante
	particionRestante->id = '\0';
	particionRestante->inicio = particion->inicio + particion->tamanio;
	particionRestante->tamanio = particionMayor.tamanio - particion->tamanio;
	particionRestante->dato = &segmento[particion->inicio + particion->tamanio];
	particionRestante->libre = true;

	//Agrego la partición restante a la lista si tengo lugar
	if(particionRestante->inicio == tamanioTotal){
		//printf("No tengo más lugar en mi segmento, así que no agregué partición restante a la lista\n");
	} else {
		list_add_in_index(list_particiones, indexParticionMayor, particionRestante);
	}

	//Grabo el dato en memoria la particion restante
	//printf("Grabo el dato en memoria la particion restante\n");
	//memcpy(particionRestante->dato, contenido, particionRestante->tamanio);

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
			ptrAEliminar = list_get(list_particiones, 0);
			ptrAEliminar->id = '\0';
			ptrAEliminar->libre = true;
			list_replace(list_particiones, 0, ptrAEliminar);
			//printf("Se eliminó la partición %c de la posición 0, usando eliminar_particion\n", id);
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
			ptrAEliminar = list_get(list_particiones, index);
			ptrAEliminar->id = '\0';
			ptrAEliminar->libre = true;
			list_replace(list_particiones, index, ptrAEliminar);
			//printf("Se eliminó la partición %c de la posición %d, usando eliminar_particion\n", id, index);
			return 1;
		}
	}
	return 0;
}

void liberar_memoria(t_memoria segmento) {
// Esta funcion libera los recursos tomados en crear_memoria()

	libero_memoria(list_particiones);
	return;
}

t_list* particiones(t_memoria segmento) {
	/* Esta funcion devuelve una lista en el formato t_list de las
	 commonslibrary con la siguiente descripcion por cada particion */
	t_particion* particionAux;
	t_list* list_segmento, *list_auxiliar;
	int i, totalLista;

	list_auxiliar = list_create();
	list_segmento = list_create();

	totalLista = list_particiones->elements_count;

	for (i = (totalLista-1); i >= 0; i--) {
		particionAux = list_get(list_particiones, i);
		list_add(list_auxiliar, particionAux);
	}

	list_segmento = list_take(list_auxiliar, totalLista);

	totalLista = list_segmento->elements_count;

//	for (i = 0; i < totalLista; i++) {
//		particionAux = list_get(list_segmento, i);
//		if (particionAux->libre == true) {
//			printf("VACIO[%d:%d:%d]\n", particionAux->inicio,
//					(particionAux->inicio + particionAux->tamanio - 1),
//					particionAux->tamanio);
//		} else {
//			printf("%c[%d:%d:%d]:", particionAux->id, particionAux->inicio,
//					(particionAux->inicio + particionAux->tamanio - 1),
//					particionAux->tamanio);
//			for (n = particionAux->inicio; n < (particionAux->tamanio + particionAux->inicio); n++) {
//				printf("%c", segmento[n]);
//			}
//			printf("\n");
//		}
//	}

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
