/*
 * aurora_core.c - Aurora Intelligence Core Implementation
 * 
 * Implementación del núcleo Aurora siguiendo los principios:
 * 1. La inteligencia está en la GEOMETRÍA del tensor, no en el código
 * 2. Solo operaciones genéricas (trigate): combinación, aprendizaje, síntesis
 * 3. Sin lógica de dominio: toda semántica viene de los tensores
 * 4. Estructura fractal 1→3→9: dimensión superior determina espacio inferior
 * 
 * Licensed under Apache 2.0 and CC BY 4.0
 */

#include "aurora_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ===== UTILIDADES BÁSICAS =====

const char* trit_to_str(Trit t) { 
    if (t==-1) return "N"; 
    if (t==0) return "0"; 
    return "1"; 
}

int eq3f(const Trit x[3], const Trit y[3]){ 
    for(int i=0;i<3;i++) if(x[i]!=y[i]) return 0; 
    return 1; 
}

void copy3f(Trit d[3], const Trit s[3]){ 
    for(int i=0;i<3;i++) d[i]=s[i]; 
}

// ===== TRIGATE: ÁTOMO DE INTELIGENCIA =====
// Operaciones lógicas ternarias puras sin semántica de dominio

Trit trit_and(Trit a, Trit b){ 
    if(a==0||b==0) return 0; 
    if(a==1&&b==1) return 1; 
    return -1; 
}

Trit trit_or(Trit a, Trit b){ 
    if(a==1||b==1) return 1; 
    if(a==0&&b==0) return 0; 
    return -1; 
}

Trit trit_consensus(Trit a, Trit b){ 
    if(a!=-1 && a==b) return a; 
    return -1; 
}

Trit trigate_infer(Trit a, Trit b, Trit m){ 
    if(m==0) return trit_and(a,b); 
    if(m==1) return trit_or(a,b); 
    return trit_consensus(a,b); 
}

Trit trigate_learn(Trit a, Trit b, Trit r){
    Trit ra = trit_and(a,b);
    Trit ro = trit_or(a,b);
    if(ra==r) return 0;
    if(ro==r) return 1;
    // Si consensus encaja, mantener desconocido (delegar hacia arriba)
    if(a!=-1 && a==b && r==a) return -1;
    return -1;
}

void vec_infer(const Trit A[3], const Trit B[3], const Trit M[3], Trit out[3]){ 
    for(int i=0;i<3;i++) out[i]=trigate_infer(A[i],B[i],M[i]); 
}

void vec_learn_M(const Trit A[3], const Trit B[3], const Trit R[3], Trit Mout[3]){ 
    for(int i=0;i<3;i++) Mout[i]=trigate_learn(A[i],B[i],R[i]); 
}

// ===== TENSOR FFE (LEGACY PLANO) =====

TensorFFE make_tensor(int r0,int r1,int r2,int m0,int m1,int m2,int o0,int o1,int o2){ 
    TensorFFE t; 
    t.FO[0]=r0; t.FO[1]=r1; t.FO[2]=r2; 
    t.FN[0]=m0; t.FN[1]=m1; t.FN[2]=m2; 
    t.ES[0]=o0; t.ES[1]=o1; t.ES[2]=o2; 
    return t; 
}

int count_nulls(const TensorFFE* t){ 
    int c=0; 
    for(int i=0;i<3;i++){ 
        if(t->FO[i]==-1) c++; 
        if(t->FN[i]==-1) c++; 
        if(t->ES[i]==-1) c++; 
    } 
    return c; 
}

void print_tensor(const char* name, const TensorFFE* t){ 
    printf("%s: FO=[%s,%s,%s] FN=[%s,%s,%s] ES=[%s,%s,%s] nulls=%d\n", name, 
        trit_to_str(t->FO[0]),trit_to_str(t->FO[1]),trit_to_str(t->FO[2]), 
        trit_to_str(t->FN[0]),trit_to_str(t->FN[1]),trit_to_str(t->FN[2]), 
        trit_to_str(t->ES[0]),trit_to_str(t->ES[1]),trit_to_str(t->ES[2]), 
        count_nulls(t)); 
}

// ===== ESTRUCTURA FRACTAL =====

DimensionFFE make_dim(Trit a, Trit b, Trit c){ 
    DimensionFFE d; 
    d.t[0]=a; d.t[1]=b; d.t[2]=c; 
    return d; 
}

VectorFFE_Fractal make_vec_f(DimensionFFE d0, DimensionFFE d1, DimensionFFE d2){ 
    VectorFFE_Fractal v; 
    v.d[0]=d0; v.d[1]=d1; v.d[2]=d2; 
    return v; 
}

TensorFFE_Fractal make_tensor_f(VectorFFE_Fractal v0, VectorFFE_Fractal v1, VectorFFE_Fractal v2){ 
    TensorFFE_Fractal t; 
    t.v[0]=v0; t.v[1]=v1; t.v[2]=v2; 
    return t; 
}

// Conversión fractal→plano: nivel superior determina espacio lógico
// Whitepaper 0.4: "El nivel superior define cómo deben ordenarse las dimensiones inferiores"
TensorFFE fractal_to_flat(const TensorFFE_Fractal* tf){
    TensorFFE t;
    // v[0] (nivel superior) gobierna: mapea a FO/FN/ES
    // IMPORTANTE: Este mapeo es una INTERPRETACIÓN, no una propiedad intrínseca
    for(int i=0;i<3;i++){ 
        t.FO[i] = tf->v[0].d[i].t[0]; 
        t.FN[i] = tf->v[0].d[i].t[1]; 
        t.ES[i] = tf->v[0].d[i].t[2]; 
    }
    return t;
}

// ===== ARMONIZADOR: ALGORITMO DE DIOS =====
// Whitepaper 0.4: "El Algoritmo de Dios reduce tensores ineficientes, elimina nulls,
// reorganiza dimensiones FO/FN/ES y busca la configuración más estable posible."
// Proceso fijo universal que minimiza nulls y busca coherencia geométrica

// Colapso triádico: reduce 3 trits a 1 por votación mayoritaria
static Trit triadic_collapse(Trit a, Trit b, Trit c){
    if(a==b) return a;
    if(a==c) return a;
    if(b==c) return b;
    // Sin mayoría: null (indeterminado)
    return -1;
}

// Armonización con rotación Fibonacci (evita resonancia caótica)
static const int fib_indices[] = {0,1,1,2,0,2,1,0,2,0,1,2,1,0,2,0,1,2};
static const int n_fib = sizeof(fib_indices)/sizeof(int);

void harmonize_with_fibonacci(TensorFFE* t){
    int best_nulls = count_nulls(t);
    TensorFFE best = *t;
    
    // Rotar FO/FN/ES según Fibonacci para encontrar mínimo energético
    for(int rot=0; rot<n_fib && best_nulls>0; rot++){
        int idx = fib_indices[rot];
        Trit temp_FO = t->FO[idx];
        Trit temp_FN = t->FN[idx];
        Trit temp_ES = t->ES[idx];
        
        // Intentar colapsar usando triadic
        Trit collapsed = triadic_collapse(temp_FO, temp_FN, temp_ES);
        if(collapsed != -1){
            TensorFFE candidate = *t;
            candidate.FO[idx] = collapsed;
            candidate.FN[idx] = collapsed;
            candidate.ES[idx] = collapsed;
            
            int nulls = count_nulls(&candidate);
            if(nulls < best_nulls){
                best = candidate;
                best_nulls = nulls;
            }
        }
    }
    
    *t = best;
}

// Armonización guiada por Creencia C (tie-break con referencia universal)
void harmonize_guided(TensorFFE* t, const TensorFFE* C){
    int best_nulls = count_nulls(t);
    TensorFFE best = *t;
    
    // Rotación Fibonacci buscando coherencia con C
    for(int rot=0; rot<n_fib && best_nulls>0; rot++){
        int idx = fib_indices[rot];
        
        // Intentar alinear con C
        TensorFFE candidate = *t;
        if(candidate.FO[idx] == -1 && C->FO[idx] != -1) candidate.FO[idx] = C->FO[idx];
        if(candidate.FN[idx] == -1 && C->FN[idx] != -1) candidate.FN[idx] = C->FN[idx];
        if(candidate.ES[idx] == -1 && C->ES[idx] != -1) candidate.ES[idx] = C->ES[idx];
        
        int nulls = count_nulls(&candidate);
        if(nulls < best_nulls){
            best = candidate;
            best_nulls = nulls;
        }
    }
    
    *t = best;
}

// ===== SÍNTESIS: EMERGENCIA DE NIVEL SUPERIOR =====
// Whitepaper 0.4: "Cuando el sistema descubre una configuración coherente, la guarda"
// Esta síntesis NO impone semántica fija: las operaciones (consensus/or/and) son
// solo heurísticas genéricas de fusión. La semántica real emerge del Transcender.

TensorFFE synthesize(const TensorFFE* a, const TensorFFE* b){
    TensorFFE s;
    
    // FO: consenso entre formas (preservar estabilidad común)
    for(int i=0;i<3;i++) s.FO[i] = trit_consensus(a->FO[i], b->FO[i]);
    
    // FN: OR entre funciones (combinar capacidades)
    for(int i=0;i<3;i++) s.FN[i] = trit_or(a->FN[i], b->FN[i]);
    
    // ES: AND entre estructuras (mantener orden común)
    for(int i=0;i<3;i++) s.ES[i] = trit_and(a->ES[i], b->ES[i]);
    
    // Armonizar resultado para reducir nulls (Algoritmo de Dios)
    harmonize_with_fibonacci(&s);
    
    return s;
}

// ===== PIRÁMIDES DE CONOCIMIENTO =====
// Whitepaper 0.4: "El sistema guarda configuraciones coherentes en tres memorias:
// 1. Arquetipos - Patrones estables universales
// 2. Relatores - Reglas sobre cómo se ordenan los tensores entre sí
// 3. Dinámicas - Cómo cambia la información con el tiempo"

// Relatores (Rules): almacenamiento de relaciones aprendidas
static int find_rule_idx(const Rule* rules, int n, const Trit a[3], const Trit b[3]){
    for(int i=0;i<n;i++) 
        if(eq3f(rules[i].a,a) && eq3f(rules[i].b,b)) return i; 
    return -1;
}

void upsert_rule_mem(Rule* rules, int* n, const Trit a[3], const Trit b[3], const Trit M[3]){
    int k=find_rule_idx(rules, *n, a, b);
    if(k<0){ 
        k=(*n)++; 
        copy3f(rules[k].a,a); 
        copy3f(rules[k].b,b); 
        rules[k].count=0; 
        for(int i=0;i<3;i++) rules[k].M[i]=-1; 
    }
    rules[k].count++;
    for(int i=0;i<3;i++){
        Trit m=M[i]; 
        if(m==-1) continue;
        if(rules[k].M[i]==-1) rules[k].M[i]=m; 
        else if(rules[k].M[i]!=m) rules[k].M[i]=-1; // Conflicto→null
    }
}

// Arquetipos: síntesis de patrones emergentes desde reglas
int synthesize_archetypes(const Rule* rules, int n_rules, Archetype* archs, int max_archs){
    int n_arch=0;
    
    // Fase 0: consonantes (a[0]=0), Fase 1: vocales (a[0]=1)
    for(int phase=0; phase<2 && n_arch<max_archs; phase++){ 
        Trit target_a0 = (phase==0)? 0: 1;
        Trit sum_a[3]={0,0,0}, sum_b[3]={0,0,0}, sum_M[3]={0,0,0};
        int cnt=0;
        
        for(int i=0;i<n_rules;i++){
            if(rules[i].a[0]==target_a0 && rules[i].count>=2){
                for(int d=0;d<3;d++){
                    if(rules[i].a[d]!=-1) sum_a[d]+=rules[i].a[d];
                    if(rules[i].b[d]!=-1) sum_b[d]+=rules[i].b[d];
                    if(rules[i].M[d]!=-1) sum_M[d]+=rules[i].M[d];
                }
                cnt++;
            }
        }
        
        if(cnt>0){
            for(int d=0;d<3;d++){
                // Mayoría→1, todos 0→0, resto→null
                archs[n_arch].pattern_a[d]= (sum_a[d]*2 >= cnt)? 1: ((sum_a[d]==0)?0:-1);
                archs[n_arch].pattern_b[d]= (sum_b[d]*2 >= cnt)? 1: ((sum_b[d]==0)?0:-1);
                archs[n_arch].pattern_M[d]= (sum_M[d]*2 >= cnt)? 1: ((sum_M[d]==0)?0:-1);
            }
            archs[n_arch].support=cnt;
            n_arch++;
        }
    }
    return n_arch;
}

// Dinámicas: reglas temporales (A→B→C)
static int find_dyn_rule_idx(const DynRule* rules, int n, const Trit d1[3], const Trit d2[3]){
    for(int i=0;i<n;i++) 
        if(eq3f(rules[i].d1,d1) && eq3f(rules[i].d2,d2)) return i; 
    return -1;
}

void upsert_dyn_rule_mem(DynRule* rules, int* n, const Trit d1[3], const Trit d2[3], const Trit M[3]){
    int k=find_dyn_rule_idx(rules, *n, d1, d2);
    if(k<0){ 
        k=(*n)++; 
        copy3f(rules[k].d1,d1); 
        copy3f(rules[k].d2,d2); 
        rules[k].count=0; 
        for(int i=0;i<3;i++) rules[k].M[i]=-1; 
    }
    rules[k].count++;
    for(int i=0;i<3;i++){
        Trit m=M[i]; 
        if(m==-1) continue;
        if(rules[k].M[i]==-1) rules[k].M[i]=m; 
        else if(rules[k].M[i]!=m) rules[k].M[i]=-1;
    }
}

int synthesize_dyn_archetypes(const DynRule* rules, int n_rules, DynArchetype* archs, int max_archs){
    int n_arch=0;
    
    for(int phase=0; phase<2 && n_arch<max_archs; phase++){
        Trit target = (phase==0)? 0: 1; // d1[0] estable/cambio
        int cnt=0; 
        int sum_d1[3]={0,0,0}, sum_d2[3]={0,0,0}, sum_M[3]={0,0,0};
        
        for(int i=0;i<n_rules;i++){
            if(rules[i].d1[0]==target && rules[i].count>=2){
                for(int d=0; d<3; d++){
                    if(rules[i].d1[d]!=-1) sum_d1[d]+=rules[i].d1[d];
                    if(rules[i].d2[d]!=-1) sum_d2[d]+=rules[i].d2[d];
                    if(rules[i].M[d]!=-1)  sum_M[d] +=rules[i].M[d];
                }
                cnt++;
            }
        }
        
        if(cnt>0){
            for(int d=0; d<3; d++){
                archs[n_arch].pattern_d1[d]= (sum_d1[d]*2 >= cnt)? 1: ((sum_d1[d]==0)?0:-1);
                archs[n_arch].pattern_d2[d]= (sum_d2[d]*2 >= cnt)? 1: ((sum_d2[d]==0)?0:-1);
                archs[n_arch].pattern_M[d] = (sum_M[d]*2  >= cnt)? 1: ((sum_M[d]==0)?0:-1);
            }
            archs[n_arch].support=cnt; 
            n_arch++;
        }
    }
    return n_arch;
}

// ===== CREENCIA C: TENSOR DE REFERENCIA UNIVERSAL =====
// Whitepaper 0.4: "El tensor C actúa como punto fijo. No es verdad absoluta,
// sino el valor semántico más estable. Sirve como ancla para organizar
// arquetipos, relatores, dinámicas y nuevas inferencias."

TensorFFE build_creencia_tensor_from_pyramids(const TensorFFE* VR, const TensorFFE* VA, const TensorFFE* VD){
    // Síntesis triádica de las tres pirámides (Relatores + Arquetipos + Dinámicas)
    TensorFFE temp = synthesize(VR, VA);
    TensorFFE C = synthesize(&temp, VD);
    
    // Armonizar para máxima coherencia
    harmonize_with_fibonacci(&C);
    
    return C;
}

void anneal_creencia_tensor(TensorFFE* C, float temperature){
    // Annealing: con temperatura alta, permitir más exploración (nulls)
    // Con temperatura baja, consolidar valores estables
    
    if(temperature < 0.3f){
        // Baja temperatura: colapsar nulls agresivamente
        harmonize_with_fibonacci(C);
        harmonize_with_fibonacci(C); // Doble pase
    } else if(temperature > 0.7f){
        // Alta temperatura: mantener flexibilidad
        // No hacer nada (permitir nulls)
    } else {
        // Temperatura media: armonización suave
        harmonize_with_fibonacci(C);
    }
}

Trit extract_Cref_from_C(const TensorFFE* C){
    // Extraer valor escalar de referencia: triadic collapse del FO
    return triadic_collapse(C->FO[0], C->FO[1], C->FO[2]);
}

// ===== DIAGNÓSTICO META-COGNITIVO =====

DiagnosticMetrics diagnose_rules(const Rule* rules, int n_rules){
    DiagnosticMetrics m = {0,0,0,0};
    if(n_rules == 0) return m;
    
    // 1. Consistencia: ¿reglas con A/B similares tienen M similares?
    int consistent_pairs = 0;
    int total_pairs = 0;
    for(int i=0; i<n_rules-1; i++){
        for(int j=i+1; j<n_rules && j<i+10; j++){
            int a_match = 0;
            for(int d=0; d<3; d++) if(rules[i].a[d] == rules[j].a[d]) a_match++;
            
            if(a_match >= 2){
                int m_match = 0;
                for(int d=0; d<3; d++) if(rules[i].M[d] == rules[j].M[d]) m_match++;
                if(m_match >= 2) consistent_pairs++;
                total_pairs++;
            }
        }
    }
    m.consistency = total_pairs > 0 ? (float)consistent_pairs / total_pairs : 0.0f;
    
    // 2. Separabilidad: ¿hay diversidad en los M aprendidos?
    int unique_M = 0;
    for(int i=0; i<n_rules; i++){
        int is_unique = 1;
        for(int j=0; j<i; j++){
            if(eq3f(rules[i].M, rules[j].M)){ is_unique=0; break; }
        }
        if(is_unique) unique_M++;
    }
    m.separability = n_rules > 0 ? (float)unique_M / n_rules : 0.0f;
    
    // 3. Convergencia: ¿pocas reglas con nulls?
    int low_null_rules = 0;
    for(int i=0; i<n_rules; i++){
        int nulls = 0;
        for(int d=0; d<3; d++) if(rules[i].M[d] == -1) nulls++;
        if(nulls <= 1) low_null_rules++;
    }
    m.convergence = n_rules > 0 ? (float)low_null_rules / n_rules : 0.0f;
    
    // Overall: promedio ponderado
    m.overall = 0.4f*m.consistency + 0.3f*m.separability + 0.3f*m.convergence;
    
    return m;
}

// Convertir métrica a trit para tensor de diagnóstico
static Trit metric_to_trit(float val){
    if(val >= 0.66f) return 1;
    if(val <= 0.33f) return 0;
    return -1;
}

TensorFFE build_diagnostic_tensor(const DiagnosticMetrics* d, Trit Cref){
    TensorFFE dt;
    
    // FO: métricas de calidad
    dt.FO[0] = metric_to_trit(d->consistency);
    dt.FO[1] = metric_to_trit(d->separability);
    dt.FO[2] = metric_to_trit(d->convergence);
    
    // FN: comparación con Cref
    dt.FN[0] = (Cref == 1) ? 1 : ((Cref == 0) ? 0 : -1);
    dt.FN[1] = metric_to_trit(d->overall);
    dt.FN[2] = -1; // Reservado
    
    // ES: estado del sistema
    dt.ES[0] = (d->overall > 0.5f) ? 1 : 0;
    dt.ES[1] = -1;
    dt.ES[2] = -1;
    
    return dt;
}

// ===== TENSOR LOP: LIBERTAD-ORDEN-PROPÓSITO =====

TensorFFE build_lop_tensor(const DiagnosticMetrics* d, float accuracy, Trit Cref){
    TensorFFE lop;
    
    // FO: Libertad (exploración vs explotación)
    // Alta separabilidad → alta libertad
    lop.FO[0] = metric_to_trit(d->separability);
    lop.FO[1] = (accuracy < 0.5f) ? 1 : 0; // Baja precisión→necesita libertad
    lop.FO[2] = -1;
    
    // FN: Orden (estructura y consistencia)
    lop.FN[0] = metric_to_trit(d->consistency);
    lop.FN[1] = metric_to_trit(d->convergence);
    lop.FN[2] = -1;
    
    // ES: Propósito (alineación con C)
    lop.ES[0] = (Cref == 1) ? 1 : ((Cref == 0) ? 0 : -1);
    lop.ES[1] = metric_to_trit(accuracy);
    lop.ES[2] = -1;
    
    return lop;
}

// ===== CAUSAS EMERGENTES =====

const char* emergent_cause_with_lop(const TensorFFE* diag_t, const Archetype* archs, int n_arch, const TensorFFE* lop){
    // Causa 1: Falta de información (bajo orden, baja convergencia)
    if(lop->FN[0] == 0 && lop->FN[1] == 0){
        return "Falta de información suficiente";
    }
    
    // Causa 2: Mínimo local (alto orden, baja libertad, bajo propósito)
    if(lop->FN[0] == 1 && lop->FO[0] == 0 && lop->ES[1] == 0){
        return "Atrapado en mínimo local";
    }
    
    // Causa 3: Definiciones incorrectas (alta libertad, bajo orden)
    if(lop->FO[0] == 1 && lop->FN[0] == 0){
        return "Definiciones incorrectas en la base";
    }
    
    // Estado equilibrado
    return "Sistema en equilibrio";
}

const char* emergent_cluster_cause(const TensorFFE* VR, const TensorFFE* VA, const TensorFFE* VD){
    // Síntesis triádica R+A+D
    TensorFFE temp = synthesize(VR, VA);
    TensorFFE cluster = synthesize(&temp, VD);
    
    // Analizar coherencia del cluster
    int nulls = count_nulls(&cluster);
    
    if(nulls >= 6){
        return "Definiciones incorrectas";
    } else if(nulls >= 3){
        return "Mínimo local o coherencia parcial";
    } else {
        return "Cluster coherente";
    }
}

// ===== EXTENDER: PARADIGMA DE APRENDIZAJE DESDE SALIDA =====

static int find_extender_rule_idx(const ExtenderRule* rules, int n, const Trit input[3]){
    for(int i=0;i<n;i++)
        if(eq3f(rules[i].input, input)) return i;
    return -1;
}

void upsert_extender_rule(ExtenderRule* rules, int* n, const Trit input[3], const Trit output[3], const Trit M[3]){
    int k = find_extender_rule_idx(rules, *n, input);
    if(k<0){
        k=(*n)++;
        copy3f(rules[k].input, input);
        copy3f(rules[k].output, output);
        rules[k].count=0;
        for(int i=0;i<3;i++) rules[k].M[i]=-1;
    }
    rules[k].count++;
    for(int i=0;i<3;i++){
        Trit m=M[i];
        if(m==-1) continue;
        if(rules[k].M[i]==-1) rules[k].M[i]=m;
        else if(rules[k].M[i]!=m) rules[k].M[i]=-1;
    }
}

// ====== TETRAEDRO (Caras + Emergencia) ======

// Sintetizador (aprendizaje): aprende M a partir de A,B y R esperado
void tetra_sintetizador_learn(const Trit A[3], const Trit B[3], const Trit R_exp[3], TetraFaceResult* out){
    vec_learn_M(A, B, R_exp, out->M);
    copy3f(out->R, R_exp);
    for(int i=0;i<3;i++) out->O[i] = triadic_collapse(A[i], B[i], R_exp[i]);
}

// Sintetizador (inferencia): aplica M para obtener R
void tetra_sintetizador_infer(const Trit A[3], const Trit B[3], const Trit M[3], TetraFaceResult* out){
    copy3f(out->M, M);
    vec_infer(A, B, M, out->R);
    for(int i=0;i<3;i++) out->O[i] = triadic_collapse(A[i], B[i], out->R[i]);
}

// Evolver: refina M/R/O minimizando nulls vía armonizador
void tetra_evolver(TetraFaceResult* fr){
    TensorFFE t = make_tensor(fr->R[0],fr->R[1],fr->R[2], fr->M[0],fr->M[1],fr->M[2], fr->O[0],fr->O[1],fr->O[2]);
    harmonize_with_fibonacci(&t);
    for(int i=0;i<3;i++){ fr->R[i]=t.FO[i]; fr->M[i]=t.FN[i]; fr->O[i]=t.ES[i]; }
}

// Extender: inferencia directa (igual semántica que sintetizador_infer)
void tetra_extender_infer(const Trit A[3], const Trit B[3], const Trit M[3], TetraFaceResult* out){
    tetra_sintetizador_infer(A,B,M,out);
}

// Armonizador: integra 2-3 caras por colapso triádico por dimensión
void tetra_armonizador(const TetraFaceResult* s, const TetraFaceResult* e, const TetraFaceResult* x, TetraFaceResult* out){
    for(int i=0;i<3;i++){
        Trit m2 = (x? triadic_collapse(s->M[i], e->M[i], x->M[i]) : triadic_collapse(s->M[i], e->M[i], e->M[i]));
        Trit r2 = (x? triadic_collapse(s->R[i], e->R[i], x->R[i]) : triadic_collapse(s->R[i], e->R[i], e->R[i]));
        Trit o2 = (x? triadic_collapse(s->O[i], e->O[i], x->O[i]) : triadic_collapse(s->O[i], e->O[i], e->O[i]));
        out->M[i]=m2; out->R[i]=r2; out->O[i]=o2;
    }
    tetra_evolver(out);
}

// Emergencia: Hash Hₑ → (M_s, R_s, O_s)
void tetra_emerge(const TetraFaceResult* s, const TetraFaceResult* e, const TetraFaceResult* x, Trit Ms[3], Trit Rs[3], Trit Os[3]){
    for(int i=0;i<3;i++){
        Ms[i] = triadic_collapse(s->M[i], e->M[i], x?x->M[i]:e->M[i]);
        Rs[i] = triadic_collapse(s->R[i], e->R[i], x?x->R[i]:e->R[i]);
        Os[i] = triadic_collapse(s->O[i], e->O[i], x?x->O[i]:e->O[i]);
    }
}

// Transcender paso (Nivel 1): procesa un trío de vectores (va,vb,vc)
VectorFFE_Fractal transcender_step(const VectorFFE_Fractal* va, const VectorFFE_Fractal* vb, const VectorFFE_Fractal* vc){
    // Operar por dimensión FFE (d[0],d[1],d[2])
    TetraFaceResult s[3], e2[3], x2[3], merged[3];
    Trit Ms[3], Rs[3], Os[3];
    DimensionFFE dM, dR, dO;

    // Para cada dimensión i, tomar el primer trit (t[0]) como valor operativo
    for(int i=0;i<3;i++){
        Trit A[3] = { va->d[i].t[0], va->d[i].t[1], va->d[i].t[2] };
        Trit B[3] = { vb->d[i].t[0], vb->d[i].t[1], vb->d[i].t[2] };
        Trit C[3] = { vc->d[i].t[0], vc->d[i].t[1], vc->d[i].t[2] };

        // Sintetizador (aprende M para reproducir C como resultado)
        tetra_sintetizador_learn(A,B,C,&s[i]);
        // Evolver (refina)
        e2[i] = s[i]; tetra_evolver(&e2[i]);
        // Extender (consistencia adicional)
        tetra_extender_infer(A,B,e2[i].M,&x2[i]);
        // Armonizador (fusiona caras)
        tetra_armonizador(&e2[i], &x2[i], NULL, &merged[i]);
    }

    // Emergencia de vector superior: M_s, R_s, O_s por dimensión
    for(int k=0;k<3;k++){
        Ms[k] = triadic_collapse(merged[0].M[k], merged[1].M[k], merged[2].M[k]);
        Rs[k] = triadic_collapse(merged[0].R[k], merged[1].R[k], merged[2].R[k]);
        Os[k] = triadic_collapse(merged[0].O[k], merged[1].O[k], merged[2].O[k]);
    }

    // Construir vector superior: d0=Fn(M_s), d1=Fo(R_s), d2=Es(O_s)
    dM = make_dim(Ms[0], Ms[1], Ms[2]);
    dR = make_dim(Rs[0], Rs[1], Rs[2]);
    dO = make_dim(Os[0], Os[1], Os[2]);
    return make_vec_f(dR, dM, dO); // Orden: [FO,FN,ES] coherente con fractal_to_flat
}

TensorFFE_Fractal transcender_n1(const TensorFFE_Fractal* A, const TensorFFE_Fractal* B, const TensorFFE_Fractal* C){
    // Nivel 1: aplicar a v[0], v[1], v[2]
    VectorFFE_Fractal v0 = transcender_step(&A->v[0], &B->v[0], &C->v[0]);
    VectorFFE_Fractal v1 = transcender_step(&A->v[1], &B->v[1], &C->v[1]);
    VectorFFE_Fractal v2 = transcender_step(&A->v[2], &B->v[2], &C->v[2]);
    return make_tensor_f(v0, v1, v2);
}

// ===== DESCUBRIMIENTO DE ROLES (FO/FN/ES) =====
// Whitepaper 0.4: "El sistema prueba combinaciones siguiendo la serie de Fibonacci
// para evitar bucles infinitos, interpretaciones inestables y decisiones arbitrarias."
// Evaluamos las 6 permutaciones posibles de asignación de (FO,FN,ES) a (d0,d1,d2)
// Métrica: construir un tensor plano hipotético, armonizar y contar nulls.
// Menos nulls => mejor layout (coherencia geométrica).

static TensorFFE build_hypo_tensor_from_layout(const VectorFFE_Fractal* v, int fo, int fn, int es){
    TensorFFE t;
    for(int i=0;i<3;i++){
        // Usamos los tres trits de la dimensión elegida como la tríada para FO/FN/ES respectivamente
        t.FO[i] = v->d[fo].t[i];
        t.FN[i] = v->d[fn].t[i];
        t.ES[i] = v->d[es].t[i];
    }
    harmonize_with_fibonacci(&t);
    return t;
}

RoleLayout discover_vector_roles(const VectorFFE_Fractal* v){
    int perms[6][3] = {
        {0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}
    };
    RoleLayout best = {0,1,2, 1000};
    for(int k=0;k<6;k++){
        TensorFFE hypo = build_hypo_tensor_from_layout(v, perms[k][0], perms[k][1], perms[k][2]);
        int n = count_nulls(&hypo);
        if(n < best.nulls_after){
            best.idx_FO = perms[k][0];
            best.idx_FN = perms[k][1];
            best.idx_ES = perms[k][2];
            best.nulls_after = n;
        }
    }
    return best;
}

void algorithm_god_roles(const TensorFFE_Fractal* tf, RoleLayout out_layouts[3]){
    for(int i=0;i<3;i++){
        out_layouts[i] = discover_vector_roles(&tf->v[i]);
    }
}

TensorFFE_Fractal algorithm_god_step(const TensorFFE_Fractal* A, const TensorFFE_Fractal* B, const TensorFFE_Fractal* C, RoleLayout layouts_out[3]){
    // 1. Descubrir roles en cada tensor fuente (podría servir como guía para futuras fases)
    algorithm_god_roles(A, layouts_out); // sólo usamos A como referencia inicial de roles
    // 2. Transcender nivel 1 para obtener tensor emergente superior
    TensorFFE_Fractal emerg = transcender_n1(A,B,C);
    return emerg;
}

// ===== ESCALAR TERNARIO (BALANCED TERNARY) =====
static inline int trit_to_signed(const Trit t){ return (t==1)? 1 : (t==0? 0 : -1); }

double tensor_balanced_scalar(const TensorFFE* t){
    // Orden: FO[0..2], FN[0..2], ES[0..2]
    int digits[9] = {
        trit_to_signed(t->FO[0]), trit_to_signed(t->FO[1]), trit_to_signed(t->FO[2]),
        trit_to_signed(t->FN[0]), trit_to_signed(t->FN[1]), trit_to_signed(t->FN[2]),
        trit_to_signed(t->ES[0]), trit_to_signed(t->ES[1]), trit_to_signed(t->ES[2])
    };
    double val = 0.0;
    double w = 1.0/3.0; // 3^{-1}
    for(int k=0;k<9;k++){
        val += digits[k] * w;
        w /= 3.0; // siguiente peso 3^{-(k+2)}
    }
    return val;
}

void tensor_balanced_digits(const TensorFFE* t, char out[16]){
    char* p = out;
    const Trit seq[9] = {
        t->FO[0], t->FO[1], t->FO[2], t->FN[0], t->FN[1], t->FN[2], t->ES[0], t->ES[1], t->ES[2]
    };
    for(int i=0;i<9;i++){
        *p++ = (seq[i]==1)? '+' : (seq[i]==0? '0' : '-');
    }
    *p = '\0';
}
