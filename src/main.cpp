
#include <unistd.h> // para introducir parametros
#include "Compresor.h"

using namespace std;


int main(int argc, char *argv[]) 
{

	// Parametros para validacion
	int cflag = 0;
 	int dflag = 0;
 	int errflag = 0; // sirve para corregir el caso: -d -c || -c -d
 	int c;
	//

	char* nombreDelArchivo;
     
    // getopt: command-line parser
    while ((c = getopt (argc, argv, "cd")) != -1)
    {
        switch (c)
        {
           case 'c':
            	cflag = 1;
            	if (dflag == 1) {
            		errflag = 1;
            	}
            	break;
           case 'd':
            	dflag = 1; 
            	if (cflag == 1) {
            		errflag = 1;
            	}
            	break; 
           case '?': // parametros invalidos, ej: -fsfs, -sdfsd,s etc.
           		errflag = 1;
           		 	        
        }
    }

    // Validacion de flags
    if ( errflag == 1 ) {

    	cout << "Comando incorrecto. Los comandos posibles son: -c y -d" << endl;  
    	return 1; 

    } else {

        Compresor* compresor = new Compresor();
        
        // obtenemos el nombre del archivo
        nombreDelArchivo = argv[2];   // argv[2] es la posicion en argv del 3er parametro

    	if (cflag == 1 ) 
    	{ 
            compresor->comprimir(nombreDelArchivo);
        }
    	
    	if (dflag == 1 ) 
    	{
            compresor->descomprimir(nombreDelArchivo);
    	};

        delete compresor;
    }

return 0;

}