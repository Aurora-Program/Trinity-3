#include <stdio.h>
#include <stdbool.h>


#include "trinity-3.c"
#include "util.h"
#include "types.h"





// --- Utilidades de impresión ---


// --- main de prueba ---
int main(void) {
    // Tus datos de ejemplo
    Dimension d1 = {{u, c, n}};
    Dimension d2 = {{c, c, c}};
    Dimension d3 = {{u, c, n}};

    // Guardamos todas las dimensiones en un vector de análisis para poder operar con bucles.
    VectorFFE v1 = {{d1, d2, d3}};
    VectorFFE t;  // salida ordenada Fo,Es,Fn

    // d_orden: “contador base 3”
    // generar_dimension(0) -> (c,c,c)
    Dimension d_orden = generar_dimension(0);

    for (int i=0; i<27; ++i) { // 3^3 = 27 combinaciones
        d_orden = generar_dimension(i);
        printf("Probando orden %d: (%c,%c,%c)\n", i,
               trit_char(d_orden.value[0]),
               trit_char(d_orden.value[1]),
               trit_char(d_orden.value[2]));

        bool coherente = ordenar_vector_por_roles(&v1, &d_orden, &t);
        if (coherente) {
            puts("Vector potencialmente coherente. Salida ordenada:");
            imprimir_vectorOrdenado(&t);
            //terminar
          //  break;
        } else {
            puts("Vector INCOHERENTE según la regla (no se pudo ordenar).");
        }
        puts("-----");
    }   

    bool coherente = ordenar_vector_por_roles(&v1, &d_orden, &t);




    if (coherente) {
        puts("Vector potencialmente coherente. Salida ordenada:");
        imprimir_vectorOrdenado(&t);
    } else {
        puts("Vector INCOHERENTE según la regla (no se pudo ordenar).");
    }

    // Puedes probar más casos:
    // d_orden = generar_dimension(3); // -> (c,u,c)
    // coherente = ordenar_vector_por_roles(&v1, &d_orden, &t);
    // ...

    return 0;
}