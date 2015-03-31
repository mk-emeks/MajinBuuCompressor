

#include "IO_processor.h"

using namespace std;

void IO_processor::crear_archivo_para_escritura(char* nombre_del_archivo){

	////////////////// MODIFICARRRRRRRRRRRRRRRRRRRRRRRR
	/*string s_nombre;
	char* c_nombre; 

	s_nombre = nombre_del_archivo;
	s_nombre = s_nombre + ".1"; // extencion de nuestro tp -> la quize guardar como constante pero no pude! =S

	c_nombre = new char[s_nombre.size()];

	for (int i= 0 ; i < s_nombre.size() ; i++) {
		c_nombre[i] = s_nombre[i];

		//cout << c_nombre[i] ;
	}*/

	fd = fopen( nombre_del_archivo , "wb+"); //abrimos el archivo modo write binario, obs: si existe lo pisa!

	//delete [] c_nombre;

	iniciar_buffer();
}

void IO_processor::abrir_archivo_para_lectura(char* nombreDelArchivo){

	fd = fopen(nombreDelArchivo , "rb+");
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

	bits_EOF = 0;

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

			generar_EOF();
		
			exception_intentar_escribir_despues_del_ultimo_bit = true; // bandera para detener el proceso si se intenta, despues de haber dicho que era el ultimo, grabar una vez más.		
			cout << "Se ha finalizado la escritura del archivo" << endl; 		
		}

		// como es el ultimo no hace falta resetiar (el usuario no debe olvidarse de usar luego "cerrar_archivo()")
	}	
}

void IO_processor::generar_EOF() {
	
	// se va fijar en el iterador del subbuffer cuanto falta para rellenar.
	// el caso en donde falta completar, guardar un 1 y el resto de 0's hasta llenar el subbuffer, luego lo escribira.
	// el caso en donde estaba lleno, guardara y creara un nuevo byte con el eof.

	if (iterador_sub_buffer < SUB_BUFFER_SIZE) { // caso en el que el subbuffer no estaba lleno

		/** generando eof -> caso byte incompleto **/
		agregar_en_sub_buffer(UNO); // ojo con la constante
		if (!(iterador_sub_buffer == 7)) {

			for (int i = iterador_sub_buffer+1 ; i < SUB_BUFFER_SIZE ; i++) {

				agregar_en_sub_buffer(CERO);

			}
					
		}

	escribir_en_el_buffer(true);

	} else { // justo llenaste el buffer con el ultimo bit

		escribir_en_el_buffer(false);
				
		/** generando eof -> caso byte incompleto **/
		resetiar_sub_buffer();
		agregar_en_sub_buffer(UNO); // grabaria los 0's  pero el sub buffer esta "limpio" por el reset 
		escribir_en_el_buffer(true);
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
		cout << "Se leyeron: " << maximo_leido << " caracteres" << endl;
		
		// se lee de forma adelantada para saber si sique algo, si no sigue, estamos en presencia del ultimo bloque
		// por lo tanto debemos no considerar los bits del eof
		if ( ! (notEndOfFile()) ) {
			cout << "estoy en ultimo bloque" << endl;
			quitar_EOF();
		}

	} else {
		cout << "ya no tengo mas que leer -> hasta acá tiene sentido leer bits" << endl;
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

	} else { // entra en el primer caso y en todos los casos donde ya haya leido todos los caracteres indicedos en el buffer

		if ( !(cargar_buffer_con_contenido_del_archivo()) ) {
			
			lectura_correcta = false; // fin de archivo	
		}

		// solo tiene que escribir si hay algo nuevo, es decir, si hizo una nueva indice 
		if (lectura_correcta) {

			// (OJO) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			resetiar_buffer();
			/* (!)
				Si el reset vuelve a ser el de antes, debemos no usarlo y cambiarlo por la linea comentada,
				ya que si resetiamos estamos liberando la memoria: solo debemos reiniciar el iterador!
			*/
			//iterador_buffer = 0; 

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
bool IO_processor::leer(C_Byte& bit_representado_en_byte){

	bool lectura_correcta = true;

	if ( iterador_sub_buffer < maximo_leido_sub_buffer ) {

		leer_bit_del_sub_buffer(bit_representado_en_byte);
		
	} else { //entra en el primer caso y en todos los casos donde ya haya leido todos los caracteres indicedos en el subbuffer

		maximo_leido_sub_buffer = SUB_BUFFER_SIZE; // ES SIEMPRE CONSTANTE SI LOGRO cargar DEL SUB BUFFER 

		if ( !(leer_del_buffer()) ) { 
			
			lectura_correcta = false; // FIN DE ARCHIVO	-> sale sin escribir nada en la variable que se le paso.
		}

		if (lectura_correcta) {
			
			// (OJO) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			resetiar_contadores_sub_buffer();

			// si se carga el ultimo sub buffer, tengo que contemplar el ajuste de los iteradoes debido a bites de EOF
			if (iterador_buffer == maximo_leido) { 
				//cout << "estoy en el ultimo byte y tengo " << this->bits_EOF << " bits de eof ajustados"  << endl;
				maximo_leido_sub_buffer = (maximo_leido_sub_buffer - this->bits_EOF);
				
			}

			leer_bit_del_sub_buffer(bit_representado_en_byte);
			
		}
	}

	return lectura_correcta;

}

bool IO_processor::notEndOfFile() {

	if  ( feof(fd) ) {

		return false;

	} else {

		return true;
	}
}


/** 
	la magia es pensar que los buffers van a leer hasta donde se les dice, y cuando acaben, por como es el codigo
	van a intentar leer devuelta y ahi van a entrar en else de cargar_buffer_con_contenido_del_archivo
**/
void IO_processor::quitar_EOF() {

	C_Byte sub_buffer_aux;
	sub_buffer_aux = buffer[maximo_leido-1];

	bool encontre_un_1 = false;
	//this->bits_EOF = 0;
	C_Byte mascara_iterador = UNO;


	// cuenta cuantos bist hay que sacar.
	while ( !encontre_un_1 ) {
		
		if ( sub_buffer_aux  &  mascara_iterador) {
		
			encontre_un_1 = true;
		} 

		this->bits_EOF++; // seguro 1 hay
		mascara_iterador = mascara_iterador << 1; // va cambiando el uno en cada posicion
		// obs: en este caso por como es el eof leemos de DER->IZQ
	}

	//cout << "cantidad de bits reales de eof: " << this->bits_EOF << endl;

	// hay que modificar los maximos asi se lee solo lo que es dato
	if (this->bits_EOF == 8) { // caso: el eof no llega a un byte de tamaño
		maximo_leido--; 
		bits_EOF = 0; // ya lo descarte decrementando maximo_leido!
	
	} // else: caso: el eof es de menos de un byte -> se rre ajusta con bits eof en el metodo leer

}

// ESCRITURA Y LECTURA DE 4 BYTES -> INDICES

void IO_processor::escribir(LInt indice) {

	LInt mascara_iterador = 0x80000000; // 10000000000000000000000000000000

	//cout << "indice: " << bitset<32>(indice) << endl; 
	//cout << sizeof(LInt)*8 << endl;

	for ( int i = 0 ; i < (sizeof(LInt)*8) ; i++) { // sizeof(LInt) nos devuelve 4 porque LIint ocupa 4 bytes, * 8 = 32 bits
		
		if ( indice  &  mascara_iterador) {
		
			escribir(UNO,false); // no es lo mas eficiente del mundo, pero es mas comodo usar lo que ya existe

		} else {

			escribir(CERO,false);
		}

		//cout << "mascara: " << bitset<32>(mascara_iterador) << endl;
		mascara_iterador = mascara_iterador >> 1; // va cambiando el uno en cada posicion
		// obs: nosotros grabamos y leemos de IZQ -> DER
	
	}
}


bool IO_processor::leer(LInt& indice){

	LInt mascara_iterador = 0x80000000; // 10000000000000000000000000000000
	indice = 0x00000000;

	bool lectura_correcta = true;
	C_Byte comparador;

	for ( int i = 0 ; i < (sizeof(LInt)*8) ; i++) { 
		
		if (leer(comparador)) { // condicional, que de ser correcto indice a la variable comparador

			if (comparador & UNO) {

				indice = (indice | mascara_iterador); 
				//cout << "indice: " << bitset<32>(indice) << endl; 
			}

			// else; indice 0 en la posicion correspondiente -> no hace falta, se inicializo en 0's

		}else{

			lectura_correcta = false;
		}

		mascara_iterador = mascara_iterador >> 1;
		
	}

return lectura_correcta;

}


// ESCRITURA PARA LA RECONSTRUCCION DEL ARCHIVO

