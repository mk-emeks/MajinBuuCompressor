
#include "Aritmetico.h"

#define minimo 0x00000000
#define maximo 0xFFFFFFFF

#define primerCuarto (maximo/4 + 1)
#define mitad (2*primerCuarto)
#define tercerCuarto (3*primerCuarto)

const short int CANTIDAD_NIVELES = 9; // constante cantidad de nivel para un modelo (!)

Aritmetico::Aritmetico() {

	modelo_anterior = Estructurado;

	//cantidad_bits_comprimidos = 0;
	//cantidad_de_breaks = 0;

	this->piso = minimo;
	this->techo = maximo; // F->f es lo mismo!    
	this->tiraDeBits = 0;
	contador_underflow = 0; // este es el contador que ira acumulando los casos cuando se de underflow. 


	// Inicializamos Niveles
 	this->niveles = new Nivel*[CANTIDAD_NIVELES]; // pedimos memoria para la "lista"

	//nivel 0 
	Valor_nivel valores_nivel0[1] = { 0 };
	unsigned char i = 0; // contador para inicializar el numero de nivel
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel0, 1, i, false);

	// nivel 1 
	Valor_nivel valores_nivel1[1] = { 1 };
	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel1, 1, i, false);

	// nivel 2 
	Valor_nivel valores_nivel2[2] = { 2 , 3 };
	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] =  new Nivel(valores_nivel2, 2, i, false);

	// nivel 3
	Valor_nivel valores_nivel3[4] = { 4 , 5 , 6 , 7 };
	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel3, 4, i, false);

	// nivel 4 
	Valor_nivel valores_nivel4[8] = { 8 , 9 , 10 , 11 , 12 , 13 , 14 , 15 };
	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel4, 8, i, false);

	// nivel 5 
	Valor_nivel valores_nivel5[16];

	for (int i = 0 ; i < 16 ; i++)
	{
		valores_nivel5[i] = 16+i;
	}

	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel5, 16, i, false);

	// nivel 6 
	Valor_nivel valores_nivel6[32];

	for (int i = 0 ; i < 32 ; i++)
	{
		valores_nivel6[i] = 32+i;
	}

	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel6, 32, i, false);

	// nivel 7 
	Valor_nivel valores_nivel7[64];

	for (int i = 0 ; i < 64 ; i++)
	{
		valores_nivel7[i] = 64+i;
	}

	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel7, 64, i, false);

	// nivel 8 
	Valor_nivel valores_nivel8[128];

	for (int i = 0 ; i < 128 ; i++)
	{
		valores_nivel8[i] = 128+i;
	}

	i++;
	//cout << "nivel:" << i << endl;
	this->niveles[i] = new Nivel(valores_nivel8, 128, i, true);

 
}


// (!)
void Aritmetico::set_nombre_archivo_compresion(char* nombre) {

	io_processor = new IO_processor();
	io_processor->crear_archivo_para_escritura(nombre);
}

void Aritmetico::set_nombre_archivo_descompresion(char* nombre) {

	io_processor = new IO_processor();
	io_processor->abrir_archivo_para_lectura(nombre);
}

Aritmetico::~Aritmetico() {

	for (int i = 0 ; i < CANTIDAD_NIVELES ; i++) {
		delete niveles[i];
	}

	delete[] niveles;

	// (!)
	delete io_processor;

	
}

void Aritmetico::comprimir_valor(Valor valor_procesado) {

	/* Si valor_procesado = eof, salgo del proceso. */
	//cout << "valor: " << valor_procesado << endl;
	bool fue_procesado = false;

	intAritmetico piso_relativo,techo_relativo,frecuencia_nivel;
	bool hubo_caracter_especial;

	unsigned short int iterador = 0;

	while (!fue_procesado) {

		//cout << "comprimo en el nivel: "<< iterador << endl;

		this->niveles[iterador]->generar_valores_intervalo_compresion(piso_relativo,techo_relativo,frecuencia_nivel,hubo_caracter_especial,valor_procesado);

		if ( hubo_caracter_especial ) { // se emitio un escape o es_eof

			if (this->niveles[iterador]->es_ultimo_nivel()) { // si es escape tiene que parar.

				fue_procesado = true;

			} else{
				
				iterador++;

			}
			
		} else { 

			fue_procesado = true;

		}

		actualizarLimites(piso_relativo,techo_relativo,frecuencia_nivel);
		renormalizarCompresion();
	
	}
	
}

void Aritmetico::actualizarLimites (intAritmetico& piso_relativo, intAritmetico& techo_relativo, intAritmetico& frecuencia_nivel) {

	intAritmetico cota_inferior_relativa = piso_relativo;
	intAritmetico cota_superior_relativa = techo_relativo;
	intAritmetico frecuencia_total_relativa_al_nivel = frecuencia_nivel;

    unsigned long long int rango = (unsigned long long int)( this->techo - this->piso ) + 1;

    this->techo = this->piso + (intAritmetico) (/*del cast*/(/*divide*/(unsigned long long int)(rango * cota_superior_relativa) / frecuencia_total_relativa_al_nivel) - 1);
    this->piso = this->piso + (intAritmetico) (((unsigned long long int)(rango * cota_inferior_relativa)) / frecuencia_total_relativa_al_nivel);
  
}


void Aritmetico::renormalizarCompresion() {
	
	while (true) {
		if (techo < mitad) {
			outputBitMasUnderflow(0, false);
			//cout << "techo < mitad" << endl;
		}
		else if (piso >= mitad) {
			outputBitMasUnderflow(1, false);
			this->piso -= mitad;
			this->techo -= mitad;
			//cout << "piso >= mitad";
		}
		else if (piso >= primerCuarto && techo < tercerCuarto) {
			contador_underflow++;
			piso -= primerCuarto;
			techo -= primerCuarto;
			//cout << "underflow" << endl;
		}
		else {
			//this->cantidad_de_breaks++;
			break;
		}

		piso = 2*piso;
		techo = 2*techo+1;

	}
}

void Aritmetico::terminarCompresion() {

	contador_underflow++;
	if (piso < primerCuarto) {
		outputBitMasUnderflow(0, true);
	}
	else outputBitMasUnderflow(1, true);
	
}


void Aritmetico::outputBitMasUnderflow(int bit, bool finDeCompresion) {

	bool es_el_ultimo_bit = false;

	if (bit == 1) {
		//cout << "1";
		//cantidad_bits_comprimidos++;
		this->io_processor->escribir(UNO, es_el_ultimo_bit);
		while(contador_underflow > 0) {
			if (finDeCompresion && contador_underflow == 1) {
				es_el_ultimo_bit = true;
			}
			//cout << "0";
			//cantidad_bits_comprimidos++;
			this->io_processor->escribir(CERO, es_el_ultimo_bit);
		
			contador_underflow--;
		}
	}
	else {
		//cout << "0";
		//cantidad_bits_comprimidos++;
		this->io_processor->escribir(CERO, es_el_ultimo_bit);
		while(contador_underflow > 0) {
			if (finDeCompresion && contador_underflow == 1) {
				es_el_ultimo_bit = true;
			}
			//cout << "1";
			//cantidad_bits_comprimidos++;
			this->io_processor->escribir(UNO, es_el_ultimo_bit);
			contador_underflow--;
		}
	}
}

// (!) obs: esta por fuera
void Aritmetico::empezar_descompresion() {
	
	C_Byte bit;
	for (int i = 0; i < 32; i++) {

		bit = this->io_processor->leer(); // si llego a estar en fin de archivo devuelvo CERO
		tiraDeBits = 2*tiraDeBits+bit; 
	}
	
}

// pre: se debe invocar primero el metodo: "empezar_descompresion"
Valor Aritmetico::descomprimir_valor(){
	
	intAritmetico piso_relativo,techo_relativo,frecuencia_relativa, frecuenciaAcumulada;
	
	unsigned short int posicion_caracter;
	bool hubo_caracter_especial;

	bool fue_procesado = false;
	unsigned short int iterador = 0; // es para recuperar al nivel al que peretence el caracter

	while (!fue_procesado) {

		//cout << " el nivel es: " << iterador << endl;
		unsigned long long int rango = (unsigned long long int)( this->techo - this->piso ) + 1;
		
		//la verdad que hasta el momento no estoy muy seguro como funciona esto
		intAritmetico frecuencia_relativa = this->niveles[iterador]->get_frecuencia_nivel();
		//cout << "frecuenciaRelativaDelNivel: " << frecuencia_relativa << endl;
		frecuenciaAcumulada = (intAritmetico)((((unsigned long long int)(tiraDeBits - piso)+1)*frecuencia_relativa-1) / rango);
		//cout << "frecuenciaAcumulada: " << frecuenciaAcumulada << endl;
		this->niveles[iterador]->generar_valores_intervalo_descompresion(piso_relativo, techo_relativo, frecuenciaAcumulada ,hubo_caracter_especial, posicion_caracter);
		//cout << "nivel: " << iterador << endl;
		//cout << "piso: " << piso_relativo << " techo: " << techo_relativo << " escape: " << hubo_escape << " posChar: " << posicion_caracter << endl;

		if (hubo_caracter_especial) { // quiere decir que se emitio un escape, por lo tanto tiene que seguir recorriendo los niveles
		
			if ( this->niveles[iterador]->es_ultimo_nivel() ) {

				fue_procesado = true;

			} else {

				iterador++;
			}
			
		
		} else {

			fue_procesado = true; // lo tenemos!

		}	

		actualizarLimites(piso_relativo,techo_relativo,frecuencia_relativa); // aca actualiza los intervalos techo y piso
		renormalizarDescompresion();
	
	}

	return this->niveles[iterador]->get_valor(posicion_caracter);
	

}

void Aritmetico::renormalizarDescompresion() {

	C_Byte bit;

	while(true) {
		if (techo < mitad) {
			/*NADA*/
		}
		else if (piso >= mitad) {
			tiraDeBits -= mitad;
			piso -= mitad;
			techo -= mitad;
		}
		else if (piso >= primerCuarto && techo < tercerCuarto) {
			tiraDeBits -= primerCuarto;
			piso -= primerCuarto;
			techo -= primerCuarto;
		}
		else break;
		piso = 2*piso;
		techo = 2*techo+1;

		bit = this->io_processor->leer(); // si llego a estar en fin de archivo devuelvo CERO
		tiraDeBits = 2*tiraDeBits+bit;

	}
}

IO_processor* Aritmetico::get_IO_processor(){

	return this->io_processor;
}

void Aritmetico::cambiar_modelo(Modelos caracter_predominante){

	switch(caracter_predominante) {

		case Estructurado: instanciar_modelo_Estructurado(); 

		break; 

		case Semi_invertido: instanciar_modelo_Semi_invertido();

		break;

		case Casi_Estructurado: instanciar_modelo_Casi_Estructurado();

		break;

		case Invertido: instanciar_modelo_Invertido();

		break;

		//default: instrucción(es);

	};
	
}


void Aritmetico::ordenar_niveles(){

	// burbujeo comun
    for (int i = 0; i< CANTIDAD_NIVELES; i++) {
        
        for (int j = 1; j< CANTIDAD_NIVELES; j++) {
            
            if (this->niveles[j-1]->get_numero_de_nivel() > this->niveles[j]->get_numero_de_nivel()) {

                Nivel* n_aux = this->niveles[j];
                this->niveles[j] = this->niveles[j-1];
                this->niveles[j-1] = n_aux;
            }
        }
    }

    // buscar forma eficiente metida en buble!!!! para que solo cambie en los 2 que tiene que cambiar!
    // reasigna el orden de las banderas
    for (int i = 0 ; i < CANTIDAD_NIVELES-1; i++) {
    	this->niveles[i]->set_ultimo_nivel(false);
    }
    this->niveles[CANTIDAD_NIVELES-1]->set_ultimo_nivel(true);
}


void Aritmetico::instanciar_modelo_Estructurado() {

	if (this->modelo_anterior != Estructurado) { // arrana en Estructurado
		ordenar_niveles(); // reordenar
		this->modelo_anterior = Estructurado;
	}

}

void Aritmetico::instanciar_modelo_Invertido() {

	if ( this->modelo_anterior != Invertido ) {
		ordenar_niveles();
		inversion_niveles();
		this->modelo_anterior = Invertido;
	}	

}

void Aritmetico::inversion_niveles() {

	// 8,7,6,5,4,3,2,1,0
	short int j = (CANTIDAD_NIVELES-1);
	for ( short int i = 0 ; i < (CANTIDAD_NIVELES / 2) ; i++) {

		Nivel* nivel_aux = this->niveles[i];
		this->niveles[i] = this->niveles[j];
		this->niveles[j] = nivel_aux;

		if (i == 0) { // si es la primera vez.
			//cambiar quien es el ultimo nivel!
			this->niveles[i]->set_ultimo_nivel(false);
			this->niveles[j]->set_ultimo_nivel(true);
		}

		j--;
	}
}

void Aritmetico::instanciar_modelo_Semi_invertido(){

	if ( this->modelo_anterior != Semi_invertido ) {
		ordenar_niveles();
		acomodar_modelo_Semi_invertido();
		this->modelo_anterior = Semi_invertido;
	}	
}

// se invierte solo la primer mitad
void Aritmetico::acomodar_modelo_Semi_invertido() {

	// 4,3,2,1,0,5,6,7,8 -> not best
	short int j = 4;
	for ( short int i = 0 ; i < 3 ; i++) {

		Nivel* nivel_aux = this->niveles[i];
		this->niveles[i] = this->niveles[j];
		this->niveles[j] = nivel_aux;

		j--;

	}

	// 3,4,2,1,0,5,6,7,8 -> mejor xD
	Nivel* aux = this->niveles[0];
	this->niveles[0] = this->niveles[1];
	this->niveles[1] = aux;

}

void Aritmetico::instanciar_modelo_Casi_Estructurado() {

	if ( this->modelo_anterior != Casi_Estructurado ) {
		ordenar_niveles();
		acomodar_modelo_Casi_Estructurado();
		this->modelo_anterior = Casi_Estructurado;
	}	

}

void Aritmetico::acomodar_modelo_Casi_Estructurado() {

	// 0,3,2,1,4,5,6,7,8
	short int j = 3;
	for ( short int i = 1 ; i < 2 ; i++) {  // una sola vez!

		Nivel* nivel_aux = this->niveles[i];
		this->niveles[i] = this->niveles[j];
		this->niveles[j] = nivel_aux;

		j--;
	}
}


/*LInt Aritmetico::get_cantidad_bits_comprimidos() {
	return this->cantidad_bits_comprimidos;
}


LInt Aritmetico::get_cantidad_de_breaks() {
	return this->cantidad_de_breaks;
}*/