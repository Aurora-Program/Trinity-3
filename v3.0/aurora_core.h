/*
 * aurora_core.h
 * 
 * NÚCLEO DEL MODELO AURORA - Arquitectura Fractal de Inteligencia
 * 
 * PRINCIPIO FUNDAMENTAL:
 * La inteligencia NO está en el código, está en la GEOMETRÍA del tensor.
 * 
 * ESTRUCTURA FRACTAL JERÁRQUICA 1→3→9:
 * - Nivel 0: Trit (valor cuántico: 0, 1, -1)
 * - Nivel 1: DimensionFFE (3 trits → unidad mínima FFE)
 * - Nivel 2: VectorFFE_Fractal (3 dimensiones → Forma/Función/Estructura)
 * - Nivel 3: TensorFFE_Fractal (3 vectores → operación completa)
 * 
 * La dimensión superior determina el espacio lógico de las inferiores.
 * 
 * Licensed under Apache 2.0 and CC BY 4.0
 */

#ifndef AURORA_CORE_H
#define AURORA_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ===== TIPOS BÁSICOS =====
typedef int Trit; // 0, 1, -1 (null/indeterminado)

// ===== ESTRUCTURA FRACTAL JERÁRQUICA =====
typedef struct { Trit t[3]; } DimensionFFE;                   // Nivel 1: {a,b,c}
typedef struct { DimensionFFE d[3]; } VectorFFE_Fractal;      // Nivel 2: 3 dimensiones
typedef struct { VectorFFE_Fractal v[3]; } TensorFFE_Fractal; // Nivel 3: 3 vectores

// TensorFFE plano (legacy para compatibilidad con síntesis)
typedef struct { Trit FO[3]; Trit FN[3]; Trit ES[3]; } TensorFFE;

// ===== TRIGATE: ÁTOMO DE INTELIGENCIA =====
Trit trit_and(Trit a, Trit b);
Trit trit_or(Trit a, Trit b);
Trit trit_consensus(Trit a, Trit b);
Trit trigate_infer(Trit a, Trit b, Trit m);
Trit trigate_learn(Trit a, Trit b, Trit r);

// Operaciones vectoriales
void vec_infer(const Trit A[3], const Trit B[3], const Trit M[3], Trit out[3]);
void vec_learn_M(const Trit A[3], const Trit B[3], const Trit R[3], Trit Mout[3]);

// Utilidades de comparación y copia
int eq3f(const Trit x[3], const Trit y[3]);
void copy3f(Trit d[3], const Trit s[3]);

// ===== CONSTRUCTORES Y HELPERS =====
const char* trit_to_str(Trit t);
DimensionFFE make_dim(Trit a, Trit b, Trit c);
VectorFFE_Fractal make_vec_f(DimensionFFE d0, DimensionFFE d1, DimensionFFE d2);
TensorFFE_Fractal make_tensor_f(VectorFFE_Fractal v0, VectorFFE_Fractal v1, VectorFFE_Fractal v2);
TensorFFE make_tensor(int r0,int r1,int r2,int m0,int m1,int m2,int o0,int o1,int o2);
int count_nulls(const TensorFFE* t);
void print_tensor(const char* name, const TensorFFE* t);

// Conversión
TensorFFE fractal_to_flat(const TensorFFE_Fractal* tf);

// ===== MEMORIA: REGLAS Y ARQUETIPOS =====
typedef struct { Trit a[3]; Trit b[3]; Trit M[3]; int count; } Rule;
typedef struct { Trit pattern_a[3]; Trit pattern_b[3]; Trit pattern_M[3]; int support; } Archetype;

// Dinámicas (KB de cambio temporal)
typedef struct { Trit d1[3]; Trit d2[3]; Trit M[3]; int count; } DynRule;
typedef struct { Trit pattern_d1[3]; Trit pattern_d2[3]; Trit pattern_M[3]; int support; } DynArchetype;

void upsert_rule_mem(Rule* rules, int* n, const Trit a[3], const Trit b[3], const Trit M[3]);
int synthesize_archetypes(const Rule* rules, int n_rules, Archetype* archs, int max_archs);

void upsert_dyn_rule_mem(DynRule* rules, int* n, const Trit d1[3], const Trit d2[3], const Trit M[3]);
int synthesize_dyn_archetypes(const DynRule* rules, int n_rules, DynArchetype* archs, int max_archs);

// ===== ALGORITMO DE DIOS: ARMONIZADOR =====
// Proceso fijo que descubre lógica, elimina nulls y produce coherencia
void harmonize_with_fibonacci(TensorFFE* t);
void harmonize_guided(TensorFFE* t, const TensorFFE* C);

// ===== TRANSCENDER: SÍNTESIS FRACTAL =====
TensorFFE synthesize(const TensorFFE* a, const TensorFFE* b);

// ===== CREENCIA C: ANCLA DE COHERENCIA =====
TensorFFE build_creencia_tensor_from_pyramids(const TensorFFE* VR, const TensorFFE* VA, const TensorFFE* VD);
void anneal_creencia_tensor(TensorFFE* C, float temperature);
Trit extract_Cref_from_C(const TensorFFE* C);

// ===== DIAGNÓSTICO Y LOP =====
typedef struct {
    float consistency;
    float separability;
    float convergence;
    float overall;
} DiagnosticMetrics;

DiagnosticMetrics diagnose_rules(const Rule* rules, int n_rules);
TensorFFE build_diagnostic_tensor(const DiagnosticMetrics* d, Trit Cref);
TensorFFE build_lop_tensor(const DiagnosticMetrics* d, float accuracy, Trit Cref);

// Causas emergentes
const char* emergent_cause_with_lop(const TensorFFE* diag_t, const Archetype* archs, int n_arch, const TensorFFE* lop);
const char* emergent_cluster_cause(const TensorFFE* VR, const TensorFFE* VA, const TensorFFE* VD);

// ===== EXTENDER: APRENDIZAJE DESDE SALIDA =====
// Paradigma nuevo: entrenar con secuencia de salida completa
typedef struct {
    Trit input[3];   // Tensor del token actual
    Trit output[3];  // Tensor del próximo token (char o separador)
    Trit M[3];       // Modo aprendido
    int count;
} ExtenderRule;

void upsert_extender_rule(ExtenderRule* rules, int* n, const Trit input[3], const Trit output[3], const Trit M[3]);

// ===== TETRAEDRO & TRANSCENDER =====
// Un Tetraedro opera sobre tres vectores (uno por tensor A, B, C)
// y produce una síntesis (M_s, R_s, O_s) para el vector superior.

typedef struct {
    Trit M[3];
    Trit R[3];
    Trit O[3];
} TetraFaceResult;

// Caras funcionales del tetraedro (modos aprendizaje/inferencia)
// Sintetizador: combina A y B (aprende M si tiene R esperado)
void tetra_sintetizador_learn(const Trit A[3], const Trit B[3], const Trit R_exp[3], TetraFaceResult* out);
void tetra_sintetizador_infer(const Trit A[3], const Trit B[3], const Trit M[3], TetraFaceResult* out);

// Evolver: armoniza y refina M/R/O para reducir nulls
void tetra_evolver(TetraFaceResult* fr);

// Extender: proyección directa (igual que sintetizador infer) para claridad semántica
void tetra_extender_infer(const Trit A[3], const Trit B[3], const Trit M[3], TetraFaceResult* out);

// Armonizador: integra caras (si hay varias) por colapso triádico
void tetra_armonizador(const TetraFaceResult* s, const TetraFaceResult* e, const TetraFaceResult* x, TetraFaceResult* out);

// Emergencia: Hash semántico que produce el vector superior
void tetra_emerge(const TetraFaceResult* s, const TetraFaceResult* e, const TetraFaceResult* x, Trit Ms[3], Trit Rs[3], Trit Os[3]);

// Transcender (Nivel 1): aplica tetraedros a v[0], v[1], v[2]
VectorFFE_Fractal transcender_step(const VectorFFE_Fractal* va, const VectorFFE_Fractal* vb, const VectorFFE_Fractal* vc);
TensorFFE_Fractal transcender_n1(const TensorFFE_Fractal* A, const TensorFFE_Fractal* B, const TensorFFE_Fractal* C);

// ===== DESCUBRIMIENTO DE ROLES (FO/FN/ES) =====
// Layout de roles para un vector fractal: cuál dimensión actúa como Forma, Función, Estructura.
typedef struct {
    int idx_FO; // índice (0..2) que se asigna a FO
    int idx_FN; // índice (0..2) que se asigna a FN
    int idx_ES; // índice (0..2) que se asigna a ES
    int nulls_after; // métrica de entropía (nulls) tras armonización de prueba
} RoleLayout;

// Descubre la asignación óptima de roles en un VectorFFE_Fractal
RoleLayout discover_vector_roles(const VectorFFE_Fractal* v);

// Aplica descubrimiento de roles a todo el tensor fractal (por vector)
void algorithm_god_roles(const TensorFFE_Fractal* tf, RoleLayout out_layouts[3]);

// Paso de integración completo: aplica roles + transcender nivel 1
TensorFFE_Fractal algorithm_god_step(const TensorFFE_Fractal* A, const TensorFFE_Fractal* B, const TensorFFE_Fractal* C, RoleLayout layouts_out[3]);

// ===== ESCALAR TERNARIO (BALANCED TERNARY) PARA VISUALIZACIÓN =====
// Convierte un TensorFFE en un escalar en base 3 balanceada (−1,0,+1) con pesos 3^{-(k+1)}
double tensor_balanced_scalar(const TensorFFE* t);
// Representación compacta de 9 dígitos balanceados: '+', '0', '-'
void tensor_balanced_digits(const TensorFFE* t, char out[16]);
int count_nulls(const TensorFFE* t);
void print_tensor(const char* name, const TensorFFE* t);

#endif // AURORA_CORE_H
