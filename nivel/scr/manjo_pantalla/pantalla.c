/*
 * pantalla.c
 *
 *  Created on: 30/05/2013
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <mario_para_todos/ambiente_config.h>
#include <mario_para_todos/grabar.h>
#include <mario_para_todos/ambiente_config.h>
#include "tad_items.h"
#include <curses.h>
#include "nivel_p.h"


void inicializo_pantalla() {
	log_in_disk_niv(LOG_LEVEL_TRACE, "inicializo_pantalla");
	int rows, cols;
	int q, p;

	int x = 1;
	int y = 1;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&rows, &cols);

	p = cols;
	q = rows;

}

void recusos_pantalla(t_list *recursos,ITEM_NIVEL** ListaItems) {


	log_in_disk_niv(LOG_LEVEL_TRACE, "recusos_pantalla");

	int index = 0;

	void _list_elements(t_recusos *list_recursos) {
		CrearItem(ListaItems, list_recursos->SIMBOLO,
				list_recursos->posX,list_recursos->posY, RECURSO_ITEM_TYPE,
				list_recursos->cantidad);

		index++;

	}

	list_iterate(recursos, (void*) _list_elements);

}

void libero_recursos_pantalla(t_list *recursos) {
	log_in_disk_niv(LOG_LEVEL_TRACE, "libero_recursos_pantalla");

	int index = 0;

	void _list_elements(t_recusos *list_recursos) {

		log_in_disk_niv(LOG_LEVEL_TRACE,
				"Libero el  recurso %s con el simbolo %c y la candidad %d en la poscion [x,y] [%d][%d] ",
				list_recursos->NOMBRE, list_recursos->SIMBOLO,
				list_recursos->cantidad, list_recursos->posX,
				list_recursos->posY);
		free(list_recursos);
		index++;

	}

	list_iterate(recursos, (void*) _list_elements);

}



void personaje_pantalla(char personaje, int x, int y, ITEM_NIVEL **ListaItems) {


	log_in_disk_niv(LOG_LEVEL_TRACE, "personaje_pantalla");



	CrearPersonaje(ListaItems, personaje, x, y);

	log_in_disk_niv(LOG_LEVEL_TRACE, "Creo el personaje %c en la posicion [%d;%d] ",personaje,x,y);

}
