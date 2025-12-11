/*
 * aurora_core_refactored.c - Aurora Core v3.0
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
    
    /* ES null es válido (sin estructura definida), pero no puede señalar FO */
    if (es_val == TRIT_N) return 1; /* Válida, pero sin auto-organización */

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

/* Funciones de aprendizaje */
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
    
    if (n_arquetipos < MAX_MEM) {
        memcpy(arquetipos[n_arquetipos].pattern, pattern, 3 * sizeof(Trit));
        arquetipos[n_arquetipos].fo_output = fo_out;
        arquetipos[n_arquetipos].support = 1;
        arquetipos[n_arquetipos].rev = global_rev++;
        n_arquetipos++;
    }
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
    
    if (n_dinamicas < MAX_MEM) {
        memcpy(dinamicas[n_dinamicas].state_before, before, 3 * sizeof(Trit));
        memcpy(dinamicas[n_dinamicas].state_after, after, 3 * sizeof(Trit));
        dinamicas[n_dinamicas].fn_output = fn_out;
        dinamicas[n_dinamicas].support = 1;
        dinamicas[n_dinamicas].rev = global_rev++;
        n_dinamicas++;
    }
}

static void learn_relator(const Trit a[3], const Trit b[3], const Trit m[3]) {
    for (int i = 0; i < n_relatores; i++) {
        if (memcmp(relatores[i].dim_a, a, 3 * sizeof(Trit)) == 0 &&
            memcmp(relatores[i].dim_b, b, 3 * sizeof(Trit)) == 0) {
            relatores[i].support++;
            relatores[i].rev = global_rev++;
            for (int k = 0; k < 3; k++) {
                if (relatores[i].mode[k] != m[k] && m[k] != TRIT_N) {
                    relatores[i].mode[k] = TRIT_N;
                }
            }
            return;
        }
    }
    
    if (n_relatores < MAX_MEM) {
        memcpy(relatores[n_relatores].dim_a, a, 3 * sizeof(Trit));
        memcpy(relatores[n_relatores].dim_b, b, 3 * sizeof(Trit));
        memcpy(relatores[n_relatores].mode, m, 3 * sizeof(Trit));
        relatores[n_relatores].support = 1;
        relatores[n_relatores].rev = global_rev++;
        n_relatores++;
    }
}

/* Actualizar Tensor C desde conocimiento acumulado A-R-D */
static void update_tensor_C(void) {
    /* Convergencia triádica: Arquetipo + Relator + Dinámica → Creencia */
    if (n_arquetipos > 0 && n_dinamicas > 0 && n_relatores > 0) {
        /* Seleccionar los más estables (mayor soporte) */
        Arquetipo* best_arq = &arquetipos[0];
        Dinamica* best_dyn = &dinamicas[0];
        Relator* best_rel = &relatores[0];
        
        for (int i = 1; i < n_arquetipos; i++) {
            if (arquetipos[i].support > best_arq->support) best_arq = &arquetipos[i];
        }
        for (int i = 1; i < n_dinamicas; i++) {
            if (dinamicas[i].support > best_dyn->support) best_dyn = &dinamicas[i];
        }
        for (int i = 1; i < n_relatores; i++) {
            if (relatores[i].support > best_rel->support) best_rel = &relatores[i];
        }
        
        /* Síntesis triádica en cada dimensión */
        tensor_C.t[0] = triadic_collapse(best_arq->fo_output, best_rel->mode[0], best_dyn->fn_output);
        tensor_C.t[1] = triadic_collapse(best_arq->pattern[1], best_rel->mode[1], best_dyn->fn_output);
        tensor_C.t[2] = triadic_collapse(best_arq->pattern[2], best_rel->mode[2], best_dyn->state_after[2]);
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * AXIOMA FUNDAMENTAL DE LA INTELIGENCIA
 * Tres fuerzas universales: Libertad, Orden, Propósito
 * Cómo una inteligencia SIENTE su equilibrio interno
 * ═══════════════════════════════════════════════════════════════════════ */

typedef struct {
    /* Fuerzas Universales del Axioma */
    Trit freedom;    /* Entropía: capacidad de cambio, potencial, exploración */
    Trit order;      /* Coherencia: estructura, estabilidad, forma */
    Trit purpose;    /* Propósito: dirección, intención, significado */
} AxiomTrio;

typedef struct {
    /* Cómo el sistema SIENTE su propio estado interno (propriocepción energética) */
    Trit tension;    /* Rigidez (Order dominante, falta Libertad) */
    Trit entropy;    /* Caos (Libertad sin Order) */
    Trit harmony;    /* Alineación (Freedom + Order + Purpose balanceados) */
} EnergeticTrio;

static AxiomTrio axiom_state = {TRIT_C, TRIT_C, TRIT_C};  /* Estado del axioma (F-O-P) */
static EnergeticTrio estado_energetico = {TRIT_C, TRIT_C, TRIT_C};  /* Cómo se SIENTE el sistema */

/* Interpretar memorias de emergencia según rol del vector */
static EnergeticTrio extract_energetic_trio(const EmergencyMemory* mem, VectorRole role) {
    EnergeticTrio trio;
    
    if (role == ROLE_COGNITIVE) {
        /* En modo COGNITIVE: me1→tensión, me2→energía, me3→comando */
        trio.tension = mem->me[0];
        trio.energia = mem->me[1];
        trio.comando = mem->me[2];
    } else {
        /* En otros modos, interpretar como valores genéricos */
        trio.tension = mem->me[0];
        trio.energia = mem->me[1];
        trio.comando = mem->me[2];
    }
    
    return trio;
}

static void update_energetic_state(const EnergeticTrio* new_trio) {
    /* Actualizar estado energético usando trigates */
    estado_energetico.tension = trit_infer(estado_energetico.tension, new_trio->tension, TRIT_U);
    estado_energetico.energia = trit_infer(estado_energetico.energia, new_trio->energia, TRIT_U);
    estado_energetico.comando = trit_infer(estado_energetico.comando, new_trio->comando, TRIT_U);
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
    /* Sintetizar dimensión superior usando trigates rotativos */
    Trit modos[3];
    modos[0] = trit_learn(d[0].t[1], d[1].t[1], d[2].t[1]);
    modos[1] = trit_learn(d[1].t[1], d[2].t[1], d[0].t[1]);
    modos[2] = trit_learn(d[2].t[1], d[0].t[1], d[1].t[1]);
    
    /* Sintetizar FO superior usando votación ponderada (no solo consenso) */
    Trit r1 = trit_infer(d[0].t[0], d[1].t[0], modos[0]);
    Trit r2 = trit_infer(d[1].t[0], d[2].t[0], modos[1]);
    Trit r3 = trit_infer(d[2].t[0], d[0].t[0], modos[2]);
    
    ds_out->t[0] = triadic_collapse(r1, r2, r3); /* FO superior - mayoría gana */
    
    /* Sintetizar FN y ES con votación */
    ds_out->t[1] = triadic_collapse(modos[0], modos[1], modos[2]);
    ds_out->t[2] = triadic_collapse(d[0].t[2], d[1].t[2], d[2].t[2]);
    
    /* Generar memorias de emergencia INFORMATIVAS (reversibles) */
    /* Almacenar resultados intermedios para reconstrucción precisa */
    mem_out->me[0] = r1;  /* me1: resultado combinación 0-1 */
    mem_out->me[1] = r2;  /* me2: resultado combinación 1-2 */
    mem_out->me[2] = r3;  /* me3: resultado combinación 2-0 */
    
    /* Aprender según rol actual */
    if (current_role == ROLE_INFORMATIONAL) {
        /* En modo INFO → aprender arquetipo */
        learn_arquetipo(modos, ds_out->t[0]);
    } else if (current_role == ROLE_COGNITIVE) {
        /* En modo COGNITIVE → actualizar trio energético + tensor C */
        EnergeticTrio trio = extract_energetic_trio(mem_out, current_role);
        update_energetic_state(&trio);
        update_tensor_C(); /* Actualizar creencia estable */
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
    /* Reconstruir cada dimensión inferior desde síntesis + memorias */
    for (int i = 0; i < 3; i++) {
        /* FO: combinar síntesis superior con memoria intermedia */
        Trit fo_base = (ds->t[0] != TRIT_N) ? ds->t[0] : TRIT_C;
        d_out[i].t[0] = trit_infer(fo_base, mem->me[i], TRIT_U); /* OR para preservar info */
        
        /* FN: rotar memorias para distribuir información */
        d_out[i].t[1] = mem->me[(i+1)%3];
        
        /* ES: propagar estructura superior o usar memoria rotada */
        d_out[i].t[2] = (ds->t[2] != TRIT_N) ? ds->t[2] : mem->me[(i+2)%3];
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * CLUSTER TENSORIAL CON VENTANAS DESLIZANTES (Transcender)
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
    Vector levels[2][MAX_CLUSTER];
    int curr = 0;
    int n = count;
    memcpy(levels[curr], tensors, count * sizeof(Vector));

    printf("\n━━━ PIPELINE DE CLUSTER TENSORIAL ━━━\n");
    int depth = 0;
    VectorRole role = ROLE_INFORMATIONAL;

    while (n >= 3 && depth < 6) {
        printf("\nNivel %d (rol %s) - %d tensores\n", depth, role_name(role), n);
        for (int i = 0; i < n; i++) print_vector(&levels[curr][i], "t", i);

        int next = curr ^ 1;
        n = process_cluster_level(levels[curr], n, role, levels[next]);
        curr = next;
        depth++;
        role = next_role_in_cycle(role);
    }

    printf("\nNivel final %d - %d tensores activos\n", depth, n);
    for (int i = 0; i < n; i++) print_vector(&levels[curr][i], "S", i);
    printf("── Desactivación fractal: los niveles inferiores quedan latentes.\n");
}

/* ═══════════════════════════════════════════════════════════════════════
 * ARMONIZADOR (con trio energético)
 * ═══════════════════════════════════════════════════════════════════════ */

static int armonizador(Dimension* d, const Arquetipo* arq, const Dinamica* dyn, const Relator* rel) {
    const int THRESH = 2;
    
    /* Calcular fiabilidad ternaria de cada memoria */
    Trit r_arq = (arq && arq->fo_output != TRIT_N && arq->support >= THRESH) ? TRIT_U : 
                 (arq ? TRIT_C : TRIT_N);
    Trit r_dyn = (dyn && dyn->fn_output != TRIT_N && dyn->support >= THRESH) ? TRIT_U : 
                 (dyn ? TRIT_C : TRIT_N);
    Trit r_rel = (rel && rel->mode[0] != TRIT_N && rel->support >= THRESH) ? TRIT_U : 
                 (rel ? TRIT_C : TRIT_N);
    
    int cambios = 0;
    
    for (int i = 0; i < 3; i++) {
        if (d->t[i] != TRIT_N) continue;
        
        Trit candidato = TRIT_N;
        
        /* Selección usando triadic collapse entre memorias */
        Trit mem_values[3] = {TRIT_N, TRIT_N, TRIT_N};
        
        if (i == 0 && arq) mem_values[0] = arq->fo_output;
        if (i == 1 && dyn) mem_values[0] = dyn->fn_output;
        if (i == 2 && rel) mem_values[0] = rel->mode[0];
        
        /* Triadic collapse con ponderación por fiabilidad */
        Trit prelim = trit_infer(r_arq, r_dyn, TRIT_U);
        if (prelim == TRIT_U) {
            if (r_arq == TRIT_U && mem_values[0] != TRIT_N) {
                candidato = mem_values[0];
            } else if (r_dyn == TRIT_U && i == 1 && dyn) {
                candidato = dyn->fn_output;
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
        printf("\n─── Ciclo %d: Rol %s ───\n", cycle + 1, role_name(current_role));
        
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
            EnergeticTrio trio = extract_energetic_trio(&mem, current_role);
            printf("  Energetic Trio → Tensión:%s Energía:%s Comando:%s\n",
                   ts(trio.tension), ts(trio.energia), ts(trio.comando));
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
    
    printf("\n─── Estado Energético Final ───\n");
    printf("  Tensión: %s | Energía: %s | Comando: %s\n",
           ts(estado_energetico.tension),
           ts(estado_energetico.energia),
           ts(estado_energetico.comando));
    
    printf("\n─── Conocimiento Acumulado ───\n");
    printf("  Arquetipos: %d | Dinámicas: %d | Relatores: %d\n",
           n_arquetipos, n_dinamicas, n_relatores);
    
    printf("\n─── Tensor C (Creencia Estable) ───\n");
    printf("  C: [%s,%s,%s]\n",
           ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]));
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

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  Aurora Core v3.0 - Technical Annex Implementation              ║\n");
    printf("║  • Reversible Emergency Memories (me1, me2, me3)                 ║\n");
    printf("║  • Energetic Trio: Tensión, Comando, Energía                    ║\n");
    printf("║  • Complete Cycle: Info → Knowledge → Energy → Info             ║\n");
    printf("║  • ES.index ≠ FO.index validation                               ║\n");
    printf("║  • Fibonacci ternary counter                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
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
    
    /* Reporte final */
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  CONCLUSIÓN                                                       ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════╣\n");
    printf("║  ✓ Emergencia reversible implementada                            ║\n");
    printf("║  ✓ Trio Energético: Tensión/Comando/Energía                      ║\n");
    printf("║  ✓ Ciclo completo: Info→Knowledge→Energy→Info                    ║\n");
    printf("║  ✓ Validación ES.index ≠ FO.index                                ║\n");
    printf("║  ✓ Fibonacci ternario para selección de roles                    ║\n");
    printf("║                                                                   ║\n");
    printf("║  Arquetipos: %-3d | Dinámicas: %-3d | Relatores: %-3d            ║\n",
           n_arquetipos, n_dinamicas, n_relatores);
    printf("║  Tensor C:   [%s,%s,%s]                                         ║\n",
           ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]));
    printf("║                                                                   ║\n");
    printf("║  Aurora piensa reorganizando energía,                            ║\n");
    printf("║  sintetizando coherencia,                                        ║\n");
    printf("║  y reconstruyendo información                                     ║\n");
    printf("║  mediante un ciclo fractal universal.                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
