
#ifndef __IO_PROCESSOR_H__
#define __IO_PROCESSOR_H__

#include "Declaracion_de_constantes.h"

// Definiciones:

typedef unsigned char C_Byte; // -> compress byte

const LInt BUFFER_SIZE = 5000000; // (!) puede ser distinto a la MEMORIA_POR_ACCESO ? Si
const short unsigned int SUB_BUFFER_SIZE = 8; // (!): debe iterar 8 veces para completar un byte

const C_Byte UNO = 0x01;
const C_Byte CERO = 0x00;

// Clase Input-Output Processor:
/*
	Esta clase controla de a un archivo por vez. Y se usa para leer o generar un archivo comprimido.
*/

class IO_processor {

	private:

		FILE* fd;

		C_Byte* buffer;	
		LInt iterador_buffer;
		LInt maximo_leido; 
		
		C_Byte sub_buffer;
		short unsigned int iterador_sub_buffer; 
		short unsigned int iterador_inverzo_sub_buffer;
		short unsigned int maximo_leido_sub_buffer;

		/*
			Excepcion que evita que el IO_processor aumente el tamaño del archivo en caso que el usuario cometa
			el error de querer seguir escribiendo cuando antes dijo que habia escrito el ultimo bit.
		*/
		bool exception_intentar_escribir_despues_del_ultimo_bit;

		/* overhead **/
		unsigned short cantidad_de_bloques;

		void iniciar_buffer();

		void resetiar_buffer(); // resetea iterador

		void resetiar_sub_buffer(); // resetea y limpia

			void resetiar_contadores_sub_buffer(); // resetea contadores
			void limpiar_sub_buffer(); // pone al sub buffer todo 0's

		// ESCRITURA

		void escribir_en_el_buffer(bool es_el_ultimo_byte);

		void agregar_en_buffer(C_Byte un_byte);

		void agregar_en_sub_buffer(C_Byte bit_representado_en_byte);

		void guardar_contenido_del_buffer_en_el_archivo();


		// LECTURA

		bool cargar_buffer_con_contenido_del_archivo();

		bool leer_del_buffer();

		void cargar_sub_buffer_con_contenido_del_buffer();

		void leer_bit_del_sub_buffer(C_Byte& bit_representado_en_byte);

		bool notEndOfFile();

	public:

		// obs: si no estan codiados no deberian aparecer
		//IO_processor();
		//~IO_processor();

		// obs: se recomienda fuertemente utilizar este metodo cuando no se requiera hacer nada mas con el archivo en cuestion
		void cerrar_archivo();

 			// Metodos para la compresion de un archivo .1

		/*
			post: devuelve 1 si pudo crear el archivo correctamente, y 0 si a ocurrido un error.
				  Crear un archivo del tipo: "nombre_del_archivo".1
		*/
		void crear_archivo_para_escritura(char* nombre_del_archivo); // POR AHORA NO VERIFICAMOS NADA!!!!!!!!!!!!!!!!!!!!!

		/*
			pre: se debe crear primero el archivo con el metodo "crear_archivo_para_escribir".
				 Se debe pasar el bit que se quiere escribir y si sera el ultimo.
		
			ejemplo: el valor esperado de 'bit_representado_en_byte' es;
				0x01 byte -> 1 bit
				0x00 byte -> 0 bit
		*/	
		void escribir(C_Byte bit_representado_en_byte , bool es_el_ultimo_bit);

			// Metodos para la descompresion de un archivo .1

		// pre: para obtener un correcto funcionamiento el nombre del archivo deberia ser .1 (extencion de nuestros archivos comprimidos)
		// post: devuelve 1 si pudo abrir el archivo correctamente, y 0 si a ocurrido un error
		void abrir_archivo_para_lectura(char* nombreDelArchivo);  // POR AHORA NO VERIFICAMOS NADA!!!!!!!!!!!!!!!!!!!!!

		/*
			pre: se debe abrir primero el archivo con el metodo "arbir_archivos_para_lectura".
	 			 Se le pasa el "contenedor" donde guarda el byte emitido.
	 	*/
		// post: false indica que se termino el archivo(lo que leyo es basura). True que se leyo correctamente
		C_Byte leer();

		/** OVERHEAD **/

		// pre: hacerlo luego de set_nombre_archivo_compresion **/
	    void reservar_espacio_para_overhead(LInt espacio_reservado);

	    // usar de forma consecutiva segun este orden
	    void escribir_en_overhead( unsigned short cantidad_de_bloques );

	    void escribir_en_overhead( LInt* indices, unsigned short cantidad_de_bloques );

	    void escribir_en_overhead( LInt cantidad_de_sub_bloques );

	    void escribir_en_overhead ( UChar* modelos , LInt cantidad_modelos_agrupados_por_byte);

	    // usar de forma consecutiva segun este orden
		unsigned short get_cantidad_bloques();

		void cargar_overhead(LInt* bloque_de_indices);

		LInt get_cantidad_de_sub_bloques();

		void cargar_overhead(UChar* modelos_agrupados_por_byte, LInt cantidad_modelos_agrupados_por_byte);
};

#endif