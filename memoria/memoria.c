#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include "memoria.h"
#include <commons/string.h>

t_list* listaParticiones;
char aBuscar;
int tamanioTotal;

t_memoria crear_memoria(int tamanio) {
	// Crea el segmento de memoria a particionar
	printf("Voy a crear el segmento de memoria\n");
	t_memoria segmento = (t_memoria)malloc(tamanio);
	t_particion particion;
	void* particionAux;

	printf("Cree el segmento de memoria\n");

	printf("Voy a crear la lista de particiones\n");
	listaParticiones = list_create();

	printf("Cree la lista de particiones\n");

	tamanioTotal = tamanio;

	particion.id = ' ';
	particion.inicio = 0;
	particion.libre = true;
	particion.tamanio = tamanio;
	particion.dato = segmento;

	printf("Inicialicé la partición\n");

	particionAux = &particion;

	printf("Voy a agregar la partición inicial\n");

	list_add(listaParticiones, particionAux);

	printf("Agregué la partición inicial\n");

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
	if (listaParticiones->elements_count == 1) {
		printf("No tenía elementos en la lista de particiones, así que procedo a agregarla\n");
		particion.inicio = 0;
		particion.dato = segmento;
		particion.libre = false;
		data = &particion;
		list_add(listaParticiones, data);
		printf("Agregué la partición; ahora voy por el espacio restante\n");
		restante.id = ' ';
		restante.inicio = (int)segmento + particion.tamanio - 1;
		restante.tamanio = tamanioTotal - particion.tamanio;
		restante.dato = segmento + particion.tamanio -1;
		restante.libre = true;
		printf("Setié todos los datos para que esté lista la partición restante\n");
		nodeParticion = &restante;
		list_add_in_index(listaParticiones, (listaParticiones->elements_count - 1), nodeParticion);
		printf("Agregué la partición restante en el index %d", listaParticiones->elements_count - 1);
	} else {
		printf("Obtengo el primer nodo de la lista, para comparar e insertar ordenado\n");
		nodeParticion = list_get(listaParticiones, 0);
		printf("Empiezo a recorrer la lista en busca de la posición en la que insertar la partición\n");
		printf("Mi elements_count es %d\n", listaParticiones->elements_count);
		for(i = 0; i < listaParticiones->elements_count; i++){
			nodeParticion = list_get(listaParticiones, i);
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
				list_add_in_index(listaParticiones, indexMayorParticion, nodeParticion);
				printf("Agregué mi partición a la lista\n");
				if(particion.tamanio < mayorParticion.tamanio){
					printf("Como la partición no ocupó todo el espacio disponible, creo el restante\n");
					restante.id = ' ';
					restante.inicio = offset + tamanio -1;
					restante.tamanio = mayorParticion.tamanio - particion.tamanio;
					restante.dato = segmento + offset + tamanio - 1;
					restante.libre = true;
					nodeParticion = &restante;
					list_add_in_index(listaParticiones, indexMayorParticion+1, nodeParticion);
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
	//auxiliar = list_find(listaParticiones, (void*) esID);
	if(auxiliar != NULL){
		auxiliar->libre = true;
	} else {
		return 0;
	}
	return 1;
}

void liberar_memoria(t_memoria segmento) {
	// Esta funcion libera los recursos tomados en crear_memoria()
	free(segmento);
	//list_destroy_and_destroy_elements(listaParticiones, element_destroyer);
	return;
}

t_list* particiones(t_memoria segmento) {
	/* Esta funcion devuelve una lista en el formato t_list de las
	commonslibrary con la siguiente descripcion por cada particion */
	int i, fin;
	t_particion* auxiliar;

	for(i = 0; i < listaParticiones->elements_count; i++){
		 auxiliar = list_get(listaParticiones, i);
		 fin = (*auxiliar).tamanio - (*auxiliar).inicio - 1;
		 printf("%c[%d:%d:%d]:%s\n", (*auxiliar).id, (*auxiliar).inicio, fin, (*auxiliar).tamanio, (*auxiliar).dato);
	}
	return listaParticiones;
}
