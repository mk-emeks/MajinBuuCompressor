
#ifndef DECLARACION_DE_CONSTANTES_H_
#define DECLARACION_DE_CONSTANTES_H_

// librerias generales:
#include <iostream>
//#include <stdlib.h>
#include <string>
//#include <bitset>
#include <math.h>
#include <stdio.h>

// librerias para correrlo en arquitectura de x64
//#include <bits/predefs.h>
//#include <bits/c++config.h>

using namespace std;


// definicion de tipos:
typedef unsigned char UChar;
typedef unsigned long int LInt;

// definicion de constantes:

// compresor
const LInt MEMORIA_POR_ACCESO = 1400000; /* (!)(!)(!) Deben ser multiplos con TAMANIO_SUB_BLOQUE (!)(!)(!) */
const unsigned short TAMANIO_SUB_BLOQUE = 71; // 71 para rigth! 91 para left!
const UChar OVERHEAD_MODELOS_POR_BYTE = 0x04; // el overhead del cambio de modelo, ocupa 2 bits por sub_bloque, por lo tanto entra 4 en un byte

// obs: los modelos se recorren distinto luego de un escape*
enum Modelos { Estructurado = 0, Invertido = 1 , Semi_invertido = 2 , Casi_Estructurado = 3 };

// BS Y BSleft
const LInt MAXIMOLINT = 0xffffffff; // obs: si cambiamos de tipo de dato hay que cambiar esto 

// MTF
const short unsigned int MAX_CARACTERES = 256;

#endif		
