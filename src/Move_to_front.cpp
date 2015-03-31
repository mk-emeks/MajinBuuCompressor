
#include "Move_to_front.h"

UChar* Move_to_front::moveToFront(UChar* tira_localizada, LInt	long_tira, Modelos* modelos_predominantes_aux)
{
	// Armamos la "tira ordenada de 0 a 255"
	UChar* tira_ordenada = new UChar[MAX_CARACTERES];
    ordenar_tira(tira_ordenada);

    // Contadores de frecuencia de aparicion de caracteres numericos: (debe ser del mismo tipo que la constante TAMANIO_SUB_BLOQUE) 
    unsigned short* acumuladores = new unsigned short[MAX_CARACTERES];
    unsigned short it_sub_bloques = 0;    

    // Inicializo en 0
    for (int i = 0; i < MAX_CARACTERES; i++) {
        acumuladores[i] = 0;
    }

    // Mtf:
	UChar* tiraMtf = tira_localizada;
	LInt cant_chars = 0 ;

    LInt it_aux = 0; // FUNDAMENTAL!!!!!!!!!!!!!!!!!!!!!!!
    for (LInt i = 0 ; i < long_tira ; i++) {
        
        bool igual = false;
        LInt j = -1;

        // j antes de llegar a 255 deberia activar la bandera
        while(!igual)
        {
            if ( tira_localizada[i] == tira_ordenada[j + 1] )
            {
            	igual = true;
            }
            j++;
        }
           
        tiraMtf[i] = (UChar)j; // guardando posicion en tira de salida
        
        acumuladores[j]++; // aumenta la cantidad de apariciones de j
        
        it_aux++;
        if ((it_aux == TAMANIO_SUB_BLOQUE || (i == long_tira-1) )) { 
            it_aux = 0;

            // ELECCION MODELOS:

            modelos_predominantes_aux[it_sub_bloques] = Estructurado; // por default es el estructurado!

            // Casi_Estructurado("3") vs estructurado

            unsigned short contador_caracteres_modelos_Casi_Estructurado = 0;

            for ( int t = 2 ; t < 8  ; t++) { // nivel 2 + 3 vs 1
                contador_caracteres_modelos_Casi_Estructurado += acumuladores[t];
            }

            if ( (contador_caracteres_modelos_Casi_Estructurado > acumuladores[1]) ) {
                modelos_predominantes_aux[it_sub_bloques] = Casi_Estructurado;
            }

            // Semi_invertido("2") vs estructurado

            unsigned short contador_caracteres_modelos_Semi_invertido = 0;

            for ( int t = 2 ; t < 16 ; t++) { // del nivel 2 al 4to vs 0 y 1
                contador_caracteres_modelos_Semi_invertido += acumuladores[t];
            }

            unsigned short contadores_caracteres_0_y_1 = acumuladores [0] + acumuladores[1];

            if (contador_caracteres_modelos_Semi_invertido > contadores_caracteres_0_y_1) {
                modelos_predominantes_aux[it_sub_bloques] = Semi_invertido;
            }

            // invertidos("1") vs estructurado

            //if (modelos_predominantes_aux[it_sub_bloques] != Semi_invertido) {

            unsigned short contador_caracteres_modelo_invertido = 0; // debe ser del mismo tipo que la constante TAMANIO_SUB_BLOQUE   

            for (int t = 64 ; t < MAX_CARACTERES ; t++) { // 6+7+8 vs 0
                contador_caracteres_modelo_invertido += acumuladores[t];
            }

            // si la suma total da mayor que la cantidad de 0's , se invierte el modelo
            if ( contador_caracteres_modelo_invertido > acumuladores[0]) { 
                modelos_predominantes_aux[it_sub_bloques] = Invertido;
            }
            //}

            // FIN ELECCION MODELOS

            // reinicia los contadores
            for (int t = 0; t < MAX_CARACTERES; t++) {
                acumuladores[t] = 0;
            }
            it_sub_bloques++;
        } 

        // swap de lista ordenada MTF : se hace de atras para adelante!
        UChar aux_swap;
        for( LInt k = j ; k > 0 ; k--)
        {
            aux_swap = tira_ordenada[k];
            tira_ordenada[k] = tira_ordenada[k-1];
            tira_ordenada[k-1] = aux_swap;
        }

    }       

    delete [] acumuladores;
    return tiraMtf;
}

UChar* Move_to_front::vueltaMoveToFront(UChar* tiraMTF, LInt long_tira) {

    // armamos la "tira ordenada de 0 a 255"
    UChar* tira_ordenada = new UChar[MAX_CARACTERES];
    UChar* tira_localizada = new UChar[long_tira];

    ordenar_tira(tira_ordenada);

    for (LInt i = 0 ; i < long_tira ; i++) {
        
        tira_localizada[i] = (UChar)tira_ordenada[(int)tiraMTF[i]];

        // swap de lista ordenada MTF : se hace de atras para adelante!

        UChar aux_swap;
        for( int k = (int)tiraMTF[i] ; k > 0 ; k--)
        {
            aux_swap = tira_ordenada[k];
            tira_ordenada[k] = tira_ordenada[k-1];
            tira_ordenada[k-1] = aux_swap;
        }
            
    }       

    return tira_localizada;
}


void Move_to_front::ordenar_tira(UChar* tira_ordenada) {
    
    // modo comun:
    /*for ( LInt i = 0 ; i < MAX_CARACTERES ; i++ ) 
    {
        tira_ordenada[i] = (UChar)i;
    }*/

    // modo hardcore:
    LInt j = 0;

    // minusculas
    for ( LInt i = 97 ; i <= 122 ; i++ ) 
    {
        tira_ordenada[j] = (UChar)i;
        j++;
    }

    // mayusculas
    for ( LInt i = 65 ; i <= 90 ; i++ ) 
    {
        tira_ordenada[j] = (UChar)i;
        j++;
    }

    // simbolos y numeros
    for ( LInt i = 32 ; i <= 64 ; i++ ) 
    {
        tira_ordenada[j] = (UChar)i;
        j++;
    }

    for ( LInt i = 91 ; i <= 96 ; i++ ) 
    {
        tira_ordenada[j] = (UChar)i;
        j++;
    }

    // cosas raras 1
    for ( LInt i = 0 ; i <= 31 ; i++ ) 
    {
        tira_ordenada[j] = (UChar)i;
        j++;
    }

    // cosas raras 2
    for ( LInt i = 123 ; i <= 255 ; i++ ) 
    {
        tira_ordenada[j] = (UChar)i;
        j++;
    }

}
