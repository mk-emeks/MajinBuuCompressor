
#ifndef MOVE_TO_FRONT_H_
#define MOVE_TO_FRONT_H_

#include "Declaracion_de_constantes.h"

class Move_to_front{ 

private:

	void ordenar_tira(UChar* tira_ordenada);

public:

	/* 
	Post: devuelve el move to front de la tira localizada
	*/
	UChar* moveToFront(UChar* tira_localizada, LInt	long_tira, Modelos* caracteres_predominantes_aux); // cuando se libera la memoria de las tiras?

	UChar* vueltaMoveToFront(UChar* tiraMTF, LInt long_tira);
}; 

#endif