#ifndef UTIL_H
#define UTIL_H

#include "types.h"

// utility printing
char trit_char(Trit t);
void imprimir_trit(Trit trit);
void imprimir_dimension(Dimension dim);            // print a single Dimension
void imprimir_vectorFFE(VectorFFE vec);            // compact vector printer
void imprimir_vectorOrdenado(const VectorFFE *t);  // detailed ordered-vector printer
void imprimir_tensorFFE(TensorFFE tensor);
void imprimir_tensorAurora(TensorAurora ta);
void imprimir_tensorSimple(TensorSimple ts);


// Prototipo de tus nuevas funciones (defined in trinity-3.c):
Dimension generar_dimension(int index);
bool ordenar_vector_por_roles(const VectorFFE *v, const Dimension *d, VectorFFE *temp);
bool ordenar_vector_por_roles_desde_Es(const VectorFFE *v, const Dimension *d, VectorFFE *temp);

#endif