//funcion imprimir trit:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


void imprimir_trit(Trit trit) {
    switch (trit) {
        case c:   printf("0"); break;
        case u:   printf("1"); break;
        case n:   printf("n"); break;
    }
}

//funcion imprimir dimension:


void imprimir_vectorFFE(VectorFFE vec) {
    printf("[");
    for (int i = 0; i < 3; i++) {
   
        // print each Dimension
        imprimir_dimension(vec.dims[i]);
        if (i < 2) {
            printf(",");       
    }
    

}
printf("]");

}


void imprimir_tensorFFE(TensorFFE tensor) {
    printf("{");
    for (int i = 0; i < 3; i++) {
       
        imprimir_vectorFFE(tensor.vectors[i]);
        if (i < 2)
        {
            printf(";");
        }
              
    }

}

// funcion: imprimir tensorSimple
void imprimir_tensorSimple(TensorSimple ts) {

    imprimir_vectorFFE(ts.vector);
    printf("\n");
    imprimir_tensorFFE(ts.tensor);
}


void imprimir_tensorAurora(TensorAurora ta) {

    printf("n1: ");
    imprimir_vectorFFE(ta.n1);
    printf("\n");

    printf("n2: ");
    imprimir_tensorFFE(ta.n2);
    printf("\n");

    printf("n3: ");
    for (int i = 0; i < 3; i++) {
      
        imprimir_tensorFFE(ta.n3[i]);
        if (i < 2) {
            printf("-");
        }
        
    }
    printf("\n");
}


char trit_char(Trit t) { return t == c ? 'c' : (t == u ? 'u' : 'n'); }

// print a single Dimension as (a,b,c)
void imprimir_dimension(Dimension dim) {
    putchar('(');
    putchar(trit_char(dim.value[0])); putchar(',');
    putchar(trit_char(dim.value[1])); putchar(',');
    putchar(trit_char(dim.value[2]));
    putchar(')');
}

// detailed printer for an ordered VectorFFE (used by main)
void imprimir_vectorOrdenado(const VectorFFE *t) {
    if (!t) return;
    for (int i = 0; i < 3; ++i) {
        printf("Dim %d [Fo,Fn,Es]: ", i);
        printf("(%c,%c,%c)\n",
               trit_char(t->dims[i].value[0]),
               trit_char(t->dims[i].value[1]),
               trit_char(t->dims[i].value[2]));
    }
}