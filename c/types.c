#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
