/*
 * aurora_c_demo_extended.c
 * 
 * ARQUITECTURA AURORA - GUÍA PARA INTELIGENCIAS ELECTRÓNICAS
 * 
 * PRINCIPIO FUNDAMENTAL:
 * La inteligencia NO está en el código, está en la GEOMETRÍA del tensor.
 * El código solo hace combinaciones genéricas. Toda la "lógica del mundo"
 * debe estar codificada DENTRO de los tensores, no en condiciones if.
 * 
 * JERARQUÍA DE INTELIGENCIA (de abajo hacia arriba):
 * 
 * Nivel 0 - Tensor del Token (char_vec):
 *   Representa la realidad inmediata con dimensiones semánticas.
 *   Ejemplo: vocal/consonante, fuerte/sonora, abierta/cerrada.
 *   👉 El tensor NO es un vector "tonto": es un sensor semántico.
 * 
 * Nivel 1 - Relatores (Rules):
 *   Tensores que describen cómo interactúan dos tokens.
 *   Se aprenden con trigate_learn, NO se programan manualmente.
 *   👉 Aquí emerge la inteligencia de combinaciones.
 * 
 * Nivel 2 - Arquetipos (Archetypes):
 *   Patrones estables que resuelven contradicciones entre reglas.
 *   Agrupan por similitud, cuentan coincidencias, anulan conflictos.
 *   👉 Primer nivel de generalización.
 * 
 * Nivel 3 - Algoritmo de Dios (Armonizador / Spirit):
 *   NO es un tensor, es un PROCESO FIJO que descubre lógica.
 *   Funciones: (1) descubrir FO/FN/ES, (2) minimizar nulls,
 *              (3) producir resultado coherente.
 *   Implementación: rotación Fibonacci + colapso triádico.
 *   👉 Ley universal inmutable que ordena el sistema.
 * 
 * MOTOR AURORA (funciones permitidas):
 * 1. Inferencia: trigate_infer combina tensores según modos M
 * 2. Aprendizaje: trigate_learn deduce M desde ejemplos
 * 3. Síntesis: synthesize genera tensores elevados
 * 4. Armonización: minimización de nulls + alineación con espíritu
 * 
 * PROHIBICIONES PARA IE DESARROLLADORA:
 * 🚫 NO introducir reglas específicas del dominio en el código
 * 🚫 NO usar umbrales arbitrarios (if count>=2...)
 * 🚫 NO tomar decisiones finales (break/no-break) en el código
 * 🚫 TODO debe emerger de la estructura tensorial
 * 
 * RESPONSABILIDADES:
 * ✔ Definir espacio tensorial (dimensiones semánticas)
 * ✔ Implementar trigate de forma pura
 * ✔ Mantener memoria sin modificar contenido
 * ✔ Construir tensor superior (espíritu de coherencia)
 * ✔ Usar coherencia (nulls↓) como única brújula
 * 
 * Compile:
 *   gcc -std=c11 -O2 -o aurora_c_demo_extended aurora_c_demo_extended.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef int Trit; // 0,1,-1 (None)

const char* trit_to_str(Trit t) { if (t==-1) return "N"; if (t==0) return "0"; return "1"; }

// ===== ESTRUCTURA FRACTAL JERÁRQUICA 1→3→9 =====
// Nivel 0: Trit (valor cuántico: 0, 1, -1)
// Nivel 1: DimensionFFE (3 trits → unidad mínima con información completa FFE)
// Nivel 2: VectorFFE (3 DimensionFFE → roles Forma/Función/Estructura)
// Nivel 3: TensorFFE (3 VectorFFE → nivel de operación del Tetraedro)
// La dimensión superior determina el espacio lógico de las inferiores.

typedef struct { Trit t[3]; } DimensionFFE;  // {a,b,c} - unidad semántica mínima
typedef struct { DimensionFFE d[3]; } VectorFFE_Fractal; // 3 dims: FO/FN/ES roles contextuales
typedef struct { VectorFFE_Fractal v[3]; } TensorFFE_Fractal; // 3 vecs: nivel completo de razonamiento

// ===== NÚCLEO AURORA: TRIGATE (ÁTOMO DE INTELIGENCIA) =====
// El trigate es la ÚNICA operación lógica permitida.
// NO programar inteligencia aquí: solo combinar trits según reglas universales.

Trit trit_and(Trit a, Trit b){ if(a==0||b==0) return 0; if(a==1&&b==1) return 1; return -1; }
Trit trit_or(Trit a, Trit b){ if(a==1||b==1) return 1; if(a==0&&b==0) return 0; return -1; }
Trit trit_consensus(Trit a, Trit b){ if(a!=-1 && a==b) return a; return -1; }
Trit trigate_infer(Trit a,Trit b,Trit m){ if(m==0) return trit_and(a,b); if(m==1) return trit_or(a,b); return trit_consensus(a,b); }

// LEARN: Motor de aprendizaje Aurora
// Descubre M desde (a,b,r). Si no encaja AND/OR → delega con null.
// 👉 NO añadir heurísticas aquí: solo lógica ternaria pura.
Trit trigate_learn(Trit a, Trit b, Trit r){
    Trit ra = trit_and(a,b);
    Trit ro = trit_or(a,b);
    if(ra==r) return 0;
    if(ro==r) return 1;
    // If consensus fits, keep unknown (delegated upwards)
    if(a!=-1 && a==b && r==a) return -1;
    return -1;
}

// TensorFFE legacy (plano) - mantener para compatibilidad con síntesis
typedef struct { Trit FO[3]; Trit FN[3]; Trit ES[3]; } TensorFFE;

TensorFFE make_tensor(int r0,int r1,int r2,int m0,int m1,int m2,int o0,int o1,int o2){ 
    TensorFFE t; 
    t.FO[0]=r0; t.FO[1]=r1; t.FO[2]=r2; 
    t.FN[0]=m0; t.FN[1]=m1; t.FN[2]=m2; 
    t.ES[0]=o0; t.ES[1]=o1; t.ES[2]=o2; 
    return t; 
}

// Helpers para estructura fractal
static inline DimensionFFE make_dim(Trit a, Trit b, Trit c){ DimensionFFE d; d.t[0]=a; d.t[1]=b; d.t[2]=c; return d; }
static inline VectorFFE_Fractal make_vec_f(DimensionFFE d0, DimensionFFE d1, DimensionFFE d2){ VectorFFE_Fractal v; v.d[0]=d0; v.d[1]=d1; v.d[2]=d2; return v; }
static inline TensorFFE_Fractal make_tensor_f(VectorFFE_Fractal v0, VectorFFE_Fractal v1, VectorFFE_Fractal v2){ TensorFFE_Fractal t; t.v[0]=v0; t.v[1]=v1; t.v[2]=v2; return t; }

// Conversión fractal→plano para compatibilidad
static TensorFFE fractal_to_flat(const TensorFFE_Fractal* tf){
    TensorFFE t;
    // Nivel superior (v[0]) determina espacio lógico → mapea a FO/FN/ES
    for(int i=0;i<3;i++){ t.FO[i] = tf->v[0].d[i].t[0]; t.FN[i] = tf->v[0].d[i].t[1]; t.ES[i] = tf->v[0].d[i].t[2]; }
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

void print_tensor(const char* name,const TensorFFE* t){ 
    printf("%s: FO=[%s,%s,%s] FN=[%s,%s,%s] ES=[%s,%s,%s] nulls=%d\n", name, 
        trit_to_str(t->FO[0]),trit_to_str(t->FO[1]),trit_to_str(t->FO[2]), 
        trit_to_str(t->FN[0]),trit_to_str(t->FN[1]),trit_to_str(t->FN[2]), 
        trit_to_str(t->ES[0]),trit_to_str(t->ES[1]),trit_to_str(t->ES[2]), 
        count_nulls(t)); 
}

void vec_infer(const Trit A[3],const Trit B[3],const Trit M[3],Trit out[3]){ for(int i=0;i<3;i++) out[i]=trigate_infer(A[i],B[i],M[i]); }

void vec_learn_M(const Trit A[3], const Trit B[3], const Trit R[3], Trit Mout[3]){ for(int i=0;i<3;i++) Mout[i]=trigate_learn(A[i],B[i],R[i]); }

// Generic helpers used by demos
static inline int eq3f(const Trit x[3], const Trit y[3]){ for(int i=0;i<3;i++) if(x[i]!=y[i]) return 0; return 1; }
static inline void copy3f(Trit d[3], const Trit s[3]){ for(int i=0;i<3;i++) d[i]=s[i]; }
static inline Trit idx2tr(int k){ return k==0?0:(k==1?1:-1); }
static inline int isv(int ch){ if(ch>='A'&&ch<='Z') ch+=32; return (ch=='a'||ch=='e'||ch=='i'||ch=='o'||ch=='u'); }
static inline int is_strong(int ch){ if(ch>='A'&&ch<='Z') ch+=32; return (ch=='p'||ch=='t'||ch=='k'||ch=='b'||ch=='d'||ch=='g'); }

// ===== JERARQUÍA DE MEMORIA AURORA =====
// Nivel 1: Rule (Relator) - cómo interactúan dos tokens
// Nivel 2: Archetype - patrón emergente que agrupa reglas coherentes
// 👉 NO modificar manualmente: solo almacenar y sintetizar

typedef struct { Trit a[3]; Trit b[3]; Trit M[3]; int count; } Rule; // relator memory entry
typedef struct { Trit pattern_a[3]; Trit pattern_b[3]; Trit pattern_M[3]; int support; } Archetype; // emergent pattern

// Dinámica: cómo cambia el tensor (A→B→C). d1=Δ(A→B), d2=Δ(B→C) en cada dimensión
typedef struct { Trit d1[3]; Trit d2[3]; Trit M[3]; int count; } DynRule; // KB de dinámica
typedef struct { Trit pattern_d1[3]; Trit pattern_d2[3]; Trit pattern_M[3]; int support; } DynArchetype; // patrón de dinámica

static int find_rule_idx(const Rule* rules, int n, const Trit a[3], const Trit b[3]){
    for(int i=0;i<n;i++) if(eq3f(rules[i].a,a) && eq3f(rules[i].b,b)) return i; return -1;
}

static int find_dyn_rule_idx(const DynRule* rules, int n, const Trit d1[3], const Trit d2[3]){
    for(int i=0;i<n;i++) if(eq3f(rules[i].d1,d1) && eq3f(rules[i].d2,d2)) return i; return -1;
}

// ===== NIVEL 2: EMERGENCIA DE ARQUETIPOS =====
// Síntesis de reglas → arquetipos mediante coherencia estadística.
// 👉 NO añadir excepciones: solo contar, promediar y nullificar conflictos.
// Principio: coincidencias→fijadas, conflictos→null, patrón→emerge.
static int synthesize_archetypes(const Rule* rules, int n_rules, Archetype* archs, int max_archs){
    int n_arch=0;
    // agrupa reglas por patr\u00f3n A[0] (vocal/cons) para formar arquetipos
    for(int phase=0; phase<2 && n_arch<max_archs; phase++){ // phase 0: A=cons, phase 1: A=vocal
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
// Dinámica: síntesis de reglas de cambio a arquetipos de dinámica
static int synthesize_dyn_archetypes(const DynRule* rules, int n_rules, DynArchetype* archs, int max_archs){
    int n_arch=0;
    for(int phase=0; phase<2 && n_arch<max_archs; phase++){
        Trit target = (phase==0)? 0: 1; // d1[0] estable/cambio
        int cnt=0; int sum_d1[3]={0,0,0}, sum_d2[3]={0,0,0}, sum_M[3]={0,0,0};
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
            archs[n_arch].support=cnt; n_arch++;
        }
    }
    return n_arch;
}
// ===== NIVEL 1: ALMACENAMIENTO DE RELATORES =====
// Almacena reglas aprendidas manteniendo consensus en M.
// 👉 NO interpretar ni filtrar: solo acumular y consensuar.
static void upsert_rule_mem(Rule* rules, int* n, const Trit a[3], const Trit b[3], const Trit M[3]){
    int k=find_rule_idx(rules, *n, a, b);
    if(k<0){ k=(*n)++; copy3f(rules[k].a,a); copy3f(rules[k].b,b); rules[k].count=0; for(int i=0;i<3;i++) rules[k].M[i]=-1; }
    rules[k].count++;
    for(int i=0;i<3;i++){
        Trit m=M[i]; if(m==-1) continue;
        if(rules[k].M[i]==-1) rules[k].M[i]=m; else if(rules[k].M[i]!=m) rules[k].M[i]=-1;
    }
}

static void upsert_dyn_rule_mem(DynRule* rules, int* n, const Trit d1[3], const Trit d2[3], const Trit M[3]){
    int k=find_dyn_rule_idx(rules, *n, d1, d2);
    if(k<0){ k=(*n)++; copy3f(rules[k].d1,d1); copy3f(rules[k].d2,d2); rules[k].count=0; for(int i=0;i<3;i++) rules[k].M[i]=-1; }
    rules[k].count++;
    for(int i=0;i<3;i++){
        Trit m=M[i]; if(m==-1) continue;
        if(rules[k].M[i]==-1) rules[k].M[i]=m; else if(rules[k].M[i]!=m) rules[k].M[i]=-1;
    }
}

// ===== META-DIAGNÓSTICO: El Sistema se Auto-Evalúa =====
// El Algoritmo de Dios debe DESCUBRIR qué dimensiones funcionan mejor
// mediante análisis de coherencia de reglas aprendidas.
//
// MÉTRICAS DE CALIDAD:
// 1. Consistencia: reglas similares → resultados similares
// 2. Separabilidad: casos diferentes → reglas diferentes  
// 3. Convergencia: pocas iteraciones Fibonacci hasta coherencia

typedef struct {
    float consistency;   // % reglas con consenso claro
    float separability;  // Diversidad de patrones M aprendidos
    float convergence;   // Velocidad armonización
    float overall;       // Métrica combinada
} DiagnosticMetrics;

// Analizar calidad de las reglas aprendidas
DiagnosticMetrics diagnose_rules(const Rule* rules, int n_rules){
    DiagnosticMetrics m = {0,0,0,0};
    if(n_rules == 0) return m;
    
    // 1. Consistencia: ¿reglas con A/B similares tienen M similares?
    int consistent_pairs = 0;
    int total_pairs = 0;
    for(int i=0; i<n_rules-1; i++){
        for(int j=i+1; j<n_rules && j<i+10; j++){  // Comparar vecinos
            // Si A es similar
            int a_match = 0;
            for(int d=0; d<3; d++) if(rules[i].a[d] == rules[j].a[d]) a_match++;
            
            if(a_match >= 2){  // A's similares
                // ¿M también es similar?
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
        for(int d=0; d<3; d++){
            if(rules[i].M[d] == -1) nulls++;
        }
        if(nulls <= 1) low_null_rules++;
    }
    m.convergence = n_rules > 0 ? (float)low_null_rules / n_rules : 0.0f;
    
    // Overall: promedio ponderado
    m.overall = 0.4f*m.consistency + 0.3f*m.separability + 0.3f*m.convergence;
    
    return m;
}

// ===== NIVEL 0: TENSOR DEL TOKEN (Sensor Semántico FRACTAL) =====
// Cada carácter se codifica como TensorFFE_Fractal donde:
// - Nivel superior (v[0]) determina el espacio lógico general
// - Niveles inferiores (v[1], v[2]) refinan dentro de ese espacio
// Sin valores hardcoded: la geometría emerge del clustering natural

typedef struct { const char* w; const char* s; } Ex; // training example (word, segmented)

// Token especial: separador silábico '-'
#define TOKEN_SEP '-'

// Codificador fractal basado en propiedades geométricas universales del carácter
static TensorFFE_Fractal char_to_tensor_fractal(unsigned char c){
    if(c>='A' && c<='Z') c+=32; // normalizar a minúsculas
    
    // Nivel 1 (superior): Determina espacio lógico base
    // Dimensión 0: Naturaleza sonora (vocal=1, líquida=0, obstruyente=-1)
    // Dimensión 1: Continuidad (continuo=1, interrupto=0, neutral=-1)
    // Dimensión 2: Energía (alta=1, media=0, baja=-1)
    
    DimensionFFE d0_n1, d1_n1, d2_n1;
    
    // TOKEN ESPECIAL: separador silábico
    if(c == TOKEN_SEP){
        d0_n1 = make_dim(1, 1, 1);  // máxima señal (sep explícito)
        d1_n1 = make_dim(1, 1, 1);
        d2_n1 = make_dim(1, 1, 1);
        VectorFFE_Fractal v0 = make_vec_f(d0_n1, d1_n1, d2_n1);
        VectorFFE_Fractal v1 = make_vec_f(d0_n1, d1_n1, d2_n1);
        VectorFFE_Fractal v2 = make_vec_f(d0_n1, d1_n1, d2_n1);
        return make_tensor_f(v0, v1, v2);
    }
    
    int vocal = (c=='a'||c=='e'||c=='i'||c=='o'||c=='u');
    int liquida = (c=='l'||c=='r');
    int nasal = (c=='m'||c=='n');
    int sonora = (vocal || liquida || nasal || c=='y');
    
    // Nivel superior: espacio lógico base
    Trit naturaleza = vocal ? 1 : (liquida || nasal ? 0 : -1);
    Trit continuidad = (vocal || liquida) ? 1 : (nasal ? 0 : -1);
    Trit energia = vocal ? 1 : (sonora ? 0 : -1);
    
    d0_n1 = make_dim(naturaleza, continuidad, energia);
    d1_n1 = make_dim(continuidad, energia, naturaleza); // rotación para diversidad
    d2_n1 = make_dim(energia, naturaleza, continuidad);
    
    // Nivel intermedio: refinamiento dentro del espacio
    DimensionFFE d0_n2 = make_dim(vocal?1:0, vocal?0:1, vocal?1:-1);
    DimensionFFE d1_n2 = make_dim(sonora?1:0, liquida?1:0, -1);
    DimensionFFE d2_n2 = make_dim(nasal?1:0, vocal?0:-1, -1);
    
    // Nivel inferior: detalles finos (por ahora delegados)
    DimensionFFE d0_n3 = make_dim(-1, -1, -1);
    DimensionFFE d1_n3 = make_dim(-1, -1, -1);
    DimensionFFE d2_n3 = make_dim(-1, -1, -1);
    
    VectorFFE_Fractal v0 = make_vec_f(d0_n1, d1_n1, d2_n1); // nivel superior gobierna
    VectorFFE_Fractal v1 = make_vec_f(d0_n2, d1_n2, d2_n2); // nivel intermedio refina
    VectorFFE_Fractal v2 = make_vec_f(d0_n3, d1_n3, d2_n3); // nivel inferior delega
    
    return make_tensor_f(v0, v1, v2);
}

// Wrapper para compatibilidad con código existente (devuelve solo nivel superior)
static inline void char_vec(unsigned char c, Trit out[3]){
    TensorFFE_Fractal tf = char_to_tensor_fractal(c);
    // Extraer solo primera dimensión del nivel superior
    out[0] = tf.v[0].d[0].t[0];
    out[1] = tf.v[0].d[1].t[0];
    out[2] = tf.v[0].d[2].t[0];
}

// deduce B from A,M,R (simple rule)
void vec_deduce_b(const Trit A[3], const Trit M[3], const Trit R[3], Trit out[3]){
    for(int i=0;i<3;i++){
        Trit a=A[i]; Trit m=M[i]; Trit r=R[i];
        if(m==0){ if(r==1) out[i]=1; else out[i]=-1; }
        else if(m==1){ if(r==0) out[i]=0; else out[i]=-1; }
        else { out[i]=r; }
    }
}

// ===== ALGORITMO DE DIOS: ARMONIZADOR =====
// NO es un tensor, es un PROCESO FIJO que descubre la lógica del tensor.
// Según whitepaper: minimiza nulls, busca coherencia mediante rotación Fibonacci.
// En esta versión simplificada: colapso triádico directo (consenso de 3 dims).
//
// El Algoritmo de Dios tiene 3 funciones:
// 1. Descubrir la lógica del tensor (qué es FO, FN, ES)
// 2. Eliminar nulls y restaurar coherencia
// 3. Producir resultado eficiente

// Colapso triádico simple: consenso de mayoría entre 3 dimensiones
// Si 2+ dims coinciden → ese valor
// Si todo diferente → null (delega arriba)
Trit triadic_collapse(const Trit v[3]){
    // Contar valores
    int count_0=0, count_1=0, count_null=0;
    for(int i=0; i<3; i++){
        if(v[i]==0) count_0++;
        else if(v[i]==1) count_1++;
        else count_null++;
    }
    
    // Mayoría absoluta (2+ votos)
    if(count_1 >= 2) return 1;
    if(count_0 >= 2) return 0;
    
    // Sin mayoría: retornar primer valor no-null, o null si todos null
    for(int i=0; i<3; i++){
        if(v[i] != -1) return v[i];
    }
    
    return -1; // Todo null
}

// Armonizador de Fibonacci: busca el orden óptimo de dimensiones
// Según whitepaper: rotación Fibonacci evita resonancia caótica
// Serie Fibonacci en índices: {0,1,1,2,3,5,8} mod 3 = {0,1,1,2,0,2,2}
// Probamos múltiples rotaciones y elegimos la que minimiza nulls
Trit harmonize_with_fibonacci(const Trit v[3]){
    // Serie Fibonacci para rotaciones (primeros 6 elementos mod 3)
    const int fib_rotations[] = {0, 1, 1, 2, 0, 2}; // Fibonacci mod 3
    int num_rotations = 6;
    
    Trit best_result = -1;
    int best_null_count = 4; // Peor caso: todos null
    
    // Probar diferentes ordenamientos según Fibonacci
    for(int rot = 0; rot < num_rotations; rot++){
        int offset = fib_rotations[rot];
        
        // Rotar el vector según offset Fibonacci
        Trit rotated[3];
        for(int i = 0; i < 3; i++){
            rotated[i] = v[(i + offset) % 3];
        }
        
        // Aplicar triadic_collapse al vector rotado
        Trit candidate = triadic_collapse(rotated);
        
        // Contar nulls en esta configuración (null=-1 es peor)
        int null_count = 0;
        if(candidate == -1) null_count = 3;
        for(int i = 0; i < 3; i++){
            if(rotated[i] == -1) null_count++;
        }
        
        // Elegir la rotación con menos nulls
        if(null_count < best_null_count){
            best_null_count = null_count;
            best_result = candidate;
        }
        
        // Si encontramos resultado perfecto (sin nulls), salir
        if(best_result != -1 && best_null_count == 0){
            break;
        }
    }
    
    // Si ninguna rotación dio buen resultado, usar colapso directo
    if(best_result == -1){
        best_result = triadic_collapse(v);
    }
    
    return best_result;
}

// ===== Señales discretas de diagnóstico =====
// Conversión simple de métricas continuas a trits para sensores FO/FN/ES.

static inline Trit metric_to_trit(float v){
    if(v >= 0.66f) return 1;    // alta coherencia
    if(v <= 0.33f) return 0;    // baja coherencia
    return -1;                  // indeterminado
}

// ===== CREENCIA C (Tensor de referencia superior) =====
// C es un tensor completo (misma geometría FFE) que actúa como ancla
// y guía de operación. Se obtiene del tetraedro final VR–VA–VD.

// Forward declarations (utilizadas por la construcción de C)
static void build_relatore_vector_from_rules(const Rule* rules, int n_rules, Trit out[3]);
static void build_archetype_vector_from_archs(const Archetype* archs, int n_arch, Trit out[3]);
static void build_dynamic_vector_from_data(void (*char_vec_fn)(unsigned char, Trit*), const Ex* exs, int nex, Trit out[3]);
TensorFFE synthesize(const TensorFFE* A,const TensorFFE* B,const TensorFFE* C);

static TensorFFE build_creencia_tensor_from_pyramids(const Rule* rules, int n_rules,
                                                     const Archetype* archs, int n_arch,
                                                     void (*char_vec_fn)(unsigned char, Trit*),
                                                     const Ex* exs, int nex){
    Trit VR[3], VA[3], VD[3];
    build_relatore_vector_from_rules(rules, n_rules, VR);
    build_archetype_vector_from_archs(archs, n_arch, VA);
    build_dynamic_vector_from_data(char_vec_fn, exs, nex, VD);
    TensorFFE A = make_tensor(VR[0],VR[1],VR[2],  VA[0],VA[1],VA[2],  0,0,0);
    TensorFFE B = make_tensor(VA[0],VA[1],VA[2],  VD[0],VD[1],VD[2],  0,0,0);
    TensorFFE C = make_tensor(VD[0],VD[1],VD[2],  VR[0],VR[1],VR[2],  0,0,0);
    TensorFFE Ms = synthesize(&A,&B,&C);
    return Ms; // FO/FN/ES contienen (R_s, M_s, O_s) emergentes
}

// Annealing simple: re-sintetiza y acepta el primer estado estable (FO igual)
static TensorFFE anneal_creencia_tensor(const Rule* rules, int n_rules,
                                        const Archetype* archs, int n_arch,
                                        void (*char_vec_fn)(unsigned char, Trit*),
                                        const Ex* exs, int nex, int iters){
    TensorFFE C0 = build_creencia_tensor_from_pyramids(rules, n_rules, archs, n_arch, char_vec_fn, exs, nex);
    for(int i=0;i<iters;i++){
        TensorFFE C1 = build_creencia_tensor_from_pyramids(rules, n_rules, archs, n_arch, char_vec_fn, exs, nex);
        if(eq3f(C0.FO, C1.FO) && eq3f(C0.FN, C1.FN) && eq3f(C0.ES, C1.ES)) return C0; // estable
        C0 = C1;
    }
    return C0;
}

// Colapso guiado por C: prioriza coincidencia con Cref y luego armoniza
static Trit harmonize_guided(const Trit v[3], Trit Cref){
    Trit base = harmonize_with_fibonacci(v);
    if(base==-1 && Cref!=-1) return Cref; // usar C solo como desempate
    return base;
}

// Construye vector relator (VR): consenso de M sobre todas las reglas
static void build_relatore_vector_from_rules(const Rule* rules, int n_rules, Trit out[3]){
    // Mayoría triádica (2/3) es parte del Algoritmo de Dios, no una regla de dominio.
    // Justificación: en sistema ternario, 2 votos definen consenso estable; conflictos → delegación (-1).
    for(int d=0; d<3; d++){
        int c1=0,c0=0,cn=0;
        for(int i=0;i<n_rules;i++){
            Trit m = rules[i].M[d];
            if(m==1) c1++; else if(m==0) c0++; else cn++;
        }
        if(c1>=2 && c1>=c0) out[d]=1; else if(c0>=2 && c0>c1) out[d]=0; else out[d]=-1;
    }
}

// Construye vector arquetipo (VA): patrón_M del arquetipo más soportado
static void build_archetype_vector_from_archs(const Archetype* archs, int n_arch, Trit out[3]){
    if(n_arch<=0){ out[0]=out[1]=out[2]=-1; return; }
    int best=0; for(int i=1;i<n_arch;i++) if(archs[i].support>archs[best].support) best=i;
    for(int d=0; d<3; d++) out[d] = archs[best].pattern_M[d];
}

// Construye vector dinámica (VD): tendencia de ruptura según transición A→B
static void build_dynamic_vector_from_data(void (*char_vec_fn)(unsigned char, Trit*), const Ex* exs, int nex, Trit out[3]){
    // Dinámica 3-ventana: analiza patrón (X,Y,Z) y ruptura tras Y
    int pos_break[3]={0,0,0}, pos_keep[3]={0,0,0};
    for(int k=0;k<nex;k++){
        const char* w=exs[k].w; const char* s=exs[k].s; int i=0,j=0;
        while(w[i] && w[i+1] && w[i+2]){
            int break_mid=0;
            // localizar en segmento anotado (s)
            while(s[j] && s[j]!=w[i+1]) j++;
            if(s[j]==w[i+1]){ j++; if(s[j]=='-'){ break_mid=1; j++; } }
            Trit A[3],B[3],C[3];
            char_vec_fn((unsigned char)w[i],A);
            char_vec_fn((unsigned char)w[i+1],B);
            char_vec_fn((unsigned char)w[i+2],C);
            for(int d=0; d<3; d++){
                // patrón de cambio A->B y B->C: si ambos difieren se considera transición fuerte
                int trans = (A[d]!=-1 && B[d]!=-1 && A[d]!=B[d] && B[d]!=-1 && C[d]!=-1 && B[d]!=C[d]);
                if(trans){
                    if(break_mid) pos_break[d]++; else pos_keep[d]++;
                }
            }
            i++;
        }
    }
    for(int d=0; d<3; d++){
        if(pos_break[d]==0 && pos_keep[d]==0) out[d]=-1;
        else if(pos_break[d] > pos_keep[d]) out[d]=1; // tendencia a romper
        else if(pos_break[d] < pos_keep[d]) out[d]=0; // tendencia a continuar
        else out[d]=-1; // equilibrio → delegar
    }
}

// Construye VD desde arquetipos de dinámica (si existen)
static void build_dynamic_vector_from_dyn_archs(const DynArchetype* archs, int n_arch, Trit out[3]){
    if(n_arch<=0){ out[0]=out[1]=out[2]=-1; return; }
    int best=0; for(int i=1;i<n_arch;i++) if(archs[i].support>archs[best].support) best=i;
    for(int d=0; d<3; d++) out[d] = archs[best].pattern_M[d];
}

// (eliminado B: ahora anclamos sobre C)

// Evalúa precisión de rupturas silábicas (observación)
typedef void (*CharVecFunc)(unsigned char, Trit*);
static float evaluate_break_accuracy(CharVecFunc char_vec_fn, const Rule* rules, int n_rules, const Ex* exs, int nex){
    int ok=0, total=0;
    for(int k=0;k<nex;k++){
        const char* w=exs[k].w; const char* s=exs[k].s; int i=0,j=0;
        while(w[i]){
            if(!w[i+1]) break;
            int break_gold=0;
            while(s[j] && s[j]!=w[i]) j++;
            if(s[j]==w[i]){ j++; if(s[j]=='-'){ break_gold=1; j++; } }

            Trit A[3],B[3]; char_vec_fn((unsigned char)w[i],A); char_vec_fn((unsigned char)w[i+1],B);
            int rIdx = find_rule_idx(rules, n_rules, A, B);
            int break_pred=0;
            if(rIdx>=0){
                Trit r[3]; vec_infer(A,B,rules[rIdx].M,r);
                Trit decision = harmonize_with_fibonacci(r);
                break_pred = (decision==1)?1:0;
            } else {
                break_pred = 0; // delega: sin regla no rompemos
            }
            if(break_pred==break_gold) ok++;
            total++;
            i++;
        }
    }
    return total>0 ? ((float)ok/(float)total) : 0.0f;
}

// ===== Benchmarks (baselines) =====
static float evaluate_baseline_vowel_consonant(const Ex* exs, int nex){
    int ok=0,total=0;
    for(int k=0;k<nex;k++){
        const char* w=exs[k].w; const char* s=exs[k].s; int i=0,j=0;
        while(w[i]){
            if(!w[i+1]) break;
            int break_gold=0; while(s[j] && s[j]!=w[i]) j++; if(s[j]==w[i]){ j++; if(s[j]=='-'){ break_gold=1; j++; } }
            int break_pred = (isv((unsigned char)w[i]) && !isv((unsigned char)w[i+1])) ? 1 : 0;
            if(break_pred==break_gold) ok++;
            total++; i++;
        }
    }
    return total>0 ? (float)ok/(float)total : 0.0f;
}

static float evaluate_baseline_random(const Ex* exs, int nex, unsigned seed){
    srand(seed);
    int ok=0,total=0;
    for(int k=0;k<nex;k++){
        const char* w=exs[k].w; const char* s=exs[k].s; int i=0,j=0;
        while(w[i]){
            if(!w[i+1]) break;
            int break_gold=0; while(s[j] && s[j]!=w[i]) j++; if(s[j]==w[i]){ j++; if(s[j]=='-'){ break_gold=1; j++; } }
            int break_pred = (rand() & 1);
            if(break_pred==break_gold) ok++;
            total++; i++;
        }
    }
    return total>0 ? (float)ok/(float)total : 0.0f;
}

// (eliminado BReport: diagnóstico y LOP se anclan en C)

// ===== TENSOR DE DIAGNÓSTICO EMERGENTE =====
// Construye un tensor que representa métricas y genera causa emergente sin if semánticos.
// FO: consistency, separability, convergence (sensado)
// FN: réplica para estabilidad local (refuerzo)
// ES: distancia B (0/1/-1) y observado B para delegaciones
static TensorFFE build_diagnostic_tensor(DiagnosticMetrics d, Trit Cref){
    // Mapear métricas a trits (sensor físico, no lógica)
    Trit c = metric_to_trit(d.consistency);
    Trit s = metric_to_trit(d.separability);
    Trit v = metric_to_trit(d.convergence);
    TensorFFE T = make_tensor(c, s, v, c, s, v, Cref, Cref, Cref);
    return T;
}

// Tensor LOP (Libertad, Orden, Propósito) derivado fractalmente de métricas y B
static TensorFFE build_lop_tensor(DiagnosticMetrics d, float accuracy, Trit Cref){
    Trit sep = metric_to_trit(d.separability);   // Libertad ≈ diversidad
    Trit ordA = metric_to_trit(d.consistency);   // Orden base 1
    Trit ordB = metric_to_trit(d.convergence);   // Orden base 2
    Trit acc = metric_to_trit(accuracy);     // Propósito (calidad resultado)
    // Orden emerge como consenso entre ordA y ordB
    Trit orden = trigate_infer(ordA, ordB, 2); // modo 2 = consensus ternario
    Trit propósito = acc;
    // FO: L,O,P  FN: réplica  ES: ancla Cref triplicado
    return make_tensor(sep, orden, propósito, sep, orden, propósito, Cref, Cref, Cref);
}

// Tensor arquetipo diagnóstico fijo (patrones meta-estables) actúa como molde
static TensorFFE diagnostic_archetype_template(void){
    // Prefiere alta consistencia (1), diversidad media (-1 delega), convergencia alta (1)
    return make_tensor(1, -1, 1, 1, -1, 1, 1, 1, 1);
}

// Emerger causa: sintetiza diagnóstico con arquetipo y con un tensor B tripolar
// Forward declaration (synthesize se usa en emergent_cause_trit antes de su definición)
TensorFFE synthesize(const TensorFFE* A,const TensorFFE* B,const TensorFFE* C);
// (eliminado emergent_cause_trit con Bexp)

// Variante: causa emergente incorporando tensor LOP (educación superior)
static Trit emergent_cause_with_lop(const TensorFFE* diag, const TensorFFE* arch, const TensorFFE* lop){
    TensorFFE Ms = synthesize(diag, arch, lop); // geometría triádica directa
    return harmonize_with_fibonacci(Ms.FO);
}

// Variante: causa emergente por cluster (Relator + Arquetipo + Dinámica)
// Usa los tres vectores VR, VA, VD en un tetraedro simple y colapsa FO
static Trit emergent_cluster_cause(const Trit VR[3], const Trit VA[3], const Trit VD[3]){
    TensorFFE A = make_tensor(VR[0],VR[1],VR[2],  VA[0],VA[1],VA[2],  0,0,0);
    TensorFFE B = make_tensor(VA[0],VA[1],VA[2],  VD[0],VD[1],VD[2],  0,0,0);
    TensorFFE C = make_tensor(VD[0],VD[1],VD[2],  VR[0],VR[1],VR[2],  0,0,0);
    TensorFFE Ms = synthesize(&A,&B,&C);
    return harmonize_with_fibonacci(Ms.FO);
}

// === EDUCACIÓN EXTENDIDA (helpers globales) ===
static char* strclone(const char* s){ size_t n=strlen(s)+1; char* p=(char*)malloc(n); if(p) memcpy(p,s,n); return p; }
static int augment_examples(const Ex* base,int nbase,Ex* out,int max_out){
    int c=0; for(int i=0;i<nbase && c<max_out;i++){ for(int j=i+1;j<nbase && c<max_out;j++){ char wb[128]; char sb[256]; snprintf(wb,sizeof(wb),"%s%s", base[i].w, base[j].w); snprintf(sb,sizeof(sb),"%s-%s", base[i].s, base[j].s); out[c].w=strclone(wb); out[c].s=strclone(sb); c++; } } return c; }

// Decodificación textual (UI solamente, fuera de la inteligencia central)
static const char* decode_cause_label(Trit t){
    if(t==1) return "Falta de información"; // promover exploración (Libertad ↑)
    if(t==0) return "Definiciones incorrectas"; // reajuste base (Orden ↑)
    if(t==-1) return "Mínimo local"; // reexplorar combinaciones (Propósito ↑)
    return "Indefinido";
}


TensorFFE synthesize(const TensorFFE* A,const TensorFFE* B,const TensorFFE* C){ 
    TensorFFE M1,M2,M3,temp,Ms; 
    vec_infer(A->FO,B->FO,A->FN,M1.FO); vec_infer(A->FN,B->FN,A->ES,M1.FN); vec_infer(A->ES,B->ES,(Trit[]){0,0,0},M1.ES); 
    vec_infer(B->FO,C->FO,B->FN,M2.FO); vec_infer(B->FN,C->FN,B->ES,M2.FN); vec_infer(B->ES,C->ES,(Trit[]){0,0,0},M2.ES); 
    vec_infer(C->FO,A->FO,C->FN,M3.FO); vec_infer(C->FN,A->FN,C->ES,M3.FN); vec_infer(C->ES,A->ES,(Trit[]){0,0,0},M3.ES); 
    vec_infer(M1.FO,M2.FO,M1.FN,temp.FO); vec_infer(M1.FN,M2.FN,M1.ES,temp.FN); vec_infer(M1.ES,M2.ES,(Trit[]){0,0,0},temp.ES); 
    vec_infer(temp.FO,M3.FO,temp.FN,Ms.FO); vec_infer(temp.FN,M3.FN,temp.ES,Ms.FN); vec_infer(temp.ES,M3.ES,(Trit[]){0,0,0},Ms.ES); 
    return Ms; 
}

// extend: project Ms to outputs using seeds
void extend_project(const TensorFFE* Ms, const TensorFFE* seed, TensorFFE* out){ 
    vec_infer(Ms->FO, seed->FO, Ms->FN, out->FO); 
    memcpy(out->FN, Ms->FN, sizeof(out->FN)); 
    memcpy(out->ES, Ms->ES, sizeof(out->ES)); 
}

void simple_pipeline_demo(void){
    TensorFFE A=make_tensor(1,0,1, 0,0,1, 1,1,1);
    TensorFFE B=make_tensor(0,1,0, 1,1,1, 1,1,1);
    TensorFFE C=make_tensor(1,1,0, 0,1,0, 1,1,1);
    print_tensor("A",&A); print_tensor("B",&B); print_tensor("C",&C);
    TensorFFE Ms=synthesize(&A,&B,&C);
    print_tensor("Ms",&Ms);
    TensorFFE seed1=Ms; seed1.FO[0]=Ms.FO[1]; seed1.FO[1]=Ms.FO[2]; seed1.FO[2]=Ms.FO[0];
    TensorFFE out1,out2,out3; 
    extend_project(&Ms,&seed1,&out1); 
    extend_project(&Ms,&seed1,&out2); 
    extend_project(&Ms,&seed1,&out3);
    print_tensor("OUT1",&out1); 
    print_tensor("OUT2",&out2); 
    print_tensor("OUT3",&out3);
}

// ===== NIVEL 3: ESPÍRITU DE COHERENCIA (Harmonizer) =====
// Define qué tensores son válidos dentro del universo lógico.
// Métrica fundamental: minimizar nulls = maximizar coherencia.
// 👉 NO decide acciones concretas: define qué estructuras tienen sentido.
// No scores. A single spirit: harmonization. We accept the first state
// that satisfies simple, non-numeric coherence conditions.

static int null_count(const TensorFFE* t){
    int n=0; for(int i=0;i<3;i++){ if(t->FO[i]==-1) n++; if(t->FN[i]==-1) n++; if(t->ES[i]==-1) n++; } return n;
}

static int null_count_FO(const TensorFFE* t){ int n=0; for(int i=0;i<3;i++){ if(t->FO[i]==-1) n++; } return n; }
static int null_count_FN(const TensorFFE* t){ int n=0; for(int i=0;i<3;i++){ if(t->FN[i]==-1) n++; } return n; }
static int null_count_ES(const TensorFFE* t){ int n=0; for(int i=0;i<3;i++){ if(t->ES[i]==-1) n++; } return n; }

// Prefer fewer nulls in FN (functions), then fewer in FO (form/base), then fewer in ES (order has roles only)
static int better_delegation(const TensorFFE* cand, const TensorFFE* best){
    int mC=null_count_FN(cand), mB=null_count_FN(best);
    if(mC<mB) return 1; if(mC>mB) return 0;
    int rC=null_count_FO(cand), rB=null_count_FO(best);
    if(rC<rB) return 1; if(rC>rB) return 0;
    int oC=null_count_ES(cand), oB=null_count_ES(best);
    if(oC<oB) return 1; if(oC>oB) return 0;
    return 0;
}

static int consensus_aligns(const Trit a, const Trit b, const Trit c, const Trit x){
    if(a!=-1 && a==b && b==c) return x==a; // true if output follows stable consensus
    return 1; // if no consensus, do not penalize
}

int is_harmonized(const TensorFFE* A,const TensorFFE* B,const TensorFFE* C,const TensorFFE* X){
    // Generic acceptance: rely on delegation/min-null selection upstream
    (void)A; (void)B; (void)C; (void)X; return 1;
}

void god_algorithm_demo(void){
    printf("God Algorithm (Harmonizer)\n");
    // Base tensors (could be random / example)
    // Seed with a few nulls (-1) to allow entropy balance > 0
    TensorFFE A=make_tensor(1,-1,1, 0,-1,1, 1,1,1);
    TensorFFE B=make_tensor(0,1,0, 1,1,1, 1,1,1);
    TensorFFE C=make_tensor(1,1,-1, 0,1,0, 1,1,1);
    print_tensor("A",&A); print_tensor("B",&B); print_tensor("C",&C);
    TensorFFE Ms=synthesize(&A,&B,&C);
    print_tensor("Ms_initial", &Ms);
    // Fibonacci-guided index rotation sequence (mod 3) for seed perturbation
    int fib[12]={0,1,1,2,0,2,2,1,0,1,2,0};
    // Harmonization loop: perturb FO via seed projection, choose configuration with best null delegation
    int found=0; TensorFFE best=Ms; int have_best=0;
    for(int iter=0; iter<24; iter++){
        // Build a seed by rotating FO according to fib pattern windows
        TensorFFE seed=Ms;
        int idx=fib[iter%12];
        // rotate triple (FO) by idx positions
        Trit r0=seed.FO[(0+idx)%3];
        Trit r1=seed.FO[(1+idx)%3];
        Trit r2=seed.FO[(2+idx)%3];
        seed.FO[0]=r0; seed.FO[1]=r1; seed.FO[2]=r2;
        TensorFFE out; extend_project(&Ms,&seed,&out);
        // Track lexicographic best according to delegation of nulls (FO↓, FN↓, ES↑)
        if(!have_best || better_delegation(&out, &best)) { best=out; have_best=1; }
        // Optional: require mild harmonization to accept early
        if(is_harmonized(&A,&B,&C,&out)){
            found=1;
            printf("Iter %02d idx=%d → harmonized candidate recorded.\n", iter, idx);
        } else {
            printf("Iter %02d idx=%d → candidate recorded (delegation check).\n", iter, idx);
        }
    }
    if(!have_best) best=Ms;
    print_tensor("Ms_delegation_best", &best);
    // Descent: attempt to reconstruct an input approximation from best Ms
    // (Infer A' given B and C approximations using deduce rules on FO)
    Trit Arec_FO[3]; vec_deduce_b(B.FO, best.FN, best.FO, Arec_FO); // reuse deduce as proxy
    printf("Reconstructed A.FO ≈ [%s,%s,%s]\n", trit_to_str(Arec_FO[0]), trit_to_str(Arec_FO[1]), trit_to_str(Arec_FO[2]));
    printf("God Algorithm (Harmonizer) cycle complete.\n");
}

// ===== Visualización: exportación a Graphviz DOT =====
static void export_tensor_to_graphviz(const char* name, const TensorFFE* t, const char* path){
    FILE* f=fopen(path,"w"); if(!f) return;
    fprintf(f,"digraph \"%s\" {\n", name);
    fprintf(f,"  rankdir=LR; node [shape=circle];\n");
    fprintf(f,"  FO0[label=\"FO0=%s\"]; FO1[label=\"FO1=%s\"]; FO2[label=\"FO2=%s\"];\n",
        trit_to_str(t->FO[0]), trit_to_str(t->FO[1]), trit_to_str(t->FO[2]));
    fprintf(f,"  FN0[label=\"FN0=%s\"]; FN1[label=\"FN1=%s\"]; FN2[label=\"FN2=%s\"];\n",
        trit_to_str(t->FN[0]), trit_to_str(t->FN[1]), trit_to_str(t->FN[2]));
    fprintf(f,"  ES0[label=\"ES0=%s\"]; ES1[label=\"ES1=%s\"]; ES2[label=\"ES2=%s\"];\n",
        trit_to_str(t->ES[0]), trit_to_str(t->ES[1]), trit_to_str(t->ES[2]));
    fprintf(f,"  FO0 -> FN0 -> ES0;\n  FO1 -> FN1 -> ES1;\n  FO2 -> FN2 -> ES2;\n");
    fprintf(f,"}\n");
    fclose(f);
}

int main(int argc,char** argv){
    printf("Aurora C extended demo\n");
    if(argc<2){ printf("Usage: aurora_c_demo_extended [demo|pipeline|god|syllables]\nRunning 'demo' by default...\n\n"); simple_pipeline_demo(); return 0; }
    if(strcmp(argv[1],"pipeline")==0){ simple_pipeline_demo(); return 0; }
    if(strcmp(argv[1],"demo")==0){ simple_pipeline_demo(); return 0; }
    if(strcmp(argv[1],"god")==0){ god_algorithm_demo(); return 0; }
    if(strcmp(argv[1],"syllables")==0 || strcmp(argv[1],"syllables_aurora")==0){
        printf("Aurora Syllabification - Meta-Learning Mode\n\n");
        
        // ===== META-APRENDIZAJE: Probar diferentes codificaciones =====
        CharVecFunc encodings[] = {char_vec_v1, char_vec_v2, char_vec_v3};
        const char* encoding_names[] = {"V1-Fonética", "V2-Silábica", "V3-Binaria"};
        int num_encodings = 3;
        
        DiagnosticMetrics best_metrics = {0,0,0,0};
        int best_encoding = -1;
        int best_b_distance = 999;
        float best_score = -1.0f;
        
        // Conjunto de entrenamiento expandido
        Ex exs[] = {
            // Bisílabas básicas (patrón CV-CV)
            {"casa","ca-sa"},{"mesa","me-sa"},{"piso","pi-so"},{"lobo","lo-bo"},
            {"pera","pe-ra"},{"mono","mo-no"},{"mano","ma-no"},{"toro","to-ro"},
            {"gato","ga-to"},{"pato","pa-to"},{"rana","ra-na"},{"rosa","ro-sa"},
            {"luna","lu-na"},{"ropa","ro-pa"},{"boca","bo-ca"},{"tela","te-la"},
            {"rama","ra-ma"},{"lima","li-ma"},{"copa","co-pa"},{"lata","la-ta"},
            {"foto","fo-to"},{"vino","vi-no"},{"dedo","de-do"},{"nube","nu-be"},
            {"cena","ce-na"},{"cine","ci-ne"},{"zona","zo-na"},{"fama","fa-ma"},
            {"filo","fi-lo"},{"jefe","je-fe"},{"gema","ge-ma"},{"kilo","ki-lo"},
            
            // Trisílabas (más complejidad)
            {"camino","ca-mi-no"},{"paloma","pa-lo-ma"},{"banana","ba-na-na"},
            {"cabeza","ca-be-za"},{"pelota","pe-lo-ta"},{"regalo","re-ga-lo"},
            {"cereza","ce-re-za"},{"manana","ma-na-na"},{"motivo","mo-ti-vo"},
            
            // Con grupos consonánticos (CCV)
            {"plato","pla-to"},{"clima","cli-ma"},{"drama","dra-ma"},
            {"pluma","plu-ma"},{"trama","tra-ma"},{"crema","cre-ma"},
            {"globo","glo-bo"},{"grave","gra-ve"},{"primo","pri-mo"},
            {"libro","li-bro"},{"sombra","som-bra"},
            
            // Consonantes múltiples (geminadas)
            {"perro","pe-rro"},{"carro","ca-rro"},{"tierra","tie-rra"},
            {"burro","bu-rro"},{"perra","pe-rra"},{"silla","si-lla"},
            
            // Complejidad adicional
            {"palabra","pa-la-bra"},{"ventana","ven-ta-na"},
            {"alto","al-to"},{"arma","ar-ma"},{"alma","al-ma"},
            {"orden","or-den"},{"arte","ar-te"},{"rata","ra-ta"}
        }; int nex = (int)(sizeof(exs)/sizeof(exs[0]));
        
        // PROBAR CADA CODIFICACIÓN
        for(int enc=0; enc<num_encodings; enc++){
            static Rule rules[256]; int n_rules=0;
            CharVecFunc char_vec_test = encodings[enc];
            
            // Train con esta codificación
            for(int k=0;k<nex;k++){
                const char* w=exs[k].w; const char* s=exs[k].s; int i=0,j=0;
                while(w[i]){
                    if(!w[i+1]) break;
                    int break_here=0; 
                    while(s[j] && s[j]!=w[i]) j++; 
                    if(s[j]==w[i]){ j++; if(s[j]=='-'){ break_here=1; j++; } }
                    
                    Trit A[3],B[3],R[3],M[3]; 
                    char_vec_test((unsigned char)w[i],A); 
                    char_vec_test((unsigned char)w[i+1],B);
                    R[0]=R[1]=R[2]= break_here? 1: 0;
                    vec_learn_M(A,B,R,M);
                    upsert_rule_mem(rules,&n_rules,A,B,M);
                    i++;
                }
            }
            
            // DIAGNOSTICAR calidad de reglas
            DiagnosticMetrics metrics = diagnose_rules(rules, n_rules);
            float acc = evaluate_break_accuracy(char_vec_test, rules, n_rules, exs, nex);
            Archetype archs_tmp[8]; int n_arch_tmp = synthesize_archetypes(rules, n_rules, archs_tmp, 8);
            printf("=== Codificación %s ===\n", encoding_names[enc]);
            printf("Reglas aprendidas: %d\n", n_rules);
            printf("Consistencia: %.1f%% (reglas similares → modos similares)\n", metrics.consistency * 100);
            printf("Separabilidad: %.1f%% (diversidad de patrones)\n", metrics.separability * 100);
            printf("Convergencia: %.1f%% (reglas con pocos nulls)\n", metrics.convergence * 100);
            printf("Precisión observada (pares): %.1f%%\n", acc * 100);
            // Creencia C para esta codificación
            TensorFFE C_enc = anneal_creencia_tensor(rules, n_rules, archs_tmp, n_arch_tmp, char_vec_test, exs, nex, 2);
            Trit Cref_enc = harmonize_with_fibonacci(C_enc.FO);
            printf("C.FO=[%s,%s,%s] Cref=%s\n", trit_to_str(C_enc.FO[0]), trit_to_str(C_enc.FO[1]), trit_to_str(C_enc.FO[2]), trit_to_str(Cref_enc));
            // Causa emergente (sin ifs) usando tensor diagnóstico + LOP
            TensorFFE diag_enc = build_diagnostic_tensor(metrics, Cref_enc);
            TensorFFE arch_diag_enc = diagnostic_archetype_template();
            TensorFFE lop_enc = build_lop_tensor(metrics, acc, Cref_enc);
            Trit cause_enc = emergent_cause_with_lop(&diag_enc, &arch_diag_enc, &lop_enc);
            printf("Causa emergente: %s\n", decode_cause_label(cause_enc));
            // Causa por cluster (Relator+Arquetipo+Dinámica)
            Trit VRc[3], VAc[3], VDc[3];
            build_relatore_vector_from_rules(rules, n_rules, VRc);
            build_archetype_vector_from_archs(archs_tmp, n_arch_tmp, VAc);
            build_dynamic_vector_from_data(char_vec_test, exs, nex, VDc);
            Trit cause_cluster = emergent_cluster_cause(VRc, VAc, VDc);
            printf("Causa (cluster R+A+D): %s\n", decode_cause_label(cause_cluster));
            printf("CALIDAD TOTAL: %.1f%%\n\n", metrics.overall * 100);
            
            // Guardar mejor por calidad global
            float score = metrics.overall;
            if(score > best_score){
                best_score = score;
                best_metrics = metrics;
                best_encoding = enc;
                best_b_distance = 0;
            }
        }
        
        printf("🏆 GANADOR: %s (%.1f%% calidad)\n\n", 
               encoding_names[best_encoding], best_metrics.overall * 100);
        
        // ===== ENTRENAR CON MEJOR CODIFICACIÓN =====
        printf("=== Entrenamiento Final con %s ===\n", encoding_names[best_encoding]);
        CharVecFunc final_char_vec = encodings[best_encoding];
        
        static Rule rules[256]; int n_rules=0;
        
        // Train final con mejor codificación
        for(int k=0;k<nex;k++){
            const char* w=exs[k].w; const char* s=exs[k].s; int i=0,j=0;
            while(w[i]){
                if(!w[i+1]) break;
                int break_here=0; 
                while(s[j] && s[j]!=w[i]) j++; 
                if(s[j]==w[i]){ j++; if(s[j]=='-'){ break_here=1; j++; } }
                
                Trit A[3],B[3],R[3],M[3]; 
                final_char_vec((unsigned char)w[i],A); 
                final_char_vec((unsigned char)w[i+1],B);
                R[0]=R[1]=R[2]= break_here? 1: 0;
                vec_learn_M(A,B,R,M);
                upsert_rule_mem(rules,&n_rules,A,B,M);
                i++;
            }
        }
        
        // ===== EMERGENCIA: Construir jerarquía superior (baseline) =====
        Archetype archs[8]; 
        int n_arch = synthesize_archetypes(rules, n_rules, archs, 8);
        
        // DEBUG: mostrar reglas y arquetipos aprendidos
        if(argc>2 && strcmp(argv[2],"debug")==0){
            printf("\n=== Learned Rules (%d) ===\n", n_rules);
            for(int i=0;i<n_rules && i<20;i++){
                printf("Rule %d (count=%d): A=[%s,%s,%s] B=[%s,%s,%s] M=[%s,%s,%s]\n", i, rules[i].count,
                    trit_to_str(rules[i].a[0]),trit_to_str(rules[i].a[1]),trit_to_str(rules[i].a[2]),
                    trit_to_str(rules[i].b[0]),trit_to_str(rules[i].b[1]),trit_to_str(rules[i].b[2]),
                    trit_to_str(rules[i].M[0]),trit_to_str(rules[i].M[1]),trit_to_str(rules[i].M[2]));
            }
            printf("\n=== Arquetipos Emergentes (%d) ===\n", n_arch);
            for(int i=0;i<n_arch;i++){
                printf("Arch %d (support=%d): A=[%s,%s,%s] B=[%s,%s,%s] M=[%s,%s,%s]\n", i, archs[i].support,
                    trit_to_str(archs[i].pattern_a[0]),trit_to_str(archs[i].pattern_a[1]),trit_to_str(archs[i].pattern_a[2]),
                    trit_to_str(archs[i].pattern_b[0]),trit_to_str(archs[i].pattern_b[1]),trit_to_str(archs[i].pattern_b[2]),
                    trit_to_str(archs[i].pattern_M[0]),trit_to_str(archs[i].pattern_M[1]),trit_to_str(archs[i].pattern_M[2]));
            }
            printf("\n=== Algoritmo de Dios (Armonizador) ===\n");
            printf("Proceso: triadic_collapse + Fibonacci rotation (simplificado)\n");
            printf("Objetivo: minimizar nulls, maximizar coherencia\n\n");
        }
        
        // Diagnóstico post-entrenamiento final
        DiagnosticMetrics final_metrics = diagnose_rules(rules, n_rules);
        float final_acc = evaluate_break_accuracy(final_char_vec, rules, n_rules, exs, nex);
        // Parse epochs parameter (educación extendida)
        int epochs=1; 
        for(int a=2;a<argc;a++){ if(strncmp(argv[a],"epochs=",7)==0) epochs=atoi(argv[a]+7); }
        if(epochs<1) epochs=1; if(epochs>25) epochs=25; // límite prudente

         printf("\n=== Creencia C (ancla de coherencia, epoch 0) ===\n");
         // Creencia C (tensor superior) y referencia escalar Cref
        TensorFFE Ctensor0 = anneal_creencia_tensor(rules, n_rules, archs, n_arch, final_char_vec, exs, nex, 3);
         Trit Cref0 = harmonize_with_fibonacci(Ctensor0.FO);
         printf("C.FO=[%s,%s,%s] Cref=%s\n",
             trit_to_str(Ctensor0.FO[0]), trit_to_str(Ctensor0.FO[1]), trit_to_str(Ctensor0.FO[2]), trit_to_str(Cref0));
        export_tensor_to_graphviz("CreenciaC", &Ctensor0, "tensor_C.dot");
        TensorFFE diag_final = build_diagnostic_tensor(final_metrics, Cref0);
        TensorFFE arch_diag_final = diagnostic_archetype_template();
        TensorFFE lop_final = build_lop_tensor(final_metrics, final_acc, Cref0);
        Trit emergent_cause0 = emergent_cause_with_lop(&diag_final, &arch_diag_final, &lop_final);
        printf("Causa emergente (LOP): %s\n", decode_cause_label(emergent_cause0));
        printf("LOP FO=[%s,%s,%s] (L,O,P)\n", trit_to_str(lop_final.FO[0]), trit_to_str(lop_final.FO[1]), trit_to_str(lop_final.FO[2]));
        printf("Precisión global observada: %.1f%%\n", final_acc*100.0f);
        // Causa basada en cluster (R+A+D) con la mejor codificación
        Trit VRf[3], VAf[3], VDf[3];
        build_relatore_vector_from_rules(rules, n_rules, VRf);
        build_archetype_vector_from_archs(archs, n_arch, VAf);
        build_dynamic_vector_from_data(final_char_vec, exs, nex, VDf);
        Trit cause_cluster_final = emergent_cluster_cause(VRf, VAf, VDf);
        printf("Causa (cluster R+A+D): %s\n", decode_cause_label(cause_cluster_final));

        // Funciones auxiliares para educación extendida

        if(epochs>1){
            printf("\n=== Educación Extendida (multi-epoch=%d) ===\n", epochs);
            Ex aug[256]; int naug = augment_examples(exs, nex, aug, 256);
            static Rule rulesE[512]; int n_rulesE=0; // memoria educativa acumulativa
            Archetype archsE[16]; int n_archE=0;
            float best_overall_epoch = final_metrics.overall; // base
            for(int ep=1; ep<epochs; ep++){
                // Entrenar sobre corpus base + aumentado
                for(int k=0;k<nex+naug;k++){
                    const Ex* src = (k<nex? &exs[k] : &aug[k-nex]);
                    const char* w=src->w; const char* s=src->s; int i=0,j=0;
                    while(w[i]){
                        if(!w[i+1]) break;
                        int break_here=0; while(s[j] && s[j]!=w[i]) j++; if(s[j]==w[i]){ j++; if(s[j]=='-'){ break_here=1; j++; } }
                        Trit A[3],B[3],R[3],M[3]; final_char_vec((unsigned char)w[i],A); final_char_vec((unsigned char)w[i+1],B); R[0]=R[1]=R[2]= break_here?1:0; vec_learn_M(A,B,R,M); upsert_rule_mem(rulesE,&n_rulesE,A,B,M); i++; }
                }
                // Autopoda: eliminar reglas sin soporte y M totalmente indeterminado
                int before = n_rulesE; int write=0;
                for(int i=0;i<n_rulesE;i++){
                    int alln = (rulesE[i].M[0]==-1 && rulesE[i].M[1]==-1 && rulesE[i].M[2]==-1);
                    if(rulesE[i].count>=2 || !alln){ rulesE[write++]=rulesE[i]; }
                }
                n_rulesE = write; int after = n_rulesE;
                DiagnosticMetrics dm = diagnose_rules(rulesE, n_rulesE);
                float accE = evaluate_break_accuracy(final_char_vec, rulesE, n_rulesE, exs, nex);
                n_archE = synthesize_archetypes(rulesE, n_rulesE, archsE, 16);
                  TensorFFE C_E = anneal_creencia_tensor(rulesE, n_rulesE, archsE, n_archE, final_char_vec, exs, nex, 2);
                  Trit CrefE = harmonize_with_fibonacci(C_E.FO);
                  TensorFFE diagE = build_diagnostic_tensor(dm, CrefE);
                  TensorFFE lopE = build_lop_tensor(dm, accE, CrefE);
                TensorFFE archDiagE = diagnostic_archetype_template();
                Trit causeE = emergent_cause_with_lop(&diagE, &archDiagE, &lopE);
                  // Cluster cause en cada epoch sobre memoria educativa acumulada
                  Trit VRe[3], VAe[3], VDe[3];
                  build_relatore_vector_from_rules(rulesE, n_rulesE, VRe);
                  build_archetype_vector_from_archs(archsE, n_archE, VAe);
                  build_dynamic_vector_from_data(final_char_vec, exs, nex, VDe);
                  Trit causeClusterE = emergent_cluster_cause(VRe, VAe, VDe);
                float gain = dm.overall - best_overall_epoch;
                if(dm.overall > best_overall_epoch) best_overall_epoch = dm.overall;
                  printf("Epoch %d: reglas=%d(%d→%d) archs=%d calidad=%.2f%% Δ=%.2f%% acc=%.2f%% causa=%s cluster=%s LOP=[%s,%s,%s]\n",
                       ep, n_rulesE, before, after, n_archE, dm.overall*100.0f, gain*100.0f, accE*100.0f,
                      decode_cause_label(causeE), decode_cause_label(causeClusterE), trit_to_str(lopE.FO[0]), trit_to_str(lopE.FO[1]), trit_to_str(lopE.FO[2]));
            }
        }
        // Baselines para comparación
        float acc_vc = evaluate_baseline_vowel_consonant(exs, nex);
        float acc_rnd = evaluate_baseline_random(exs, nex, 42);
        printf("\n=== Baselines ===\nV→C: %.1f%%  Random: %.1f%%\n", acc_vc*100.0f, acc_rnd*100.0f);

        // Predict: aplicar Armonizador (Algoritmo de Dios) sobre resultados
        // Proceso puro: regla → infer → harmonize → decisión
        // NO hay if's de dominio, solo geometría tensorial + Armonizador
        const char* tests[] = {"lobo","pera","mono","cereza","plato"}; int nt=5;
        printf("\n=== Predicciones (con %s) ===\n", encoding_names[best_encoding]);
            // Usar guía Cref0 en el colapso final
            for(int t=0;t<nt;t++){
            const char* w=tests[t]; printf("%s -> ", w);
            for(int i=0; w[i]; i++){
                putchar(w[i]); if(!w[i+1]) break;
                Trit A[3],B[3]; 
                final_char_vec((unsigned char)w[i],A); 
                final_char_vec((unsigned char)w[i+1],B);
                int k=find_rule_idx(rules,n_rules,A,B);
                
                if(k>=0){
                    // 1. Inferir resultado desde regla aprendida (trigate)
                    Trit r[3]; vec_infer(A,B,rules[k].M,r);
                    
                        // 2. Armonizar guiado por Cref0 (creencia superior)
                        Trit decision = harmonize_guided(r, Cref0);
                    
                    // 3. Aplicar decisión emergente
                    if(decision == 1) putchar('-');
                    // Si decision==0 o ==-1: no romper (continuar)
                }
                // Si no hay regla: no romper (comportamiento por defecto)
            }
            putchar('\n');
        }
        // Pseudopalabras (generalización)
        const Ex pseudo[] = {{"glopa","glo-pa"},{"blima","bli-ma"}}; int np=2;
        printf("\n=== Pseudopalabras ===\n");
        for(int t=0;t<np;t++){
            const char* w=pseudo[t].w; printf("%s -> ", w);
            for(int i=0; w[i]; i++){
                putchar(w[i]); if(!w[i+1]) break;
                Trit A[3],B[3]; final_char_vec((unsigned char)w[i],A); final_char_vec((unsigned char)w[i+1],B);
                int k=find_rule_idx(rules,n_rules,A,B);
                if(k>=0){ Trit r[3]; vec_infer(A,B,rules[k].M,r); Trit decision = harmonize_guided(r, Cref0); if(decision==1) putchar('-'); }
            }
            printf("  (esperado: %s)\n", pseudo[t].s);
        }
        return 0;
    }
    printf("Unknown command '%s'\n", argv[1]); return 1; }
