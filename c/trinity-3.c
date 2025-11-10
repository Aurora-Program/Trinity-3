#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "types.h"



// Genera una Dimension a partir de un número interpretado en base 3.
// 0 -> (c,c,c)
// 1 -> (c,c,u)
// 2 -> (c,c,n)
// 3 -> (c,u,c)
// ...


/*
 * Ordena v -> temp en el orden fijo [Fo, Es, Fn] por cada dimensión.
 * - d.value[i] ∈ {0(c), 1(u), 2(n)} indica la POSICIÓN donde está Fo en v.dims[i].
 * - Entre las otras dos posiciones, exactamente una debe valer 'u' (1) -> Es.
 * - La restante será Fn.
 * Devuelve true si las 3 dimensiones cumplen la lógica; false en caso contrario.
 */


// funcion lut triate inferarencia:

Trit trit_infer(Trit a, Trit b, Trit m) {
    if (m == c) { // AND
        if (a == c || b == c) return c;
        if (a == u && b == u) return u;
        return n;
    } else if (m == u) { // OR
        if (a == u || b == u) return u;
        if (a == c && b == c) return c;
        return n;
    } else { // CONSENSUS
        if (a != n && a == b) return a;
        return n;
    }
}


// esta funcion obtiene los tres formas y  las cabna de 2 en dos,  y hace lo mismo con las y p

Dimension inger_Sintetizer (VectorFFE ve){
    Dimension vr;
    for (int i = 0; i < 3; i++) {
        vr.value[0] = trit_infer(ve.dims[i].value[0], ve.dims[i+1].value[0], ve.dims[i].value[2]);
        vr.value[1] = trit_infer(ve.dims[i].value[1], ve.dims[i].value[2], ve.dims[i].value[0]);
        vr.value[2] = trit_infer(ve.dims[i].value[2], ve.dims[i].value[0], ve.dims[i].value[1]);
    }
    return vr;


    Dimension vr;
    for (int i = 0; i < 3; i++) {
        ve.dims[i].value[0] = trit_infer(ve.dims[i].value[0], ve.dims[i].value[1], ve.dims[i].value[2]);
        ve.dims[i].value[1] = trit_infer(ve.dims[i].value[1], ve.dims[i].value[2], ve.dims[i].value[0]);
        ve.dims[i].value[2] = trit_infer(ve.dims[i].value[2], ve.dims[i].value[0], ve.dims[i].value[1]);
    }
    return vr;





}




Trit  trit_deduce_b(Trit a, Trit m, Trit r) {
    (void)a; // unused in current simplified rule
    if (m == c) { // AND
        if (r == u) return u;
        else return n;
    } else if (m == u) { // OR
        if (r == c) return c;
        else return n;
    } else { // CONSENSUS
        return r;
    }
}

Trit trit_deduce_a(Trit b, Trit m, Trit r) {
    (void)b; // unused in current simplified rule
    if (m == c) { // AND
        if (r == u) return u;
        else return n;
    } else if (m == u) { // OR
        if (r == c) return c;
        else return n;
    } else { // CONSENSUS
        return r;
    }
}

Trit trit_learn_m(Trit a, Trit b, Trit r) {
    if (r == c) {
        if (a == c && b == c) return c;
        if (a == c || b == c) return n;
        return u;
    } else if (r == u) {
        if (a == u || b == u) return u;
        if (a == u && b == u) return u;
        return c;
    } else { // CONSENSUS
        if (a == b && a != n) return n;
        return n;
    }
}



// pero es mas complicaod que este ahora necesitmsmo operar tres vectores de tres dimensiones.
// el primer reto que tenemos es sabes es cual es el rol de cada dimension en cada vector (alfa,beta,gama) = cuale el ES? cuando los sabemos el resto. asi con cada vector... Pero solo hay una forma de saberlo
// intentandolo 0,0,0 0,1,2 1,0,2 ... hasta las 6 combinaciones posibles ( seguimos la serie de Fibannaci para generar las combinaciones?  )
// serie de fibannaci para generar las combinaciones de roles posibles
// 0,1,1,2,3,5,8,13...  => con trists 0,1,2  =>  0=c,1=u,2=n  0,1,2 => c,u,n
//relacion en ternario:
// 0=> 0 0 0
// 1=> 0 0 1
// 2=> 0 0 2
// 3=> 0 1 0
// 5=> 0 2 0
// 8=> 1 0 0
// 13=>1 2 0
// 21=>2 0 1
// 34=>2 1 0

//. Es solo se puedes saber cuando se pone en relacion los tres vectores entres si.
// para ellos hay que ir probanndo hasta encontrar una combinacion que sea completamente cohrente, es decir todos cuadran entre si. los roles elegigods, las m, y los resultados R.
//En eses momento que ya sabemos como funciona, es cuando juntamos todas la diemnsiones del mismo tipo y generamos el vecto de emergencia y usando los valores emergentes
// f(o1,o2,o3) => Oe = f(R1,M1,R2) => Re y f(m1,m2,m3) => Me // tienes setido para ti?


// check order es un algoritmo para ver si el valor de la hipostesis de orde es posiblo:
// por ejemplo si elegimos 0,0,0 todos los Fo Son el primer valor de cada dimension. luego deber haber un 0 en los dos siguientes valores de las otras dos dimensiones. la dimensin restante deberia ser Fn.
// c represetna cero, u uno y n null.
// ejemplo:  hipotesis 0,0,0:  (c,c,n) , (c,u,c) , (u,n,c) => valores de Fo: c,c,u => correcto Valores= ES tiene que ser 0,0,0.  vmeos que 2d posicon del primertc, la 3ra del segundo y la 3ra del segundo.
// se cumple la hipotesis iniciamos el proceso para confirma la coherencia de este caso.
// Fo= c,c,u => Fn = u,n,c => Es = n,c,c
// Lo pasamos al tetraedro para validarlo!

// estrucutura de operador de trigate:





Dimension generar_dimension(int index) {
    Dimension d;

    // Cada posición es un dígito en base 3 (de derecha a izquierda)
    for (int i = 0; i < 3; i++) {
        int digit = index % 3;
        d.value[i] = (Trit)digit;
        index /= 3;
    }

    // Si prefieres que el orden sea izquierda->derecha, invierte el array:
    Trit tmp = d.value[0];
    d.value[0] = d.value[2];
    d.value[2] = tmp;

    return d;
}

#include <stdbool.h>

// d->value[i] = índice donde está Es en la dimensión i  (0,1,2)
// v->dims[i].value[es_idx] = índice (codificado como Trit) donde está Fo: c(0), u(1), n(2)
// Regla: Es NO puede auto-apuntarse -> v->dims[i].value[es_idx] != u (1)
bool ordenar_vector_por_roles(const VectorFFE *v, const Dimension *d, VectorFFE *temp) {
    if (!v || !d || !temp) return false;

    for (int i = 0; i < 3; ++i) {
        int es_idx = (int)d->value[i];
        if (es_idx < 0 || es_idx > 2) return false;

        Trit apuntado_por_es = v->dims[i].value[es_idx];

        // Prohibido “auto-apuntarse”: si Es apunta a 1(u), se está apuntando a sí misma
        if (apuntado_por_es == u) return false;

        // Fo es el índice al que apunta Es (0=c o 2=n)
        int fo_idx = (int)apuntado_por_es;  // 0 o 2
        if (fo_idx != 0 && fo_idx != 2) return false; // por seguridad

        // Fn es el que falta
        int fn_idx = 3 - fo_idx - es_idx;

        // Salida SIEMPRE en orden [Fo, Es, Fn]
        temp->dims[i].value[0] = v->dims[i].value[fo_idx];
        temp->dims[i].value[1] = v->dims[i].value[es_idx];
        temp->dims[i].value[2] = v->dims[i].value[fn_idx];
    }
    return true;
}





int calcFn(int Fo, int Es) {
    // Implementa la lógica para calcular Fn basado en Fo y Es
    // Esta es una función de ejemplo; ajusta según la lógica real
    // Si Fo y Es coinciden, por defecto devolvemos Fo (permite hipótesis como 0,0,0)
    if (Fo == Es) return Fo;
    if (Fo == c && Es == n) return u;
    if (Fo == u && Es == c) return n;
    if (Fo == n && Es == c) return u;
    return n; // Valor por defecto
}


// Comprobamos la hipótesis de orden es validad para esta asignacin fibonaccio
// parametors: v vector de analis, o valore de orden a comprabar, Temp parametro de salida de vector temporal para pasar al analis del tetraedro si funciona.

bool checkOrder(VectorFFE* v, Dimension o, VectorFFE* Temp) {
    // Interpretamos la hipótesis `o` como una permutación de índices:
    // o.value[0] = índice para Fo, o.value[1] = índice para Fn, o.value[2] = índice para Es
    int fo = (int)o.value[0];
    int fn = (int)o.value[1];
    int es = (int)o.value[2];

    // Validar índices
    if (fo < 0 || fo > 2 || fn < 0 || fn > 2 || es < 0 || es > 2) {
        return false;
    }

    // Reordenamos cada dimensión según la hipótesis (simple reordenamiento)
    for (int i = 0; i < 3; ++i) {
        Temp->dims[i].value[0] = v->dims[i].value[fo];
        Temp->dims[i].value[1] = v->dims[i].value[fn];
        Temp->dims[i].value[2] = v->dims[i].value[es];
    }

    // Para validar coherencia aplicamos la regla trigate: Fn debe ser calcFn(Fo, Es)
    // Temp layout: [0]=Fo, [1]=Fn, [2]=Es
    for (int i = 0; i < 3; ++i) {
        int fo_val = (int)Temp->dims[i].value[0];
        int es_val = (int)Temp->dims[i].value[2];
        int expected_fn = calcFn(fo_val, es_val);
        if (expected_fn != (int)Temp->dims[i].value[1]) {
            return false; // inconsistencia en esta dimension
        }
    }
    return true;
}

// -------------------- EMERGENCIA -------------------- //
// 3 trits -> 1 trit (mayoría; si todos distintos -> u)
static inline Trit emerger(Trit a, Trit b, Trit c) {
    if (a == b || a == c) return a;  // mayoría a
    if (b == c) return b;            // mayoría b
    return u;                        // todos distintos -> u
}

// 1 trit -> 3 tríos canónicos que sintetizan en r
// out[k] es un trío (Dimension) con 3 trits.
static inline void descender(Trit r, Dimension out[3]) {
    switch (r) {
        case c:
            // Todos sintetizan a c (mayoría c)
            out[0] = (Dimension){{c, c, c}};
            out[1] = (Dimension){{c, c, u}}; 
            out[2] = (Dimension){{c, c, n}};
            break;
        case u:
            // Incluimos el caso "todos distintos" explícito (c,u,n)
            out[0] = (Dimension){{u, u, u}};   // mayoría u
            out[1] = (Dimension){{c, u, n}};   // distintos -> u
            out[2] = (Dimension){{u, u, c}};   // mayoría u
            break;
        case n:
            // Todos sintetizan a n (mayoría n)
            out[0] = (Dimension){{n, n, n}};
            out[1] = (Dimension){{n, n, c}};
            out[2] = (Dimension){{n, n, u}};
            break;
    }
}

// load tetraedro, this funciont load 6 trigate from a tetraedro. 3 from Fo, info an the other forn fn base in O. So 






// Operar 3 dimensiones trit-wise:
void dimension_infer(const Dimension* A, const Dimension* B, const Dimension* M, Dimension* out) {
    for (int i = 0; i < 3; i++) {
        out->value[i] = trit_infer(A->value[i], B->value[i], M->value[i]);
    }
}

// deduce B from A,M,R (simple rule)
void dimension_deduce_b(const Dimension* A, const Dimension* M, const Dimension* R, Dimension* out) {
    for (int i = 0; i < 3; i++) {
        out->value[i] = trit_deduce_b(A->value[i], M->value[i], R->value[i]);
    }
}

void dimension_deduce_a(const Dimension* B, const Dimension* M, const Dimension* R, Dimension* out) {
    for (int i = 0; i < 3; i++) {
        out->value[i] = trit_deduce_a(B->value[i], M->value[i], R->value[i]);
    }
}

void dimension_learn_m(const Dimension* A, const Dimension* B, const Dimension* R, Dimension* out) {
    for (int i = 0; i < 3; i++) {
        out->value[i] = trit_learn_m(A->value[i], B->value[i], R->value[i]);
    }
}   


// operar 3 VectorFFE trit-wise:
void vectorFFE_infer(const VectorFFE* A, const VectorFFE* B, const VectorFFE* M, VectorFFE* out) {
    for (int i = 0; i < 3; i++) {
        dimension_infer(&A->dims[i], &B->dims[i], &M->dims[i], &out->dims[i]);
    }
}

// deduce B from A,M,R (simple rule)
void vectorFFE_deduce_b(const VectorFFE* A, const VectorFFE* M, const VectorFFE* R, VectorFFE* out) {
    for (int i = 0; i < 3; i++) {
        dimension_deduce_b(&A->dims[i], &M->dims[i], &R->dims[i], &out->dims[i]);
    }
}

// deduce A from B,M,R (simple rule)
void vectorFFE_deduce_a(const VectorFFE* B, const VectorFFE* M, const VectorFFE* R, VectorFFE* out) {
    for (int i = 0; i < 3; i++) {
        dimension_deduce_a(&B->dims[i], &M->dims[i], &R->dims[i], &out->dims[i]);
    }
}

// learn M from A,B,R (simple rule)
void vectorFFE_learn_m(const VectorFFE* A, const VectorFFE* B, const VectorFFE* R, VectorFFE* out) {
    for (int i = 0; i < 3; i++) {
        dimension_learn_m(&A->dims[i], &B->dims[i], &R->dims[i], &out->dims[i]);
    }
}   

// operar 3 TensorFFE trit-wise:
void tensorFFE_infer(const TensorFFE* A, const TensorFFE* B, const TensorFFE* M, TensorFFE* out) {
    for (int i = 0; i < 3; i++) {
        vectorFFE_infer(&A->vectors[i], &B->vectors[i], &M->vectors[i], &out->vectors[i]);
    }
}


// deduce B from A,M,R (simple rule)
void tensorFFE_deduce_b(const TensorFFE* A, const TensorFFE* M, const TensorFFE* R, TensorFFE* out) {
    for (int i = 0; i < 3; i++) {
        vectorFFE_deduce_b(&A->vectors[i], &M->vectors[i], &R->vectors[i], &out->vectors[i]);
    }
}

// reordenar tensor segun semilla



/*

// reordenar tensor segun semilla

void imprimir_tensorAurora(TensorAurora ta) {
    printf("TensorAurora:\n");
    printf("VectorFFE n1:\n");
    imprimir_vectorFFE(ta.n1);
    printf("\nTensorFFE n2:\n");
    imprimir_tensorFFE(ta.n2);
    printf("\nTensorFFE n3[3]:\n");
    for (int i = 0; i < 3; i++) {
        printf("n3[%d]:\n", i);
        imprimir_tensorFFE(ta.n3[i]);
        printf("\n");
    }
}



#include "util.c"

int main() {
    Dimension d1 = {{c, u, n}};
    Dimension d2 = {{c, u, c}};
    Dimension d3 = {{u, n, n}};

    VectorFFE v1 = {{d1, d2, d1}};
    VectorFFE v2 = {{d3, d2, d3}};
    VectorFFE v3 = {{d1, d1, d3}};

    TensorFFE t1 = {{v1, v2, v3}};
    TensorFFE t2 = {{v2, v2, v1}};
    TensorFFE t3 = {{v1, v3, v1}};

    TensorSimple ts1 = {v1, t1};

    TensorAurora ta1 = {v2, t2, {t3, t1, t2}};

    imprimir_tensorAurora(ta1);

    imprimir_tensorSimple(ts1);

    // fin de main.c

    return 0;
}


// funcion emergencia de dimension - devuevel una dimension creando un hash de tres trits que opera el mismo rol (por ejemplo R) de tres tensores diferentes y devuelve una dimension emergente FFE
// por lo tanto el parametro de entrada es un array de tres dimensiones y el parametro de salida es una dimension



void dimension_emergencia(const VectorFFE* v, Dimension* out) {
    for (int i = 0; i < 3; i++) {
        // Crear una dimension emergente a partir de las tres dimensiones de entrada
        // Aqui simplemente tomamos el valor mayor como ejemplo

        for (int j = 0; j < 3; j++) {
            out->value[j] = n; // Inicializar como null
        }

        Trit a = v->dims[i].value[0];
        Trit b = v->dims[i].value[1];
        Trit c = v->dims[i].value[2];

        if (a == u || b == u || c == u) {
            out->value[i] = u;
        } else if (a == c || b == c || c == c) {
            out->value[i] = c;
        } else {
            out->value[i] = n;
        }
    }
}

// se operan las tres dimensiones de cada role obteniedo las emergcia Re, Me, Oe y se crear un vectorFFE que es la emergencia de los los tres vectores

void vectorFFE_emergencia(const VectorFFE* v, VectorEmergenteFFE* out) {
    for (int i = 0; i < 3; i++) {
        dimension_emergencia(&v->dims[i], &out->dims[i]);
    }
}






// segun esto R seria el valor emergente? // M seria el valor de como se llego a ese valor emergente? // O seria el valor original?


// funcion emergencia de vectorFFE - devuevel una dimension creando un hash de tre 

// Esta funcion obteine tres vectores de de tres tensores diferentes y crea un vector emergente FFE a partir de ellos. Devuelve un vectorFFE


void vectorFFE_emergencia(const TensorFFE* v, VectorEmergenteFFE* out) {
    for (int i = 0; i < 3; i++) {
        dimension_emergencia(&v->dims[i], &out->dims[i]);
    }
}
*/