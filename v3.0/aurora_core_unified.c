/*
 * aurora_core_unified.c - Aurora Unified Core
 * 
 * Implementación completa siguiendo:
 * 1. Glosario técnico (Trit, Dimension, Vector, TensorBasic, Trigate, Tetraedro)
 * 2. Tres memorias separadas: Arquetipo, Dinámica, Relator
 * 3. Tetraedro único con tres modos energéticos (Operativo/Gestión/Memoria)
 * 4. El sistema opera/aprende/gestiona con la MISMA lógica (trigates)
 * 5. Emergencia = colapso al centro del tetraedro (espirales áureas en 4 caras)
 * 
 * Licencias: Apache 2.0 + CC BY 4.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ═══════════════════════════════════════════════════════════════════════
 * GLOSARIO TÉCNICO FUNDAMENTAL
 * ═══════════════════════════════════════════════════════════════════════ */

/* Trit: Unidad mínima de información */
typedef int Trit; /* 1 (true/correct), 0 (false/under), -1 (null/indeterminado) */

/* Dimension FFE: 3 trits con roles FO/FN/ES contextuales */
typedef struct {
    Trit t[3]; /* [trit0, trit1, trit2] - roles dependen del contexto */
} Dimension;

/* Vector FFE: 3 dimensiones - unidad operativa del Tetraedro */
typedef struct {
    Dimension d[3]; /* [dim0, dim1, dim2] */
} Vector;

/* TensorBasic: Estructura de dos niveles (síntesis + base) */
typedef struct {
    Dimension synthesis;  /* Nivel superior: dimensión de síntesis */
    Vector base;          /* Nivel inferior: vector completo */
} TensorBasic;

/* TensorAurora: Estructura completa 1-3-9 (máxima del sistema) */
typedef struct {
    Dimension level1;     /* 1 dimensión */
    Vector level2;        /* 3 dimensiones */
    TensorBasic level3[3]; /* 9 dimensiones (3 tensores básicos) */
} TensorAurora;

/* ═══════════════════════════════════════════════════════════════════════
 * UTILIDADES BÁSICAS
 * ═══════════════════════════════════════════════════════════════════════ */

static const char* ts(Trit t) {
    return t == 1 ? "1" : (t == 0 ? "0" : "N");
}

static int eq_dim(const Dimension* a, const Dimension* b) {
    return a->t[0] == b->t[0] && a->t[1] == b->t[1] && a->t[2] == b->t[2];
}

static void copy_dim(Dimension* dst, const Dimension* src) {
    dst->t[0] = src->t[0];
    dst->t[1] = src->t[1];
    dst->t[2] = src->t[2];
}

static int count_nulls_dim(const Dimension* d) {
    int c = 0;
    for(int i = 0; i < 3; i++) if(d->t[i] == -1) c++;
    return c;
}

static int count_nulls_vec(const Vector* v) {
    return count_nulls_dim(&v->d[0]) + 
           count_nulls_dim(&v->d[1]) + 
           count_nulls_dim(&v->d[2]);
}

/* ═══════════════════════════════════════════════════════════════════════
 * TRIGATE: ÁTOMO DE LA INTELIGENCIA
 * Operaciones: AND₃, OR₃, CONSENSUS
 * Modos: Inferencia, Aprendizaje, Deducción
 * ═══════════════════════════════════════════════════════════════════════ */

/* Operaciones ternarias básicas */
static Trit trit_and(Trit a, Trit b) {
    if (a == 0 || b == 0) return 0;
    if (a == 1 && b == 1) return 1;
    return -1;
}

static Trit trit_or(Trit a, Trit b) {
    if (a == 1 || b == 1) return 1;
    if (a == 0 && b == 0) return 0;
    return -1;
}

static Trit trit_consensus(Trit a, Trit b) {
    if (a != -1 && a == b) return a;
    return -1;
}

/* Trigate - Modo Inferencia: (A,B,M)→R */
static Trit trit_infer(Trit a, Trit b, Trit m) {
    if (m == 0) return trit_and(a, b);
    if (m == 1) return trit_or(a, b);
    return trit_consensus(a, b);
}

/* Trigate - Modo Aprendizaje: (A,B,R)→M */
static Trit trit_learn(Trit a, Trit b, Trit r) {
    if (trit_and(a, b) == r) return 0;
    if (trit_or(a, b) == r) return 1;
    if (a != -1 && a == b && r == a) return -1;
    return -1;
}

/* Trigate - Modo Deducción: (A,M,R)→B */
static Trit trit_deduce_b(Trit a, Trit m, Trit r) {
    /* Para cada modo, intentar deducir B que satisfaga R = infer(A,B,M) */
    if (m == 0) { /* AND */
        if (a == 1 && r == 1) return 1;
        if (a == 1 && r == 0) return 0;
        if (a == 0) return -1; /* Puede ser cualquiera */
    } else if (m == 1) { /* OR */
        if (a == 1) return -1; /* Ya da 1, B no importa */
        if (a == 0 && r == 1) return 1;
        if (a == 0 && r == 0) return 0;
    } else { /* CONSENSUS */
        if (r == -1) return -1;
        return r; /* B debe ser igual a R */
    }
    return -1;
}

/* ═══════════════════════════════════════════════════════════════════════
 * TRES MEMORIAS DEL SISTEMA (Glosario)
 * ═══════════════════════════════════════════════════════════════════════ */

#define MAX_MEM 256

/* Arquetipo: patrón estable que emerge → determina FO superior */
typedef struct {
    Trit pattern[3];  /* Patrón de modos que se combinan */
    Trit fo_output;   /* FO superior resultante */
    int support;      /* Cuántas veces confirmado */
    unsigned long rev;
} Arquetipo;

/* Dinámica: transformación temporal → determina FN superior */
typedef struct {
    Trit state_before[3];  /* Estado t-1 */
    Trit state_after[3];   /* Estado t */
    Trit fn_output;        /* FN superior (dirección del cambio) */
    int support;
    unsigned long rev;
} Dinamica;

/* Relator: meta-patrón de orden → determina cómo se comparan dimensiones */
typedef struct {
    Trit dim_a[3];    /* Dimensión A */
    Trit dim_b[3];    /* Dimensión B */
    Trit mode[3];     /* Modo M que relaciona A y B */
    int support;
    unsigned long rev;
} Relator;

/* Knowledge Base: tres pirámides */
static Arquetipo arquetipos[MAX_MEM];
static int n_arquetipos = 0;

static Dinamica dinamicas[MAX_MEM];
static int n_dinamicas = 0;

static Relator relatores[MAX_MEM];
static int n_relatores = 0;

static unsigned long global_rev = 1;

/* ═══════════════════════════════════════════════════════════════════════
 * FUNCIONES DE APRENDIZAJE DE MEMORIAS
 * ═══════════════════════════════════════════════════════════════════════ */

static void learn_arquetipo(const Trit pattern[3], Trit fo_out) {
    /* Buscar si ya existe */
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].pattern[0] == pattern[0] &&
            arquetipos[i].pattern[1] == pattern[1] &&
            arquetipos[i].pattern[2] == pattern[2]) {
            arquetipos[i].support++;
            arquetipos[i].rev = global_rev++;
            if (arquetipos[i].fo_output != fo_out && fo_out != -1) {
                /* Conflicto: degradar a null */
                arquetipos[i].fo_output = -1;
            }
            return;
        }
    }
    
    /* Crear nuevo */
    if (n_arquetipos < MAX_MEM) {
        arquetipos[n_arquetipos].pattern[0] = pattern[0];
        arquetipos[n_arquetipos].pattern[1] = pattern[1];
        arquetipos[n_arquetipos].pattern[2] = pattern[2];
        arquetipos[n_arquetipos].fo_output = fo_out;
        arquetipos[n_arquetipos].support = 1;
        arquetipos[n_arquetipos].rev = global_rev++;
        n_arquetipos++;
    }
}

static void learn_dinamica(const Trit before[3], const Trit after[3], Trit fn_out) {
    for (int i = 0; i < n_dinamicas; i++) {
        if (dinamicas[i].state_before[0] == before[0] &&
            dinamicas[i].state_before[1] == before[1] &&
            dinamicas[i].state_before[2] == before[2] &&
            dinamicas[i].state_after[0] == after[0] &&
            dinamicas[i].state_after[1] == after[1] &&
            dinamicas[i].state_after[2] == after[2]) {
            dinamicas[i].support++;
            dinamicas[i].rev = global_rev++;
            if (dinamicas[i].fn_output != fn_out && fn_out != -1) {
                dinamicas[i].fn_output = -1;
            }
            return;
        }
    }
    
    if (n_dinamicas < MAX_MEM) {
        dinamicas[n_dinamicas].state_before[0] = before[0];
        dinamicas[n_dinamicas].state_before[1] = before[1];
        dinamicas[n_dinamicas].state_before[2] = before[2];
        dinamicas[n_dinamicas].state_after[0] = after[0];
        dinamicas[n_dinamicas].state_after[1] = after[1];
        dinamicas[n_dinamicas].state_after[2] = after[2];
        dinamicas[n_dinamicas].fn_output = fn_out;
        dinamicas[n_dinamicas].support = 1;
        dinamicas[n_dinamicas].rev = global_rev++;
        n_dinamicas++;
    }
}

static void learn_relator(const Trit a[3], const Trit b[3], const Trit m[3]) {
    for (int i = 0; i < n_relatores; i++) {
        if (relatores[i].dim_a[0] == a[0] && relatores[i].dim_a[1] == a[1] && relatores[i].dim_a[2] == a[2] &&
            relatores[i].dim_b[0] == b[0] && relatores[i].dim_b[1] == b[1] && relatores[i].dim_b[2] == b[2]) {
            relatores[i].support++;
            relatores[i].rev = global_rev++;
            for (int k = 0; k < 3; k++) {
                if (relatores[i].mode[k] != m[k] && m[k] != -1) {
                    relatores[i].mode[k] = -1;
                }
            }
            return;
        }
    }
    
    if (n_relatores < MAX_MEM) {
        relatores[n_relatores].dim_a[0] = a[0];
        relatores[n_relatores].dim_a[1] = a[1];
        relatores[n_relatores].dim_a[2] = a[2];
        relatores[n_relatores].dim_b[0] = b[0];
        relatores[n_relatores].dim_b[1] = b[1];
        relatores[n_relatores].dim_b[2] = b[2];
        relatores[n_relatores].mode[0] = m[0];
        relatores[n_relatores].mode[1] = m[1];
        relatores[n_relatores].mode[2] = m[2];
        relatores[n_relatores].support = 1;
        relatores[n_relatores].rev = global_rev++;
        n_relatores++;
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * TETRAEDRO: 4 MÓDULOS
 * Sintetizador, Evolver, Extender, Armonizador
 * ═══════════════════════════════════════════════════════════════════════ */

/* Sintetizador: F(d1,d2,d3,memoria)→s
 * Combina las FO de tres dimensiones usando trigates
 * Los modos vienen del Evolver
 * Usa Arquetipo como memoria
 */
static Trit sintetizador(const Dimension d[3], const Trit modos_evolver[3], const Arquetipo* arq) {
    /* Tres trigates combinando FO de las dimensiones */
    Trit r1 = trit_infer(d[0].t[0], d[1].t[0], modos_evolver[0]);
    Trit r2 = trit_infer(d[1].t[0], d[2].t[0], modos_evolver[1]);
    Trit r3 = trit_infer(d[2].t[0], d[0].t[0], modos_evolver[2]);
    
    /* Combinar los 3 resultados con el arquetipo */
    Trit temp = trit_consensus(r1, r2);
    Trit final = trit_consensus(temp, r3);
    
    if (arq && final == -1) {
        /* Si hay null, intentar usar arquetipo */
        final = arq->fo_output;
    }
    
    return final;
}

/* Evolver: combina modos (FN) de las dimensiones de entrada
 * Genera los modos para el Sintetizador
 * También produce el FO superior vía Arquetipo
 */
static void evolver(const Dimension d[3], Trit modos_out[3], const Arquetipo* arq) {
    /* Tres trigates combinando FN (t[1]) de las dimensiones */
    modos_out[0] = trit_learn(d[0].t[1], d[1].t[1], d[2].t[1]);
    modos_out[1] = trit_learn(d[1].t[1], d[2].t[1], d[0].t[1]);
    modos_out[2] = trit_learn(d[2].t[1], d[0].t[1], d[1].t[1]);
    
    /* Los modos aprendidos se usan para generar FO superior vía arquetipo */
    if (arq) {
        learn_arquetipo(modos_out, arq->fo_output);
    }
}

/* Extender: F(s,memoria)→d1,d2,d3
 * Reconstruye el vector inferior desde la dimensión superior + Dinámica
 */
static void extender(const Dimension* superior, const Dinamica* dyn, Dimension out[3]) {
    /* Hash de FN superior con dinámica para obtener modos */
    Trit m0 = dyn ? trit_infer(superior->t[1], dyn->fn_output, 1) : superior->t[1];
    Trit m1 = dyn ? trit_infer(superior->t[1], dyn->fn_output, 0) : superior->t[1];
    Trit m2 = dyn ? trit_infer(superior->t[1], dyn->fn_output, -1) : superior->t[1];
    
    /* Reconstruir dimensiones usando los modos y el FO superior */
    for (int i = 0; i < 3; i++) {
        out[i].t[0] = trit_deduce_b(superior->t[0], m0, superior->t[0]);
        out[i].t[1] = trit_deduce_b(superior->t[0], m1, superior->t[1]);
        out[i].t[2] = trit_deduce_b(superior->t[0], m2, superior->t[2]);
    }
}

/* Armonizador: trigate fundamental
 * A = valor creencia C
 * B = Arquetipo
 * R = Dinámica
 * M = Relator
 * Elimina nulls, encuentra coherencia
 */
static int armonizador(Dimension* d, const Arquetipo* arq, const Dinamica* dyn, const Relator* rel) {
    /* Nueva lógica v2.1: selección autosimilar usando trigates y fiabilidad ternaria
     * Fiabilidad de cada memoria se expresa como Trit:
     *  1 = estable (support >= THRESH y valor no null)
     *  0 = presente pero débil / valor null
     * -1 = ausente
     * Para cada posición FO/FN/ES se hace triadic collapse entre las memorias relevantes
     */
    const int THRESH = 2; /* Umbral simple de estabilidad */
    Trit r_arq = (arq && arq->fo_output != -1 && arq->support >= THRESH) ? 1 : (arq ? 0 : -1);
    Trit r_dyn = (dyn && dyn->fn_output != -1 && dyn->support >= THRESH) ? 1 : (dyn ? 0 : -1);
    Trit r_rel = (rel && rel->mode[0] != -1 && rel->support >= THRESH) ? 1 : (rel ? 0 : -1);

    int cambios = 0;

    for (int i = 0; i < 3; i++) {
        if (d->t[i] != -1) continue; /* ya resuelto */

        /* Para cada índice usamos trigate para priorizar: 
         * FO → preferencia Arquetipo vs Dinámica (modo OR); si ambos nulos considerar Relator
         * FN → preferencia Dinámica vs Arquetipo (modo AND) para asegurar coherencia funcional
         * ES → consenso entre Relator y Arquetipo; si falla usar Dinámica como fallback
         */
        Trit candidato = -1;
        if (i == 0) { /* FO */
            Trit prelim = trit_infer(r_arq, r_dyn, 1); /* OR */
            if (prelim == 1) {
                /* elegir entre arq/dyn el más estable */
                if (r_arq == 1 && arq && arq->fo_output != -1) candidato = arq->fo_output;
                else if (r_dyn == 1 && dyn && dyn->fn_output != -1) candidato = dyn->fn_output; /* uso simbólico */
            } else if (r_rel == 1 && rel) {
                candidato = rel->mode[0];
            }
        } else if (i == 1) { /* FN */
            Trit prelim = trit_infer(r_dyn, r_arq, 0); /* AND */
            if (prelim == 1) {
                if (dyn && dyn->fn_output != -1) candidato = dyn->fn_output;
            } else if (r_arq == 1 && arq) {
                candidato = arq->fo_output; /* reinterpretado como función local */
            } else if (r_rel == 1 && rel) {
                candidato = rel->mode[1];
            }
        } else { /* ES */
            Trit prelim = trit_infer(r_rel, r_arq, -1); /* CONSENSUS */
            if (prelim == 1) {
                if (rel) candidato = rel->mode[2];
            } else if (r_dyn == 1 && dyn) {
                candidato = dyn->fn_output; /* fallback estructural */
            } else if (r_arq == 1 && arq) {
                candidato = arq->fo_output;
            }
        }

        if (candidato != -1) {
            d->t[i] = candidato;
            cambios++;
        }
    }
    return cambios;
}

/* ═══════════════════════════════════════════════════════════════════════
 * COLAPSO TRIÁDICO: CONVERGENCIA AL CENTRO DEL TETRAEDRO
 * Cuando L, O, P alcanzan equilibrio → colapso al centro
 * Espirales áureas en las 4 caras
 * ═══════════════════════════════════════════════════════════════════════ */

/* Colapso triádico: votación mayoritaria de 3 trits */
static Trit triadic_collapse(Trit a, Trit b, Trit c) {
    if (a == b) return a;
    if (a == c) return a;
    if (b == c) return b;
    return -1; /* Sin mayoría */
}

/* Distancia al centro en una cara (proyección 2D) */
static float distancia_al_centro_2d(Trit a, Trit b) {
    /* Centro = ambos valores coherentes (no null) */
    if (a == -1 || b == -1) return 1.0f;
    if (a == b) return 0.0f;
    return 0.5f;
}

/* Distancia al centro del tetraedro (4 caras) */
static float distancia_al_centro_tetraedro(const Dimension* lop) {
    /* Incorporar ponderación áurea para aproximar trayectoria espiral.
     * Pesos: 1, φ, φ^2, φ^3 normalizados.
     */
    const float PHI = 1.61803398875f;
    float w0 = 1.0f;
    float w1 = PHI;
    float w2 = PHI * PHI;
    float w3 = w2 * PHI;
    float norm = w0 + w1 + w2 + w3;

    float d_LO = distancia_al_centro_2d(lop->t[0], lop->t[1]);
    float d_LP = distancia_al_centro_2d(lop->t[0], lop->t[2]);
    float d_OP = distancia_al_centro_2d(lop->t[1], lop->t[2]);
    float d_3D = (lop->t[0] == lop->t[1] && lop->t[1] == lop->t[2] && lop->t[0] != -1) ? 0.0f : 1.0f;

    /* Distancia ponderada: enfatiza progresión hacia centro en fases sucesivas */
    return (d_LO * w0 + d_LP * w1 + d_OP * w2 + d_3D * w3) / norm;
}

/* Detectar si el elemento está en el centro del tetraedro */
static int en_centro_tetraedro(const Dimension* lop) {
    return distancia_al_centro_tetraedro(lop) < 0.1f;
}

/* Emergencia: cuando se alcanza el centro, TODO el tetraedro
 * se convierte en UN SOLO PUNTO del tetraedro superior
 */
static Dimension emergencia_nivel_superior(const Vector* knowledge, const Dimension* estado) {
    Dimension punto_emergente;
    
    /* Colapso triádico de las 3 dimensiones del conocimiento */
    punto_emergente.t[0] = triadic_collapse(
        knowledge->d[0].t[0],
        knowledge->d[1].t[0],
        knowledge->d[2].t[0]
    );
    
    punto_emergente.t[1] = triadic_collapse(
        knowledge->d[0].t[1],
        knowledge->d[1].t[1],
        knowledge->d[2].t[1]
    );
    
    punto_emergente.t[2] = triadic_collapse(
        knowledge->d[0].t[2],
        knowledge->d[1].t[2],
        knowledge->d[2].t[2]
    );
    
    /* Armonizar con estado energético */
    for (int i = 0; i < 3; i++) {
        if (punto_emergente.t[i] == -1 && estado->t[i] != -1) {
            punto_emergente.t[i] = estado->t[i];
        }
    }
    
    return punto_emergente;
}

/* ═══════════════════════════════════════════════════════════════════════
 * MODOS ENERGÉTICOS DEL TETRAEDRO
 * NO son tres tetraedros separados
 * ES UN SOLO TETRAEDRO cambiando de modo según energía dominante
 * ═══════════════════════════════════════════════════════════════════════ */

typedef enum {
    MODE_OPERATIVO,  /* FO dominante: explorar, calcular, crear */
    MODE_GESTION,    /* FN dominante: corregir, reorganizar, decidir */
    MODE_MEMORIA     /* ES dominante: consolidar, estabilizar, dormir */
} ModoEnergetico;

/* Estado energético = Dimension FFE */
static Dimension estado_energetico = {{0, 0, 0}};
static ModoEnergetico modo_actual = MODE_OPERATIVO;

/* Selector de modo: usa TRIGATES para decidir (no if-else) */
static ModoEnergetico select_modo_trigate(const Dimension* estado, const Dimension* input) {
    /* Calcular dominancia de cada modo usando trigates */
    Trit dom_operativo = trit_infer(estado->t[0], input->t[0], 1); /* OR */
    Trit dom_gestion = trit_infer(estado->t[1], input->t[1], 0);   /* AND */
    Trit dom_memoria = trit_infer(estado->t[2], input->t[2], -1);  /* CONSENSUS */
    
    /* Colapso triádico para decidir */
    if (dom_operativo == 1 && dom_gestion != 1 && dom_memoria != 1) {
        return MODE_OPERATIVO;
    } else if (dom_gestion == 1) {
        return MODE_GESTION;
    } else if (dom_memoria == 1) {
        return MODE_MEMORIA;
    }
    
    return MODE_OPERATIVO; /* Default */
}

/* Actualizar estado energético usando trigate */
static void update_estado_energetico(const Dimension* input) {
    /* El MISMO trigate que procesa conocimiento procesa energía */
    for (int i = 0; i < 3; i++) {
        Trit nuevo = trit_infer(estado_energetico.t[i], input->t[i], 1);
        estado_energetico.t[i] = nuevo;
    }
    
    /* Seleccionar nuevo modo */
    modo_actual = select_modo_trigate(&estado_energetico, input);
}

/* ═══════════════════════════════════════════════════════════════════════
 * CICLOS DEL TETRAEDRO (comportamientos por modo)
 * ═══════════════════════════════════════════════════════════════════════ */

static void cycle_operativo(const Dimension d[3]) {
    printf("  [Modo Operativo] Explorando y aprendiendo...\n");
    
    /* Evolver + Sintetizador + aprendizaje activo */
    Trit modos[3];
    evolver(d, modos, NULL);
    
    Trit fo_superior = sintetizador(d, modos, NULL);
    
    /* Aprender arquetipo */
    learn_arquetipo(modos, fo_superior);
    
    /* NO autopoda (mantener diversidad) */
}

static void cycle_gestion(void) {
    printf("  [Modo Gestión] Armonizando y corrigiendo...\n");
    
    /* Armonizador intenso en todas las memorias */
    for (int i = 0; i < n_arquetipos; i++) {
        Dimension temp;
        temp.t[0] = arquetipos[i].fo_output;
        temp.t[1] = arquetipos[i].pattern[0];
        temp.t[2] = arquetipos[i].pattern[1];
        armonizador(&temp, &arquetipos[i], NULL, NULL);
    }
    
    /* Autopoda moderada (support >= 2) */
    int nueva_n = 0;
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].support >= 2) {
            arquetipos[nueva_n++] = arquetipos[i];
        }
    }
    n_arquetipos = nueva_n;
}

static void cycle_memoria(void) {
    printf("  [Modo Memoria] Consolidando en sueño profundo...\n");
    
    /* Autopoda agresiva (support >= 3) */
    int nueva_n_arq = 0;
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].support >= 3) {
            arquetipos[nueva_n_arq++] = arquetipos[i];
        }
    }
    printf("  [Autopoda] Arquetipos: %d → %d\n", n_arquetipos, nueva_n_arq);
    n_arquetipos = nueva_n_arq;
    
    /* Apoptosis si muy pocos arquetipos */
    if (n_arquetipos < 3) {
        printf("  [Apoptosis] Sistema caótico, reiniciando conocimiento base...\n");
        n_arquetipos = 0;
        n_dinamicas = 0;
        n_relatores = 0;
    }
    
    /* Armonización final */
    printf("  [Memoria consolidada] Sistema estabilizado\n");
}

/* ═══════════════════════════════════════════════════════════════════════
 * TENSOR C: CREENCIA DE REFERENCIA
 * Se construye como tetraedro de Relator + Arquetipo + Dinámica
 * ═══════════════════════════════════════════════════════════════════════ */

static Dimension tensor_C = {{-1, -1, -1}};

static void build_tensor_C(void) {
    /* Tomar los arquetipos/dinámicas/relatores más fuertes */
    Trit arq_fo = (n_arquetipos > 0) ? arquetipos[0].fo_output : -1;
    Trit dyn_fn = (n_dinamicas > 0) ? dinamicas[0].fn_output : -1;
    Trit rel_es = (n_relatores > 0) ? relatores[0].mode[0] : -1;
    
    /* Construir dimensión C */
    tensor_C.t[0] = arq_fo;
    tensor_C.t[1] = dyn_fn;
    tensor_C.t[2] = rel_es;
    
    /* Armonizar */
    armonizador(&tensor_C, n_arquetipos > 0 ? &arquetipos[0] : NULL, NULL, NULL);
}

/* ═══════════════════════════════════════════════════════════════════════
 * CICLO RECURSIVO PRINCIPAL
 * ═══════════════════════════════════════════════════════════════════════ */

/* Procesar un input recursivamente hasta alcanzar coherencia o límite */
static int process_recursive(const Dimension* input, int depth, int max_depth) {
    if (depth >= max_depth) {
        printf("  [Recursión] Límite de profundidad alcanzado (%d)\n", depth);
        return 0;
    }
    
    printf("\n┌─── Nivel recursivo %d ───┐\n", depth);
    
    /* Actualizar estado energético */
    update_estado_energetico(input);
    
    printf("  Estado: [%s,%s,%s] → Modo: ", 
           ts(estado_energetico.t[0]), ts(estado_energetico.t[1]), ts(estado_energetico.t[2]));
    
    /* Ejecutar ciclo según modo */
    if (modo_actual == MODE_OPERATIVO) {
        printf("OPERATIVO\n");
        Dimension d[3];
        /* Generar datos variados para aprendizaje */
        for (int i = 0; i < 3; i++) {
            d[i].t[0] = (input->t[i] + depth) % 3 - 1;
            d[i].t[1] = (input->t[(i+1)%3] - depth) % 3 - 1;
            d[i].t[2] = (input->t[(i+2)%3] * depth) % 3 - 1;
        }
        cycle_operativo(d);
        
        /* Aprender también dinámicas y relatores */
        if (depth > 0) {
            Dimension prev = {{(depth-1) % 3 - 1, depth % 2, (depth+1) % 3 - 1}};
            learn_dinamica(prev.t, input->t, input->t[1]);
            learn_relator(input->t, d[0].t, d[1].t);
        }
        
    } else if (modo_actual == MODE_GESTION) {
        printf("GESTIÓN\n");
        cycle_gestion();
        
    } else {
        printf("MEMORIA\n");
        cycle_memoria();
    }
    
    printf("  Memorias: Arq=%d Dyn=%d Rel=%d\n", 
           n_arquetipos, n_dinamicas, n_relatores);
    
    /* Construir Tensor C y verificar distancia */
    build_tensor_C();
    float dist = distancia_al_centro_tetraedro(&tensor_C);
    printf("  Tensor C: [%s,%s,%s] → distancia=%.3f\n",
           ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]), dist);
    
    /* Condición de emergencia */
    if (en_centro_tetraedro(&tensor_C)) {
        printf("  🌟 EMERGENCIA: Centro alcanzado en nivel %d\n", depth);
        Vector knowledge = {{{1,0,0}, {0,1,0}, {0,0,1}}};
        Dimension superior = emergencia_nivel_superior(&knowledge, &estado_energetico);
        printf("  Punto emergente: [%s,%s,%s]\n",
               ts(superior.t[0]), ts(superior.t[1]), ts(superior.t[2]));
        return 1; /* Emergencia exitosa */
    }
    
    /* Generar siguiente input basado en estado actual + ruido Fibonacci */
    Dimension next_input;
    int fib_a = 1, fib_b = 1;
    for (int i = 0; i < depth % 5; i++) {
        int temp = fib_a + fib_b;
        fib_a = fib_b;
        fib_b = temp;
    }
    
    for (int i = 0; i < 3; i++) {
        /* Combinar estado actual + tensor C + secuencia Fibonacci */
        Trit base = trit_infer(estado_energetico.t[i], tensor_C.t[i], i % 3 - 1);
        next_input.t[i] = (base + (fib_b % 3) - 1 + i) % 3 - 1;
        if (next_input.t[i] < -1) next_input.t[i] = -1;
        if (next_input.t[i] > 1) next_input.t[i] = 1;
    }
    
    printf("  Siguiente: [%s,%s,%s]\n", 
           ts(next_input.t[0]), ts(next_input.t[1]), ts(next_input.t[2]));
    printf("└─────────────────────────┘\n");
    
    /* Recursión */
    return process_recursive(&next_input, depth + 1, max_depth);
}

/* ═══════════════════════════════════════════════════════════════════════
 * DEMO PRINCIPAL
 * ═══════════════════════════════════════════════════════════════════════ */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  Aurora Core Unified v2.1 - Recursivo + Phi + Auto-Sueño        ║\n");
    printf("║  Tetraedro Único Trimodal con Espirales Áureas                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    /* Inicializar estado energético */
    estado_energetico.t[0] = 1;  /* Operativo activo */
    estado_energetico.t[1] = 0;  /* Gestión baja */
    estado_energetico.t[2] = 0;  /* Memoria baja */
    
    /* ━━━ FASE 1: ALIMENTAR CON INFORMACIÓN VARIADA ━━━ */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  FASE 1: Alimentación con patrones diversos\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    Dimension inputs[] = {
        {{1, 0, -1}},   /* Patrón 1 */
        {{0, 1, 1}},    /* Patrón 2 */
        {{1, 1, 0}},    /* Patrón 3 */
        {{-1, 0, 1}},   /* Patrón 4 */
        {{1, -1, 1}},   /* Patrón 5 */
        {{0, 0, 1}},    /* Patrón 6 */
        {{1, 1, -1}},   /* Patrón 7 */
        {{-1, 1, 0}},   /* Patrón 8 */
    };
    
    for (int i = 0; i < 8; i++) {
        printf("\n─── Input %d: [%s,%s,%s] ───\n", i+1,
               ts(inputs[i].t[0]), ts(inputs[i].t[1]), ts(inputs[i].t[2]));
        
        update_estado_energetico(&inputs[i]);
        
        /* Aprender patrones */
        Dimension d[3];
        for (int j = 0; j < 3; j++) {
            d[j].t[0] = inputs[i].t[j];
            d[j].t[1] = inputs[i].t[(j+1)%3];
            d[j].t[2] = inputs[i].t[(j+2)%3];
        }
        
        if (modo_actual == MODE_OPERATIVO) {
            cycle_operativo(d);
            /* Aprender también otras memorias */
            if (i > 0) {
                learn_dinamica(inputs[i-1].t, inputs[i].t, inputs[i].t[1]);
                learn_relator(inputs[i].t, d[0].t, d[1].t);
            }
        } else if (modo_actual == MODE_GESTION) {
            cycle_gestion();
        }
        
        printf("Memorias: Arq=%d Dyn=%d Rel=%d | Modo=%s\n",
               n_arquetipos, n_dinamicas, n_relatores,
               modo_actual == MODE_OPERATIVO ? "OPERATIVO" : 
               (modo_actual == MODE_GESTION ? "GESTIÓN" : "MEMORIA"));
    }
    
    /* ━━━ FASE 2: PROCESAMIENTO RECURSIVO HASTA EMERGENCIA ━━━ */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  FASE 2: Procesamiento recursivo (búsqueda de emergencia)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    Dimension seed = {{1, 1, 1}}; /* Semilla coherente */
    int emergencia_alcanzada = process_recursive(&seed, 0, 20);
    
    /* ━━━ FASE 3: REPORTE FINAL ━━━ */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  REPORTE FINAL\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nConocimiento acumulado:\n");
    printf("  Arquetipos: %d\n", n_arquetipos);
    printf("  Dinámicas:  %d\n", n_dinamicas);
    printf("  Relatores:  %d\n", n_relatores);
    
    build_tensor_C();
    printf("\nTensor C final: [%s,%s,%s]\n",
           ts(tensor_C.t[0]), ts(tensor_C.t[1]), ts(tensor_C.t[2]));
    printf("Distancia al centro: %.4f\n", distancia_al_centro_tetraedro(&tensor_C));
    
    printf("\nEstado energético final: [%s,%s,%s]\n",
           ts(estado_energetico.t[0]), ts(estado_energetico.t[1]), ts(estado_energetico.t[2]));
    printf("Modo final: %s\n", 
           modo_actual == MODE_OPERATIVO ? "OPERATIVO" : 
           (modo_actual == MODE_GESTION ? "GESTIÓN" : "MEMORIA"));
    
    if (emergencia_alcanzada) {
        printf("\n✨ EMERGENCIA EXITOSA: El sistema alcanzó el centro del tetraedro\n");
    } else {
        printf("\n🌀 Sistema en espiral: continúa evolucionando hacia el centro\n");
    }
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  El conocimiento gestiona su energía                             ║\n");
    printf("║  La energía estructura su conocimiento                           ║\n");
    printf("║  NO SON DOS PROCESOS - SON EL MISMO TETRAEDRO                    ║\n");
    printf("║                                                                   ║\n");
    printf("║  Ponderación áurea (φ=1.618) en distancia al centro             ║\n");
    printf("║  Armonizador con selección trigate (sin prioridades fijas)      ║\n");
    printf("║  Auto-sueño según Pacto de Coherencia                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
