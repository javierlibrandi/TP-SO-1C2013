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

	printf("El segmento tiene ahora %d partciones\n", list_particiones->elements_count);
	particiones(segmento);

	return segmento;
}

int almacenar_particion(t_memoria segmento, char id, int tamanio, char* contenido) {
	/* Crea una particion dentro del segmento de memoria de tamaño,
	identificador y contenido especificado. Devuelve el valor numerico -1
	en caso de error (ej: tamaño de la particion mayor que el tamaño total
	del segmento, id duplicado, etc.), 1 en caso de exito y 0 en caso de
	no encontrar una particion libre lo suficientemente grande para
	almacenar la solicitud. */

	int i, offset, indexMayorParticion;
	void *data;
	t_particion particion, mayorParticion, restante;
	t_particion* nodeParticion;

	//indexMayorParticion -> índice de la posición de la posición más grande en la lista

	printf("Va a comenzar el almacenamiento de la partición\n");

	particion.id = id;
	particion.tamanio = tamanio;
	mayorParticion.tamanio = 0;

	printf("Inicializo los primeros datos de la partición\n");

	//TODO: FALTA VALIDACIÓN POR PARTICIÓN MAYOR AL SEGMENTO
	if (list_particiones->elements_count == 1) {
		printf("No tenía elementos en la lista de particiones, así que procedo a agregarla\n");
		particion.inicio = 0;
		particion.dato = segmento;
		particion.libre = false;
		data = &particion;
		list_add(list_particiones, data);
		printf("Agregué la partición; ahora voy por el espacio restante\n");
		restante.id = ' ';
		restante.inicio = (int)segmento + particion.tamanio - 1;
		restante.tamanio = tamanioTotal - particion.tamanio;
		restante.dato = segmento + particion.tamanio -1;
		restante.libre = true;
		printf("Setié todos los datos para que esté lista la partición restante\n");
		nodeParticion = &restante;
		list_add_in_index(list_particiones, (list_particiones->elements_count - 1), nodeParticion);
		printf("Agregué la partición restante en el index %d", list_particiones->elements_count - 1);
	} else {
		printf("Obtengo el primer nodo de la lista, para comparar e insertar ordenado\n");
		nodeParticion = list_get(list_particiones, 0);
		printf("Empiezo a recorrer la lista en busca de la posición en la que insertar la partición\n");
		printf("Mi elements_count es %d\n", list_particiones->elements_count);
		for(i = 0; i < list_particiones->elements_count; i++){
			nodeParticion = list_get(list_particiones, i);
			printf("Analizo el nodo de la posición %d\n", i);
			if ((*nodeParticion).id == particion.id){
				//Si el id está duplicado, retorno -1
				printf("La partición está duplicada\n");
				return -1;
			} else {
				if((*nodeParticion).libre == true){
				printf("Encontré una partición libre\n");
				} else {
					if((*nodeParticion).tamanio > mayorParticion.tamanio){
						//Si encuentro una partición libre de mayor tamaño a la actual, me la guardo
						printf("La partición que encontré es la más grande hasta ahora\n");
						mayorParticion = *nodeParticion;
						indexMayorParticion = i;
					}
				}
			}
			offset = offset + (*nodeParticion).tamanio;
		}
		if(mayorParticion.tamanio == 0){
			//Si no encontré ninguna partición de mayor tamaño que 0, estaban todas ocupadas, por lo cual, retorno 0
			printf("Todas mis particiones estaban ocupadas\n");
			return -1;
		} else {
			if(mayorParticion.tamanio < particion.tamanio){
				printf("No encontré una partición lo suficientemente grande\n");
				return 0;
			} else {
				//Actualizo la lista
				printf("Encontré lugar! Voy a meter la partición\n");
				particion.inicio = offset;
				particion.dato = segmento + offset - 1;
				particion.libre = false;
				nodeParticion = &particion;
				printf("Mi partición está lista para ser agregada\n");
				list_add_in_index(list_particiones, indexMayorParticion, nodeParticion);
				printf("Agregué mi partición a la lista\n");
				if(particion.tamanio < mayorParticion.tamanio){
					printf("Como la partición no ocupó todo el espacio disponible, creo el restante\n");
					restante.id = ' ';
					restante.inicio = offset + tamanio -1;
					restante.tamanio = mayorParticion.tamanio - particion.tamanio;
					restante.dato = segmento + offset + tamanio - 1;
					restante.libre = true;
					nodeParticion = &restante;
					list_add_in_index(list_particiones, indexMayorParticion+1, nodeParticion);
				}
			}
		}
	}
	return 1;
}

/*int _es_ID(t_particion* part) {
	return string_equals_ignore_case(aBuscar, part.id);
}*/

int eliminar_particion(t_memoria segmento, char id) {
	/* Esta funcion elimina la particion dentro del segmento de memoria
	correspondiente al identificador enviado como parametro. Devuelve el
	valor numérico 1 en caso de exito y 0 en caso de no encontrar una
	particion con dicho identificador */
	t_particion* auxiliar = NULL;
	//auxiliar = list_find(list_particiones, (void*) esID);
	if(auxiliar != NULL){
		auxiliar->libre = true;
	} else {
		return 0;
	}
	return 1;
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

	for(i = 0; i < (list_particiones->elements_count); i++){
			particionAux = list_get(list_particiones, i);
			if(((t_particion*)particionAux)->id == ' '){
				printf("VACIO[%d:%d:%d]\n", ((t_particion*)particionAux)->inicio, (((t_particion*)particionAux)->inicio + ((t_particion*)particionAux)->tamanio - 1), ((t_particion*)particionAux)->tamanio);
			} else {
				printf("%c[%d:%d:%d]:%s\n", ((t_particion*)particionAux)->id, ((t_particion*)particionAux)->inicio, (((t_particion*)particionAux)->inicio + ((t_particion*)particionAux)->tamanio - 1), ((t_particion*)particionAux)->tamanio, ((t_particion*)particionAux)->dato);
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
		free(((t_particion*)particion)->dato);
		free(particion);
		index++;
	}
	list_iterate(list_particiones, (void*) _list_elements);
}
