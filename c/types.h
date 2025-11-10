#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

typedef enum {
    c = 0,
    u = 1,
    n = 2
} Trit;

typedef struct {
    Trit value[3];
} Dimension;

typedef struct {
    Dimension dims[3];
} VectorFFE;



typedef struct {
    VectorFFE vectors[3];
} TensorFFE;

typedef struct {
    VectorFFE vector;
    TensorFFE tensor;
} TensorSimple;

typedef struct {

    VectorFFE n1;
    TensorFFE n2;
    TensorFFE n3[3];

} TensorAurora;

typedef struct {

    TensorFFE n1;
    TensorFFE n2[3];
    TensorSimple n3[3][3];
} Tensorcluster;


struct Trigate_operators
{
    int A;
    int B;
    int M;
    int R;

};


// Estructura para la operacion de tetraedro.

struct Tetraedro_operators
{
    Trigate_operators Fo1;
    Trigate_operators Fo2;
    Trigate_operators Fo3;
    Trigate_operators Fe1;
    Dimnsion S;


};


#endif