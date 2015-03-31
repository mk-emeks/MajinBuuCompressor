#include <iostream>
#include <stdio.h>
#include <bitset>
#include <string>
#include "IO_processor.h"

#include "Nombre_archivo.h"
#include "Writer.h"

//definidas en IO_PROCESSOR
/*const unsigned char UNO = 0x01;
const unsigned char CERO = 0x00;*/

using namespace std;

int main(int argc, char *argv[]) {
		
	cout << "IO_processor Tests:" << endl; //-------------------------------------------------

	cout << "constantes definidas; UNO: " << (int)UNO << " CERO: " << (int)CERO << endl;  

	IO_processor* procesador;
	
	Nombre_archivo* nombre_archivo = new Nombre_archivo(argv[1]);
	nombre_archivo->append(".1"); 
	
	//ESCRITURA  //-------------------------------------------------

	// @BEGIN write
	procesador = new IO_processor();
	procesador->crear_archivo_para_escritura(nombre_archivo->cast_to_ptrchar());

	// @TEST: guardar menos de un byte
	/*
	procesador->escribir(UNO,false);
	procesador->escribir(UNO,false);
	procesador->escribir(UNO,false);
	procesador->escribir(UNO,true);
	*/
	
	// @TEST: guardar menos de un byte 2
	
	procesador->escribir(CERO,false);
	procesador->escribir(UNO,false);
	procesador->escribir(CERO,false);
	procesador->escribir(CERO,false);
	procesador->escribir(CERO,false);
	procesador->escribir(CERO,false);
	procesador->escribir(CERO,false);
	procesador->escribir(UNO,true);
	

	// @TEST: guardar 1 bloque y medio; termina con byte completo de eof
	
	/*
	bitset<8> aux = 'a';
	cout << aux << endl;
	bool bandera = false;
	cout << "el byte guardado es: ";
	for (int i = 0 ; i < (BUFFER_SIZE) ; i++) {
		for (int it = 7 ; it>=0 ; it--) {

			//if ( (it == 0) && (i == BUFFER_SIZE-1) ) { bandera = true; }

			cout << aux[it]; 
 			if (aux[it] == 1) {
				procesador->escribir(UNO,bandera);
			} else {
				procesador->escribir(CERO,bandera);
			}
			
		}
		cout << "-";
	}
	
	aux = 'b';
	for (int i = 0 ; i < (BUFFER_SIZE/2) ; i++) {
		for (int it = 7 ; it>=0 ; it--) {

			if ( (it == 0) && (i == ((BUFFER_SIZE/2)-1)) ) { bandera = true; }

			cout << aux[it]; 
 			if (aux[it] == 1) {
				procesador->escribir(UNO,bandera);
			} else {
				procesador->escribir(CERO,bandera);
			}
			
		}
		cout << "-";
	}*/


	//INDICES
	
	/*LInt indice = 0x34245231;
	cout << (bitset<32>)indice << endl;
	procesador->escribir(indice);
		procesador->escribir(UNO,false);
			procesador->escribir(UNO,false);
				procesador->escribir(UNO,false);
	procesador->escribir(UNO,true);
	*/
	


	// @AFTER all
	procesador->cerrar_archivo();
	delete procesador;

	//LECTURA  //-------------------------------------------------

	// @BEGIN read
	procesador = new IO_processor();
	procesador->abrir_archivo_para_lectura(nombre_archivo->cast_to_ptrchar());


	// @TEST: Leer archivo comprimido

	// recuperacion hardoce
	C_Byte* recuperados = new C_Byte[1];
	LInt t_bloque = 0;

	unsigned char c_byte_leido = '@'; // arranco inicializado de gusto, se pisa al toque
	bitset<8> aux2;
	bool no_termino = true;
	
	int i = 7; 
	do {
		//c_byte_leido = 0;
		no_termino = procesador->leer(c_byte_leido);

		if ( c_byte_leido == UNO) {
			aux2[i] = 1;
		} else{
			aux2[i] = 0;

		}
		cout << aux2[i] << " itera test; " << i << endl;

		i--;

		if ( i == -1 ) {
			i = 7;
		// grabamos lo recuperado
		recuperados[t_bloque] = UNO; // deberia hacerr un metodo para pasar los bits que se van leyendo!
		t_bloque++;		
		cout << aux2 << "-" << endl;
		}

	} while (no_termino); 
	
	// escribimos lo recuperado
	Arch_writer* escritor = new Arch_writer();

	nombre_archivo->borrar_extencion(".1");
	cout << nombre_archivo->cast_to_ptrchar()<<endl;
	cout << "tamaño del bloque a guardar" << t_bloque << endl;
	if (escritor->set_nombre_archivo_nuevo(nombre_archivo->cast_to_ptrchar())) {
		escritor->escribir(recuperados,t_bloque);
		escritor->cerrar_archivo();

	}

	
	delete escritor;

	// algo no me cerraba de hacerlo asi.
	/*while (no_termino) {
		
		aux2 = 0;
		for (int i = 7 ; i>=0; i--) {
			
			
			no_termino = procesador->leer(c_byte_leido);
			
			if ( c_byte_leido == UNO) {
				aux2[i] = 1;
			} else{
				aux2[i] = 0;
			}
			cout << aux2[i] << " itera test" << endl;

			if (!no_termino) {
				cout << "ya sos false" << endl;
				break;

			}

			
		}
		cout << aux2 << "-"<<endl;

	}*/
	

	//INDICES

	/*
	LInt indice2 = 0x00000000;
	procesador->leer(indice2);
	cout << (bitset<32>)indice2 << endl;
	*/



	// @AFTER all
	procesador->cerrar_archivo();
	delete procesador;


	// EOF
	/*
	C_Byte sub_buffer_aux;
	sub_buffer_aux = 0x80;

	cout << "el byte de prueba es: " << (bitset<8>)(sub_buffer_aux) << endl;

	bool encontre_un_1 = false;
	unsigned short int bits_EOF = 0;
	C_Byte mascara_iterador = UNO;



	while ( !encontre_un_1 ) {
		
		cout << "mascara iterador: " <<  (bitset<8>)(mascara_iterador)<< endl;

		if ( sub_buffer_aux & mascara_iterador) {
			
			cout << "encontre el uno" << endl;
			encontre_un_1 = true;
		} 

		bits_EOF++; // seguro 1 hay
		mascara_iterador = mascara_iterador << 1; // va cambiando el uno en cada posicion
	
	}	

	cout << "bits eof: " << bits_EOF << endl;
	*/


	// Nombre_Archivo

	/*
	Nombre_archivo* n_archivo = new Nombre_archivo("hola");

	cout << n_archivo->get_nombre_archivo() << endl;

	n_archivo->append(".1");

	cout << n_archivo->get_nombre_archivo() << endl;

	//n_archivo->cambiar_nombre("huna");

	if (n_archivo->borrar_extencion(".1")) { cout << "lo encontre" << endl;};
	cout << n_archivo->get_nombre_archivo() << endl;

	cout << n_archivo->cast_to_ptrchar() << endl;

	n_archivo->append("frutilli");


	cout << n_archivo->cast_to_ptrchar() << endl;

	if (n_archivo->borrar_extencion("lli")) { cout << "lo encontre" << endl;};
	cout << n_archivo->get_nombre_archivo() << endl;

	delete n_archivo;
	*/

	//fin
	delete nombre_archivo;

	return 0;
}

	/*
	cout << "Partiendo el indice en 4 bytes" << endl; //-------------------------------------------------

	unsigned long int indice = 0x1dfbaf23; // "indice prueba"

	cout << "indice original: " << indice << endl;

	unsigned char* indice_en_bytes = new unsigned char[4]; // 32 bits / 8 bits = 4 bytes 

	for ( int i = 0 ; i<4 ; i++) {

		unsigned long int indice_achicado = (indice >> 8); //le saco 1 byte

		//cout << "indice achicado: " << indice_achicado << endl;

		unsigned long int aux = ( indice_achicado << 8 ); // al long truncado lo incremento en una unidad
		indice_en_bytes[i] = indice - aux;

		cout << "indice actual: " << indice << " aux: " << aux << " parte " << i << " : " << (int)indice_en_bytes[i] << endl;

		indice = indice_achicado;

		cout << "indice nuevo: " << indice <<endl;
	}
	

	/*
	cout << "Reconstruccion del Indice:" << endl; //-------------------------------------------------

	unsigned long int indice_recontruido = 0x00000000;

	for (int i = 3 ; i>= 0 ; i--) {

		cout << "parte " << i << " : " << (unsigned long int)indice_en_bytes[i] << endl;
		unsigned long int aux = indice_en_bytes[i];

		aux = (aux << (8*i));

		indice_recontruido = indice_recontruido + aux;

		cout << "indice reconstruido parcialmente: " << indice_recontruido << endl;


	}

	delete [] indice_en_bytes;
	*/


	// CACA --------------------------------------------------------------------------------------------------

	/*cout << "test leyendo de a bits" << endl; //-------------------------------------------------
	unsigned char sub_buffer = 0x23;
	unsigned char aux;

	for (int ir = 7 ; ir >= 0 ; ir-- ){
		
		aux = sub_buffer & (0x01<< ir);
		aux = aux >> ir;

		bitset<8> byte = aux;
		cout << "la bit es " << byte << endl;
	}*/

	/*cout << "grabando bist 2" << endl; //-------------------------------------------------
	unsigned char numero = UNO;
	bitset<8> byte = numero << 7;

	numero = (numero << 7 );

	cout <<  numero  <<" en bits "<< byte << endl;

	unsigned char aux = UNO;

	aux = aux << 6;

	numero = (numero | aux) ;

	bitset<8> byte1 = numero;

	cout << numero << " en bits " << byte1 << " aux es "<< (bitset<8>)aux<< endl;*/

	/*cout << "test: Grabando bits apartir de Bytes" << endl; //-------------------------------------------------
	//unsigned short int uno = 0xffff; // ocupa 2 bytes
	//unsigned char UNO = 0x01;
	unsigned char numero = 0x00;
	unsigned char resultado = numero & UNO;

	if ( -2 ) {
		cout << (int)resultado << "tengo un uno" <<endl;
	} else {
		cout << (int)resultado << "tengo un cero" <<endl;
	}*/
	

	//bitset<8> byte = uno;

	//cout << "numero oginial: " << uno << " - " << byte << endl;


	/*
	cout << "test: Laburando con Bits" << endl; //-------------------------------------------------
	
	unsigned long int numero = 0x0000ffff;
	unsigned char c_byte = ( (numero << 8) >> 24);
	bitset<8> byte = c_byte; // verificar que el unsigned char tiene los ultimos 

	cout << "numero oginial: " << numero << " corrimiento: " << (numero >> 24) << " - " << (int)c_byte << " - "<< c_byte <<  " - " << byte << endl;
	*/

