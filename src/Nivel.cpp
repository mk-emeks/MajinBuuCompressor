
#include "Nivel.h"

Nivel::Nivel (Valor_nivel* valores_requeridos, N_valor_nivel cantidad_valores, unsigned char numero_de_nivel , bool es_ultimo_nivel) {

	this->numero_de_nivel = numero_de_nivel;
	this->ultimo_nivel = es_ultimo_nivel;

	this->cantidad_caracteres_por_nivel = cantidad_valores; // esto se matiene estatico! (cantidad de valores - caracter especial : en este nivel) 
	this->frecuencia_total = (intAritmetico)(cantidad_valores+1); // a medida que procese caracteres va ir aumentando

	this->valores = new Valor_nivel[cantidad_valores]; // ojo: esta lista no guarda y no debe y no puede guardar al caracter especial (esc o eof)

	this->frecuencia_caracteres = new Probabilidad[frecuencia_total];  // contempla la probabilidad del escape

	int j = 0; //contador
	for (int i = (cantidad_valores-1) ; i >= 0 ; i--) // ahora si los guarda invertidos!
	{
		this->valores[j] = valores_requeridos[i];
		this->frecuencia_caracteres[j] = 1 ; // inician equiprobables
	
		// muestra las frecuencias iniciales:
		//cout <<"caracter: "<< (int)this->valores[j] << "-" << this->frecuencia_caracteres[j] << "-" << ((double)this->frecuencia_caracteres[j])/((double)frecuencia_total) << scientific << "iterador:"<< j << endl;
		j++;
	}
	// caracter especial
	this->frecuencia_caracteres[cantidad_valores] = 1;

	
}



Nivel::~Nivel() {

	delete [] valores,frecuencia_caracteres;
}

bool Nivel::es_ultimo_nivel() {
	return this->ultimo_nivel;
}


/** compresion **/
void Nivel::generar_valores_intervalo_compresion(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel, bool& hubo_caracter_especial, Valor valor_procesado){

	N_valor_nivel posicion_encontrada = buscar_caracter(valor_procesado);

	calcular_valores_caracter_compresion(piso_relativo, techo_relativo, frecuencia_nivel, hubo_caracter_especial, valor_procesado , posicion_encontrada);
	
	actualizar_frecuencia_caracteres(posicion_encontrada); // actualiza los valores del caracter
}


N_valor_nivel Nivel::buscar_caracter(Valor valor_procesado) {

	// binarySearch
	int posicion_encontrada;
	int iMax = cantidad_caracteres_por_nivel;
	int iMin = 0; 

	int iMid;

	// supone valores ordenados (ojo)
	while ( iMin < iMax ) {

		// si fuese iMid = (iMax+iMin)/2 y el buscado fuera el maximo, daria overflow!
		iMid = ( iMin + ( (iMax-iMin) / 2 ) ) ;

		if (this->valores[iMid] > valor_procesado) { // cambie el signo porque los valores estan guardados al reves!
			iMin = iMid+1;	
		} else {
			iMax = iMid;
		}
	
	}

	/* 
		Comparando luego del ciclo, recorre un par mas de veces, pero siempre te da el primer miembro que cumple.
		Esto sirve en caso de que alla mas de uno que cumple la condicion 
	*/

	if ( (iMin == iMax) && (valores[iMin] == valor_procesado) ) {
		posicion_encontrada = iMin;
		//cout << "la posicion del caracter encontrado es: "<< posicion_encontrada <<  endl;
	} else {
		posicion_encontrada = cantidad_caracteres_por_nivel; // se emite escape!
		//cout << "la posicion del escape encontrado es: "<< posicion_encontrada <<  endl;
	}

	return posicion_encontrada;

}

void Nivel::inicializar_valores(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel){
	piso_relativo = 0;
	techo_relativo = 0;
	frecuencia_nivel = 0;
}


void Nivel::calcular_valores_caracter_compresion(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel, bool& hubo_caracter_especial, Valor valor_procesado , N_valor_nivel& posicion_encontrada){

	inicializar_valores(piso_relativo,techo_relativo,frecuencia_nivel); // (!)

	if ( posicion_encontrada == cantidad_caracteres_por_nivel)
	{	
		hubo_caracter_especial = true;

		techo_relativo = frecuencia_caracteres [posicion_encontrada]; // calculo cota superior
		// calculo cota inferior -> siempre es 0 (caracter especial esta primero)

		//cout << "ESC o EOF" <<" posicion encontrada: " << (int)posicion_encontrada << " piso relativo: " << piso_relativo << " techo_relativo: " << techo_relativo  << " frec_T "<< frecuencia_total << endl;

	} else {

		hubo_caracter_especial = false;

		//cout <<"posicion encontrada: " << (int)posicion_encontrada  << endl;
		for ( int i = posicion_encontrada ; i >= 0; i--)
		{
			techo_relativo = techo_relativo + frecuencia_caracteres[i]; // calculo cota superior
		}
		// (!) suma frecuencia del caracter especial porque el caracter especial esta al principio
		techo_relativo = techo_relativo + frecuencia_caracteres [cantidad_caracteres_por_nivel]; 
	
		piso_relativo = techo_relativo - frecuencia_caracteres[posicion_encontrada]; //calculo cota inferior

		//cout << " piso relativo: " << piso_relativo << " techo_relativo: " << techo_relativo  << " frec_T "<< frecuencia_total << endl;
	}		
	
	/**!**/
	frecuencia_nivel = frecuencia_total;

}

/** fin compresion **/

/** descompresion **/
intAritmetico Nivel::get_frecuencia_nivel() {

	return this->frecuencia_total;

}

void Nivel::generar_valores_intervalo_descompresion(intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico frecuenciaAcumulada, bool& hubo_caracter_especial, unsigned short int& posicion_caracter){
	
	intAritmetico acumulador;
	generar_valores_descompresion_entre_intervalos_de_caracteres(acumulador,piso_relativo, techo_relativo,frecuenciaAcumulada,hubo_caracter_especial,posicion_caracter);
	
	// actualizar_frecuencia_caracteres(posicion_caracter);
}

// metodo auxiliar: va buscando en que intervalo el techo es mayor que la frecuencia acumulada
void Nivel::generar_valores_descompresion_entre_intervalos_de_caracteres(intAritmetico& acumulador,intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico frecuenciaAcumulada, bool& hubo_caracter_especial, unsigned short int& posicion_caracter){

	// inicializo
	acumulador = 0; 

	// le sumo la frecuencia del caracter especial, ya que esta en el nivel mas bajo
	acumulador = frecuencia_caracteres[cantidad_caracteres_por_nivel]; 

	if (acumulador > frecuenciaAcumulada){ 

		piso_relativo = 0;
		techo_relativo = acumulador;
		hubo_caracter_especial = true;
		
		if (this-> ultimo_nivel) { // ver!

			posicion_caracter = eof;
			// si estoy en EOF NO ACTUALIZO!

		} else {

			posicion_caracter = this->cantidad_caracteres_por_nivel;
			actualizar_frecuencia_caracteres(posicion_caracter);
		}
		

	}else{ // (!) si ya entro aca quiere decir que no es caracter especial -> lo tiene que encontrar

		for (int i = 0 ; i < cantidad_caracteres_por_nivel ; i++) {
			
			acumulador = acumulador + frecuencia_caracteres[i]; 
			
			if (acumulador > frecuenciaAcumulada){ // se fija si el caracter tiene el piso igual al de frecuenciaAcumulada			
					
				techo_relativo = acumulador;	
				piso_relativo = acumulador - frecuencia_caracteres[i]; // encontro el caracter con esa frecuncia acumulada
				posicion_caracter = i;
				hubo_caracter_especial = false;
				actualizar_frecuencia_caracteres(posicion_caracter);

			break; // deja de iterar porque ya lo encontro

			}else {
				//cout << "aun no lo encontre" << endl;
			}
  		}
	} 

}


Valor Nivel::get_valor(unsigned short int posicion) {

	if (posicion == eof) {
		return eof;
	} else {
		return valores[posicion];
	}
}

/** fin descompresion **/

// obs: lo usa tanto la descompresion como la compresion
void Nivel::actualizar_frecuencia_caracteres(N_valor_nivel posicion_encontrada){

	this->frecuencia_total++;
	this->frecuencia_caracteres[posicion_encontrada]++;
}

Valor_nivel* Nivel::get_valores() {
	return this->valores;
}


void Nivel::set_ultimo_nivel(bool flag) {

	this->ultimo_nivel = flag;
}

unsigned char Nivel::get_numero_de_nivel() {
	return this->numero_de_nivel;
}