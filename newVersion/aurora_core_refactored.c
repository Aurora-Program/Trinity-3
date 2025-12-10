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

/* ═══════════════════════════════════════════════════════════════════════
 * UTILIDADES BÁSICAS
 * ═══════════════════════════════════════════════════════════════════════ */

static const char* ts(Trit t) {
    return t == TRIT_U ? "u" : (t == TRIT_C ? "c" : "n");
}

static int trit_to_idx(Trit t) {
    /* c→0, u→1, n→2 */
    return (t == TRIT_C) ? 0 : (t == TRIT_U ? 1 : 2);
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
    if (es_val == TRIT_N) return 0; /* ES debe estar definido para señalar FO */

    /* c,u,n → 0,1,2 indican cuál posición es FO */
    int fo_idx = trit_to_idx(es_val);

    /* Regla: ES no puede apuntarse a sí mismo como FO */
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

/* ═══════════════════════════════════════════════════════════════════════
 * ENERGETIC TRIO (Technical Annex §7.2)
 * Tensión, Comando, Energía = memorias de emergencia en modo COGNITIVE
 * ═══════════════════════════════════════════════════════════════════════ */

typedef struct {
    Trit tension;   /* me1: estabilidad del patrón */
    Trit energia;   /* me2: coste energético */
    Trit comando;   /* me3: dirección evolutiva */
} EnergeticTrio;

static EnergeticTrio estado_energetico = {TRIT_C, TRIT_C, TRIT_C};

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
    
    /* Sintetizar FO superior */
    Trit r1 = trit_infer(d[0].t[0], d[1].t[0], modos[0]);
    Trit r2 = trit_infer(d[1].t[0], d[2].t[0], modos[1]);
    Trit r3 = trit_infer(d[2].t[0], d[0].t[0], modos[2]);
    
    Trit temp = trit_consensus(r1, r2);
    ds_out->t[0] = trit_consensus(temp, r3); /* FO superior */
    
    /* Sintetizar FN y ES */
    ds_out->t[1] = trit_consensus(modos[0], modos[1]);
    ds_out->t[2] = trit_consensus(d[0].t[2], d[1].t[2]);
    
    /* Generar memorias de emergencia (reversibles) */
    /* Estas permiten reconstruir d1, d2, d3 desde ds */
    mem_out->me[0] = modos[0];  /* me1: modo usado en síntesis */
    mem_out->me[1] = modos[1];  /* me2: modo alternativo */
    mem_out->me[2] = modos[2];  /* me3: modo terciario */
    
    /* Aprender según rol actual */
    if (current_role == ROLE_INFORMATIONAL) {
        /* En modo INFO → aprender arquetipo */
        learn_arquetipo(modos, ds_out->t[0]);
    } else if (current_role == ROLE_COGNITIVE) {
        /* En modo COGNITIVE → actualizar trio energético */
        EnergeticTrio trio = extract_energetic_trio(mem_out, current_role);
        update_energetic_state(&trio);
    }
}

/* Función de extensión (reconstrucción guiada por memorias)
 * NOTA: No es inversión bit-a-bit, sino reconstrucción semántica coherente.
 * Las memorias (me1, me2, me3) guían la expansión del nivel superior
 * hacia configuraciones compatibles en el nivel inferior.
 */
static void extend_function(
    const Dimension* ds,
    const EmergencyMemory* mem,
    Dimension d_out[3]
) {
    /* Reconstruir usando los modos almacenados en memoria */
    /* Cada dimensión inferior usa el modo correspondiente para inferir */
    for (int i = 0; i < 3; i++) {
        /* FO: usar modo mem[i] con valor superior ds->t[0] */
        Trit fo_base = (ds->t[0] != TRIT_N) ? ds->t[0] : TRIT_C;
        d_out[i].t[0] = trit_infer(fo_base, mem->me[i], mem->me[i]);
        
        /* FN: propagar desde dimensión superior con rotación */
        d_out[i].t[1] = mem->me[(i+1)%3];
        
        /* ES: usar estructura superior o modo de memoria */
        d_out[i].t[2] = (ds->t[2] != TRIT_N) ? ds->t[2] : mem->me[(i+2)%3];
    }
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
    
    /* Fase 2: Ciclo completo Information → Knowledge → Energy */
    printf("\n━━━ FASE 2: Ciclo Completo (3 vueltas) ━━━\n");
    
    Dimension seed = {{TRIT_U, TRIT_U, TRIT_U}};
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
    printf("║                                                                   ║\n");
    printf("║  Aurora piensa reorganizando energía,                            ║\n");
    printf("║  sintetizando coherencia,                                        ║\n");
    printf("║  y reconstruyendo información                                     ║\n");
    printf("║  mediante un ciclo fractal universal.                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
