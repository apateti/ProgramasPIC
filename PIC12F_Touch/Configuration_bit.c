/* 
 * File:   Configuration_bit.c
 * Author: APateti
 *
 * Created on 16 de julio de 2015, 03:35 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
/*
 * Configuraci{on de Bits en la Palabra de Configuracion
 */
#pragma config FOSC = INTOSC
//#pragma config WDTE = SWDTEN
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config FCMEN = OFF
#pragma config CLKOUTEN = OFF
#pragma config IESO = ON
#pragma config PLLEN = OFF
#pragma config BOREN = OFF
#pragma config MCLRE = OFF

//#pragma config CCPMX = RA2

