
#include "Writer.h"

using namespace std;

bool Writer::set_nombre_archivo_nuevo(char* nombre_archivo) {
 	
 	bool apertura_correcta = false;

	fd = fopen( nombre_archivo , "r"); // este modo no permite abrir algo que no existe

	// si el archivo no existe proseguimos, caso contrario debemos parar.
	if (fd == NULL) {
	
		fd = fopen( nombre_archivo , "wb+"); // abrimos el archivo modo write binario.
		apertura_correcta = true;

	}else {

		//cout << "el archivo ya existe" << endl;
		cerrar_archivo();
	}

	return apertura_correcta;
}

void Writer::escribir(UChar* bloque_caracteres, LInt &t_bloque){
	
	fwrite(bloque_caracteres,sizeof(UChar),t_bloque,fd);
}

void Writer::cerrar_archivo(){

	fclose(fd);
}

/*bool Writer::hubo_error () {
		
	if  ( fd == NULL ) {
			
		cout << "hubo error" << endl;		
		return true;
			
	} else {
		cout << "no hubo error" << endl;				
		return false;
	}
}*/
