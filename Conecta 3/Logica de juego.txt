/*
 * Juego.c
 *
 *  Created on: 18 jun 2026
 *      Author: Administrador
 */

#include "Juego.h"
#include "ws2812b.h"
#include "Teclado.h"
#include "main.h"
#include "stdint.h"
//---------------------------------------------------------------------------------------------------
#define FILAS 8;
#define COLUMNAS 4;
uint8_t columna[4] = { 31, 27, 23, 19 };   //Vector que toma los valores de los leds que estan mas altos
static int tecla = 0;
static int i = 0;
//----------------------------------------------------------------------------------------------
static int jugador_actual = 1; // Arranca el Jugador 1
//----------------------------------------------------------------------------------------------
static int indice_columna = 0; // Columna seleccionada actualmente (0 a 3)
static int jugada_confirmada = 0;
//-----------------------------------------------------------------------------------------------
static const uint8_t TABLERO_LEDS[8][4] = {
    { 31, 27, 23, 19 }, // Fila 7 (Techo)
    { 30, 26, 22, 18 }, // Fila 6
    { 29, 25, 21, 17 }, // Fila 5
    { 28, 24, 20, 16 }, // Fila 4
    { 15,  11, 7, 3 },  // Fila 3
    { 14,  10, 6, 2 },  // Fila 2
    { 13,  9, 5, 1 },  // Fila 1
    { 12,  8, 4, 0 }   // Fila 0 (Piso)
};
static int tablero[8][4] = {0};  //0=vacio  1=jugado1    2=jugador2
//-----------------------------------------------------------------------------------------------
static int resultado = 0;
//-----------------------------------------------------------------------------------------------

static void cambiar_jugador(void)
{
    if (jugador_actual == 1) {
        jugador_actual = 2;
    } else {
        jugador_actual = 1;
    }
}
//Funcion para cambiar de jugador

void SeleccionarColumna(void)  //Arranca con el jugador 1
{
	    jugada_confirmada = 0;
		uint8_t r = (jugador_actual == 1) ? 50 : 0;  // Si jugador_actual=1 le asigno 50 a r
		uint8_t b = (jugador_actual == 2) ? 50 : 0;
		uint8_t g = 0;
		do {
			Barrido();
			tecla = ObtenerTecla();
			switch(tecla){
			case (1)://Apago el led y me muevo a la derecha
		            WS2812_LED_N_Color(columna[indice_columna], 0, 0, 0);
			        if (indice_columna < 3) {
						indice_columna++;}

	                break;

			case (13): //Apago el led y me muevo a la izquierda
			        WS2812_LED_N_Color(columna[indice_columna], 0, 0, 0);
							if (indice_columna > 0) {
								indice_columna--;}
			        break;

			case (8):
			        jugada_confirmada = 1;
			        break;
			default:
			break;

			}
			WS2812_LED_N_Color(columna[indice_columna], r, g, b);
			WS2812_Manda_Trama();

			    } while (jugada_confirmada == 0);
		cambiar_jugador();
}


void Gravedad (void){
	uint8_t r = (jugador_actual == 1) ? 50 : 0;
	uint8_t b = (jugador_actual == 2) ? 50 : 0;
	uint8_t g = 0;
    int fila_destino = -1;

    for (int f = 7; f >= 0; f--) {                                   //Voy restando de a una fila
            if (tablero[f][indice_columna] == 0) {
                fila_destino = f; // Aca es donde frena la ficha
             }
            else {
                break;} // Freno porque esta llena
    }
    if (fila_destino == -1) {  //Esto quiere decir que la columna esta llena inicialmente
            return;
    }
    for (int f = 7; f >= fila_destino; f--) {  //Este for es para controlar el encendio y apagado de los leds
    	WS2812_LED_N_Color(TABLERO_LEDS[f][indice_columna], r, g, b);
    	WS2812_Manda_Trama();
    	HAL_Delay(80);
    if (f > fila_destino) {
        WS2812_LED_N_Color(TABLERO_LEDS[f][indice_columna], 0, 0, 0);
        WS2812_Manda_Trama();
        }
    }
    tablero[fila_destino][indice_columna] = jugador_actual;
}

int verificar_victoria(void) {

    // --- HORIZONTAL (3 seguidas) ---
	int f = 0;
	int c = 0;
	int j=2;
	int resultado = 0;

    for (f < FILAS; f++) {                                  // Recorre las 8 filas
        for (c < COLUMNAS - 2; c++) {
            if (tablero[f][c] != 0 &&
                tablero[f][c] == tablero[f][c+1] &&
                tablero[f][c] == tablero[f][c+2]) {
                return tablero[f][c];
                resultado = tablero[f][c];// Devuelve el numero que hay en la casilla
            }
        }
    }

    // ---  VERTICAL (3 seguidas) ---
    for ( c < COLUMNAS; c++) {                              // Recorre las 4 columnas
        for ( f < FILAS - 2; f++) {
            if (tablero[f][c] != 0 &&
                tablero[f][c] == tablero[f+1][c] &&
                tablero[f][c] == tablero[f+2][c]) {
                return tablero[f][c];
                resultado = tablero[f][c];// Devuelve el numero que hay en la casilla
            }
        }
    }

    // ---DIAGONAL HACIA ABAJO---
    for (f < FILAS - 2; f++) {
        for ( c < COLUMNAS - 2; c++) {
            if (tablero[f][c] != 0 &&
                tablero[f][c] == tablero[f+1][c+1] &&
                tablero[f][c] == tablero[f+2][c+2]) {
                return tablero[f][c];
                resultado = tablero[f][c];
            }
        }
    }

    // ---  DIAGONAL HACIA ARRIBA ---
    for (j < FILAS; j++) {
        for ( c < COLUMNAS - 2; c++) {
            if (tablero[j][c] != 0 &&
                tablero[j][c] == tablero[j-1][c+1] &&
                tablero[j][c] == tablero[j-2][c+2]) {
                return tablero[j][c];
                resultado = tablero[f][c];
            }
        }
    }

    return resultado; // No ganó nadie
}


void animacion_victoria (int resultado){

	switch (resultado){
	case(0):

    break;
	case(1):
			WS2812_RESET();
			WS2812_Manda_Trama();
			WS2812_LED_N_Color(31, 0, 0, 50);  //Azul
			WS2812_LED_N_Color(28, 0, 0, 50);
			WS2812_LED_N_Color(19, 0, 0, 50);
			WS2812_LED_N_Color(16, 0, 0, 50);
			WS2812_LED_N_Color(15, 0, 0, 50);
			WS2812_LED_N_Color(14, 0, 0, 50);
			WS2812_LED_N_Color(13, 0, 0, 50);
			WS2812_LED_N_Color(3, 0, 0, 50);
			WS2812_LED_N_Color(2, 0, 0, 50);
			WS2812_LED_N_Color(1, 0, 0, 50);

			WS2812_LED_N_Color(30, 50, 40, 0);  //Amarillo
			WS2812_LED_N_Color(29, 50, 40, 0);
			WS2812_LED_N_Color(27, 50, 40, 0);
			WS2812_LED_N_Color(26, 50, 40, 0);
			WS2812_LED_N_Color(25, 50, 40, 0);
			WS2812_LED_N_Color(24, 50, 40, 0);
			WS2812_LED_N_Color(23, 50, 40, 0);
			WS2812_LED_N_Color(22, 50, 40, 0);
			WS2812_LED_N_Color(21, 50, 40, 0);
			WS2812_LED_N_Color(20, 50, 40, 0);
			WS2812_LED_N_Color(18, 50, 40, 0);
			WS2812_LED_N_Color(17, 50, 40, 0);
			WS2812_LED_N_Color(12, 50, 40, 0);
			WS2812_LED_N_Color(11, 50, 40, 0);
			WS2812_LED_N_Color(9, 50, 40, 0);
			WS2812_LED_N_Color(8, 50, 40, 0);
			WS2812_LED_N_Color(7, 50, 40, 0);
			WS2812_LED_N_Color(5, 50, 40, 0);
			WS2812_LED_N_Color(4, 50, 40, 0);
			WS2812_LED_N_Color(0, 50, 40, 0);

			WS2812_LED_N_Color(10, 0, 50, 0);  //Verde
			WS2812_LED_N_Color(6, 0, 50, 0);
			WS2812_Manda_Trama();
	break;
	case(2):
			WS2812_RESET();
			WS2812_Manda_Trama();
			WS2812_LED_N_Color(31, 50, 0, 0);
			WS2812_LED_N_Color(28, 50, 0, 0);
			WS2812_LED_N_Color(19, 50, 0, 0);
			WS2812_LED_N_Color(16, 50, 0, 0);
			WS2812_LED_N_Color(15, 50, 0, 0);
			WS2812_LED_N_Color(14, 50, 0, 0);
			WS2812_LED_N_Color(13, 50, 0, 0);
			WS2812_LED_N_Color(3, 50, 0, 0);
			WS2812_LED_N_Color(2, 50, 0, 0);
			WS2812_LED_N_Color(1, 50, 0, 0);

			WS2812_LED_N_Color(30, 50, 40, 0);  //Amarillo
			WS2812_LED_N_Color(29, 50, 40, 0);
			WS2812_LED_N_Color(27, 50, 40, 0);
			WS2812_LED_N_Color(26, 50, 40, 0);
			WS2812_LED_N_Color(25, 50, 40, 0);
			WS2812_LED_N_Color(24, 50, 40, 0);
			WS2812_LED_N_Color(23, 50, 40, 0);
			WS2812_LED_N_Color(22, 50, 40, 0);
			WS2812_LED_N_Color(21, 50, 40, 0);
			WS2812_LED_N_Color(20, 50, 40, 0);
			WS2812_LED_N_Color(18, 50, 40, 0);
			WS2812_LED_N_Color(17, 50, 40, 0);
			WS2812_LED_N_Color(12, 50, 40, 0);
			WS2812_LED_N_Color(11, 50, 40, 0);
			WS2812_LED_N_Color(9, 50, 40, 0);
			WS2812_LED_N_Color(8, 50, 40, 0);
			WS2812_LED_N_Color(7, 50, 40, 0);
			WS2812_LED_N_Color(5, 50, 40, 0);
			WS2812_LED_N_Color(4, 50, 40, 0);
			WS2812_LED_N_Color(0, 50, 40, 0);

			WS2812_LED_N_Color(10, 0, 50, 0);
			WS2812_LED_N_Color(6, 0, 50, 0);
			WS2812_Manda_Trama();
	break;
	default://Resultado solo puede tomar tres valores
	break;


	}

}
