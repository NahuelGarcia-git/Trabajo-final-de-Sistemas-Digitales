/*
 * Teclado.c
 *
 *  Created on: 12 jun 2026
 *      Author: Administrador
 */
// Teclado.c
#include "Teclado.h"
#define PERIODO 100

static uint8_t filaActual = 0;		//la fila puede tomar el valor 0, 1, 2 o 3;
static uint32_t tickAnterior = 0;	//almaceno el valor anterior del HAL_GetTick
static int teclaDetectada = 0;

static const uint16_t filas[4] = { R1_Pin, R2_Pin, R3_Pin, R4_Pin }; /*Acá defino un vector de 4
 espacios llamado filas, para almacenar los datos de las entradas R1-4*/

void Barrido(void) {
	// Primero apago todas las filas
	HAL_GPIO_WritePin(BotonMatriz, R1_Pin | R2_Pin | R3_Pin | R4_Pin,
			GPIO_PIN_RESET);
	// Y luego solo enciendo solo la fila activa
	tickAnterior = HAL_GetTick();
	teclaDetectada = 0;
	int check = 0;
	do {
		//Dentro del periodo, chequeamos el estado de la fila
		HAL_GPIO_WritePin(BotonMatriz, filas[filaActual], GPIO_PIN_SET);
		check = Chequeo(filaActual);
		if (check == 0) { // Chequeo si alguna columna de la fila actual esta en alto
			HAL_GPIO_WritePin(BotonMatriz, filas[filaActual], GPIO_PIN_RESET); // Reinicio la fila actual
			filaActual = (filaActual + 1) % 4; // Avanzo a la siguiente fila
		} else
			teclaDetectada = check;

	} while ((teclaDetectada == 0) && (HAL_GetTick() - tickAnterior < PERIODO));
	return;
}

// Chequeo secuencialmente cada columna para establecer una prioridad
int Chequeo(int fila) {
	if (HAL_GPIO_ReadPin(BotonMatriz, C1_Pin))
		return (fila * 4) + 1;
	if (HAL_GPIO_ReadPin(BotonMatriz, C2_Pin))
		return (fila * 4) + 2;
	if (HAL_GPIO_ReadPin(BotonMatriz, C3_Pin))
		return (fila * 4) + 3;
	if (HAL_GPIO_ReadPin(BotonMatriz, C4_Pin))
		return (fila * 4) + 4;
	return 0;   // ninguna tecla presionada
}

int ObtenerTecla(void) {
	int t = teclaDetectada;
	teclaDetectada = 0;   // limpia después de leer
	return t;
}
