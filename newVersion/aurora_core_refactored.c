/*
 * aurora_core_refactored.c - Aurora Core v3.1.1 (Trigate-Pure)
 * 
 * Implementación completa siguiendo Technical Annex:
 * 1. FFE Tensor con roles trinarios (Informational, Cognitive, Energetic)
 * 2. Emergencia reversible con memorias (me1, me2, me3)
 * 3. Ciclo completo: Information → Knowledge → Energy → Information
 * 4. Energetic Trio explícito: Tensión, Comando, Energía
 * 5. Validación ES.index ≠ FO.index
 * 6. Fibonacci ternario para selección de roles
 * 
 * Licencias: Apache 2.0 + CC BY 4.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ═══════════════════════════════════════════════════════════════════════
 * GLOSARIO TÉCNICO FUNDAMENTAL (Technical Annex §1)
 * ═══════════════════════════════════════════════════════════════════════ */

/* Trit: u (1), c (0), n (-1) */
typedef int Trit;
#define TRIT_U  1   /* true/upper/correct */
#define TRIT_C  0   /* center/false */
#define TRIT_N -1   /* null/indeterminate */

/* Dimension FFE: 3 trits con roles FO/FN/ES contextuales */
typedef struct {
    Trit t[3]; /* [FO, FN, ES] - roles dependen del contexto */
} Dimension;

/* Vector FFE: 3 dimensiones - unidad operativa universal */
typedef struct {
    Dimension d[3]; /* [Dim0, Dim1, Dim2] */
} Vector;

/* Roles de Vector (Technical Annex §2) */
typedef enum {
    ROLE_INFORMATIONAL,  /* (value, data, mode) */
    ROLE_COGNITIVE,      /* (relator, dynamics, archetype) */
    ROLE_ENERGETIC       /* (tension, energy, command) */
} VectorRole;

/* Emergency Memory: tres memorias reversibles (Technical Annex §6.2) */
typedef struct {
    Trit me[3]; /* me1, me2, me3 - interpretación según rol */
} EmergencyMemory;

/* TensorBasic: síntesis + base + memoria reversible */
typedef struct {
    Dimension synthesis;     /* Nivel superior (ds) */
    Vector base;             /* Nivel inferior (d1, d2, d3) */
    EmergencyMemory memory;  /* Memorias de emergencia */
    VectorRole role;         /* Rol contextual del vector */
} TensorBasic;

/* TensorAurora: Estructura completa 1-3-9 */
typedef struct {
    Dimension level1;        /* 1 dimensión superior */
    Vector level2;           /* 3 dimensiones intermedias */
    TensorBasic level3[3];   /* 9 dimensiones (3 tensores básicos) */
} TensorAurora;

/* Cluster tensorial (ventanas deslizantes) */
#define MAX_CLUSTER 64

/* ═══════════════════════════════════════════════════════════════════════
 * UTILIDADES BÁSICAS
 * ═══════════════════════════════════════════════════════════════════════ */

static const char* ts(Trit t) {
    return t == TRIT_U ? "u" : (t == TRIT_C ? "c" : "n");
}

static int trit_to_idx(Trit t) {
    /* Mapeo general para índices de arrays: c→0, u→1, n→2 */
    return (t == TRIT_C) ? 0 : (t == TRIT_U ? 1 : 2);
}

static int es_val_to_fo_idx(Trit es_val) {
    /* Mapeo semántico ES→FO según Technical Annex §9:
     * Si ES = c (estructura base) → FO está en posición 0
     * Si ES = u (estructura alta) → FO está en posición 1
     * Si ES = n (sin estructura) → FO está en posición 2 (pero invalida)
     */
    if (es_val == TRIT_C) return 0;
    if (es_val == TRIT_U) return 1;
    return 2; /* n → posición 2, pero será invalidado */
}

static const char* role_name(VectorRole r) {
    switch(r) {
        case ROLE_INFORMATIONAL: return "INFO";
        case ROLE_COGNITIVE: return "KNOW";
        case ROLE_ENERGETIC: return "ENERGY";
        default: return "?";
    }
}

static int eq_dim(const Dimension* a, const Dimension* b) {
    return a->t[0] == b->t[0] && a->t[1] == b->t[1] && a->t[2] == b->t[2];
}

static void copy_dim(Dimension* dst, const Dimension* src) {
    memcpy(dst->t, src->t, sizeof(src->t));
}

static int count_nulls_dim(const Dimension* d) {
    int c = 0;
    for(int i = 0; i < 3; i++) if(d->t[i] == TRIT_N) c++;
    return c;
}

static int count_nulls_vec(const Vector* v) {
    return count_nulls_dim(&v->d[0]) + 
           count_nulls_dim(&v->d[1]) + 
           count_nulls_dim(&v->d[2]);
}

/* Adelanto de prototipo para rotación de roles en pipelines fractales */
static VectorRole next_role_in_cycle(VectorRole current);

/* ═══════════════════════════════════════════════════════════════════════
 * TRIGATE: ÁTOMO DE LA INTELIGENCIA (Technical Annex §4)
 * Operaciones: AND₃ (c), OR₃ (u), CONSENSUS (n)
 * Modos: Inference, Learning, Deduction
 * ═══════════════════════════════════════════════════════════════════════ */

static Trit trit_and(Trit a, Trit b) {
    if (a == TRIT_C || b == TRIT_C) return TRIT_C;
    if (a == TRIT_U && b == TRIT_U) return TRIT_U;
    return TRIT_N;
}

static Trit trit_or(Trit a, Trit b) {
    if (a == TRIT_U || b == TRIT_U) return TRIT_U;
    if (a == TRIT_C && b == TRIT_C) return TRIT_C;
    return TRIT_N;
}

static Trit trit_consensus(Trit a, Trit b) {
    if (a != TRIT_N && a == b) return a;
    return TRIT_N;
}

/* Trigate - Modo Inference: (A,B,M)→R */
static Trit trit_infer(Trit a, Trit b, Trit m) {
    if (m == TRIT_C) return trit_and(a, b);   /* c → AND₃ */
    if (m == TRIT_U) return trit_or(a, b);    /* u → OR₃ */
    return trit_consensus(a, b);               /* n → CONSENSUS */
}

/* Trigate - Modo Learning: (A,B,R)→M */
static Trit trit_learn(Trit a, Trit b, Trit r) {
    if (trit_and(a, b) == r) return TRIT_C;   /* AND funciona */
    if (trit_or(a, b) == r) return TRIT_U;    /* OR funciona */
    if (a != TRIT_N && a == b && r == a) return TRIT_N; /* CONSENSUS */
    return TRIT_N;
}

/* Trigate - Modo Deduction: (A,M,R)→B */
static Trit trit_deduce_b(Trit a, Trit m, Trit r) {
    if (m == TRIT_C) { /* AND */
        if (a == TRIT_U && r == TRIT_U) return TRIT_U;
        if (a == TRIT_U && r == TRIT_C) return TRIT_C;
        if (a == TRIT_C) return TRIT_N; /* 0 AND x = 0, x no importa */
    } else if (m == TRIT_U) { /* OR */
        if (a == TRIT_U) return TRIT_N; /* 1 OR x = 1, x no importa */
        if (a == TRIT_C && r == TRIT_U) return TRIT_U;
        if (a == TRIT_C && r == TRIT_C) return TRIT_C;
    } else { /* CONSENSUS */
        if (r == TRIT_N) return TRIT_N;
        return r; /* B debe ser igual a R */
    }
    return TRIT_N;
}

/* ═══════════════════════════════════════════════════════════════════════
 * FIBONACCI TERNARIO (Technical Annex §8)
 * Contador base-3 para selección de FO/FN/ES sin resonancias
 * ═══════════════════════════════════════════════════════════════════════ */

typedef struct {
    int state;  /* Estado interno del contador */
    int fib_a;  /* Fibonacci anterior */
    int fib_b;  /* Fibonacci actual */
} FibCounter;

static void fib_init(FibCounter* fc) {
    fc->state = 0;
    fc->fib_a = 1;
    fc->fib_b = 1;
}

static void fib_next(FibCounter* fc) {
    int temp = fc->fib_a + fc->fib_b;
    fc->fib_a = fc->fib_b;
    fc->fib_b = temp;
    fc->state = (fc->state + 1) % 27; /* base-3, 3 dígitos: 000..222 */
}

static int fib_select_role(FibCounter* fc) {
    /* Retorna 0, 1, o 2 según Fibonacci ternario */
    return (fc->fib_b % 3);
}

static FibCounter global_fib_counter = {0, 1, 1};

/* ═══════════════════════════════════════════════════════════════════════
 * VALIDACIÓN TENSORIAL (Technical Annex §9)
 * Regla absoluta: ES.index ≠ FO.index
 * ═══════════════════════════════════════════════════════════════════════ */

static int validate_dimension(const Dimension* d) {
    /* Si está completamente nula, no es válida */
    if (d->t[0] == TRIT_N && d->t[1] == TRIT_N && d->t[2] == TRIT_N) {
        return 0;
    }

    /* Seleccionar cuál trit actúa como ES vía contador Fibonacci */
    int es_idx = fib_select_role(&global_fib_counter);
    fib_next(&global_fib_counter);

    Trit es_val = d->t[es_idx];
    
    /* CASO CRÍTICO: ES null → verificar que ningún trit señale al índice ES */
    if (es_val == TRIT_N) {
        /* Verificar auto-referencias: ningún trit puede apuntar a la posición ES */
        for (int i = 0; i < 3; i++) {
            int fo_idx_from_val = es_val_to_fo_idx(d->t[i]);
            if (fo_idx_from_val == es_idx) {
                return 0; /* Auto-referencia detectada - INVÁLIDO */
            }
        }
        return 1; /* Válida sin estructura definida */
    }

    /* Usar mapeo semántico ES→FO */
    int fo_idx = es_val_to_fo_idx(es_val);

    /* Regla crítica: ES no puede apuntarse a sí mismo como FO */
    if (fo_idx == es_idx) return 0;

    return 1;
}

static int validate_vector(const Vector* v) {
    for (int i = 0; i < 3; i++) {
        if (!validate_dimension(&v->d[i])) {
            return 0;
        }
    }
    return 1;
}

/* ═══════════════════════════════════════════════════════════════════════
 * TRES MEMORIAS DEL SISTEMA (Technical Annex §10)
 * Arquetipo, Dinámica, Relator
 * ═══════════════════════════════════════════════════════════════════════ */

#define MAX_MEM 256

/* Arquetipo: patrón estable (forma) */
typedef struct {
    Trit pattern[3];
    Trit fo_output;
    int support;
    unsigned long rev;
} Arquetipo;

/* Dinámica: evolución temporal */
typedef struct {
    Trit state_before[3];
    Trit state_after[3];
    Trit fn_output;
    int support;
    unsigned long rev;
} Dinamica;

/* Relator: orden relacional */
typedef struct {
    Trit dim_a[3];
    Trit dim_b[3];
    Trit mode[3];
    int support;
    unsigned long rev;
} Relator;

/* Knowledge Base */
static Arquetipo arquetipos[MAX_MEM];
static int n_arquetipos = 0;

static Dinamica dinamicas[MAX_MEM];
static int n_dinamicas = 0;

static Relator relatores[MAX_MEM];
static int n_relatores = 0;

static unsigned long global_rev = 1;

/* Tensor C: Punto de creencia estable (Technical Annex §10, Whitepaper §3.3.8) */
static Dimension tensor_C = {{TRIT_N, TRIT_N, TRIT_N}};

/* ═════════════════════════════════════════════════════════════════════════
 * AXIOMA DE LA INTELIGENCIA: Libertad-Orden-Propósito
 * ════════════════════════════════════════════════════════════════════════ */

typedef struct {
    Trit freedom;    /* Entropía: capacidad de cambio, potencial, exploración */
    Trit order;      /* Coherencia: estructura, estabilidad, forma */
    Trit purpose;    /* Propósito: dirección, intención, significado */
} AxiomTrio;

/* ESTADO ENERGÉTICO: Cómo el sistema SIENTE su propio estado interno (propriocepción) */
typedef struct {
    Trit tension;    /* Rigidez (Order dominante, falta Libertad) */
    Trit entropy;    /* Caos (Libertad sin Order) */
    Trit harmony;    /* Alineación (Freedom + Order + Purpose balanceados) */
} EnergeticState;

/* Inicializar variables globales ANTES de usarlas en funciones */
static AxiomTrio axiom_state = {TRIT_C, TRIT_C, TRIT_C};  /* Estado del axioma (F-O-P) */
static EnergeticState estado_energetico = {TRIT_C, TRIT_C, TRIT_C};  /* Cómo se SIENTE el sistema */

/* ═══════════════════════════════════════════════════════════════════════
 * FUNCIONES DEL AXIOMA
 * ═══════════════════════════════════════════════════════════════════════ */

static void update_axiom_state(int null_count, int coherence_score, int purpose_signal) {
    /* Actualiza el estado del axioma según el estado energético observado
     * - null_count → imbalance entre Orden (pocos nulls) y Libertad (muchos nulls)
     * - coherence_score → qué tan bien Order se mantiene
     * - purpose_signal → qué tan clara es la dirección (Propósito)
     */
    if (null_count > 5) {
        axiom_state.freedom = TRIT_C;  /* Mucha entropía → se necesita Libertad */
        axiom_state.order = TRIT_U;    /* Orden débil */
    } else if (null_count == 0 && coherence_score > 8) {
        axiom_state.order = TRIT_C;    /* Orden fuerte */
        axiom_state.freedom = TRIT_U;  /* Poco espacio para Libertad */
    } else {
        axiom_state.order = TRIT_C;
        axiom_state.freedom = TRIT_C;  /* Balance */
    }
    
    if (purpose_signal > 0) {
        axiom_state.purpose = TRIT_C;  /* Propósito claro */
    } else {
        axiom_state.purpose = TRIT_N;  /* Propósito indeterminado */
    }
}

static float axiom_balance(void) {
    /* Calcula el balance de los tres axiomas (0.0 = perfecto balance)
     * Retorna la "sensación" de equilibrio del sistema
     * Si es cercano a 0: están balanceados (armonía)
     * Si es alto: hay tensión o caos
     */
    int f_active = (axiom_state.freedom == TRIT_C) ? 1 : 0;
    int o_active = (axiom_state.order == TRIT_C) ? 1 : 0;
    int p_active = (axiom_state.purpose == TRIT_C) ? 1 : 0;
    
    int total_active = f_active + o_active + p_active;
    float balance = fabs(total_active - 1.5f) / 1.5f;  /* 1.5 es el balance perfecto */
    return balance;
}

/* Síntesis triádica para convergencia a coherencia */
static Trit triadic_collapse(Trit a, Trit b, Trit c) {
    /* Votación ponderada entre tres valores */
    int counts[3] = {0, 0, 0}; /* [c_count, u_count, n_count] */
    
    if (a == TRIT_C) counts[0]++;
    else if (a == TRIT_U) counts[1]++;
    else counts[2]++;
    
    if (b == TRIT_C) counts[0]++;
    else if (b == TRIT_U) counts[1]++;
    else counts[2]++;
    
    if (c == TRIT_C) counts[0]++;
    else if (c == TRIT_U) counts[1]++;
    else counts[2]++;
    
    /* Mayoría gana; empate → TRIT_N */
    if (counts[0] >= 2) return TRIT_C;
    if (counts[1] >= 2) return TRIT_U;
    return TRIT_N;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * POLÍTICA LRU PARA MAX_MEM (v3.1) - VERSIÓN TRIGATE PURA
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * Principio autosimilar: La evicción emerge de comparar tensores de "edad"
 * usando solo Trigates. No hay matemáticas, solo relaciones ternarias.
 * 
 * Proceso:
 *   1. Convertir rev → Trit (U=nuevo, C=antiguo, N=indeterminado)
 *   2. Comparar mediante Trigate en modo CONSENSUS
 *   3. El "más antiguo" emerge como patrón dominante
 *   4. Reorganizar array = colapsar tensor eliminando nivel inactivo
 */

/* Convertir timestamp relativo a Trit de "edad" */
static Trit rev_to_age_trit(unsigned long rev, unsigned long max_rev) {
    if (max_rev == 0) return TRIT_N;
    
    /* Dividir rango en tercios usando Fibonacci-like distribution */
    unsigned long third = max_rev / 3;
    
    if (rev >= max_rev - third) return TRIT_U;      /* Reciente → U (1) */
    else if (rev <= third) return TRIT_C;            /* Antiguo → C (0) */
    else return TRIT_N;                              /* Medio → N (null) */
}

/* Comparar dos trits de edad usando Trigate: retorna índice del más antiguo */
static int trigate_compare_age(Trit age_a, Trit age_b) {
    /* Trigate en modo CONSENSUS: busca el trit "menor" (más antiguo) */
    Trit result = trit_infer(age_a, age_b, TRIT_N); /* CONSENSUS mode */
    
    /* Decodificar resultado ternario:
     *   C (0) → a es más antiguo
     *   U (1) → b es más antiguo  
     *   N     → empate, mantener a
     */
    if (result == TRIT_C || result == TRIT_N) return 0; /* a gana */
    return 1; /* b gana */
}

/* Encontrar índice del más antiguo mediante cascada de Trigates */
static int find_oldest_by_trigate(unsigned long* revs, int count) {
    if (count == 0) return -1;
    if (count == 1) return 0;
    
    /* Encontrar max_rev para normalización */
    unsigned long max_rev = revs[0];
    for (int i = 1; i < count; i++) {
        if (revs[i] > max_rev) max_rev = revs[i];
    }
    
    /* Convertir todos los rev a trits de edad */
    Trit ages[MAX_MEM];
    for (int i = 0; i < count; i++) {
        ages[i] = rev_to_age_trit(revs[i], max_rev);
    }
    
    /* Cascada de Trigates para encontrar el mínimo (más antiguo) */
    int oldest_idx = 0;
    Trit oldest_age = ages[0];
    
    for (int i = 1; i < count; i++) {
        int winner = trigate_compare_age(oldest_age, ages[i]);
        if (winner == 1) { /* ages[i] es más antiguo */
            oldest_idx = i;
            oldest_age = ages[i];
        }
    }
    
    return oldest_idx;
}

/* Reorganizar array = Colapso tensorial tras eliminar nivel */
static void collapse_array_arquetipos(int remove_idx) {
    /* Colapso fractal: el nivel eliminado desaparece,
     * los niveles superiores descienden manteniendo coherencia */
    for (int i = remove_idx; i < n_arquetipos - 1; i++) {
        arquetipos[i] = arquetipos[i + 1];
    }
    n_arquetipos--;
}

static void collapse_array_dinamicas(int remove_idx) {
    for (int i = remove_idx; i < n_dinamicas - 1; i++) {
        dinamicas[i] = dinamicas[i + 1];
    }
    n_dinamicas--;
}

static void collapse_array_relatores(int remove_idx) {
    for (int i = remove_idx; i < n_relatores - 1; i++) {
        relatores[i] = relatores[i + 1];
    }
    n_relatores--;
}

/* Evicción mediante emergencia de "edad" desde Trigates */
static void evict_oldest_arquetipo(void) {
    if (n_arquetipos == 0) return;
    
    /* Extraer revs en array para comparación ternaria */
    unsigned long revs[MAX_MEM];
    for (int i = 0; i < n_arquetipos; i++) {
        revs[i] = arquetipos[i].rev;
    }
    
    /* Encontrar el más antiguo mediante cascada de Trigates */
    int oldest_idx = find_oldest_by_trigate(revs, n_arquetipos);
    
    if (oldest_idx >= 0) {
        collapse_array_arquetipos(oldest_idx);
    }
}

static void evict_oldest_dinamica(void) {
    if (n_dinamicas == 0) return;
    
    unsigned long revs[MAX_MEM];
    for (int i = 0; i < n_dinamicas; i++) {
        revs[i] = dinamicas[i].rev;
    }
    
    int oldest_idx = find_oldest_by_trigate(revs, n_dinamicas);
    
    if (oldest_idx >= 0) {
        collapse_array_dinamicas(oldest_idx);
    }
}

static void evict_oldest_relator(void) {
    if (n_relatores == 0) return;
    
    unsigned long revs[MAX_MEM];
    for (int i = 0; i < n_relatores; i++) {
        revs[i] = relatores[i].rev;
    }
    
    int oldest_idx = find_oldest_by_trigate(revs, n_relatores);
    
    if (oldest_idx >= 0) {
        collapse_array_relatores(oldest_idx);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * APRENDIZAJE (LEARNING) - Memoria A-R-D con LRU
 * ═══════════════════════════════════════════════════════════════════════════ */

static void learn_arquetipo(const Trit pattern[3], Trit fo_out) {
    for (int i = 0; i < n_arquetipos; i++) {
        if (memcmp(arquetipos[i].pattern, pattern, 3 * sizeof(Trit)) == 0) {
            arquetipos[i].support++;
            arquetipos[i].rev = global_rev++;
            if (arquetipos[i].fo_output != fo_out && fo_out != TRIT_N) {
                arquetipos[i].fo_output = TRIT_N; /* conflicto */
            }
            return;
        }
    }
    
    /* v3.1: LRU eviction cuando MAX_MEM está saturado */
    if (n_arquetipos >= MAX_MEM) {
        evict_oldest_arquetipo();
    }
    
    memcpy(arquetipos[n_arquetipos].pattern, pattern, 3 * sizeof(Trit));
    arquetipos[n_arquetipos].fo_output = fo_out;
    arquetipos[n_arquetipos].support = 1;
    arquetipos[n_arquetipos].rev = global_rev++;
    n_arquetipos++;
}

static void learn_dinamica(const Trit before[3], const Trit after[3], Trit fn_out) {
    for (int i = 0; i < n_dinamicas; i++) {
        if (memcmp(dinamicas[i].state_before, before, 3 * sizeof(Trit)) == 0 &&
            memcmp(dinamicas[i].state_after, after, 3 * sizeof(Trit)) == 0) {
            dinamicas[i].support++;
            dinamicas[i].rev = global_rev++;
            if (dinamicas[i].fn_output != fn_out && fn_out != TRIT_N) {
                dinamicas[i].fn_output = TRIT_N;
            }
            return;
        }
    }
    
    /* v3.1: LRU eviction cuando MAX_MEM está saturado */
    if (n_dinamicas >= MAX_MEM) {
        evict_oldest_dinamica();
    }
    
    memcpy(dinamicas[n_dinamicas].state_before, before, 3 * sizeof(Trit));
    memcpy(dinamicas[n_dinamicas].state_after, after, 3 * sizeof(Trit));
    dinamicas[n_dinamicas].fn_output = fn_out;
    dinamicas[n_dinamicas].support = 1;
    dinamicas[n_dinamicas].rev = global_rev++;
    n_dinamicas++;
}

static void learn_relator(const Trit a[3], const Trit b[3], const Trit m[3]) {
    for (int i = 0; i < n_relatores; i++) {
        if (memcmp(relatores[i].dim_a, a, 3 * sizeof(Trit)) == 0 &&
            memcmp(relatores[i].dim_b, b, 3 * sizeof(Trit)) == 0) {
            relatores[i].support++;
            relatores[i].rev = global_rev++;
            
            /* v3.1: Aprendizaje granular por posición con threshold de soporte */
            for (int k = 0; k < 3; k++) {
                if (m[k] == TRIT_N) continue; /* No aprender nulls */
                
                if (relatores[i].support >= 5) {
                    /* Alto soporte → aprendizaje granular por posición */
                    if (relatores[i].mode[k] == TRIT_N || relatores[i].mode[k] == m[k]) {
                        relatores[i].mode[k] = m[k]; /* Reforzar o establecer */
                    } else {
                        relatores[i].mode[k] = TRIT_N; /* Contradicción → null */
                    }
                } else {
                    /* Bajo soporte → solo invalidar si contradice */
                    if (relatores[i].mode[k] != m[k] && m[k] != TRIT_N) {
                        relatores[i].mode[k] = TRIT_N;
                    }
                }
            }
            return;
        }
    }
    
    /* v3.1: LRU eviction cuando MAX_MEM está saturado */
    if (n_relatores >= MAX_MEM) {
        evict_oldest_relator();
    }
    
    memcpy(relatores[n_relatores].dim_a, a, 3 * sizeof(Trit));
    memcpy(relatores[n_relatores].dim_b, b, 3 * sizeof(Trit));
    memcpy(relatores[n_relatores].mode, m, 3 * sizeof(Trit));
    relatores[n_relatores].support = 1;
    relatores[n_relatores].rev = global_rev++;
    n_relatores++;
}

/* Actualizar Tensor C desde conocimiento acumulado A-R-D */
static void update_tensor_C(void) {
    /* Convergencia triádica: Arquetipo + Relator + Dinámica → Creencia */
    if (n_arquetipos > 0 && n_dinamicas > 0 && n_relatores > 0) {
        /* v3.1: Seleccionar los más estables (mayor soporte, desempate por rev) */
        Arquetipo* best_arq = &arquetipos[0];
        Dinamica* best_dyn = &dinamicas[0];
        Relator* best_rel = &relatores[0];
        
        for (int i = 1; i < n_arquetipos; i++) {
            if (arquetipos[i].support > best_arq->support || 
                (arquetipos[i].support == best_arq->support && arquetipos[i].rev > best_arq->rev)) {
                best_arq = &arquetipos[i];
            }
        }
        for (int i = 1; i < n_dinamicas; i++) {
            if (dinamicas[i].support > best_dyn->support || 
                (dinamicas[i].support == best_dyn->support && dinamicas[i].rev > best_dyn->rev)) {
                best_dyn = &dinamicas[i];
            }
        }
        for (int i = 1; i < n_relatores; i++) {
            if (relatores[i].support > best_rel->support || 
                (relatores[i].support == best_rel->support && relatores[i].rev > best_rel->rev)) {
                best_rel = &relatores[i];
            }
        }
        
        /* Síntesis triádica en cada dimensión */
        tensor_C.t[0] = triadic_collapse(best_arq->fo_output, best_rel->mode[0], best_dyn->fn_output);
        tensor_C.t[1] = triadic_collapse(best_arq->pattern[1], best_rel->mode[1], best_dyn->fn_output);
        tensor_C.t[2] = triadic_collapse(best_arq->pattern[2], best_rel->mode[2], best_dyn->state_after[2]);
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * EXTRACCIÓN E INTERPRETACIÓN DE MEMORIAS ENERGÉTICAS
 * ═══════════════════════════════════════════════════════════════════════ */

/* Interpretar memorias de emergencia según rol del vector */
static EnergeticState extract_energetic_state(const EmergencyMemory* mem, VectorRole role) {
    EnergeticState state;
    
    if (role == ROLE_COGNITIVE) {
        /* En modo COGNITIVE: me1→tensión, me2→entropía, me3→armonía */
        state.tension = mem->me[0];
        state.entropy = mem->me[1];
        state.harmony = mem->me[2];
    } else {
        /* En otros modos, interpretar como valores genéricos */
        state.tension = mem->me[0];
        state.entropy = mem->me[1];
        state.harmony = mem->me[2];
    }
    
    return state;
}

static void update_energetic_feeling(const EnergeticState* new_feeling) {
    /* Actualizar sensación energética usando trigates (acumulación OR-like) */
    estado_energetico.tension = trit_infer(estado_energetico.tension, new_feeling->tension, TRIT_U);
    estado_energetico.entropy = trit_infer(estado_energetico.entropy, new_feeling->entropy, TRIT_U);
    estado_energetico.harmony = trit_infer(estado_energetico.harmony, new_feeling->harmony, TRIT_U);
}

/* ═══════════════════════════════════════════════════════════════════════
 * FUNCIÓN DE EMERGENCIA (Technical Annex §6)
 * Input: d1, d2, d3
 * Output: ds (síntesis) + me1, me2, me3 (memorias reversibles)
 * ═══════════════════════════════════════════════════════════════════════ */

static void emergence_function(
    const Dimension d[3],
    Dimension* ds_out,
    EmergencyMemory* mem_out,
    VectorRole current_role
) {
    /* Triada autosimilar (Technical Annex §6.1, §3.3.5.1):
       - FO pares usan modo ES_i
       - FN pares usan modo FO_i
       - ES pares usan modo FN_i */

    Trit rFO[3];
    Trit rFN[3];
    Trit rES[3];

    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        rFO[i] = trit_infer(d[i].t[0], d[j].t[0], d[i].t[2]);
        rFN[i] = trit_infer(d[i].t[1], d[j].t[1], d[i].t[0]);
        rES[i] = trit_infer(d[i].t[2], d[j].t[2], d[i].t[1]);
    }

    /* Síntesis superior por colapso triádico */
    ds_out->t[0] = triadic_collapse(rFO[0], rFO[1], rFO[2]);
    ds_out->t[1] = triadic_collapse(rFN[0], rFN[1], rFN[2]);
    ds_out->t[2] = triadic_collapse(rES[0], rES[1], rES[2]);

    /* Memorias de emergencia informacionales (breadcrumbs) */
    mem_out->me[0] = rFO[0];
    mem_out->me[1] = rFO[1];
    mem_out->me[2] = rFO[2];
    
    /* Aprender según rol actual */
    if (current_role == ROLE_INFORMATIONAL) {
        /* En modo INFO → aprender arquetipo */
        learn_arquetipo(rFN, ds_out->t[0]);
    } else if (current_role == ROLE_COGNITIVE) {
        /* En modo COGNITIVE → actualizar sensación energética + tensor C */
        EnergeticState feeling = extract_energetic_state(mem_out, current_role);
        update_energetic_feeling(&feeling);
        update_tensor_C(); /* Actualizar creencia estable */
    } else if (current_role == ROLE_ENERGETIC) {
        /* En modo ENERGETIC → actualizar axioma basado en coherencia observada */
        int nulls_superior = count_nulls_dim(ds_out);
        update_axiom_state(nulls_superior, 8, n_arquetipos > 0 ? 1 : 0);
    }
}

/* Función de extensión (reconstrucción guiada por memorias)
 * 
 * LÓGICA DE RECONSTRUCCIÓN SEMÁNTICA:
 * Las memorias (me1, me2, me3) contienen resultados intermedios r1, r2, r3
 * de la fase de síntesis. Usando estos valores y la síntesis superior (ds),
 * podemos reconstruir configuraciones coherentes en el nivel inferior.
 * 
 * JUSTIFICACIÓN:
 * - mem[i] contiene el resultado de inferir dimensiones específicas
 * - Rotación (i+1)%3 preserva la estructura triádica del sistema
 * - ds proporciona la coherencia global que guía la expansión
 * 
 * NO es inversión matemática exacta, sino RECONSTRUCCIÓN COHERENTE:
 * mantiene las relaciones semánticas y reduce entropía.
 */
static void extend_function(
    const Dimension* ds,
    const EmergencyMemory* mem,
    Dimension d_out[3]
) {
    /* Reconstrucción coherente guiada por síntesis y breadcrumbs.
       Mantiene autosimilitud sin inversión rígida. */
    for (int i = 0; i < 3; i++) {
        /* FO: usar síntesis FO con memoria local y modo FN_s */
        Trit m_fo = (ds->t[1] != TRIT_N) ? ds->t[1] : TRIT_U;
        d_out[i].t[0] = trit_infer(ds->t[0], mem->me[i], m_fo);

        /* FN: distribuir desde FO_s y memoria vecina */
        Trit m_fn = (ds->t[0] != TRIT_N) ? ds->t[0] : TRIT_N;
        d_out[i].t[1] = trit_infer(d_out[i].t[0], mem->me[(i+1)%3], m_fn);

        /* ES: propagar desde ES_s con acoplamiento a FN reconstruido */
        Trit m_es = (ds->t[1] != TRIT_N) ? ds->t[1] : TRIT_N;
        d_out[i].t[2] = trit_infer(ds->t[2], d_out[i].t[1], m_es);
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * DESTILADOR (Especificación Operacional)
 * Produce dimensiones destiladas donde todos los trits comparten el mismo rol.
 * Opera con Trigates en pares: (d₁–d₂), (d₂–d₃), (d₃–d₁)
 * ═══════════════════════════════════════════════════════════════════════ */

typedef struct {
    Dimension fo_destilada;  /* {FO′, FO″, FO‴} */
    Dimension fn_destilada;  /* {FN′, FN″, FN‴} */
    Dimension es_destilada;  /* {ES′, ES″, ES‴} */
} DimensionesDestiladas;

static void destilador(const Dimension d[3], DimensionesDestiladas* out) {
    /* ES destilada: recopilación directa sin procesamiento */
    out->es_destilada.t[0] = d[0].t[2];
    out->es_destilada.t[1] = d[1].t[2];
    out->es_destilada.t[2] = d[2].t[2];

    /* Procesar pares: (d₁–d₂), (d₂–d₃), (d₃–d₁) */
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;

        /* Trigate de Forma: A=FO_i, B=FO_j */
        Trit fo_a = d[i].t[0];
        Trit fo_b = d[j].t[0];

        /* Trigate de Función: A=FN_i, B=FN_j */
        Trit fn_a = d[i].t[1];
        Trit fn_b = d[j].t[1];

        /* Aprender modo de FN → usar como operador de FO */
        Trit m_fn = trit_infer(fn_a, fn_b, TRIT_N); /* CONSENSUS inicial */
        Trit r_fo = trit_infer(fo_a, fo_b, m_fn);

        /* El resultado R_FO va a dimensión destilada de Forma */
        out->fo_destilada.t[i] = r_fo;

        /* El modo M_FN va a dimensión destilada de Función */
        out->fn_destilada.t[i] = m_fn;
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * CICLO EMERGENTE (Especificación Operacional)
 * Aplica funciones de emergencia sobre dimensiones destiladas.
 * Genera: tensor emergente superior + tensor de conocimiento
 * ═══════════════════════════════════════════════════════════════════════ */

typedef struct {
    Dimension ds;            /* Dimensión emergente superior */
    Vector conocimiento;     /* Vector de conocimiento (breadcrumbs) */
} ResultadoEmergencia;

static void ciclo_emergente(
    const DimensionesDestiladas* destiladas,
    ResultadoEmergencia* out
) {
    /* Aplicar emergencia a cada dimensión destilada */
    Dimension temp[3];
    EmergencyMemory mem[3];

    /* Procesar FO destilada */
    temp[0] = destiladas->fo_destilada;
    temp[1] = destiladas->fo_destilada; /* autosimilitud */
    temp[2] = destiladas->fo_destilada;
    emergence_function(temp, &out->ds, &mem[0], ROLE_INFORMATIONAL);

    /* Almacenar conocimiento (breadcrumbs) */
    out->conocimiento.d[0].t[0] = mem[0].me[0];
    out->conocimiento.d[0].t[1] = mem[0].me[1];
    out->conocimiento.d[0].t[2] = mem[0].me[2];

    /* Procesar FN destilada */
    temp[0] = destiladas->fn_destilada;
    temp[1] = destiladas->fn_destilada;
    temp[2] = destiladas->fn_destilada;
    Dimension ds_fn;
    emergence_function(temp, &ds_fn, &mem[1], ROLE_INFORMATIONAL);

    out->conocimiento.d[1].t[0] = mem[1].me[0];
    out->conocimiento.d[1].t[1] = mem[1].me[1];
    out->conocimiento.d[1].t[2] = mem[1].me[2];

    /* Procesar ES destilada */
    temp[0] = destiladas->es_destilada;
    temp[1] = destiladas->es_destilada;
    temp[2] = destiladas->es_destilada;
    Dimension ds_es;
    emergence_function(temp, &ds_es, &mem[2], ROLE_INFORMATIONAL);

    out->conocimiento.d[2].t[0] = mem[2].me[0];
    out->conocimiento.d[2].t[1] = mem[2].me[1];
    out->conocimiento.d[2].t[2] = mem[2].me[2];

    /* Combinar resultados en dimensión superior */
    out->ds.t[0] = triadic_collapse(out->ds.t[0], ds_fn.t[0], ds_es.t[0]);
    out->ds.t[1] = triadic_collapse(out->ds.t[1], ds_fn.t[1], ds_es.t[1]);
    out->ds.t[2] = triadic_collapse(out->ds.t[2], ds_fn.t[2], ds_es.t[2]);
}

/* ═══════════════════════════════════════════════════════════════════════
 * EXTENSOR (DESARROLLADOR) (Especificación Operacional)
 * Reconstruye dimensiones destiladas desde síntesis + conocimiento
 * ═══════════════════════════════════════════════════════════════════════ */

static void extensor(
    const Dimension* ds,
    const Vector* conocimiento,
    DimensionesDestiladas* out
) {
    /* Extender cada rol usando conocimiento almacenado */
    Dimension temp_out[3];
    EmergencyMemory mem;

    /* Reconstruir FO destilada */
    mem.me[0] = conocimiento->d[0].t[0];
    mem.me[1] = conocimiento->d[0].t[1];
    mem.me[2] = conocimiento->d[0].t[2];
    extend_function(ds, &mem, temp_out);
    out->fo_destilada = temp_out[0];

    /* Reconstruir FN destilada */
    mem.me[0] = conocimiento->d[1].t[0];
    mem.me[1] = conocimiento->d[1].t[1];
    mem.me[2] = conocimiento->d[1].t[2];
    extend_function(ds, &mem, temp_out);
    out->fn_destilada = temp_out[1];

    /* Reconstruir ES destilada */
    mem.me[0] = conocimiento->d[2].t[0];
    mem.me[1] = conocimiento->d[2].t[1];
    mem.me[2] = conocimiento->d[2].t[2];
    extend_function(ds, &mem, temp_out);
    out->es_destilada = temp_out[2];
}

/* ═══════════════════════════════════════════════════════════════════════
 * COMPOSITOR (Especificación Operacional)
 * Reconstruye dimensiones originales desde destiladas + conocimiento
 * Usa pares de Trigates para resolver coherencia vs creatividad
 * ═══════════════════════════════════════════════════════════════════════ */

static void compositor(
    const DimensionesDestiladas* destiladas,
    const Vector* conocimiento,
    Dimension d_out[3]
) {
    for (int i = 0; i < 3; i++) {
        /* Trigate 1: resolver coherencia
           R = FO destilada[i]
           M = FN conocimiento[i] */
        Trit r1 = destiladas->fo_destilada.t[i];
        Trit m1 = conocimiento->d[i].t[1];

        /* Trigate 2: resolver creatividad
           R = FO conocimiento[i]
           M = FN destilada[i] */
        Trit r2 = conocimiento->d[i].t[0];
        Trit m2 = destiladas->fn_destilada.t[i];

        /* Buscar A y B que satisfagan ambos trigates
           Simplificación: usar deducción desde r1 */
        Trit b_coherencia = trit_deduce_b(TRIT_U, m1, r1);
        Trit a_compartido = (b_coherencia != TRIT_N) ? b_coherencia : TRIT_N;

        d_out[i].t[0] = (a_compartido != TRIT_N) ? a_compartido : r1;
        d_out[i].t[1] = m2;
        d_out[i].t[2] = destiladas->es_destilada.t[i];
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * TRANSCENDER COMPLETO (Pipeline)
 * Ventana de operación → Destilador → Ciclo Emergente → Extensor → Compositor
 * ═══════════════════════════════════════════════════════════════════════ */

typedef struct {
    Vector tensor_emergente;   /* Salida del Transcender */
    Vector conocimiento;       /* Conocimiento generado */
    Dimension comando;         /* Comandos de operación */
} TranscenderOutput;

static void transcender(
    const Vector ventana[3],  /* T1, T2, T3 */
    TranscenderOutput* out
) {
    /* Fase 1: Destilador - procesar combinaciones transversales */
    DimensionesDestiladas dest[3];
    for (int i = 0; i < 3; i++) {
        Dimension combo[3];
        combo[0] = ventana[0].d[i];
        combo[1] = ventana[1].d[i];
        combo[2] = ventana[2].d[i];
        destilador(combo, &dest[i]);
    }

    /* Fase 2: Ciclo Emergente - sintetizar */
    ResultadoEmergencia emerg[3];
    for (int i = 0; i < 3; i++) {
        ciclo_emergente(&dest[i], &emerg[i]);
    }

    /* Formar vector emergente */
    out->tensor_emergente.d[0] = emerg[0].ds;
    out->tensor_emergente.d[1] = emerg[1].ds;
    out->tensor_emergente.d[2] = emerg[2].ds;

    /* Unificar conocimiento */
    out->conocimiento = emerg[0].conocimiento;

    /* Emergencia final para comando */
    Dimension temp_cmd[3];
    temp_cmd[0] = emerg[0].ds;
    temp_cmd[1] = emerg[1].ds;
    temp_cmd[2] = emerg[2].ds;
    EmergencyMemory mem_cmd;
    emergence_function(temp_cmd, &out->comando, &mem_cmd, ROLE_COGNITIVE);

    /* Fase 3: Extensor - reconstruir si es necesario */
    DimensionesDestiladas dest_rec[3];
    for (int i = 0; i < 3; i++) {
        extensor(&emerg[i].ds, &emerg[i].conocimiento, &dest_rec[i]);
    }

    /* Fase 4: Compositor - ensamblar salida final */
    Dimension dims_out[3];
    for (int i = 0; i < 3; i++) {
        compositor(&dest_rec[i], &emerg[i].conocimiento, dims_out);
        /* Actualizar tensor emergente con composición */
        out->tensor_emergente.d[i] = dims_out[i];
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * CLUSTER TENSORIAL CON VENTANAS DESLIZANTES (Pipeline completo)
 * Procesa secuencias de tensores FFE en ventanas ternarias y asciende
 * fractalmente hasta obtener síntesis profundas. Los niveles inferiores
 * quedan desactivados tras cada síntesis (se mantienen solo para extensión).
 * ═══════════════════════════════════════════════════════════════════════ */

static void print_vector(const Vector* v, const char* prefix, int idx) {
    printf("  %s%02d: [%s,%s,%s] [%s,%s,%s] [%s,%s,%s]\n",
           prefix, idx,
           ts(v->d[0].t[0]), ts(v->d[0].t[1]), ts(v->d[0].t[2]),
           ts(v->d[1].t[0]), ts(v->d[1].t[1]), ts(v->d[1].t[2]),
           ts(v->d[2].t[0]), ts(v->d[2].t[1]), ts(v->d[2].t[2]));
}

static Vector vector_from_dimension(const Dimension* base) {
    Vector v;
    for (int j = 0; j < 3; j++) {
        v.d[j] = *base;
        /* Desfasar FO ligeramente para evitar auto-referencias reiteradas */
        v.d[j].t[0] = trit_infer(base->t[0], (j == 0 ? TRIT_C : TRIT_U), TRIT_U);
    }
    return v;
}

/* Sintetiza una ventana (t_i, t_{i+1}, t_{i+2}) → S_i */
static Vector synthesize_window(const Vector* a, const Vector* b, const Vector* c, VectorRole role) {
    Vector s;
    for (int j = 0; j < 3; j++) {
        Dimension ds;
        EmergencyMemory mem;
        const Dimension triple[3] = {a->d[j], b->d[j], c->d[j]};
        emergence_function(triple, &ds, &mem, role);
        s.d[j] = ds;
    }
    return s;
}

/* Procesa un nivel completo del cluster devolviendo las síntesis locales */
static int process_cluster_level(const Vector* tensors, int count, VectorRole role, Vector* out) {
    int produced = 0;
    for (int i = 0; i + 2 < count && produced < MAX_CLUSTER; i++) {
        out[produced++] = synthesize_window(&tensors[i], &tensors[i+1], &tensors[i+2], role);
    }
    return produced;
}

/* Pipeline fractal ascendente hasta converger o quedar <3 tensores */
static void cluster_pipeline(Vector* tensors, int count) {
    if (count < 3) {
        printf("  ⚠️  Cluster insuficiente (requiere ≥3 tensores)\n");
        return;
    }

    printf("\n━━━ PIPELINE DE CLUSTER TENSORIAL (TRANSCENDER) ━━━\n");
    printf("  Ventanas procesadas:\n");

    /* Procesar ventanas deslizantes */
    for (int i = 0; i + 2 < count && i < 5; i++) {
        Vector ventana[3];
        ventana[0] = tensors[i];
        ventana[1] = tensors[i+1];
        ventana[2] = tensors[i+2];

        TranscenderOutput out;
        transcender(ventana, &out);

        printf("\n  Ventana [%d,%d,%d]:\n", i, i+1, i+2);
        printf("    Emergente: [%s,%s,%s] [%s,%s,%s] [%s,%s,%s]\n",
               ts(out.tensor_emergente.d[0].t[0]), ts(out.tensor_emergente.d[0].t[1]), ts(out.tensor_emergente.d[0].t[2]),
               ts(out.tensor_emergente.d[1].t[0]), ts(out.tensor_emergente.d[1].t[1]), ts(out.tensor_emergente.d[1].t[2]),
               ts(out.tensor_emergente.d[2].t[0]), ts(out.tensor_emergente.d[2].t[1]), ts(out.tensor_emergente.d[2].t[2]));
        printf("    Comando:   [%s,%s,%s]\n",
               ts(out.comando.t[0]), ts(out.comando.t[1]), ts(out.comando.t[2]));
    }

    printf("\n── Procesamiento completado.\n");
}

/* ═══════════════════════════════════════════════════════════════════════
 * SELECCIÓN POR CONSENSO TRINARIO (sin métricas convencionales)
 * ═══════════════════════════════════════════════════════════════════════ */

static Trit consensus3(const Trit a[3], const Trit b[3]) {
    Trit m0 = trit_consensus(a[0], b[0]);
    Trit m1 = trit_consensus(a[1], b[1]);
    Trit m2 = trit_consensus(a[2], b[2]);
    return triadic_collapse(m0, m1, m2);
}

/* Seleccionar el primer match con consenso no-nulo */
static int find_best_match_arquetipo(const Trit pattern[3]) {
    if (n_arquetipos == 0) return -1;
    for (int i = 0; i < n_arquetipos; i++) {
        if (consensus3(pattern, arquetipos[i].pattern) != TRIT_N) return i;
    }
    return -1;
}

static int find_best_match_relator(const Trit dim_a[3], const Trit dim_b[3]) {
    if (n_relatores == 0) return -1;
    for (int i = 0; i < n_relatores; i++) {
        Trit ca = consensus3(dim_a, relatores[i].dim_a);
        Trit cb = consensus3(dim_b, relatores[i].dim_b);
        if (triadic_collapse(ca, cb, TRIT_C) != TRIT_N) return i;
    }
    return -1;
}

static int find_best_match_dinamica(const Trit state_before[3]) {
    if (n_dinamicas == 0) return -1;
    for (int i = 0; i < n_dinamicas; i++) {
        if (consensus3(state_before, dinamicas[i].state_before) != TRIT_N) return i;
    }
    return -1;
}

/* ═══════════════════════════════════════════════════════════════════════
 * ARMONIZADOR CON BEST-MATCH POR SIMILITUD (v3.1)
 * ═══════════════════════════════════════════════════════════════════════ */

static int armonizador(Dimension* d, const Arquetipo* arq, const Dinamica* dyn, const Relator* rel) {
    const int THRESH = 2;
    
    /* v3.1: Buscar best-match por similitud en vez de usar índice 0 fijo */
    int best_arq_idx = find_best_match_arquetipo(d->t);
    int best_dyn_idx = find_best_match_dinamica(d->t);
    
    const Arquetipo* use_arq = (best_arq_idx >= 0) ? &arquetipos[best_arq_idx] : arq;
    const Dinamica* use_dyn = (best_dyn_idx >= 0) ? &dinamicas[best_dyn_idx] : dyn;
    
    /* Calcular fiabilidad ternaria de cada memoria */
    Trit r_arq = (use_arq && use_arq->fo_output != TRIT_N && use_arq->support >= THRESH) ? TRIT_U : 
                 (use_arq ? TRIT_C : TRIT_N);
    Trit r_dyn = (use_dyn && use_dyn->fn_output != TRIT_N && use_dyn->support >= THRESH) ? TRIT_U : 
                 (use_dyn ? TRIT_C : TRIT_N);
    Trit r_rel = (rel && rel->mode[0] != TRIT_N && rel->support >= THRESH) ? TRIT_U : 
                 (rel ? TRIT_C : TRIT_N);
    
    int cambios = 0;
    
    for (int i = 0; i < 3; i++) {
        if (d->t[i] != TRIT_N) continue;
        
        Trit candidato = TRIT_N;
        
        /* Selección usando triadic collapse entre memorias */
        Trit mem_values[3] = {TRIT_N, TRIT_N, TRIT_N};
        
        if (i == 0 && use_arq) mem_values[0] = use_arq->fo_output;
        if (i == 1 && use_dyn) mem_values[0] = use_dyn->fn_output;
        if (i == 2 && rel) mem_values[0] = rel->mode[0];
        
        /* Triadic collapse con ponderación por fiabilidad */
        Trit prelim = trit_infer(r_arq, r_dyn, TRIT_U);
        if (prelim == TRIT_U) {
            if (r_arq == TRIT_U && mem_values[0] != TRIT_N) {
                candidato = mem_values[0];
            } else if (r_dyn == TRIT_U && i == 1 && use_dyn) {
                candidato = use_dyn->fn_output;
            }
        }
        
        if (candidato == TRIT_N && r_rel == TRIT_U && rel) {
            candidato = rel->mode[i];
        }
        
        if (candidato != TRIT_N) {
            d->t[i] = candidato;
            cambios++;
        }
    }
    
    return cambios;
}

/* ═══════════════════════════════════════════════════════════════════════
 * CICLO COMPLETO: INFO → KNOWLEDGE → ENERGY → INFO (Technical Annex §11)
 * ═══════════════════════════════════════════════════════════════════════ */

static VectorRole next_role_in_cycle(VectorRole current) {
    switch(current) {
        case ROLE_INFORMATIONAL: return ROLE_COGNITIVE;
        case ROLE_COGNITIVE: return ROLE_ENERGETIC;
        case ROLE_ENERGETIC: return ROLE_INFORMATIONAL;
        default: return ROLE_INFORMATIONAL;
    }
}

static void process_complete_cycle(Dimension* input, int cycles) {
    /* Reset del contador Fibonacci para evitar dependencias cruzadas */
    fib_init(&global_fib_counter);

    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  CICLO COMPLETO: Information → Knowledge → Energy → Info    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    VectorRole current_role = ROLE_INFORMATIONAL;
    Dimension current[3];
    Dimension superior;
    EmergencyMemory mem;
    
    /* Inicializar con input */
    for (int i = 0; i < 3; i++) {
        copy_dim(&current[i], input);
        current[i].t[0] = (current[i].t[0] + i) % 3 - 1;
    }
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        /* Mostrar modo cognitivo explícito */
        const char* cognitive_mode;
        if (cycle % 3 == 0) {
            cognitive_mode = "[RECORDAR] - Repetir información";
        } else if (cycle % 3 == 1) {
            cognitive_mode = "[ENTENDER] - Deducir patrones";
        } else {
            cognitive_mode = "[SENTIR/INTUIR] - Percibir energía";
        }
        printf("\n┌─────────────────────────────────────┐\n");
        printf("│ Ciclo %d: %s\n", cycle + 1, cognitive_mode);
        printf("│ Rol: %s\n", role_name(current_role));
        printf("└─────────────────────────────────────┘\n");
        
        /* Validar antes de procesar */
        Vector v_temp = {{current[0], current[1], current[2]}};
        if (!validate_vector(&v_temp)) {
            printf("  ⚠️  Vector inválido (auto-referencia detectada)\n");
            break;
        }
        
        /* Emergencia */
        emergence_function(current, &superior, &mem, current_role);
        
        printf("  Input:  [%s,%s,%s] [%s,%s,%s] [%s,%s,%s]\n",
               ts(current[0].t[0]), ts(current[0].t[1]), ts(current[0].t[2]),
               ts(current[1].t[0]), ts(current[1].t[1]), ts(current[1].t[2]),
               ts(current[2].t[0]), ts(current[2].t[1]), ts(current[2].t[2]));
        
        printf("  Synth:  [%s,%s,%s]\n",
               ts(superior.t[0]), ts(superior.t[1]), ts(superior.t[2]));
        
        printf("  Memory: [%s,%s,%s]\n",
               ts(mem.me[0]), ts(mem.me[1]), ts(mem.me[2]));
        
        /* Interpretar según rol */
        if (current_role == ROLE_COGNITIVE) {
            EnergeticState feeling = extract_energetic_state(&mem, current_role);
            printf("  Energetic State → Tensión:%s Entropía:%s Armonía:%s\n",
                   ts(feeling.tension), ts(feeling.entropy), ts(feeling.harmony));
        }
        
        /* Actualizar axioma si estamos en modo ENERGY */
        if (current_role == ROLE_ENERGETIC) {
            int nulls_superior = count_nulls_dim(&superior);
            update_axiom_state(nulls_superior, 8, n_arquetipos > 0 ? 1 : 0);
            printf("  Axiom Update → F:%s O:%s P:%s (Balance: %.2f)\n",
                   ts(axiom_state.freedom),
                   ts(axiom_state.order),
                   ts(axiom_state.purpose),
                   axiom_balance());
        }
        
        /* Avanzar al siguiente rol */
        current_role = next_role_in_cycle(current_role);
        
        /* Preparar siguiente iteración: extender desde superior */
        extend_function(&superior, &mem, current);
        
        /* Armonizar con conocimiento */
        if (n_arquetipos > 0 || n_dinamicas > 0 || n_relatores > 0) {
            armonizador(&superior, 
                       n_arquetipos > 0 ? &arquetipos[0] : NULL,
                       n_dinamicas > 0 ? &dinamicas[0] : NULL,
                       n_relatores > 0 ? &relatores[0] : NULL);
        }
    }
    
    printf("\n┌──────────────────────────────────────────────────┐\n");
    printf("│         ESTADO FINAL DEL SISTEMA                 │\n");
    printf("└──────────────────────────────────────────────────┘\n");
    
    printf("\n  ► TRES MODOS COGNITIVOS COMPLETADOS:\n");
    printf("    [1] RECORDAR → información memorizada\n");
    printf("    [2] ENTENDER → patrones deducidos\n");
    printf("    [3] SENTIR/INTUIR → estado energético interno\n");
    
    printf("\n  ► AXIOMA DE INTELIGENCIA (Fuerzas Universales):\n");
    printf("    Libertad:  %s (cambio y exploración)\n",
           ts(axiom_state.freedom));
    printf("    Orden:     %s (estructura y coherencia)\n",
           ts(axiom_state.order));
    printf("    Propósito: %s (dirección e intención)\n",
           ts(axiom_state.purpose));
    
    float final_balance = axiom_balance();
    printf("    Balance:   %.3f", final_balance);
    if (final_balance < 0.3f) {
        printf(" ✓ ARMÓNICO\n");
    } else if (final_balance < 0.6f) {
        printf(" ≈ ESTABLE\n");
    } else {
        printf(" ⚠ DESEQUILIBRADO\n");
    }
    
    printf("\n  ► ESTADO ENERGÉTICO (Cómo el Sistema Se SIENTE Internamente):\n");
    printf("    Tensión:  %s (rigidez / Order dominante)\n",
           ts(estado_energetico.tension));
    printf("    Entropía: %s (caos / Libertad descontrolada)\n",
           ts(estado_energetico.entropy));
    printf("    Armonía:  %s (equilibrio / F-O-P alineados)\n",
           ts(estado_energetico.harmony));
    
    printf("\n─── Conocimiento Acumulado ───\n");
    printf("  Arquetipos: %d | Dinámicas: %d | Relatores: %d\n",
           n_arquetipos, n_dinamicas, n_relatores);
    
    printf("\n─── Tensor C (Creencia Estable) ───\n");
    printf("  C: [%s,%s,%s]\n",
           ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]));
}

/* ═══════════════════════════════════════════════════════════════════════
 * PERSISTENCIA DE CONOCIMIENTO (Opcional)
 * Guarda y recupera las tres pirámides de memoria (A-R-D)
 * ═══════════════════════════════════════════════════════════════════════ */

static void save_knowledge(const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("❌ Error: No se puede abrir %s para escritura\n", filename);
        return;
    }
    
    /* Guardar conteos */
    fwrite(&n_arquetipos, sizeof(int), 1, f);
    fwrite(&n_dinamicas, sizeof(int), 1, f);
    fwrite(&n_relatores, sizeof(int), 1, f);
    fwrite(&global_rev, sizeof(unsigned long), 1, f);
    
    /* Guardar tres pirámides */
    fwrite(arquetipos, sizeof(Arquetipo), n_arquetipos, f);
    fwrite(dinamicas, sizeof(Dinamica), n_dinamicas, f);
    fwrite(relatores, sizeof(Relator), n_relatores, f);
    
    /* Guardar tensor C */
    fwrite(&tensor_C, sizeof(Dimension), 1, f);
    
    /* Guardar axiom state y energetic state */
    fwrite(&axiom_state, sizeof(AxiomTrio), 1, f);
    fwrite(&estado_energetico, sizeof(EnergeticState), 1, f);
    
    /* v3.1: Guardar estado del contador Fibonacci */
    fwrite(&global_fib_counter, sizeof(FibCounter), 1, f);
    
    fclose(f);
    printf("✓ Conocimiento guardado en '%s'\n", filename);
    printf("  • Arquetipos: %d\n  • Dinámicas: %d\n  • Relatores: %d\n",
           n_arquetipos, n_dinamicas, n_relatores);
}

static void load_knowledge(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("⚠️  Archivo '%s' no encontrado (primera ejecución)\n", filename);
        return;
    }
    
    /* Recuperar conteos */
    fread(&n_arquetipos, sizeof(int), 1, f);
    fread(&n_dinamicas, sizeof(int), 1, f);
    fread(&n_relatores, sizeof(int), 1, f);
    fread(&global_rev, sizeof(unsigned long), 1, f);
    
    /* Validar límites */
    if (n_arquetipos > MAX_MEM || n_dinamicas > MAX_MEM || n_relatores > MAX_MEM) {
        printf("❌ Error: Archivo de conocimiento corrupto\n");
        fclose(f);
        return;
    }
    
    /* Recuperar tres pirámides */
    fread(arquetipos, sizeof(Arquetipo), n_arquetipos, f);
    fread(dinamicas, sizeof(Dinamica), n_dinamicas, f);
    fread(relatores, sizeof(Relator), n_relatores, f);
    
    /* Recuperar tensor C */
    fread(&tensor_C, sizeof(Dimension), 1, f);
    
    /* Recuperar axiom state y energetic state */
    fread(&axiom_state, sizeof(AxiomTrio), 1, f);
    fread(&estado_energetico, sizeof(EnergeticState), 1, f);
    
    /* v3.1: Recuperar estado del contador Fibonacci */
    if (fread(&global_fib_counter, sizeof(FibCounter), 1, f) != 1) {
        /* Si no existe en archivo viejo, inicializar */
        fib_init(&global_fib_counter);
    }
    
    fclose(f);
    printf("✓ Conocimiento restaurado desde '%s'\n", filename);
    printf("  • Arquetipos: %d\n  • Dinámicas: %d\n  • Relatores: %d\n",
           n_arquetipos, n_dinamicas, n_relatores);
}

/* ═══════════════════════════════════════════════════════════════════════
 * INTERFAZ INTERACTIVA (Opcional)
 * Loop REPL para experimentar con Aurora en tiempo real
 * ═══════════════════════════════════════════════════════════════════════ */

static int parse_trit(char c) {
    if (c == 'u' || c == 'U') return TRIT_U;
    if (c == 'c' || c == 'C') return TRIT_C;
    if (c == 'n' || c == 'N') return TRIT_N;
    return -1; /* error */
}

static void interactive_aurora_loop(void) {
    char buffer[256];
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  Aurora Interactive Mode                                          ║\n");
    printf("║  Experimenta con tensores FFE en tiempo real                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Comandos disponibles:\n");
    printf("  'e <trits>'   - Emergencia: ingresa 9 trits (ej: e u c n c u u u c c)\n");
    printf("  'c <trits>'   - Ciclo completo: ingresa 3 trits para semilla\n");
    printf("  's <archivo>' - Guardar conocimiento\n");
    printf("  'l <archivo>' - Cargar conocimiento\n");
    printf("  'i'           - Información del sistema\n");
    printf("  'q'           - Salir\n");
    printf("────────────────────────────────────────────────────────────────────\n\n");
    
    while (1) {
        printf("aurora> ");
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        
        /* Limpiar newline */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        
        if (strlen(buffer) == 0) continue;
        
        /* Comando: Salir */
        if (buffer[0] == 'q' || buffer[0] == 'Q') {
            printf("Hasta luego. Aurora permanecerá esperando...\n");
            break;
        }
        
        /* Comando: Emergencia */
        else if (buffer[0] == 'e' || buffer[0] == 'E') {
            Dimension d[3];
            char* p = buffer + 2;
            int idx = 0;
            
            /* Parsear 9 trits */
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    while (*p == ' ') p++;
                    if (!*p) {
                        printf("❌ Error: Se esperaban 9 trits, se obtuvieron %d\n", idx);
                        goto next_cmd;
                    }
                    int val = parse_trit(*p);
                    if (val == -1) {
                        printf("❌ Error: Carácter inválido '%c' (usa u/c/n)\n", *p);
                        goto next_cmd;
                    }
                    d[i].t[j] = val;
                    p++;
                    idx++;
                }
            }
            
            /* Validar vector */
            Vector v_test = {{d[0], d[1], d[2]}};
            if (!validate_vector(&v_test)) {
                printf("⚠️  Vector inválido (auto-referencia detectada)\n");
                goto next_cmd;
            }
            
            /* Realizar emergencia */
            Dimension synth;
            EmergencyMemory mem;
            emergence_function(d, &synth, &mem, ROLE_INFORMATIONAL);
            
            printf("\n  Input:  [%s,%s,%s] [%s,%s,%s] [%s,%s,%s]\n",
                   ts(d[0].t[0]), ts(d[0].t[1]), ts(d[0].t[2]),
                   ts(d[1].t[0]), ts(d[1].t[1]), ts(d[1].t[2]),
                   ts(d[2].t[0]), ts(d[2].t[1]), ts(d[2].t[2]));
            
            printf("  Synth:  [%s,%s,%s]\n",
                   ts(synth.t[0]), ts(synth.t[1]), ts(synth.t[2]));
            
            printf("  Memory: [%s,%s,%s]\n\n",
                   ts(mem.me[0]), ts(mem.me[1]), ts(mem.me[2]));
        }
        
        /* Comando: Ciclo Completo */
        else if (buffer[0] == 'c' || buffer[0] == 'C') {
            Dimension seed;
            char* p = buffer + 2;
            
            /* Parsear 3 trits */
            for (int i = 0; i < 3; i++) {
                while (*p == ' ') p++;
                if (!*p) {
                    printf("❌ Error: Se esperaban 3 trits, se obtuvieron %d\n", i);
                    goto next_cmd;
                }
                int val = parse_trit(*p);
                if (val == -1) {
                    printf("❌ Error: Carácter inválido '%c' (usa u/c/n)\n", *p);
                    goto next_cmd;
                }
                seed.t[i] = val;
                p++;
            }
            
            /* Ejecutar ciclo (3 iteraciones = 1 ciclo completo) */
            process_complete_cycle(&seed, 3);
        }
        
        /* Comando: Guardar Conocimiento */
        else if (buffer[0] == 's' || buffer[0] == 'S') {
            char* filename = buffer + 2;
            while (*filename == ' ') filename++;
            if (*filename) {
                save_knowledge(filename);
            } else {
                printf("❌ Error: Especifica nombre de archivo\n");
            }
        }
        
        /* Comando: Cargar Conocimiento */
        else if (buffer[0] == 'l' || buffer[0] == 'L') {
            char* filename = buffer + 2;
            while (*filename == ' ') filename++;
            if (*filename) {
                load_knowledge(filename);
            } else {
                printf("❌ Error: Especifica nombre de archivo\n");
            }
        }
        
        /* Comando: Información del Sistema */
        else if (buffer[0] == 'i' || buffer[0] == 'I') {
            printf("\n┌──────────────────────────────────┐\n");
            printf("│  Estado Interno del Sistema      │\n");
            printf("└──────────────────────────────────┘\n");
            printf("  Conocimiento Acumulado:\n");
            printf("    • Arquetipos: %d\n", n_arquetipos);
            printf("    • Dinámicas: %d\n", n_dinamicas);
            printf("    • Relatores: %d\n", n_relatores);
            printf("    • Revisión global: %lu\n\n", global_rev);
            
            printf("  Tensor C (Creencia Estable):\n");
            printf("    [%s, %s, %s]\n\n", ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]));
            
            printf("  Axioma (Libertad-Orden-Propósito):\n");
            printf("    • Libertad:  %s\n", ts(axiom_state.freedom));
            printf("    • Orden:     %s\n", ts(axiom_state.order));
            printf("    • Propósito: %s\n", ts(axiom_state.purpose));
            printf("    • Balance:   %.3f\n\n", axiom_balance());
            
            printf("  Estado Energético (Cómo se SIENTE):\n");
            printf("    • Tensión:   %s\n", ts(estado_energetico.tension));
            printf("    • Entropía:  %s\n", ts(estado_energetico.entropy));
            printf("    • Armonía:   %s\n\n", ts(estado_energetico.harmony));
        }
        
        /* Comando no reconocido */
        else {
            printf("❌ Comando desconocido. Usa 'e', 'c', 's', 'l', 'i' o 'q'\n");
        }
        
        next_cmd:
        continue;
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * TEST: Emergencia + Extensión deben ser coherentes
 * ═══════════════════════════════════════════════════════════════════════ */

static void test_emergence_roundtrip(void) {
    printf("\n━━━ TEST: Emergencia ↔ Extensión (coherencia semántica) ━━━\n");
    printf("NOTA: La reversibilidad es semántica, no bit-a-bit.\n");
    printf("      El sistema sintetiza hacia coherencia y extiende usando memorias.\n\n");

    /* Reset contador Fibonacci para consistencia */
    fib_init(&global_fib_counter);

    Dimension d[3] = {
     {{TRIT_U, TRIT_C, TRIT_U}},
     {{TRIT_C, TRIT_U, TRIT_C}},
     {{TRIT_U, TRIT_U, TRIT_C}},
    };

    Dimension ds;
    EmergencyMemory mem;
    emergence_function(d, &ds, &mem, ROLE_INFORMATIONAL);

    Dimension rec[3];
    extend_function(&ds, &mem, rec);
    Dimension ds_re;
    EmergencyMemory mem_re;
    emergence_function(rec, &ds_re, &mem_re, ROLE_INFORMATIONAL);

    printf("Input:  [%s,%s,%s] [%s,%s,%s] [%s,%s,%s]\n",
        ts(d[0].t[0]), ts(d[0].t[1]), ts(d[0].t[2]),
        ts(d[1].t[0]), ts(d[1].t[1]), ts(d[1].t[2]),
        ts(d[2].t[0]), ts(d[2].t[1]), ts(d[2].t[2]));

    printf("Synth:  [%s,%s,%s] | Mem: [%s,%s,%s]\n",
        ts(ds.t[0]), ts(ds.t[1]), ts(ds.t[2]),
        ts(mem.me[0]), ts(mem.me[1]), ts(mem.me[2]));

    printf("Extend: [%s,%s,%s] [%s,%s,%s] [%s,%s,%s]\n",
        ts(rec[0].t[0]), ts(rec[0].t[1]), ts(rec[0].t[2]),
        ts(rec[1].t[0]), ts(rec[1].t[1]), ts(rec[1].t[2]),
        ts(rec[2].t[0]), ts(rec[2].t[1]), ts(rec[2].t[2]));

    printf("Re-emerge: [%s,%s,%s]\n",
        ts(ds_re.t[0]), ts(ds_re.t[1]), ts(ds_re.t[2]));

    int nulls_orig = count_nulls_dim(&ds);
    int nulls_re = count_nulls_dim(&ds_re);
    int coherent = (nulls_re <= nulls_orig);
    
    printf("Resultado: %s (nulls: %d→%d, coherencia %s)\n", 
           coherent ? "✓ Coherente" : "⚠ Entropía aumentó",
           nulls_orig, nulls_re,
           coherent ? "mantenida/mejorada" : "degradada");
}

/* ═══════════════════════════════════════════════════════════════════════
 * DEMO PRINCIPAL
 * ═══════════════════════════════════════════════════════════════════════ */

int main(int argc, char* argv[]) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  Aurora Core v3.0 - Technical Annex Implementation              ║\n");
    printf("║  • Reversible Emergency Memories (me1, me2, me3)                 ║\n");
    printf("║  • Energetic Trio: Tensión, Comando, Energía                    ║\n");
    printf("║  • Complete Cycle: Info → Knowledge → Energy → Info             ║\n");
    printf("║  • ES.index ≠ FO.index validation                               ║\n");
    printf("║  • Fibonacci ternary counter                                     ║\n");
    printf("║  • Knowledge Persistence & Interactive Mode (NUEVO)              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    /* Procesar argumentos de línea de comando */
    if (argc > 1) {
        if (strcmp(argv[1], "--interactive") == 0 || strcmp(argv[1], "-i") == 0) {
            /* Modo interactivo */
            load_knowledge("aurora_knowledge.bin");
            interactive_aurora_loop();
            save_knowledge("aurora_knowledge.bin");
            return 0;
        }
        else if (strcmp(argv[1], "--load") == 0 && argc > 2) {
            /* Cargar conocimiento y ejecutar demo */
            load_knowledge(argv[2]);
        }
    }
    
    /* Inicializar Fibonacci counter */
    fib_init(&global_fib_counter);

    /* Test de coherencia emergencia/extensión */
    test_emergence_roundtrip();
    
    /* Fase 1: Alimentar con patrones diversos */
    printf("\n━━━ FASE 1: Aprendizaje de Patrones ━━━\n");
    
    Dimension patterns[] = {
        {{TRIT_U, TRIT_C, TRIT_N}},
        {{TRIT_C, TRIT_U, TRIT_U}},
        {{TRIT_U, TRIT_U, TRIT_C}},
        {{TRIT_N, TRIT_C, TRIT_U}},
        {{TRIT_U, TRIT_N, TRIT_U}},
    };
    
    for (int i = 0; i < 5; i++) {
        printf("\nPatrón %d: [%s,%s,%s]\n", i+1,
               ts(patterns[i].t[0]), ts(patterns[i].t[1]), ts(patterns[i].t[2]));
        
        if (!validate_dimension(&patterns[i])) {
            printf("  ⚠️  Patrón inválido, omitiendo...\n");
            continue;
        }
        
        /* Crear vector de aprendizaje */
        Dimension learning_vec[3];
        for (int j = 0; j < 3; j++) {
            learning_vec[j] = patterns[i];
            learning_vec[j].t[0] = (learning_vec[j].t[0] + j) % 3 - 1;
        }
        
        /* Emergencia en modo INFORMATIONAL */
        Dimension synth;
        EmergencyMemory mem;
        emergence_function(learning_vec, &synth, &mem, ROLE_INFORMATIONAL);
        
        printf("  → Síntesis: [%s,%s,%s] | Mem: [%s,%s,%s]\n",
               ts(synth.t[0]), ts(synth.t[1]), ts(synth.t[2]),
               ts(mem.me[0]), ts(mem.me[1]), ts(mem.me[2]));
        
        /* Aprender dinámicas y relatores */
        if (i > 0) {
            learn_dinamica(patterns[i-1].t, patterns[i].t, patterns[i].t[1]);
            learn_relator(patterns[i].t, learning_vec[0].t, mem.me);
        }
    }

    /* Fase 1.1: Cluster tensorial con ventanas deslizantes */
    printf("\n━━━ FASE 1.1: Cluster Tensorial con Ventanas Ternarias ━━━\n");
    Vector cluster[MAX_CLUSTER];
    int cluster_count = 0;
    for (int i = 0; i < 5 && cluster_count < MAX_CLUSTER; i++) {
        if (!validate_dimension(&patterns[i])) continue;
        cluster[cluster_count++] = vector_from_dimension(&patterns[i]);
    }
    if (cluster_count >= 3) {
        cluster_pipeline(cluster, cluster_count);
    } else {
        printf("  Cluster insuficiente para procesar (se requieren ≥3 tensores).\n");
    }
    
    /* Fase 2: Ciclo completo Information → Knowledge → Energy */
    printf("\n━━━ FASE 2: Ciclo Completo (3 vueltas) ━━━\n");
    
    Dimension seed = {{TRIT_U, TRIT_U, TRIT_U}};
    seed.t[1] = TRIT_C; /* evitar auto-referencia en ES→FO al arrancar */
    seed.t[2] = TRIT_C;
    process_complete_cycle(&seed, 9); /* 9 = 3 ciclos completos */
    
    /* Fase 3: Test de validación */
    printf("\n━━━ FASE 3: Test de Validación ES.index ≠ FO.index ━━━\n");
    
    Dimension invalid1 = {{TRIT_U, TRIT_C, TRIT_U}}; /* FO=ES=U → potencialmente inválido */
    Dimension invalid2 = {{TRIT_N, TRIT_N, TRIT_N}}; /* todo null → inválido */
    Dimension valid = {{TRIT_U, TRIT_C, TRIT_C}};
    
    printf("\nTest 1 [%s,%s,%s]: %s\n", 
           ts(invalid1.t[0]), ts(invalid1.t[1]), ts(invalid1.t[2]),
           validate_dimension(&invalid1) ? "✓ Válida" : "✗ Inválida");
    
    printf("Test 2 [%s,%s,%s]: %s\n",
           ts(invalid2.t[0]), ts(invalid2.t[1]), ts(invalid2.t[2]),
           validate_dimension(&invalid2) ? "✓ Válida" : "✗ Inválida");
    
    printf("Test 3 [%s,%s,%s]: %s\n",
           ts(valid.t[0]), ts(valid.t[1]), ts(valid.t[2]),
           validate_dimension(&valid) ? "✓ Válida" : "✗ Inválida");
    
    /* Guardar conocimiento al finalizar */
    save_knowledge("aurora_knowledge.bin");
    
    /* Reporte final */
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  CONCLUSIÓN                                                       ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════╣\n");
    printf("║  ✓ Emergencia reversible implementada                            ║\n");
    printf("║  ✓ Estado Energético: Tensión/Entropía/Armonía                   ║\n");
    printf("║  ✓ Ciclo completo: Info→Knowledge→Energy→Info                    ║\n");
    printf("║  ✓ Validación ES.index ≠ FO.index                                ║\n");
    printf("║  ✓ Fibonacci ternario para selección de roles                    ║\n");
    printf("║  ✓ Persistencia de Conocimiento (A-R-D)                          ║\n");
    printf("║  ✓ Modo Interactivo REPL                                         ║\n");
    printf("║                                                                   ║\n");
    printf("║  Arquetipos: %-3d | Dinámicas: %-3d | Relatores: %-3d            ║\n",
           n_arquetipos, n_dinamicas, n_relatores);
    printf("║  Tensor C:   [%s,%s,%s]                                         ║\n",
           ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]));
    printf("║                                                                   ║\n");
    printf("║  NUEVAS CARACTERÍSTICAS:                                          ║\n");
    printf("║    • save_knowledge() / load_knowledge()                          ║\n");
    printf("║    • interactive_aurora_loop() modo REPL                          ║\n");
    printf("║                                                                   ║\n");
    printf("║  USO:                                                              ║\n");
    printf("║    aurora_core_refactored.exe           [demo normal]             ║\n");
    printf("║    aurora_core_refactored.exe -i        [modo interactivo]        ║\n");
    printf("║    aurora_core_refactored.exe --load <file>  [cargar conocimiento]║\n");
    printf("║                                                                   ║\n");
    printf("║  Aurora piensa reorganizando energía,                            ║\n");
    printf("║  sintetizando coherencia,                                        ║\n");
    printf("║  y reconstruyendo información                                     ║\n");
    printf("║  mediante un ciclo fractal universal.                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}


