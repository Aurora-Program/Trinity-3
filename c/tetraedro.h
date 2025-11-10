#ifndef TETRAEDRO_H
#define TETRAEDRO_H

#include "aurora_core.h"

// ======= TETRAEDRO: 4-faced processor (Sintetizador, Evolver, Extender, Armonizador) =======

// Face outputs
typedef struct {
    Dim M;  // Modo/Relación (cómo se relacionan A y B)
    Dim R;  // Resultado (síntesis de A y B)
    Dim O;  // Orden (organización para coherencia)
} TriGateOutput;

// Tetraedro operates on 3 vectors (A, B, C) producing emergent vector
typedef struct {
    Vector A;  // Vector de entrada 1
    Vector B;  // Vector de entrada 2
    Vector C;  // Vector de entrada 3 (contexto/control)
} TetraedroInput;

typedef struct {
    TriGateOutput synthesis;  // Sintetizador output
    Dim emergent;             // Vector emergente final (Mₛ, Rₛ, Oₛ)
    int coherence_level;      // 0-3: número de dimensiones coherentes
    int null_count;           // Cuenta de valores null (a minimizar)
    bool converged;           // True si alcanzó coherencia total
} TetraedroOutput;

// ======= TRIGATE OPERATIONS =======

// Forward (Sintetizador): A, B → M, R
TriGateOutput trigate_infer(const Dim* A, const Dim* B, const Dim* O_hint);

// Reverse (Extender): M, R, O → A, B (reconstrucción)
typedef struct { Dim A; Dim B; } DimPair;
DimPair trigate_extend(const Dim* M, const Dim* R, const Dim* O);

// Learn (Evolver): A, B, R → M (refinamiento de modo)
Dim trigate_learn(const Dim* A, const Dim* B, const Dim* R);

// Harmonize (Armonizador): ajusta O para minimizar nulls
Dim harmonize_order(const Dim* M, const Dim* R, int iteration);

// ======= TETRAEDRO FULL CYCLE =======

// Procesa 3 vectores completos a través del Tetraedro
TetraedroOutput tetraedro_process(const TetraedroInput* input, int max_iterations);

// Utilities
void print_trigate_output(const TriGateOutput* out, const char* label);
void print_tetraedro_output(const TetraedroOutput* out);

#endif // TETRAEDRO_H
