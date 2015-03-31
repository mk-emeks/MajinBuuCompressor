
#include <iostream>
#include <stdio.h>

using namespace std;

int main ( int argc , char* argv[]){

	cout << "Trabajo con los movimientos dentro de archivos:" << endl;

	FILE *fd = fopen(argv[1],"rb+");

	long int t_bloque = 1000000;
	unsigned char *bloque_caracteres = new unsigned char[t_bloque];

	if (fseek(fd,0,SEEK_END) == 0) { // leyo bien
		
		long tamanio_archivo = ftell(fd); // hiper mega robusto, te devuelve cantidad de bytes, con maximo de 2,147,483,647  

		if ( tamanio_archivo != -1) {

			cout <<"tamaño del archivo: "<< tamanio_archivo << endl;
		
			// calculo cantidad de bloques
			unsigned short cantidad_bloques = (unsigned short)(tamanio_archivo / t_bloque);   //es de 16 bits -> maximo 65,535  
			if ( (tamanio_archivo % t_bloque) > 0 ) { // contempla el caso del ultimo bloque que no llega a un tamaño de 1 mb!
				cantidad_bloques++;
			}

			cout << "cantidad de bloques a tomar: " << cantidad_bloques <<endl;

			// reposicionamiento y muestra:
			
			fseek(fd,0,SEEK_SET); // (filedescriptor, corrimiento apartir de donde nos posicionemos, hacia donde queremos posicionarnos)
			t_bloque = fread(bloque_caracteres,sizeof(unsigned char),t_bloque,fd);
		
			/*for (long int i = 0 ; i<t_bloque ; i++) {
				cout << bloque_caracteres[i];
			}*/

			// guardando en un nuevo arhivo, una etiqueta (que se agrega al final, pero va al principio del archivo) + el archivo antes abierto
			FILE *fd_new = fopen ("etiqueta_test","wb+");

			int t_bloque_aux = 8;
			unsigned char *bloque_caracteres_aux = new unsigned char[t_bloque_aux];
			
			for (int i = 0 ; i < t_bloque_aux ; i++) {
				bloque_caracteres_aux[i] = 'a';
			}

			fwrite(bloque_caracteres_aux,sizeof(unsigned char),t_bloque_aux,fd_new);  // grabo cantidad de 0s como espacio quiero reservar

			fwrite(bloque_caracteres,sizeof(unsigned char),t_bloque,fd_new); // granp el otro archivo

			fseek(fd_new,0,SEEK_SET); // volvemos al principo
	

			bloque_caracteres_aux[0] = 'h';
			bloque_caracteres_aux[1] = 'a';
			bloque_caracteres_aux[2] = 'r';
			bloque_caracteres_aux[3] = 'd';

			bloque_caracteres_aux[4] = 'c';
			bloque_caracteres_aux[5] = 'o';
			bloque_caracteres_aux[6] = 'r';
			bloque_caracteres_aux[7] = 'e';

			fwrite(bloque_caracteres_aux,sizeof(unsigned char),t_bloque_aux,fd_new); // grabo la etiqueta

			delete [] bloque_caracteres_aux;
			fclose(fd_new);
			

		} else {
			cout << "error ftell" << endl;
		}
	} else {
		cout << "error en seek inicial" << endl;
	}

	delete [] bloque_caracteres;
	fclose(fd);

}