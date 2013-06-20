/*
 * pantalla.h
 *
 *  Created on: 30/05/2013
 *      Author: utnso
 */

#ifndef PANTALLA_H_
#define PANTALLA_H_
#include <commons/collections/list.h>
#include "nivel_p.h"

void inicializo_pantalla(void);
void recusos_pantalla(t_list *recursos, ITEM_NIVEL *ListaItems);
void libero_recursos_pantalla(t_list *recursos);
void personaje_pantalla(char recursos, int x, int y,ITEM_NIVEL *ListaItems);

#endif /* PANTALLA_H_ */
