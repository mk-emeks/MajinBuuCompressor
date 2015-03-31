
#ifndef __NOMBRE_ARCHIVO_H__
#define __NOMBRE_ARCHIVO__

#include "Declaracion_de_constantes.h"

class Nombre_archivo {

	private:

		string s_nombre;

	public:

		char* cast_to_ptrchar() {

			return (char*) s_nombre.c_str();

		}

		string get_nombre_archivo() {
			
			return s_nombre;
		}

		Nombre_archivo(string s) {
			
			this->s_nombre = s;
		}

		Nombre_archivo(char* s) {
			
			this->s_nombre = s;
		}

		void cambiar_nombre(string s_nombre) {

			this->s_nombre = s_nombre;
		}

		void append(string agregado_al_final) {

			s_nombre = s_nombre + agregado_al_final;
		}

		// de existir en el nombre del archivo, borra el substring del final,
		// obviamente si coincide con lo que le pedis.
		bool borrar_extencion(string extencion_al_final) {
		
			bool esta = false;

			unsigned short int j = (extencion_al_final.size()-1);
			unsigned short int machs = 0;

			for (int i = s_nombre.size()-1  ; i >= (s_nombre.size()-extencion_al_final.size()) ; i--) {
				
				if (s_nombre[i] == extencion_al_final[j]) {
					machs++;
					//cout << s_nombre[i];
				}
				//cout << endl;
				j--;
				
			}

			if (machs == extencion_al_final.size()) {
				esta = true;
			}

			if (esta) {

				string nombre_recortado = s_nombre.substr( 0 , (s_nombre.size()-extencion_al_final.size()) ) ;
				//cout << nombre_recortado << endl;
				cambiar_nombre(nombre_recortado);

			}
			
			return esta;

		}

};


#endif