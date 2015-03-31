
#ifndef __ARITMETICO_H__
#define __ARITMETICO_H__

#include "Declaracion_de_constantes.h"

#include "Nivel.h"
#include "IO_processor.h"

class Aritmetico {

	private:

		Modelos modelo_anterior;

		// flag de tests
		//LInt cantidad_bits_comprimidos;
		//LInt cantidad_de_breaks;

		IO_processor* io_processor;

		Nivel** niveles;

		intAritmetico piso;
		intAritmetico techo;
		intAritmetico tiraDeBits;
		intAritmetico contador_underflow;

		void actualizarLimites(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel);

		void renormalizarCompresion();

		void renormalizarDescompresion();

		void outputBitMasUnderflow(int bit, bool bandera);

		// cambio modelos:
		void ordenar_niveles();
		
		void instanciar_modelo_Estructurado();

		void instanciar_modelo_Invertido();
			void inversion_niveles();

		void instanciar_modelo_Semi_invertido();
			void acomodar_modelo_Semi_invertido();

		void instanciar_modelo_Casi_Estructurado();
			void acomodar_modelo_Casi_Estructurado();

	

		
	public:

		// constructor harcodiado para el Aritmetico estructurado
		Aritmetico();

		~Aritmetico();

		void comprimir_valor(Valor valor_procesado);

		void terminarCompresion();

		void empezar_descompresion();

		// pre: se debe invocar primero el metodo: "empezar_descompresion"
		Valor descomprimir_valor();

		// (!) hacer antes de comenzar todo el proceso
		// Pre: deberia recibir el nombre del archivo(ya con la extencion que se desea) 
		void set_nombre_archivo_compresion(char* nombre);

		void set_nombre_archivo_descompresion(char* nombre);

		IO_processor* get_IO_processor();

		//recibira alguno de los modelos del enum
		void cambiar_modelo(Modelos caracter_predominante);


		//LInt get_cantidad_bits_comprimidos();

		//LInt get_cantidad_de_breaks();
};


#endif