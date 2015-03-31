
#ifndef __WRITER_H__
#define __WRITER_H__

#include "Declaracion_de_constantes.h"

class Writer {

	private:

		FILE* fd;

		//bool hubo_error ();

	public:

		void cerrar_archivo();

		// devuelve true si pudo setiar el nombre. Sino false, es decir el archivo ya existe y no va querer pisarlo
		bool set_nombre_archivo_nuevo(char* nombre_archivo);

		// escribe un bloque de caracteres de cierto tamañano
		// pre: invocar set_nombre_archivo_nuevo(char* nombre_archivo)
		// post: cuando no quie quiera guardar más, invocar cerrar_archivo()
		void escribir(UChar* bloque_caracteres, LInt &t_bloque); 
		
	
};

#endif