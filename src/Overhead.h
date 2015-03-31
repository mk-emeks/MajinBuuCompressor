
#ifndef __OVERHEAD_H__
#define __OVERHEAD_H__

#include "Declaracion_de_constantes.h"
#include "IO_processor.h"

class Overhead {

	private:

		LInt espacio_reservado;

		// indices
		LInt* bloque_de_indices; 
		unsigned short cantidad_de_bloques; 

		unsigned short it_indices;

		// modelos
		Modelos* modelos_predominantes; 
		LInt cantidad_de_sub_bloques; // cuantos modelos hubo!

		LInt it_modelos;

		UChar* modelos_agrupados_por_byte;
		LInt cantidad_modelos_agrupados_por_byte;


		void calcular_cantidad_modelos_agrupados_por_byte();
		void iniciar_modelos_predominantes();
		
		void agrupar_modelos_por_byte();
		UChar representar_modelo_en_byte(Modelos modelo); 

		void desagrupar_modelos_por_byte();
		
		void asignar_modelo_segun_representacion_en_byte(UChar representacion_modelo);

	public:
		// compresion
		Overhead(LInt t_archivo);
		~Overhead();

		// descompresion
		Overhead(IO_processor* processor);

		// initialize
		LInt get_espacio_reservado();

		// mientras comprimo
		void guardar_indice(LInt indice);

		void guardar_modelo_predominante(Modelos modelo);
	
		// para guardar compresion
		LInt* get_bloque_de_indices();

		unsigned short get_cantidad_de_bloques();

		// pre lectura descompresion
		LInt get_cantidad_de_sub_bloques();

		UChar* get_modelos_predominantes_agrupados_por_byte();

		LInt get_cantidad_modelos_agrupados_por_byte();	

		// para leer al descomprimir
		LInt get_indice_actual();

		Modelos get_modelo_predominante_actual();
	
		

};



#endif