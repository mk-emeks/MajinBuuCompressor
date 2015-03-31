
#include "Overhead.h"

// para descompresion!
Overhead::Overhead(IO_processor* processor){

	// indices
	this->cantidad_de_bloques = processor->get_cantidad_bloques();

	this->bloque_de_indices = new LInt[this->cantidad_de_bloques];
	processor->cargar_overhead(this->bloque_de_indices);

	// modelos
	this->cantidad_de_sub_bloques = processor->get_cantidad_de_sub_bloques();
	calcular_cantidad_modelos_agrupados_por_byte(); 

	this->modelos_agrupados_por_byte = new UChar[this->cantidad_modelos_agrupados_por_byte];
	processor->cargar_overhead(this->modelos_agrupados_por_byte,this->cantidad_modelos_agrupados_por_byte);

	iniciar_modelos_predominantes();

	desagrupar_modelos_por_byte(); // carga el vector de modelos predominantes segun los modelos recuperados por el processor

	// setiar iteradores;
	this->it_modelos = 0;
	this->it_indices= 0;
	
}

// para compresion!
Overhead::Overhead(LInt	t_archivo) {

	// indices -> calculo cantidad de bloques
	this-> cantidad_de_bloques = (unsigned short)(t_archivo / MEMORIA_POR_ACCESO);
	if ( (t_archivo % MEMORIA_POR_ACCESO) > 0 ) {
		this->cantidad_de_bloques++;
	}

	// Modelos -> calculo cantidad sub bloques
	this->cantidad_de_sub_bloques = (LInt)(t_archivo / TAMANIO_SUB_BLOQUE);
	if ( (t_archivo % TAMANIO_SUB_BLOQUE) > 0 ) {
		this->cantidad_de_sub_bloques++;
	}

	calcular_cantidad_modelos_agrupados_por_byte();

	// OJO con los tipos de espacio_reservado a la hora de comprimir archivos muy grandes. 
	this->espacio_reservado = 0; // (medido en bytes!)
	this->espacio_reservado += (LInt) (sizeof(unsigned short)); // espacio para guardar la cantidad de indices
	this->espacio_reservado += (LInt) (sizeof(LInt))*cantidad_de_bloques; // espacio para guardar todos los indices de los bloques
	this->espacio_reservado += (LInt) (sizeof(LInt)); // espacio para guardar la cantidad de sub bloques
	this->espacio_reservado += (LInt) (sizeof(UChar))*cantidad_modelos_agrupados_por_byte; // espacio para guardar los indices de los sub-bloques

	//cout << "cantidad bloques: " << this->cantidad_de_bloques<< " cantidad sub bloques: " << this->cantidad_de_sub_bloques << " espacio reservado: " << this->espacio_reservado << endl;


	// creamos los vectores donde iremos guardando el overhead de indices y modelos:
	this->bloque_de_indices = new LInt [this->cantidad_de_bloques];
	this->it_indices = 0;

	iniciar_modelos_predominantes();

	this->modelos_agrupados_por_byte = new UChar[this->cantidad_modelos_agrupados_por_byte];

}

void Overhead::calcular_cantidad_modelos_agrupados_por_byte() {

	// tamanio real en bytes de los modelos
	this->cantidad_modelos_agrupados_por_byte = (LInt) ( cantidad_de_sub_bloques / OVERHEAD_MODELOS_POR_BYTE );
	if ( (cantidad_de_sub_bloques % OVERHEAD_MODELOS_POR_BYTE) > 0) {
		this->cantidad_modelos_agrupados_por_byte++;
	}
	//cout << "cantidad_modelos_agrupados_por_byte: " << this->cantidad_modelos_agrupados_por_byte << endl;
}

void Overhead::iniciar_modelos_predominantes(){

	this->modelos_predominantes = new Modelos[this->cantidad_de_sub_bloques];
	this->it_modelos = 0;
}

Overhead::~Overhead(){
	delete[] this->bloque_de_indices; 
	delete[] this->modelos_predominantes;
	delete[] this->modelos_agrupados_por_byte;
}

LInt Overhead::get_espacio_reservado(){
	return this->espacio_reservado;
}


LInt* Overhead::get_bloque_de_indices(){
	return this->bloque_de_indices;
}

unsigned short Overhead::get_cantidad_de_bloques(){
	return this->cantidad_de_bloques;
}

LInt Overhead::get_cantidad_de_sub_bloques(){
	return this->cantidad_de_sub_bloques;
}

LInt Overhead::get_cantidad_modelos_agrupados_por_byte(){
	return this->cantidad_modelos_agrupados_por_byte;
}

// (!) mientas vas comprimiendo
void Overhead::guardar_indice(LInt indice){

	this->bloque_de_indices[this->it_indices] = indice;
	this->it_indices++;
}

void Overhead::guardar_modelo_predominante(Modelos modelo){

	this->modelos_predominantes[this->it_modelos] = modelo;
	this->it_modelos++;
}
// (!) mientas vas descomprimiendo
LInt Overhead::get_indice_actual(){
	this->it_indices++;
	return this->bloque_de_indices[(this->it_indices)-1];
}

Modelos Overhead::get_modelo_predominante_actual(){
	this->it_modelos++;
	return this->modelos_predominantes[(this->it_modelos)-1];
}



// pre: hacerlo cuando ya se hayan guardado todos los modelos
UChar* Overhead::get_modelos_predominantes_agrupados_por_byte() {
	agrupar_modelos_por_byte();
	return this->modelos_agrupados_por_byte;

}

// pre: hacerlo cuando ya se hayan guardado todos los modelos
void Overhead::agrupar_modelos_por_byte() {

	LInt it_modelos_agrupado_por_byte = 0;

	int max_iteraciones_buffer = OVERHEAD_MODELOS_POR_BYTE; // 4
	
	LInt i = 0;
	while( i < this->cantidad_de_sub_bloques ) {

		UChar buffer = 0x00;
		int j = 0;
		while( j < (max_iteraciones_buffer) && (i < this->cantidad_de_sub_bloques) ){
			
			buffer =  buffer | representar_modelo_en_byte(this->modelos_predominantes[i]);

			if (j < max_iteraciones_buffer-1) {
				buffer = buffer << 2;
				
			}
			j++;
			i++;
		}
		if (j == max_iteraciones_buffer) {
			this->modelos_agrupados_por_byte[it_modelos_agrupado_por_byte] = buffer;
			it_modelos_agrupado_por_byte++;
		}
		else {
			buffer = buffer << (2*(3-j));
			this->modelos_agrupados_por_byte[it_modelos_agrupado_por_byte] = buffer;
			it_modelos_agrupado_por_byte++;
		}

	}
	
}

UChar Overhead::representar_modelo_en_byte(Modelos modelo) {

	UChar modelo_representado_en_byte;

	switch(modelo) {

		case Estructurado: modelo_representado_en_byte = 0x00; 

		break; 

		case Invertido: modelo_representado_en_byte = 0x01; 
	
		break;

		case Semi_invertido: modelo_representado_en_byte = 0x02; 

		break;

		case Casi_Estructurado: modelo_representado_en_byte = 0x03; 

		break;

		};

	return modelo_representado_en_byte;
}

// pre: hacerlo cuando se leyo la cantidad_de_sub_bloques y los modelos_agrupados_por_byte
// post: te deja ya cargado el vector modelos_predominantes
void Overhead::desagrupar_modelos_por_byte(){




	UChar buffer,mascara,representacion_modelo;
	int max_iteraciones_buffer = OVERHEAD_MODELOS_POR_BYTE;

	for ( LInt i = 0; i < this->cantidad_modelos_agrupados_por_byte ; i++) {

		if ( i == (this->cantidad_modelos_agrupados_por_byte-1) ) { // solo en el ultimo
			int bytes_sobrantes = ((this->cantidad_modelos_agrupados_por_byte * 4) - this->cantidad_de_sub_bloques );
			//cout << "bytes_sobrantes: " << bytes_sobrantes << endl;
			if (bytes_sobrantes != 0){
				max_iteraciones_buffer = max_iteraciones_buffer - bytes_sobrantes;
			}
		}

		buffer = this->modelos_agrupados_por_byte[i];
		//cout << "buffer: " << (bitset<8>)(buffer) <<endl;
		mascara = 0xc0; // 1100 0000
		for ( LInt j = 0 ; j< max_iteraciones_buffer; j++) {

			//hacer mascara y asignar!
			representacion_modelo = (buffer & mascara)>> (2*(OVERHEAD_MODELOS_POR_BYTE-j-1)); 
			//cout << "representacion modelo:" << (bitset<8>)(representacion_modelo)<< endl;
			asignar_modelo_segun_representacion_en_byte(representacion_modelo);
			mascara = mascara >> 2; // testiar
		}
	}


}

void Overhead::asignar_modelo_segun_representacion_en_byte(UChar representacion_modelo){

	switch(representacion_modelo) {

		case 0x00: this->modelos_predominantes[it_modelos] = Estructurado; 

		break; 

		case 0x01: this->modelos_predominantes[it_modelos] = Invertido; 
	
		break;

		case 0x02: this->modelos_predominantes[it_modelos] = Semi_invertido;

		break;

		case 0x03: this->modelos_predominantes[it_modelos] = Casi_Estructurado;

		break;

	};

	//cout << (int)this->modelos_predominantes[it_modelos]<<"-";
	this->it_modelos++;
}
