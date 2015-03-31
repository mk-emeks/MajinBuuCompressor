
#include "blockSorting.h"

// sirve para ir llevando la posicion donde se encuentra el S del Block Sorting
void actualizarPosicionIndice(LInt &indice, LInt* vec, LInt n, LInt p) {
    if (vec[n] == 0) {
        indice = n;         
    }
    if (vec[p] == 0) {
        indice = p;
    }
}

// esta funcion evalua si vec[n] es mayor que vec[p] y devuelve TRUE en caso positivo y FALSE en caso negativo.
bool mayorQue(LInt* vec, LInt n, LInt p, UChar* tira_texto, LInt &long_tira) {
   
        LInt nAux = vec[n];
        LInt pAux = vec[p];
        LInt h = 0;

        while ((tira_texto[nAux] == tira_texto[pAux]) && h < long_tira) {
            nAux++;
            pAux++;

            if(nAux == long_tira) {nAux = 0;};
            if(pAux == long_tira) {pAux = 0;};

            h++;
        }

        return (tira_texto[nAux] > tira_texto[pAux]);

}

LInt blockSorting(UChar* salidaBs , UChar* tira_texto, LInt long_tira) {
    LInt indice = 0 ;

    LInt* vec = new LInt[long_tira]; // (!) debe ser dinamico! (limitacion del c++ para estaticos)


    for (LInt i=0 ; i < long_tira ; i++) {

        vec[i] = i;
        //cout<< vec[i].posicion << " y " << vec[i].longitud << endl;

    }

    // ARMO EL MONTICULO
    for (LInt heapsize = 0; heapsize < long_tira; heapsize++) {
        LInt n = heapsize; // HIJO
        while (n > 0) {
            LInt p = floor((n-1)/2); // PADRE
            if (mayorQue(vec,n,p,tira_texto,long_tira)) {
                LInt aux = vec[n];
                vec[n] = vec[p];
                vec[p] = aux;
                actualizarPosicionIndice(indice,vec,n,p);
                n = p;
            }
            else {
                break;
            }
        }
    }
    
    // ORDENO CON HEAPSORT
    LInt heapsize = long_tira;    
    while (heapsize > 0) {
        LInt aux = vec[0];
        vec[0] = vec[heapsize-1];
        vec[heapsize-1] = aux;
        actualizarPosicionIndice(indice,vec,0,heapsize-1);
        heapsize--;

        LInt n = 0;
        while (true) {
            LInt left = (n*2)+1;
            if (left >= heapsize) {
                break;
            }
            LInt right = left+1;
            if (right >= heapsize) {
                if (mayorQue(vec,left,n,tira_texto,long_tira)) {
                    LInt aux = vec[left];
                    vec[left] = vec[n];
                    vec[n] = aux;
                    actualizarPosicionIndice(indice,vec,left,n);    
                }
                break;    
            }
            if (mayorQue(vec,left,n,tira_texto,long_tira)) {
                if (mayorQue(vec,left,right,tira_texto,long_tira)) {
                    LInt aux = vec[left];
                    vec[left] = vec[n];
                    vec[n] = aux;
                    actualizarPosicionIndice(indice,vec,left,right);
                    n = left;
                }
                else {
                    LInt aux = vec[n];
                    vec[n] = vec[right];
                    vec[right] = aux;
                    actualizarPosicionIndice(indice,vec,n,right);
                    n = right;
                }
            }
            else {
                if (mayorQue(vec,right,n,tira_texto,long_tira)) {
                    LInt aux = vec[n];
                    vec[n] = vec[right];
                    vec[right] = aux;
                    actualizarPosicionIndice(indice,vec,n,right);
                    n = right;
                }
                else {
                    break;
                }
            }
        }
    }

    // preparamos la salida de block sorting: -> Conseguimos la ultima "columna":

    for ( LInt i = 0 ; i < long_tira ; i++) {

        if (vec[i] == 0) {
            salidaBs[i] = tira_texto[long_tira-1];
        } 
        else {
            salidaBs[i] = tira_texto[vec[i]-1];  
        }
    }

    
    // salida por consola
    /*for ( int i = 0 ; i < long_tira ; i++ ) {

        mostrarBloqueCaracteres(vec[i].posicion,vec[i].longitud);
    }*/
    delete [] vec;
    return indice;

}

UChar* vueltaBlockSorting(UChar* salidaBS, LInt indice, LInt long_tira) {

    LInt i = indice;

    // vectores que se crean para el funcionamiento del algoritmo. Estos se explican en el paper de Burrows-Wheeler, con los mismos nombres.
    LInt C[256];
    LInt P[long_tira];

    // carga C (Instrucciones en el paper de Burrows-Wheeler)
    // inicializo todos los datos de C en MAXIMOLINT, que es el MAXIMOLINT valor para nuestro tipo de dato.
    for (int h = 0; h < 256; h++) {
        C[h] = MAXIMOLINT;
    }

    // para cada dato de la tira al que no se le calculo su valor en C, se le calcula. Si ya fue calculado, sigue adelante.
    // optimizado para minimizar operaciones aritmeticas. Peor caso: 256*N
    for (LInt g = 0; g < long_tira; g++) {
        if (C[(int)salidaBS[g]] == MAXIMOLINT) {
            C[(int)salidaBS[g]] = 0;
            for (LInt t = 0; t < long_tira; t++) {
                if ((int)salidaBS[t] < (int)salidaBS[g]) {
                    C[(int)salidaBS[g]] = C[(int)salidaBS[g]] + 1;
                }
            } 
        }
    }

    // carga P (Instrucciones en el paper de Burrows-Wheeler)
    // optimizado para minimizar operaciones aritmeticas.
    LInt ultimaPos[256];
    for (int y = 0; y < 256; y++) {
        ultimaPos[y] = 0;
    }

    P[0] = 0;
    for (LInt t = 1; t < long_tira; t++) {
        
        P[t] = P[ultimaPos[(int)salidaBS[t]]];
        for (LInt h = ultimaPos[(int)salidaBS[t]]; h < t; h++) {
            if (salidaBS[h] == salidaBS[t]) {
                P[t] = P[t] + 1;
            }
        }
        ultimaPos[(int)salidaBS[t]] = t;
    }
    
    UChar* vueltaBS = new UChar[long_tira];
    
    // arma la tira original de vuelta.
    for (LInt j = long_tira-1; j > 0; j--) {
        vueltaBS[j] = salidaBS[i];
        i = P[i] + C[(int)salidaBS[i]];    
    }
    vueltaBS[0] = salidaBS[i];
    
    return vueltaBS;
}