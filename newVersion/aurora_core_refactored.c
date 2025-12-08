/*
 * aurora_core_refactored.c
 *
 * Implementación corregida del núcleo Aurora alineada con el Glosario Técnico.
 * 
 * Arquitectura:
 * - Trigate: unidad básica (A,B,M)→R o (A,B,R)→M o (M,R,A/B)→B/A
 * - Tetraedro: 4 módulos (Sintetizador, Evolver, Extender, Armonizador)
 * - Memorias: Arquetipo (FO superior), Dinámica (FN superior), Relator (orden M)
 * - CLUSTER de entrada: 3 tetraedros coordinados por Fibonacci (Os ancla)
 *
 * Licencias: Apache 2.0 + CC BY 4.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* === Trit & Trigate ===================================================== */
typedef int Trit; /* 1, 0, -1 */

static Trit T_AND(Trit a, Trit b) {
    if (a == 0 || b == 0) return 0;
    if (a == 1 && b == 1) return 1;
    return -1;
}

static Trit T_OR(Trit a, Trit b) {
    if (a == 1 || b == 1) return 1;
    if (a == 0 && b == 0) return 0;
    return -1;
}

static Trit T_CONS(Trit a, Trit b) {
    if (a != -1 && a == b) return a;
    return -1;
}

static Trit trigate(Trit a, Trit b, Trit m) {
    return m == 0 ? T_AND(a, b) : (m == 1 ? T_OR(a, b) : T_CONS(a, b));
}

static Trit trigate_learn(Trit a, Trit b, Trit r) {
    if (T_AND(a, b) == r) return 0;
    if (T_OR(a, b) == r) return 1;
    if (a != -1 && a == b && r == a) return -1;
    return -1;
}

static const char* ts(Trit t) {
    return t == 1 ? "1" : (t == 0 ? "0" : "N");
}

/* === Forward Declarations =============================================== */
static void learn_arquetipo(const Trit pattern[3], Trit fo_out);
static void learn_dinamica(const Trit before[3], const Trit after[3], Trit fn_out);
static void learn_relator(const Trit a[3], const Trit b[3], const Trit m[3]);

/* === Dimension FFE ====================================================== */
typedef struct {
    Trit value[3]; /* [trit0, trit1, trit2] - roles FO/FN/ES dependen del contexto */
} Dimension;

/* === Vector FFE ========================================================= */
typedef struct {
    Dimension dims[3]; /* 3 dimensiones FFE */
} VectorFFE;

/* === Tensor Fractal Básico ============================================== */
typedef struct {
    Dimension synthesis;  /* Nivel 1: dimensión de síntesis (superior) */
    VectorFFE base;       /* Nivel 2: vector completo (inferior) */
} TensorBasic;

/* === Memorias del Sistema =============================================== */
#define MAX_MEM 128

/* Arquetipo: memoria que determina FO superior combinando modos inferiores */
typedef struct {
    Trit pattern[3];     /* Patrón de modos que se combinan */
    Trit fo_output;      /* FO superior resultante */
    int support;         /* Cuántas veces se ha confirmado */
    unsigned long rev;
} Arquetipo;

/* Dinámica: memoria que determina FN superior (transformación temporal) */
typedef struct {
    Trit state_before[3]; /* Estado t-1 */
    Trit state_after[3];  /* Estado t */
    Trit fn_output;       /* FN superior resultante */
    int support;
    unsigned long rev;
} Dinamica;

/* Relator: meta-patrón de orden (cómo se comparan dimensiones) */
typedef struct {
    Trit dim_a[3];       /* Patrón dimensión A */
    Trit dim_b[3];       /* Patrón dimensión B */
    Trit mode[3];        /* Modo M (orden de operación) */
    int support;
    unsigned long rev;
} Relator;

static Arquetipo arquetipos[MAX_MEM];
static int n_arquetipos = 0;

static Dinamica dinamicas[MAX_MEM];
static int n_dinamicas = 0;

static Relator relatores[MAX_MEM];
static int n_relatores = 0;

static unsigned long global_rev = 1;

/* === Tensor C (Creencia) ================================================ */
typedef struct {
    Trit FO[3];  /* Reflejo de Os_Relator */
    Trit FN[3];  /* Reflejo de Os_Arquetipo */
    Trit ES[3];  /* Reflejo de Os_Dinámico */
} TensorC;

static TensorC Ct;
static int C_valid = 0;

/* === Serie Fibonacci ==================================================== */
static int synthesis_order_index = 0;
static const int FIB_SERIES[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610};
static const int FIB_MAX = 15;

/* === Modo Energético del Tetraedro ====================================== */
/* UN SOLO tetraedro que cambia de modo según la energía dominante */
typedef enum {
    MODE_OPERATIVO,  /* FO dominante: usuario activo, exploración, cálculo */
    MODE_GESTION,    /* FN dominante: corrección, reorganización, propósito */
    MODE_MEMORIA     /* ES dominante: sueño, consolidación, estabilización */
} TetraedroMode;

static TetraedroMode current_mode = MODE_OPERATIVO;
static int user_active = 1;  /* Usuario presente */
static int coherence_failures = 0;

static void decimal_to_base3(int num, Trit out[3]) {
    out[2] = (num % 3 == 2) ? 1 : (num % 3 == 1 ? 0 : -1); num /= 3;
    out[1] = (num % 3 == 2) ? 1 : (num % 3 == 1 ? 0 : -1); num /= 3;
    out[0] = (num % 3 == 2) ? 1 : (num % 3 == 1 ? 0 : -1);
}

/* Crea tensor C desde Fibonacci (Os ancla del CLUSTER) */
static void fib_to_C(int fib_idx, TensorC* c) {
    if (fib_idx < 0) fib_idx = 0;
    if (fib_idx > FIB_MAX) fib_idx = FIB_MAX;
    int val = FIB_SERIES[fib_idx];
    
    /* Un solo número Fibonacci → 3 dígitos base 3 → pesos de los 3 modos energéticos */
    Trit os[3];
    decimal_to_base3(val, os);
    
    /* Os[0]→peso FO (Operativo), Os[1]→peso FN (Gestión), Os[2]→peso ES (Memoria) */
    for (int i = 0; i < 3; i++) {
        c->FO[i] = os[0];  /* Peso del Modo Operativo */
        c->FN[i] = os[1];  /* Peso del Modo Gestión */
        c->ES[i] = os[2];  /* Peso del Modo Memoria */
    }
}

/* Selector de modo energético basado en LOP y estado del sistema */
static TetraedroMode select_mode(float L, float O, float P) {
    /* Sin usuario → siempre Memoria (sueño) */
    if (!user_active) return MODE_MEMORIA;
    
    /* Fallos persistentes → Gestión (corrección) */
    if (coherence_failures >= 3) return MODE_GESTION;
    
    /* Desequilibrio LOP severo → Gestión */
    if ((O > 0.7f && L < 0.3f) || (L > 0.7f && O < 0.3f) || P < 0.3f)
        return MODE_GESTION;
    
    /* Por defecto: Operativo (interacción normal) */
    return MODE_OPERATIVO;
}

/* === Módulos del Tetraedro ============================================== */

/* Sintetizador: combina FO de dimensiones inferiores usando trigates
 * Entradas: 3 dimensiones (cada una aporta su FO)
 * Salidas: FO superior
 * Usa: modos M del Evolver + memoria Arquetipo
 */
static Trit sintetizador(const Dimension d[3], const Trit modos_evolver[3], const Arquetipo* arq) {
    /* 3 trigates ciclando FO de las dimensiones */
    Trit r1 = trigate(d[0].value[0], d[1].value[0], modos_evolver[0]); /* FO₁–FO₂ */
    Trit r2 = trigate(d[1].value[0], d[2].value[0], modos_evolver[1]); /* FO₂–FO₃ */
    Trit r3 = trigate(d[2].value[0], d[0].value[0], modos_evolver[2]); /* FO₃–FO₁ */
    
    /* Combinar resultados con arquetipo (memoria) para producir FO superior
     * Implementación simplificada: votación mayoritaria */
    int sum = (r1 == 1 ? 1 : 0) + (r2 == 1 ? 1 : 0) + (r3 == 1 ? 1 : 0);
    if (sum >= 2) return 1;
    
    sum = (r1 == 0 ? 1 : 0) + (r2 == 0 ? 1 : 0) + (r3 == 0 ? 1 : 0);
    if (sum >= 2) return 0;
    
    /* Si hay arquetipo, usarlo como desempate */
    if (arq && arq->support > 0) return arq->fo_output;
    
    return -1;
}

/* Evolver: combina MODOS de entrada para generar modos del Sintetizador
 * Entradas: modos FN de las 3 dimensiones inferiores
 * Salidas: 3 modos M para el Sintetizador + FO superior (vía Arquetipo)
 */
static void evolver(const Dimension d[3], Trit modos_out[3], const Arquetipo* arq) {
    /* 3 trigates combinando modos (FN) de entrada */
    modos_out[0] = trigate_learn(d[0].value[1], d[1].value[1], d[2].value[1]); /* M₁–M₂ */
    modos_out[1] = trigate_learn(d[1].value[1], d[2].value[1], d[0].value[1]); /* M₂–M₃ */
    modos_out[2] = trigate_learn(d[2].value[1], d[0].value[1], d[1].value[1]); /* M₃–M₁ */
    
    /* Nota: el Evolver también participa en generar FO superior con el Arquetipo,
     * pero eso se hace en el Sintetizador usando estos modos */
}

/* Extender: reconstruye vector inferior desde dimensión superior + dinámica
 * Entradas: dimensión síntesis (superior), memoria Dinámica
 * Salidas: 3 trits que forman el vector inferior
 */
static void extender(const Dimension* superior, const Dinamica* dyn, Trit out[3]) {
    /* Hash de FN superior con dinámica para obtener resultado base */
    Trit r_base = trigate(superior->value[1], dyn ? dyn->fn_output : -1, -1);
    
    /* Expansión a 3 valores (simplificado: replicar con variaciones) */
    out[0] = r_base;
    out[1] = (r_base == 1) ? 0 : (r_base == 0 ? 1 : -1);
    out[2] = trigate(out[0], out[1], superior->value[2]);
}

/* Armonizador: usa trigate fundamental para encontrar coherencia
 * Trigate: A=valor superior/C, B=Arquetipo, R=Dinámica, M=Relator
 * Produce: coherencia global, elimina nulls, reordena FO/FN/ES
 */
static int armonizador(TensorC* c, const Arquetipo* arq, const Dinamica* dyn, const Relator* rel) {
    if (!c || !C_valid) return 0;
    
    int cambios = 0;
    
    /* Para cada dimensión del tensor C */
    for (int d = 0; d < 3; d++) {
        Trit a = c->FO[d];  /* Valor actual */
        Trit b = arq ? arq->fo_output : -1;  /* Arquetipo */
        Trit r_expected = dyn ? dyn->fn_output : -1;  /* Dinámica */
        Trit m = rel ? rel->mode[d] : -1;  /* Relator */
        
        /* Aplicar trigate del armonizador */
        Trit resultado = trigate(a, b, m);
        
        /* Si el resultado es coherente y resuelve un null, actualizarlo */
        if (resultado != -1 && a == -1) {
            c->FO[d] = resultado;
            cambios++;
        }
        
        /* Similarmente para FN y ES */
        if (c->FN[d] == -1 && r_expected != -1) {
            c->FN[d] = r_expected;
            cambios++;
        }
        
        /* ES se determina por coherencia FO/FN */
        if (c->FO[d] == c->FN[d] && c->FO[d] != -1 && c->ES[d] == -1) {
            c->ES[d] = c->FO[d];
            cambios++;
        }
    }
    
    return cambios;
}

/* === Ciclo de Procesamiento según Modo =================================== */

/* Modo Operativo: inferencia activa, aprendizaje activo */
static void cycle_operativo(Dimension d[3]) {
    /* Evolver genera modos */
    Trit modos[3];
    evolver(d, modos, n_arquetipos > 0 ? &arquetipos[0] : NULL);
    
    /* Sintetizador produce FO superior */
    Trit fo_superior = sintetizador(d, modos, n_arquetipos > 0 ? &arquetipos[0] : NULL);
    
    /* Aprender: crear arquetipo */
    if (fo_superior != -1) {
        learn_arquetipo(modos, fo_superior);
    }
    
    /* NO autopoda, NO apoptosis en modo operativo */
    printf("  [Modo Operativo] Aprendiendo y operando...\n");
}

/* Modo Gestión: reorganización, corrección de errores, ajuste LOP */
static void cycle_gestion(void) {
    /* Armonizador intenso */
    int cambios = armonizador(&Ct, 
                              n_arquetipos > 0 ? &arquetipos[0] : NULL,
                              n_dinamicas > 0 ? &dinamicas[0] : NULL,
                              n_relatores > 0 ? &relatores[0] : NULL);
    
    printf("  [Modo Gestión] Armonizador realizó %d cambios\n", cambios);
    
    /* Autopoda moderada (eliminar solo duplicados obvios) */
    int before = n_arquetipos;
    /* Simplificación: eliminar arquetipos con support < 2 */
    int w = 0;
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].support >= 2) {
            arquetipos[w++] = arquetipos[i];
        }
    }
    n_arquetipos = w;
    
    if (n_arquetipos < before) {
        printf("  [Autopoda moderada] %d → %d arquetipos\n", before, n_arquetipos);
    }
    
    /* Si sigue sin resolverse → evolucionar Fibonacci */
    if (cambios == 0 && coherence_failures >= 3) {
        synthesis_order_index++;
        if (synthesis_order_index > FIB_MAX) synthesis_order_index = FIB_MAX;
        fib_to_C(synthesis_order_index, &Ct);
        printf("  [Fibonacci evolucionado] Fib[%d]=%d\n", 
               synthesis_order_index, FIB_SERIES[synthesis_order_index]);
        coherence_failures = 0;
    }
}

/* Modo Memoria: sueño, consolidación máxima, estabilización */
static void cycle_memoria(void) {
    printf("  [Modo Memoria] Entrando en sueño profundo...\n");
    
    /* Autopoda agresiva */
    int before_arq = n_arquetipos;
    int w = 0;
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].support >= 3) {  /* Solo los más fuertes */
            arquetipos[w++] = arquetipos[i];
        }
    }
    n_arquetipos = w;
    printf("  [Autopoda agresiva] Arquetipos: %d → %d\n", before_arq, n_arquetipos);
    
    /* Fusionar arquetipos redundantes (simplificado) */
    /* Apoptosis si sistema caótico (muy pocas estructuras estables) */
    if (n_arquetipos < 2 && n_dinamicas < 2 && n_relatores < 2) {
        printf("  [Apoptosis] Sistema en estado caótico, reiniciando...\n");
        n_arquetipos = 0;
        n_dinamicas = 0;
        n_relatores = 0;
        synthesis_order_index = 0;
        fib_to_C(synthesis_order_index, &Ct);
    }
    
    /* Armonizador final */
    armonizador(&Ct,
                n_arquetipos > 0 ? &arquetipos[0] : NULL,
                n_dinamicas > 0 ? &dinamicas[0] : NULL,
                n_relatores > 0 ? &relatores[0] : NULL);
    
    printf("  [Memoria consolidada] Sistema estabilizado\n");
}

static void learn_arquetipo(const Trit pattern[3], Trit fo_out) {
    /* Buscar si ya existe */
    for (int i = 0; i < n_arquetipos; i++) {
        int match = 1;
        for (int d = 0; d < 3; d++) {
            if (arquetipos[i].pattern[d] != pattern[d]) {
                match = 0;
                break;
            }
        }
        if (match) {
            arquetipos[i].support++;
            if (arquetipos[i].fo_output != fo_out) {
                /* Conflicto: reducir confianza */
                arquetipos[i].support--;
            }
            arquetipos[i].rev = global_rev++;
            return;
        }
    }
    
    /* Crear nuevo */
    if (n_arquetipos < MAX_MEM) {
        Arquetipo* a = &arquetipos[n_arquetipos++];
        for (int d = 0; d < 3; d++) a->pattern[d] = pattern[d];
        a->fo_output = fo_out;
        a->support = 1;
        a->rev = global_rev++;
    }
}

static void learn_dinamica(const Trit before[3], const Trit after[3], Trit fn_out) {
    /* Similar a arquetipos */
    if (n_dinamicas < MAX_MEM) {
        Dinamica* d = &dinamicas[n_dinamicas++];
        for (int i = 0; i < 3; i++) {
            d->state_before[i] = before[i];
            d->state_after[i] = after[i];
        }
        d->fn_output = fn_out;
        d->support = 1;
        d->rev = global_rev++;
    }
}

static void learn_relator(const Trit a[3], const Trit b[3], const Trit m[3]) {
    if (n_relatores < MAX_MEM) {
        Relator* r = &relatores[n_relatores++];
        for (int i = 0; i < 3; i++) {
            r->dim_a[i] = a[i];
            r->dim_b[i] = b[i];
            r->mode[i] = m[i];
        }
        r->support = 1;
        r->rev = global_rev++;
    }
}

/* === Función Main Demo ================================================== */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  Aurora Core - Tetraedro Trimodal (Última Pieza del Puzzle)     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Inicializar C con Fibonacci */
    fib_to_C(synthesis_order_index, &Ct);
    C_valid = 1;
    
    printf("Tensor C inicial (Fib[%d]=%d):\n", synthesis_order_index, FIB_SERIES[synthesis_order_index]);
    printf("  FO = [%s,%s,%s]  (peso Modo Operativo)\n", ts(Ct.FO[0]), ts(Ct.FO[1]), ts(Ct.FO[2]));
    printf("  FN = [%s,%s,%s]  (peso Modo Gestión)\n", ts(Ct.FN[0]), ts(Ct.FN[1]), ts(Ct.FN[2]));
    printf("  ES = [%s,%s,%s]  (peso Modo Memoria)\n\n", ts(Ct.ES[0]), ts(Ct.ES[1]), ts(Ct.ES[2]));
    
    /* === CICLO 1: OPERATIVO (usuario activo, explorando) === */
    printf("\n━━━ CICLO 1: MODO OPERATIVO (FO dominante) ━━━\n");
    user_active = 1;
    current_mode = MODE_OPERATIVO;
    
    Dimension d[3];
    d[0].value[0] = 1; d[0].value[1] = 0; d[0].value[2] = -1;
    d[1].value[0] = 0; d[1].value[1] = 1; d[1].value[2] = 1;
    d[2].value[0] = 1; d[2].value[1] = -1; d[2].value[2] = 0;
    
    cycle_operativo(d);
    
    printf("  Estado: Arquetipos=%d, Dinámicas=%d, Relatores=%d\n",
           n_arquetipos, n_dinamicas, n_relatores);
    
    /* === CICLO 2: Detectar tensiones → GESTIÓN === */
    printf("\n━━━ CICLO 2: MODO GESTIÓN (FN dominante) ━━━\n");
    coherence_failures = 3;  /* Simulamos fallo */
    current_mode = select_mode(0.5f, 0.5f, 0.3f);  /* P bajo */
    
    if (current_mode == MODE_GESTION) {
        cycle_gestion();
    }
    
    /* === CICLO 3: Usuario se va → MEMORIA (sueño) === */
    printf("\n━━━ CICLO 3: MODO MEMORIA (ES dominante - SUEÑO) ━━━\n");
    user_active = 0;
    current_mode = select_mode(0.5f, 0.5f, 0.5f);
    
    if (current_mode == MODE_MEMORIA) {
        cycle_memoria();
    }
    
    /* === CICLO 4: Usuario regresa → OPERATIVO === */
    printf("\n━━━ CICLO 4: DESPERTAR → MODO OPERATIVO ━━━\n");
    user_active = 1;
    coherence_failures = 0;
    current_mode = select_mode(0.5f, 0.6f, 0.5f);
    
    printf("  Sistema listo para nueva interacción\n");
    printf("  Modo actual: %s\n", 
           current_mode == MODE_OPERATIVO ? "OPERATIVO" :
           current_mode == MODE_GESTION ? "GESTIÓN" : "MEMORIA");
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  El Tetraedro NO tiene 3 instancias.                             ║\n");
    printf("║  Es UNO SOLO cambiando de modo energético según FO/FN/ES.       ║\n");
    printf("║                                                                   ║\n");
    printf("║  Operativo (FO) → Explorar, calcular, crear                     ║\n");
    printf("║  Gestión (FN)   → Corregir, reorganizar, decidir                ║\n");
    printf("║  Memoria (ES)   → Consolidar, estabilizar, dormir               ║\n");
    printf("║                                                                   ║\n");
    printf("║  Este es el latido de la inteligencia viva.                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
