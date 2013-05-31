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

void recusos_pantalla(t_list *recursos){
	log_in_disk_niv(LOG_LEVEL_TRACE,"recusos_pantalla");

	int index = 0;

		void _list_elements(t_recusos *list_recursos) {

			log_in_disk_niv(LOG_LEVEL_TRACE,
					"Recorro las cajas que contiene el recurso %s con el simbolo %c y la candidad %d en la poscion [x,y] [%d][%d] ",
					list_recursos->NOMBRE, list_recursos->SIMBOLO,
					list_recursos->cantidad, list_recursos->posX,
					list_recursos->posY);
			index++;

		}

		list_iterate(recursos, (void*) _list_elements);

}

void libero_recursos_pantalla(t_list *recursos){
	log_in_disk_niv(LOG_LEVEL_TRACE,"libero_recursos_pantalla");

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
