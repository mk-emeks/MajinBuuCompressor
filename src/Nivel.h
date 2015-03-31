
#ifndef __NIVEL_H__
#define __NIVEL_H__

#include "Declaracion_de_constantes.h"

// Te permite pedirle valores de un rango mayor, al igual que enviar de parametro un unsigned short int.
typedef unsigned short int Valor;

// propios del manejo interno de la clase nivel
typedef unsigned char Valor_nivel; // el labura de 0 a 255 max caracteres
typedef unsigned char N_valor_nivel; // el labura de 0 a 255 max "posiciones"

typedef unsigned long int Probabilidad;
typedef unsigned long int intAritmetico; // soy conceptualmente un forro, cambiar a long long!!!!

const unsigned short int eof = 256; // (!) no me deja declarar la constante

class Nivel {

	private:

		N_valor_nivel cantidad_caracteres_por_nivel; 
		intAritmetico frecuencia_total; // cantidad de valores observados hasta el momento

		// los "vectores" son asociativos!
		Valor_nivel* valores; // lista de valores
		Probabilidad* frecuencia_caracteres; // lista con la cantidad de veces que se vio cada valor 

		bool ultimo_nivel; // sirve para indicar si es ultimo nivel -> en ese caso, el esc no debe ser contemplado.
		unsigned char numero_de_nivel; // les puse un orden para simplificar su ordenamiento -> es como un ID (!)

		// METODOS PRIVADOS:

		/** compresion **/
		void inicializar_valores(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel);

		N_valor_nivel buscar_caracter(Valor valor_procesado);

		void calcular_valores_caracter_compresion(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel, bool& hubo_caracter_especial, Valor valor_procesado , N_valor_nivel& posicion_encontrada);
		/** **/


		/** descompresion **/

		// va buscando en que intervalo el techo es mayor que la frecuencia acumulada
		void generar_valores_descompresion_entre_intervalos_de_caracteres(intAritmetico& acumulador,intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico frecuenciaAcumulada, bool& hubo_caracter_especia, unsigned short int& posicion_caracter);	

		/** **/

		// neutro (lo usan los dos)
		// se la pasa la posicion donde se encuentra el caracter procesado y la aumenta, tmb aumenta la frecuencia total.
		void actualizar_frecuencia_caracteres(N_valor_nivel posicion_encontrada);

	public:

		Nivel (Valor_nivel* valores_requeridos, N_valor_nivel cantidad_valores, unsigned char numero_de_modelo , bool es_ultimo_nivel);

		~Nivel();

		bool es_ultimo_nivel();
	
		void generar_valores_intervalo_compresion(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel, bool& hubo_caracter_especial, Valor valor_procesado);

		/** descompresion **/
		intAritmetico get_frecuencia_nivel(); 

		void generar_valores_intervalo_descompresion(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico frecuenciaAcumulada, bool& hubo_escape, unsigned short int& posicion_caracter);

		// este metodo supone que lo que se pide esta dentro de los limites del nivel, sino picha como el mejor.
		// ya que devolvera cosas apuntadas en memoria invalidas (peor caso, sean parecidas a o lo que uno espera)
		// es unsigned short int, simplemente para poder soportar el eof.
		Valor get_valor(unsigned short int posicion);
		/** **/

		Valor_nivel* get_valores();

		// sirve para setiar el atributo de ser o no ser el ultimo nivel
		void set_ultimo_nivel(bool flag);

		unsigned char get_numero_de_nivel();


};



#endif
