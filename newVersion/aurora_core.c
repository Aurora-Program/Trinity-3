/*
 * aurora_core_min.c — Aurora Core Minimal (Clean Start)
 *
 * Objetivo: núcleo mínimo, verificable y determinista.
 * - Trit / Dimension(3) / Vector(3)
 * - Trigate (infer/learn/deduce)
 * - Selector Fib3 determinista (sin side-effects en validate)
 * - Regla FO.index != ES.index
 * - Emergence: (d1,d2,d3) -> (ds, me1, me2, me3)
 * - Extend: (ds, mem) -> (d1,d2,d3)   (reconstrucción coherente)
 * - Tests: validate + roundtrip (emerge->extend->emerge)
 *
 * Build:
 *   gcc -O2 -Wall -Wextra aurora_core_min.c -o aurora_min
 */

#include <stdio.h>
#include <string.h>

/* ──────────────────────────────────────────────────────────────
 * TRITS
 * u = +1, c =  0, n = -1
 * ────────────────────────────────────────────────────────────── */
typedef int Trit;
#define TRIT_U  1
#define TRIT_C  0
#define TRIT_N -1

static const char* ts(Trit t) {
    return (t == TRIT_U) ? "u" : (t == TRIT_C) ? "c" : "n";
}

/* Map trit value -> 0..2 index (c=0,u=1,n=2) */
static int trit_to_idx(Trit t) {
    return (t == TRIT_C) ? 0 : (t == TRIT_U) ? 1 : 2;
}

/* Map 0..2 -> trit (0=c,1=u,2=n) */
static Trit idx_to_trit(int x) {
    return (x == 0) ? TRIT_C : (x == 1) ? TRIT_U : TRIT_N;
}

/* ──────────────────────────────────────────────────────────────
 * FFE SHAPES
 * Dimension: 3 trits (FO,FN,ES) (roles contextuales)
 * Vector:    3 dimensions
 * ────────────────────────────────────────────────────────────── */
typedef struct { Trit t[3]; } Dimension;
typedef struct { Dimension d[3]; } Vector;

typedef enum {
    ROLE_INFORMATIONAL,
    ROLE_COGNITIVE,
    ROLE_ENERGETIC
} VectorRole;

static const char* role_name(VectorRole r) {
    switch(r){
        case ROLE_INFORMATIONAL: return "INFO";
        case ROLE_COGNITIVE:     return "KNOW";
        case ROLE_ENERGETIC:     return "ENERGY";
        default:                 return "?";
    }
}

static VectorRole next_role(VectorRole r) {
    return (r == ROLE_INFORMATIONAL) ? ROLE_COGNITIVE :
           (r == ROLE_COGNITIVE)     ? ROLE_ENERGETIC :
                                      ROLE_INFORMATIONAL;
}

/* Emergency memories: 3 trits (me1,me2,me3) */
typedef struct { Trit me[3]; } EmergencyMemory;

/* ──────────────────────────────────────────────────────────────
 * TRIGATE
 * Modes:
 *   M=c -> AND₃
 *   M=u -> OR₃
 *   M=n -> CONSENSUS (si A==B!=n)
 * ────────────────────────────────────────────────────────────── */
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

/* (A,B,M)->R */
static Trit trigate_infer(Trit a, Trit b, Trit m) {
    if (m == TRIT_C) return trit_and(a,b);
    if (m == TRIT_U) return trit_or(a,b);
    return trit_consensus(a,b);
}

/* (A,B,R)->M : aprende qué modo explica mejor el resultado R */
static Trit trigate_learn(Trit a, Trit b, Trit r) {
    if (trit_and(a,b) == r) return TRIT_C;
    if (trit_or(a,b)  == r) return TRIT_U;
    if (trit_consensus(a,b) == r && r != TRIT_N) return TRIT_N;
    return TRIT_N;
}

/* ──────────────────────────────────────────────────────────────
 * FIB3 SELECTOR (determinista, puro)
 *
 * Nota: esto es un selector estable con una tabla de 27 estados.
 * Empieza como en el Annex: 000→001→002→010→012→020→022...
 * Luego recorre el resto de estados sin repetir.
 *
 * Usamos el estado (3 trits) y elegimos ES.index por dimensión:
 *   es_idx(dim j) = state_digits[j]
 * ────────────────────────────────────────────────────────────── */

typedef struct {
    unsigned idx; /* 0..26 */
} Fib3;

static void fib3_init(Fib3* f) { f->idx = 0; }
static void fib3_next(Fib3* f) { f->idx = (f->idx + 1) % 27; }

/* 27 estados base-3 (cada entrada son 3 dígitos 0..2) */
static const unsigned char FIB3_STATES[27][3] = {
    /* Prefijo del Annex */
    {0,0,0}, /* 000 */
    {0,0,1}, /* 001 */
    {0,0,2}, /* 002 */
    {0,1,0}, /* 010 */
    {0,1,2}, /* 012 */
    {0,2,0}, /* 020 */
    {0,2,2}, /* 022 */
    /* Completar cobertura (sin repetir) */
    {1,0,0}, {1,0,1}, {1,0,2},
    {1,1,0}, {1,1,2},
    {1,2,0}, {1,2,2},
    {2,0,0}, {2,0,1}, {2,0,2},
    {2,1,0}, {2,1,2},
    {2,2,0}, {2,2,2},
    /* Restantes que faltaban */
    {0,1,1}, {0,2,1},
    {1,1,1}, {1,2,1},
    {2,1,1}, {2,2,1},
};

static int fib3_es_idx(const Fib3* f, int dim_j /*0..2*/) {
    return (int)FIB3_STATES[f->idx][dim_j]; /* 0..2 */
}

/* ──────────────────────────────────────────────────────────────
 * VALIDATION: regla absoluta FO.index != ES.index
 *
 * Decisión mínima (limpia):
 * - ES.index: viene del selector Fib3 (por contexto de procesamiento)
 * - FO.index: se infiere del VALOR del trit que ocupa ES.index
 *             (c->0, u->1, n->2)
 * - inválida si FO.index == ES.index
 *
 * Esto mantiene la idea "ES selecciona FO" y preserva FO≠ES.
 * ────────────────────────────────────────────────────────────── */
static int validate_dimension_ctx(const Dimension* d, int es_idx) {
    /* dimensión completamente nula => inválida */
    if (d->t[0]==TRIT_N && d->t[1]==TRIT_N && d->t[2]==TRIT_N) return 0;

    Trit es_val = d->t[es_idx];
    int fo_idx = trit_to_idx(es_val);

    if (fo_idx == es_idx) return 0;
    return 1;
}

static int validate_vector_ctx(const Vector* v, const Fib3* f) {
    for (int j=0;j<3;j++){
        int es_idx = fib3_es_idx(f, j);
        if (!validate_dimension_ctx(&v->d[j], es_idx)) return 0;
    }
    return 1;
}

/* ──────────────────────────────────────────────────────────────
 * COHERENCE METRIC (discreta): nº de nulls
 * ────────────────────────────────────────────────────────────── */
static int nulls_dim(const Dimension* d) {
    int n=0;
    for(int i=0;i<3;i++) if(d->t[i]==TRIT_N) n++;
    return n;
}

/* ──────────────────────────────────────────────────────────────
 * EMERGENCE (minimal y determinista)
 *
 * Input:  d[3]
 * Output: ds + mem(me1..me3)
 *
 * Definición minimal:
 * - Para cada posición k (0..2):
 *     r01 = infer(d0[k], d1[k], d2[2])   (M tomado de ES del tercer dim)
 *     r12 = infer(d1[k], d2[k], d0[2])
 *     r20 = infer(d2[k], d0[k], d1[2])
 *   ds[k] = consensus mayoritario de (r01,r12,r20) (si 2 iguales y !=n)
 *   mem    = {r01, r12, r20} usando k=0 (FO) por simplicidad de núcleo
 *
 * Nota: esto es un core limpio; luego podemos hacer mem por-k o 3 memorias-dimension.
 * ────────────────────────────────────────────────────────────── */
static Trit majority3(Trit a, Trit b, Trit c) {
    if (a != TRIT_N && a == b) return a;
    if (a != TRIT_N && a == c) return a;
    if (b != TRIT_N && b == c) return b;
    return TRIT_N;
}

static void emerge(const Dimension d[3], Dimension* ds, EmergencyMemory* mem) {
    for (int k=0;k<3;k++){
        Trit r01 = trigate_infer(d[0].t[k], d[1].t[k], d[2].t[2]);
        Trit r12 = trigate_infer(d[1].t[k], d[2].t[k], d[0].t[2]);
        Trit r20 = trigate_infer(d[2].t[k], d[0].t[k], d[1].t[2]);
        ds->t[k] = majority3(r01,r12,r20);

        /* mem: guardamos la “tríada rotativa” solo para k=0 en el núcleo minimal */
        if (k==0){
            mem->me[0] = r01;
            mem->me[1] = r12;
            mem->me[2] = r20;
        }
    }
}

/* ──────────────────────────────────────────────────────────────
 * EXTEND (reconstrucción coherente, determinista)
 *
 * No es inversión bit-a-bit: mantiene estructura y baja entropía.
 * - FO: usa ds.FO combinado con mem[i] por OR (conservador)
 * - FN: rota memorias
 * - ES: propaga ds.ES si existe, sino rota memorias
 * ────────────────────────────────────────────────────────────── */
static void extend(const Dimension* ds, const EmergencyMemory* mem, Dimension out[3]) {
    for (int i=0;i<3;i++){
        Trit fo_base = (ds->t[0] != TRIT_N) ? ds->t[0] : TRIT_C;
        out[i].t[0] = trigate_infer(fo_base, mem->me[i], TRIT_U); /* OR */

        out[i].t[1] = mem->me[(i+1)%3];

        out[i].t[2] = (ds->t[2] != TRIT_N) ? ds->t[2] : mem->me[(i+2)%3];
    }
}

/* ──────────────────────────────────────────────────────────────
 * Helpers
 * ────────────────────────────────────────────────────────────── */
static void print_dim(const char* label, const Dimension* d) {
    printf("%s[%s,%s,%s]", label, ts(d->t[0]), ts(d->t[1]), ts(d->t[2]));
}

static void print_triple(const Dimension d[3]) {
    print_dim("", &d[0]); printf(" ");
    print_dim("", &d[1]); printf(" ");
    print_dim("", &d[2]);
}

/* ──────────────────────────────────────────────────────────────
 * TESTS
 * ────────────────────────────────────────────────────────────── */
static void test_validation(void) {
    printf("\n━━ TEST: VALIDATION (FO.index != ES.index) ━━\n");

    Fib3 f; fib3_init(&f);

    Vector v = {{
        {{TRIT_U, TRIT_C, TRIT_C}},
        {{TRIT_C, TRIT_U, TRIT_C}},
        {{TRIT_N, TRIT_C, TRIT_U}}
    }};

    for (int step=0; step<8; step++){
        int ok = validate_vector_ctx(&v, &f);
        printf("step=%d state=%u ESidx=[%d,%d,%d] => %s\n",
               step, f.idx,
               fib3_es_idx(&f,0), fib3_es_idx(&f,1), fib3_es_idx(&f,2),
               ok ? "VALID" : "INVALID");
        fib3_next(&f);
    }
}

static void test_roundtrip(void) {
    printf("\n━━ TEST: ROUNDTRIP (emerge -> extend -> emerge) ━━\n");

    Dimension in[3] = {
        {{TRIT_U, TRIT_C, TRIT_U}},
        {{TRIT_C, TRIT_U, TRIT_C}},
        {{TRIT_U, TRIT_U, TRIT_C}},
    };

    Dimension ds1, ds2;
    EmergencyMemory mem1, mem2;
    Dimension rec[3];

    emerge(in, &ds1, &mem1);
    extend(&ds1, &mem1, rec);
    emerge(rec, &ds2, &mem2);

    int n1 = nulls_dim(&ds1);
    int n2 = nulls_dim(&ds2);

    printf("in:     "); print_triple(in); printf("\n");
    printf("ds1:    "); print_dim("", &ds1); printf("  mem=[%s,%s,%s]\n",
           ts(mem1.me[0]), ts(mem1.me[1]), ts(mem1.me[2]));
    printf("extend: "); print_triple(rec); printf("\n");
    printf("ds2:    "); print_dim("", &ds2); printf("  mem=[%s,%s,%s]\n",
           ts(mem2.me[0]), ts(mem2.me[1]), ts(mem2.me[2]));

    printf("coherence(nulls): %d -> %d  => %s\n",
           n1, n2, (n2 <= n1) ? "OK (no aumenta entropía)" : "WARN (aumenta entropía)");
}

/* Ciclo minimal INFO->KNOW->ENERGY->INFO sobre la misma semilla */
static void demo_cycle_min(void) {
    printf("\n━━ DEMO: ROLE CYCLE (minimal) ━━\n");

    Dimension seed = {{TRIT_U, TRIT_C, TRIT_C}};
    Dimension cur[3] = { seed, seed, seed };
    VectorRole role = ROLE_INFORMATIONAL;

    for (int step=0; step<6; step++){
        Dimension ds;
        EmergencyMemory mem;

        emerge(cur, &ds, &mem);

        printf("step %d role=%s | in: ", step, role_name(role));
        print_triple(cur);
        printf(" | ds: ");
        print_dim("", &ds);
        printf(" | mem=[%s,%s,%s]\n", ts(mem.me[0]), ts(mem.me[1]), ts(mem.me[2]));

        /* avanzar rol y reconstruir */
        role = next_role(role);
        extend(&ds, &mem, cur);
    }
}

int main(void) {
    printf("Aurora Core Minimal — clean start\n");
    test_validation();
    test_roundtrip();
    demo_cycle_min();
    return 0;
}
