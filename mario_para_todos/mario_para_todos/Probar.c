/*
 * Probar.c
 *
 *  Created on: 29/07/2013
 *      Author: utnso
 */



/* PENDIENTES

DESARROLLAR - TERMINAR
*Nivel: Detección de interbloqueo
*Nivel: Elección personaje víctima. Enviar PL_TO_P_MUERTE.
*Plataforma/Orquestador: Integrar koopa
*Hacer los archivos de configuración de niveles y personajes para probar el sábado todas las situaciones pedidas (y para probar nosotros xD).

SOLUCIONAR
*Nivel: Restar recursos al desbloquear, ya que el personaje se asigna automáticamente el recurso por el que había quedado bloqueado
*Plataforma: Desconexión inesperada de personajes/niveles
*Uso de memoria/consumo de CPU
*Uso *Planificador: Modificar el quantum en tiempo de ejecución

PROBAR
*Nivel: función DESBLOQUEAR_PERSONAJES (utilizada en OBJ_CUMPLIDO, REINICIAR_NIVEL, SALIR)
*Nivel: Gráficos en pantalla
*Nivel: Restar y sumar recursos del nivel
*Eliminar personaje del nivel y del planificador cuando muere (sacarlo del mapa/lista, liberar recursos, desbloquear, etc.) -> Case P_TO_PL_SALIR /P_TO_N_SALIR
*Nivel: Reiniciar nivel(liberar recursos, desbloquear personajes, posicionar personaje, etc.) -> Case P_TO_N_REINICIAR_NIVEL
*Personaje: Reiniciar nivel
*Personaje: Reiniciar plan de niveles
*Planificador: Perder quantum al conseguir un recurso
*Recontraprobar desbloqueo de personajes
*Personaje: Muerte por deadlock
*Koopa
*Compilación por consola, server de test, makefiles, etc.

 */




