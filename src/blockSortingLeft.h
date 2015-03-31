
#ifndef BLOCKSORTINGLEFT_H_
#define BLOCKSORTINGLEFT_H_

#include "Declaracion_de_constantes.h"

/*
   Pre: Se le pasa el puntero donde se quiere que devuelva la ultima columna de la matriz ( salida del block sorting )
        luego el puntero al texto sobre el cual se quiere aplicar el block sorting y por ultimo la longitud del mismo.

   Post: Devuelve el indice para poder retornar al archivo original.
*/ 

LInt blockSortingLeft(UChar* salidaBs , UChar* tira_texto, LInt long_tira);

/*
	Pre: Se le pasa el puntero al resultado del block sorting, el indice de la tira original y la longitud de esta.

	Post: Devuelve la tira original.
*/

UChar* vueltaBlockSortingLeft(UChar* salidaBS, LInt indice, LInt long_tira);

#endif
