/*
 * phi_cref_demo.c - Stream natural-language tokens and watch C scalar converge
 *
 * This demo approximates a "pure language" collapse by folding word tensors
 * built from characters and synthesizing them into a running C accumulator.
 * We compute a balanced-ternary scalar from the emergent tensor to visualize
 * convergence; we also print digit strings '+0-' (9 digits: FO,FN,ES).
 *
 * Build:
 *   gcc -std=c11 -O2 -Wall -o phi_cref_demo phi_cref_demo.c aurora_core.o
 */

#include "aurora_core.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Simple char encoder (silábica-like): 3 trits
static void char_vec_word(unsigned char c, Trit out[3]){
    if(c>='A' && c<='Z') c+=32;
    int v = (c=='a'||c=='e'||c=='i'||c=='o'||c=='u');
    int liquid = (c=='l'||c=='r');
    out[0] = v ? 1 : 0;               // Forma: vocal/consonante
    out[1] = liquid ? 1 : 0;          // Función: líquida (transiciones)
    out[2] = v ? -1 : (liquid?0:1);   // Estructura: núcleo/soporte
}

static VectorFFE_Fractal vec_from_char(unsigned char c){
    Trit a[3]; char_vec_word(c,a);
    DimensionFFE d0 = make_dim(a[0], a[1], a[2]);
    // Replicamos para cada dimensión del vector (simplificación)
    return make_vec_f(d0,d0,d0);
}

// Compute an emergent vector from a word by folding windows of size 3
static VectorFFE_Fractal word_emergent_vector(const char* w){
    int n = (int)strlen(w);
    if(n==0){
        DimensionFFE z = make_dim(-1,-1,-1);
        return make_vec_f(z,z,z);
    }
    // Seed using first up to 3 chars
    VectorFFE_Fractal vA = vec_from_char(w[0]);
    VectorFFE_Fractal vB = (n>1)? vec_from_char(w[1]) : vA;
    VectorFFE_Fractal vC = (n>2)? vec_from_char(w[2]) : vB;
    VectorFFE_Fractal emerg_vec = transcender_step(&vA,&vB,&vC);

    for(int i=1; i<=n-3; i++){
        vA = vec_from_char(w[i]);
        vB = vec_from_char(w[i+1]);
        vC = vec_from_char(w[i+2]);
        VectorFFE_Fractal tmp = transcender_step(&vA,&vB,&vC);
        // Fuse by per-trit triadic collapse
        for(int d=0; d<3; d++){
            for(int t=0; t<3; t++){
                Trit a = emerg_vec.d[d].t[t];
                Trit b = tmp.d[d].t[t];
                Trit c = b; // bias to latest
                emerg_vec.d[d].t[t] = (a==b)? a : ((a==c)? a : ((b==c)? b : -1));
            }
        }
    }
    return emerg_vec;
}

static TensorFFE tensor_from_vector(const VectorFFE_Fractal* v){
    // Map FO from d0, FN from d1, ES from d2
    TensorFFE t;
    for(int i=0;i<3;i++){
        t.FO[i] = v->d[0].t[i];
        t.FN[i] = v->d[1].t[i];
        t.ES[i] = v->d[2].t[i];
    }
    harmonize_with_fibonacci(&t);
    return t;
}

int main(){
    const char* stream[] = {
        "ley","orden","armonia","armon\xC3\xADa","luz","camino","recto","verdad",
        // Variantes para robustez
        "justo","equilibrio","proporcion","via","claro","bien","bello","unidad",
        NULL
    };

    printf("=== Cref Streaming (balanced ternary) ===\n");
    TensorFFE C_acc = make_tensor(-1,-1,-1, -1,-1,-1, -1,-1,-1);

    int idx=0; const char* w;
    while((w = stream[idx++]) != NULL){
        VectorFFE_Fractal vword = word_emergent_vector(w);
        TensorFFE Tword = tensor_from_vector(&vword);
        // Synthesize into running C
        if(idx==1){
            C_acc = Tword;
        } else {
            C_acc = synthesize(&C_acc, &Tword);
        }
        harmonize_with_fibonacci(&C_acc);

        // Scalar & digits
        double s = tensor_balanced_scalar(&C_acc);
        char digits[16]; tensor_balanced_digits(&C_acc, digits);
        // Golden ratio for reference
        double phi = (1.0 + sqrt(5.0))*0.5;
        printf("%-10s -> scalar=%.9f  digits=%s  |phi - scalar|=%.9f\n", w, s, digits, fabs(phi - s));
    }

    printf("\nFinal C tensor:\n");
    print_tensor("C_acc", &C_acc);
    char digits[16]; tensor_balanced_digits(&C_acc, digits);
    printf("digits=%s\n", digits);
    return 0;
}
