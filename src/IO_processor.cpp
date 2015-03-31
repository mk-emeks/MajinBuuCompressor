

#include "IO_processor.h"

void IO_processor::crear_archivo_para_escritura(char* nombre_del_archivo){

	fd = fopen( nombre_del_archivo , "wb+"); //abrimos el archivo modo write binario, obs: si existe lo pisa!

	iniciar_buffer();
}

void IO_processor::abrir_archivo_para_lectura(char* nombreDelArchivo){

	fd = fopen(nombreDelArchivo , "rb");

	iniciar_buffer();
}

/** like destructor **/
void IO_processor::cerrar_archivo(){

	fclose(fd);
	delete [] buffer;
}


/** like constructor **/
void  IO_processor::iniciar_buffer(){

	// general
	sub_buffer = CERO;
	iterador_sub_buffer = 0;
	iterador_inverzo_sub_buffer = 8;

	buffer = new C_Byte[BUFFER_SIZE]; 
	iterador_buffer = 0;

	// solo lectura 
	// obs: arrancan en 0 por una cuestion de entrada de bifurcaciones, pero despues se mantiene de un tamaño fijo.
	// Aunque en el ultimo caso, este tamaño se achica segun el caso, por el eof.
	maximo_leido_sub_buffer = 0; 
	maximo_leido = 0;

	// solo escritura
	exception_intentar_escribir_despues_del_ultimo_bit = false; // bandera Exception

}

// resetiar la memoria y volver a pedir no nos da ninguna seguridad 
void  IO_processor::resetiar_buffer(){ 

	iterador_buffer = 0; 
}

void IO_processor::limpiar_sub_buffer() {
	
	sub_buffer = CERO; 
}

void  IO_processor::resetiar_contadores_sub_buffer(){

	iterador_sub_buffer = 0; 
	iterador_inverzo_sub_buffer = 8;

}

// ESCRITURA

void  IO_processor::resetiar_sub_buffer(){  // especial para escritura

	resetiar_contadores_sub_buffer();
	limpiar_sub_buffer();
	
}

void IO_processor::escribir(C_Byte bit_representado_en_byte , bool es_el_ultimo_bit){

	if ( !exception_intentar_escribir_despues_del_ultimo_bit) { // EXCEPTION

		if (iterador_sub_buffer < SUB_BUFFER_SIZE) {

			agregar_en_sub_buffer(bit_representado_en_byte);

		} else {
		
			// Se escribe el contenido del subbuffer ya que este esta lleno. 
			// Se le debe avisar si es el ultimo byte a guardar.
			escribir_en_el_buffer(false); 
			resetiar_sub_buffer();
			agregar_en_sub_buffer(bit_representado_en_byte);
		}

		if (es_el_ultimo_bit) {

			if (iterador_sub_buffer < SUB_BUFFER_SIZE) { // caso en el que el subbuffer no estaba lleno

				for (int i = iterador_sub_buffer; i < SUB_BUFFER_SIZE ; i++) {

					agregar_en_sub_buffer(CERO); // releno con 0's

				}

				escribir_en_el_buffer(true);
					

			} else { // justo llenaste el buffer con el ultimo bit

			escribir_en_el_buffer(true);
			}	

		
			exception_intentar_escribir_despues_del_ultimo_bit = true; // bandera para detener el proceso si se intenta, despues de haber dicho que era el ultimo, grabar una vez más.		
			//cout << "Se ha finalizado la escritura del archivo" << endl; 		
		}

		// como es el ultimo no hace falta resetiar (el usuario no debe olvidarse de usar luego "cerrar_archivo()")
	}	
}


/*
	Pre: El valor esperado de 'bit_representado_en_byte' es;
		0x01 byte -> 1 bit
		0x00 byte -> 0 bit
*/
void IO_processor::agregar_en_sub_buffer(C_Byte bit_representado_en_byte){
	
	C_Byte aux;
	iterador_inverzo_sub_buffer--; // recordar que arranca en 8 pero ni bien empieza baja a 7 (cuestion de claridad)

	if ( bit_representado_en_byte & UNO) {

		//cout << "1";
		aux = (UNO << iterador_inverzo_sub_buffer); // la primera vez corre 7 lugares, 2da 6, .., etc.
		sub_buffer = (sub_buffer | aux); //escribir un uno 
	
	} /*else {
		
		//cout << "0";
		//cout << "fue 0" << endl;
		//escribe un cero
		// guardar un 0 no es necesario ya que el sub buffer esta "limpio" de antemano 

	}*/
	iterador_sub_buffer++; // aumenta el contador que cuenta que tan lleno esta el buffer.

}

void IO_processor::escribir_en_el_buffer(bool es_el_ultimo_byte) {

	if  (iterador_buffer < BUFFER_SIZE) {

		agregar_en_buffer(sub_buffer);
		

	} else { // El buffer esta completo; Se procede a guardarlo, restiarlo, y agregar el nuevo caracter

		guardar_contenido_del_buffer_en_el_archivo();
		resetiar_buffer();
		agregar_en_buffer(sub_buffer);
	}

	// contempla caso limite, en el cual el ultimo caracter se agrega al buffer, pero aun no se guardo el buffer porque no llego a completarse.
	if (es_el_ultimo_byte) {

		guardar_contenido_del_buffer_en_el_archivo();
	}

}

void IO_processor::agregar_en_buffer(C_Byte un_byte){

	buffer[iterador_buffer] = un_byte;
	iterador_buffer++;
}

void IO_processor::guardar_contenido_del_buffer_en_el_archivo() {

	fwrite(buffer,sizeof(C_Byte),iterador_buffer,fd); // se pone iterador_buffer para contemplar el caso del buffer incompleto	
}


// LECTURA

//post: false indica que se termino el archivo. True que se cargo el buffer correctamente
bool IO_processor::cargar_buffer_con_contenido_del_archivo(){
	
	bool indice_correcta = true;

	if (notEndOfFile()) { // cargar buffer

		maximo_leido = (fread(buffer,sizeof(C_Byte),BUFFER_SIZE,fd));
		//cout << "Se leyeron: " << maximo_leido << " caracteres" << endl;

	} else {
		//cout << "ya no tengo mas que leer -> hasta acá tiene sentido leer bits" << endl;
		indice_correcta = false; // fin de archivo
	}

	return indice_correcta;
	
}


void IO_processor::IO_processor::cargar_sub_buffer_con_contenido_del_buffer(){

	sub_buffer = buffer[iterador_buffer];
	iterador_buffer++;

	//cout <<  " pos it buffer: " << iterador_buffer << endl;
	
}

bool IO_processor::leer_del_buffer(){

	bool lectura_correcta = true;

	if ( (iterador_buffer < maximo_leido) ) {

		cargar_sub_buffer_con_contenido_del_buffer();		

	} else { // entra en el primer caso y en todos los casos donde ya haya leido todos los caracteres cargados en el buffer

		if ( !(cargar_buffer_con_contenido_del_archivo()) ) {
			
			lectura_correcta = false; // fin de archivo	
		}

		if (lectura_correcta) {

			// (OJO) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			resetiar_buffer();
			cargar_sub_buffer_con_contenido_del_buffer();

		} 

	}

	return lectura_correcta;
}

void IO_processor::leer_bit_del_sub_buffer(C_Byte& bit_representado_en_byte){ // (OJO: mirar que es una referencia)

	C_Byte aux;

	iterador_inverzo_sub_buffer--;

	aux = sub_buffer & (UNO << iterador_inverzo_sub_buffer);
	aux = aux >> iterador_inverzo_sub_buffer;

	//cout <<  " pos it inverzo sub buffer: " << iterador_inverzo_sub_buffer << endl;
	//cout <<  " pos it sub buffer: " << iterador_sub_buffer << endl;

	iterador_sub_buffer++; // aumenta el contador que cuenta que tan lleno esta el buffer.
	
	bit_representado_en_byte = aux; // devolvemos el bit representado
	//cout << bit_representado_en_byte << '*' << endl; // obs: cuando viene un 0 no muestra nada la consola

}

/*
	Post: El valor devuelto en 'bit_representado_en_byte' es;
		0x01 byte -> 1 bit
		0x00 byte -> 0 bit
*/
/*
	(!)false: el archivo termino, es decir lo que cargo no es parte del archivo.
	De hecho, dejara lo que tenia la variable antes de entrar.
*/
C_Byte IO_processor::leer(){

	C_Byte bit_representado_en_byte = CERO;

	bool lectura_correcta = true;

	if ( iterador_sub_buffer < maximo_leido_sub_buffer ) {

		leer_bit_del_sub_buffer(bit_representado_en_byte);
		
	} else { //entra en el primer caso y en todos los casos donde ya haya leido todos los caracteres indicedos en el subbuffer

		maximo_leido_sub_buffer = SUB_BUFFER_SIZE; // ES SIEMPRE CONSTANTE SI LOGRO cargar DEL SUB BUFFER 

		if ( !(leer_del_buffer()) ) { 
			
			lectura_correcta = false; // FIN DE ARCHIVO	-> cuando se le acaba el archivo comienza a devolver 0's
			// obs: si pasara el caso en el que el ultimo subbuffer esta completado con basura
			// el metodo devolvera la basura hasta, que se termine el archivo
			// esto no es problema, ya que estos bits no influyen al descompresor en la busquedad del caracter eof. 
		}

		if (lectura_correcta) {
			
			// (OJO) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			resetiar_contadores_sub_buffer();
			leer_bit_del_sub_buffer(bit_representado_en_byte);
			
		}
	}

	return bit_representado_en_byte;

}

bool IO_processor::notEndOfFile() {

	if  ( feof(fd) ) {

		return false;

	} else {

		return true;
	}
}

/** OVERHEAD **/

// (!) pre: hacerlo luego de set_nombre_archivo_compresion **/
void IO_processor::reservar_espacio_para_overhead(LInt espacio_reservado) {

	UChar* relleno_temporal = new UChar[espacio_reservado];
	
	// cargando relleno
	for ( LInt i = 0; i<espacio_reservado ; i++) {

		relleno_temporal[i] = '0';

	}

	// escribiendo relleno
	fwrite(relleno_temporal,sizeof(UChar),espacio_reservado,this->fd);

	delete [] relleno_temporal;


}

// ESCRIBIR 1
void IO_processor::escribir_en_overhead( unsigned short cantidad_de_bloques ) {

	unsigned short* aux = new unsigned short;
	*aux = cantidad_de_bloques;

	rewind(this->fd); 
	fwrite(aux,sizeof(unsigned short),1,this->fd);
	delete aux;

}

// ESCRIBIR 2
void IO_processor::escribir_en_overhead( LInt* indices, unsigned short cantidad_de_bloques ) {

	fwrite(indices,sizeof(LInt),cantidad_de_bloques,this->fd);

}

// ESCRIBIR 3 
void IO_processor::escribir_en_overhead ( LInt cantidad_de_sub_bloques) {

	LInt* aux = new LInt;
	*aux = cantidad_de_sub_bloques;

	fwrite(aux,sizeof(LInt),1,this->fd);
	delete aux;

}

// ESCRIBIR 4
void IO_processor::escribir_en_overhead ( UChar* modelos , LInt cantidad_modelos_agrupados_por_byte) {

	/*for (LInt i = 0; i < cantidad_modelos_agrupados_por_byte; i++) {
		cout << bitset<8>(modelos[i]) << "-";
	}
	cout << endl;*/
	fwrite(modelos,sizeof(UChar),cantidad_modelos_agrupados_por_byte,this->fd);
	this->cerrar_archivo(); // (!) (!) (!) sin no se escribe en este orden 1,2,3,4 LA CAGAS!


}

// LEER 1
unsigned short IO_processor::get_cantidad_bloques() { // guardo la cantidad de bloques

	unsigned short* cantidad_de_bloques = new unsigned short;

	fread(cantidad_de_bloques,sizeof(unsigned short),1,this->fd);
    
    this->cantidad_de_bloques = *cantidad_de_bloques;

	return this->cantidad_de_bloques;

}

// LEER 2
void IO_processor::cargar_overhead(LInt* bloque_de_indices) {
	
	fread(bloque_de_indices,sizeof(LInt),this->cantidad_de_bloques,this->fd);
	
}

// LEER 3
LInt IO_processor::get_cantidad_de_sub_bloques(){
	LInt* cantidad_de_sub_bloques = new LInt;
	LInt  cantidad_de_sub_bloques_aux;

	fread(cantidad_de_sub_bloques,sizeof(LInt),1,this->fd);

	cantidad_de_sub_bloques_aux = *cantidad_de_sub_bloques;

	return cantidad_de_sub_bloques_aux;
}

// LEER 4
void IO_processor::cargar_overhead(UChar* modelos_agrupados_por_byte, LInt cantidad_modelos_agrupados_por_byte){
	
	fread(modelos_agrupados_por_byte,sizeof(UChar),cantidad_modelos_agrupados_por_byte,this->fd);

}