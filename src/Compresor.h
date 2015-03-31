
#ifndef COMPRESOR_H_
#define COMPRESOR_H_


#include "Declaracion_de_constantes.h"

#include "Aritmetico.h"
#include "Nombre_archivo.h"
#include "Move_to_front.h"
#include "Overhead.h"

#include "blockSorting.h"
#include "blockSortingLeft.h"

#include "Writer.h"

class Compresor{

	private:

		string extencion_archivo;

		// para ir leyendo el archivo a comprimir o descomprimir
		LInt t_bloque;
		UChar* bloque_caracteres;

		// para manejar la compresion y descompresion
		Aritmetico* aritmetico;
		Move_to_front* mtf;
		Overhead* overhead;
		Writer* escritor;

		bool not_end_of_file(FILE* &fd);

	public:

		Compresor();
		~Compresor();

		// deberia comprimir el archivo que se pide, creando un archivo: nombre_archivo.1
		void comprimir(char* nombre_archivo);

		// pre: se le debe pasar un archivo .1
		void descomprimir(char* nombre_archivo_con_extencion);
};

#endif