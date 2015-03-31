
#include "Compresor.h"

using namespace std;

Compresor::Compresor(){

	this->extencion_archivo = ".buu";

	this->aritmetico = new Aritmetico();
	this->mtf = new Move_to_front();

	this->escritor = new Writer();

}

Compresor::~Compresor() {

	delete this->aritmetico, this->mtf, this->escritor;
}

void Compresor::comprimir(char* nombre_archivo){

    // abrimos el archivo -> (ver block sorting caso: archivo vacio)
    FILE* fd = fopen(nombre_archivo, "rb"); //abrimos el archivo modo read
	this->t_bloque = MEMORIA_POR_ACCESO; 

	// calculos de tamanio de overhead
	fseek(fd,0,SEEK_END);
	LInt t_archivo = ftell(fd);
	this->overhead = new Overhead(t_archivo);
	fseek(fd,0,SEEK_SET); // volvemos al inicio en el archivo original

	// asignamos extencion del archivo 
	Nombre_archivo* nombre_archivo_con_extencion = new Nombre_archivo(nombre_archivo);
	nombre_archivo_con_extencion->append(extencion_archivo);

	// aca internamente se crea el IO_processor de la clase aritmetico
	this->aritmetico->set_nombre_archivo_compresion(nombre_archivo_con_extencion->cast_to_ptrchar()); 
	delete nombre_archivo_con_extencion;

	IO_processor* processor = this->aritmetico->get_IO_processor();

	// reservamos el espacio para el overhead en el archivo comprimido
	processor->reservar_espacio_para_overhead(this->overhead->get_espacio_reservado());

	bool fin_compresion = false; // bandera para terminar compresion
	while (!fin_compresion)
	{

        this->bloque_caracteres = new UChar[this->t_bloque];

		// leemos de a bloques:	
        if (not_end_of_file(fd)) {

        	//fread devuelve la cantidad que leyo	
        	this->t_bloque = (fread(this->bloque_caracteres,sizeof(UChar),this->t_bloque,fd));
        	//cout << "cantidad tomada por bloque: " << this->t_bloque << endl;

        	// lectura adelantada
        	if ( !(not_end_of_file(fd)) ) { // si es fin de archivo, debemos poner en true la bandera
				//cout << "estoy en ultimo bloque" << endl;
				fin_compresion = true;
			}

        }
		
		//cout << "Block Sorting:" << endl;

		UChar* resultado_parcial = new UChar[this->t_bloque];

		//obs: notar que el BS necesita introducir el resultado en un NUEVO vector.
		//LInt indiceParcial = blockSortingLeft(resultado_parcial, this->bloque_caracteres, this->t_bloque);
		LInt indiceParcial = blockSorting(resultado_parcial, this->bloque_caracteres, this->t_bloque);

		this->overhead->guardar_indice(indiceParcial);

		//cout << "Move to front:" << endl;

		// creamos "overhead" parcial de modelos (queda por fuera de la clase overhead)
		LInt cantidad_de_sub_bloques_aux = (LInt)(this->t_bloque / TAMANIO_SUB_BLOQUE);
		if ( (this->t_bloque % TAMANIO_SUB_BLOQUE) > 0 ) {
			cantidad_de_sub_bloques_aux++;
		}
		Modelos* modelos_predominantes_aux = new Modelos[cantidad_de_sub_bloques_aux];


		resultado_parcial= this->mtf->moveToFront(resultado_parcial,this->t_bloque, modelos_predominantes_aux);

		/*cout << "modelos predominantes:" << endl;
		for (int i = 0; i < cantidad_de_sub_bloques_aux; i++) {
			cout << (int)modelos_predominantes_aux[i] << "-";

		}
		cout << endl;
		cout << "cantidad de modelos mostrados: " << cantidad_de_sub_bloques_aux << endl;*/

		//cout << "Compresion!" << endl;

		int it_sub_bloque = 0;
		aritmetico->cambiar_modelo(modelos_predominantes_aux[it_sub_bloque]);
		it_sub_bloque++;
		for (int i = 0; i < this->t_bloque; i++) {
			if ( (i != 0 ) && (i % TAMANIO_SUB_BLOQUE  == 0) ) {
				aritmetico->cambiar_modelo(modelos_predominantes_aux[it_sub_bloque]);
				it_sub_bloque++;
			}
			aritmetico->comprimir_valor(resultado_parcial[i]);
		}
		
		if (fin_compresion) {
			aritmetico->comprimir_valor(eof);
			aritmetico->terminarCompresion();
		}

		// grabamos los modelos predominantes -> dump de modelos predominantes parciales a modelos predominantes totales
		for (int i = 0; i < cantidad_de_sub_bloques_aux; i++) {
			this->overhead->guardar_modelo_predominante(modelos_predominantes_aux[i]);
		}

		// Performance
		/*cout << "cantidad bits comprimidos " << this->aritmetico->get_cantidad_bits_comprimidos() << endl;
		cout << "cantidad bytes comprimidos " << (this->aritmetico->get_cantidad_bits_comprimidos()/8) << endl;
		cout << "cantidad de breaks " << this->aritmetico->get_cantidad_de_breaks() << endl;
		cout << "cantidad de caracteres" << this->t_bloque << endl;*/

		/** LIBERAR MEMORIA **/
		delete [] this->bloque_caracteres;
		delete [] resultado_parcial;
		delete [] modelos_predominantes_aux;
	
	}

	// escribir overhead
	processor->escribir_en_overhead(this->overhead->get_cantidad_de_bloques());
	processor->escribir_en_overhead(this->overhead->get_bloque_de_indices(), this->overhead->get_cantidad_de_bloques());

	processor->escribir_en_overhead(this->overhead->get_cantidad_de_sub_bloques());
	processor->escribir_en_overhead(this->overhead->get_modelos_predominantes_agrupados_por_byte(), this->overhead->get_cantidad_modelos_agrupados_por_byte());

	/** LIBERAR MEMORIA **/
	delete this->overhead;
	
	/** CERRAR ARCHIVO **/
	fclose(fd);
}


void Compresor::descomprimir(char* nombre_archivo_con_extencion){

	Nombre_archivo* nombre_archivo = new Nombre_archivo(nombre_archivo_con_extencion);

	if (nombre_archivo->borrar_extencion(extencion_archivo)) {

		// crear archivo para la descompression
		if (escritor->set_nombre_archivo_nuevo(nombre_archivo->cast_to_ptrchar())) {
			
			// (!) procedimiento: y leyendo el archivo (descomprimirlo) e ir generando la salida
			
			// obs: se le pasa el archivo con extencion, ya que sobre el mismo queremos descomprimir!
			aritmetico->set_nombre_archivo_descompresion(nombre_archivo_con_extencion); // aca internamente se crea el IO_processor de la clase aritmetico
			
			// cargamos el overhead del archivo comprimido
			this->overhead = new Overhead(aritmetico->get_IO_processor());

			//cout << "Descompresion" << endl;
			aritmetico->empezar_descompresion(); // (!)

			this->t_bloque = MEMORIA_POR_ACCESO;
			bool fin_descompresion = false;

			while (!fin_descompresion) {

				this->bloque_caracteres = new UChar[this->t_bloque];
				LInt long_tira = 0;

				LInt indice_actual = this->overhead->get_indice_actual();
				Valor caracterDescomprimido;

				while ( (long_tira < MEMORIA_POR_ACCESO) ) {
					
					aritmetico->cambiar_modelo(this->overhead->get_modelo_predominante_actual());
					int it_sub_bloque = 0;

					while ( (long_tira < MEMORIA_POR_ACCESO) && (it_sub_bloque < TAMANIO_SUB_BLOQUE) ) {
						
						caracterDescomprimido = aritmetico->descomprimir_valor();

						if (caracterDescomprimido == eof) {
							//cout << "encontro EOF" << endl;
							fin_descompresion =true;
							break;
						}

						this->bloque_caracteres[long_tira] = (UChar)caracterDescomprimido;

						long_tira++;
						it_sub_bloque++;
					}

					if (caracterDescomprimido == eof) {
						//cout << "encontro EOF" << endl;
						fin_descompresion =true;
						break;
					}

				}
				
				//cout << "vuelta Move to front:" << endl;

				UChar* resultado_parcial = new UChar[long_tira];
				resultado_parcial = this->mtf->vueltaMoveToFront(this->bloque_caracteres, long_tira);

				//cout << "vuelta Block Sorting:" << endl;

				//this->bloque_caracteres = vueltaBlockSortingLeft(resultado_parcial,indice_actual,long_tira);
				this->bloque_caracteres = vueltaBlockSorting(resultado_parcial,indice_actual,long_tira);

				this->escritor->escribir(bloque_caracteres,long_tira);

				delete [] this->bloque_caracteres, resultado_parcial;
				

			}

			this->escritor->cerrar_archivo();
			delete this->overhead;

		} else{
			cout << "error al descomprimir: ya existe el archivo" << endl;
		}

	} else {
		cout << "error al descomprimir: el archivo que desea descomprimir, tiene una extención incorrecta o ni siquiera existe" << endl;
	}

	delete nombre_archivo;

}

bool Compresor::not_end_of_file(FILE* &fd) {
	if  ( feof(fd) ) {
		return false;
	} else {
		return true;
	}
}
