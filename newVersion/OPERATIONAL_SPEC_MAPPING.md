# Aurora Core — Operational Specification Mapping
## Technical Annex § v1.1 → C Implementation (aurora_core_refactored.c)

---

## Overview

The **Operational Specification** (attached) defines the formal structure of Aurora's Transcender as a pipeline of components. This document maps each specification section to its corresponding C implementation in `aurora_core_refactored.c` (v3.1.1).

---

## Data Structure Mapping

### § 1. Trit (Unidad Mínima de Información)

**Spec:** Trit = 0 | 1 | 2 (ternary digit)  
**Implementation:**

```c
typedef int Trit;
#define TRIT_U  1   /* u: true/upper/correct */
#define TRIT_C  0   /* c: center/false */
#define TRIT_N -1   /* n: null/indeterminate */
```

**Note:** Implementation uses -1 for null (TRIT_N) for mathematical convenience; maps naturally to three-state logic.

---

### § 2. Dimensión (conjunto de 3 trits)

**Spec:** Dimensión: {trit₁-trit₂-trit₃} with roles FO/FN/ES (contextual)  
**Implementation:**

```c
typedef struct {
    Trit t[3]; /* [FO, FN, ES] - roles contextuales */
} Dimension;
```

---

### § 3. Vector (conjunto de 3 dimensiones)

**Spec:** Vector: ({d₁}, {d₂}, {d₃}) — unidad operativa universal  
**Implementation:**

```c
typedef struct {
    Dimension d[3]; /* [Dim0, Dim1, Dim2] */
} Vector;

typedef enum {
    ROLE_INFORMATIONAL,  /* (value, data, mode) */
    ROLE_COGNITIVE,      /* (relator, dynamics, archetype) */
    ROLE_ENERGETIC       /* (tension, energy, command) */
} VectorRole;
```

---

### § 4-5. Tensor (Nivel Vectorial) & Tensor Fractal

**Spec:** 3-level fractal: {d1} / ({d2},{d3},{d4}) / [(...), (...), (...)]  
**Implementation:**

```c
typedef struct {
    Dimension synthesis;    /* Nivel 1 */
    Vector base;            /* Nivel 2 */
    EmergencyMemory memory; /* Emergencia reversible */
    VectorRole role;        /* Rol contextual */
} TensorBasic;

typedef struct {
    Dimension level1;       /* 1 dimensión */
    Vector level2;          /* 3 dimensiones */
    TensorBasic level3[3];  /* 9 dimensiones */
} TensorAurora;
```

---

### § 6. Cluster de Tensores & § 7. Ventana de Operación

**Spec:** Sliding window (T₁, T₂, T₃) → (T₂, T₃, T₄) → ...  
**Implementation:**

```c
#define MAX_CLUSTER 64

/* Selector: ventana deslizante */
static int selector(const Vector* cluster, int count, int start_idx, Vector window_out[3]) {
    if (start_idx < 0 || start_idx + 2 >= count) return 0;
    window_out[0] = cluster[start_idx];
    window_out[1] = cluster[start_idx + 1];
    window_out[2] = cluster[start_idx + 2];
    return 1;
}
```

---

## Core Components Mapping

### § 8. Contador (Fibonacci base 3)

**Spec:** State evolution: C_(n+1) = (C_n + C_(n-1)) mod 3  
- Generates 39 trits (3 dimensions × 3 tensores × 3 levels)
- Determines position of role ES within each dimension
- No heuristics; purely structural modulation

**Implementation:**

```c
typedef struct {
    int state;  /* Current state */
    int fib_a;  /* Previous Fibonacci */
    int fib_b;  /* Current Fibonacci */
} FibCounter;

static void fib_next(FibCounter* fc) {
    int temp = fc->fib_a + fc->fib_b;
    fc->fib_a = fc->fib_b;
    fc->fib_b = temp;
    fc->state = (fc->state + 1) % 27;
}

/* Selector determinista de posición ES */
static int counter_select_es_position(void) {
    static FibCounter fc_spec;
    static int initialized = 0;
    if (!initialized) {
        fib_init(&fc_spec);
        initialized = 1;
    }
    fib_next(&fc_spec);
    return (fc_spec.fib_b % 3);
}
```

---

### § 9. Validador

**Spec:** Verify structural validity:
- Rule: ES.index ≠ FO.index (no self-reference)
- Reject dimension {0-1-2} (error state)
- ES=null case: verify no trit maps to ES position

**Implementation:**

```c
static int validate_dimension(const Dimension* d) {
    /* Null check */
    if (d->t[0] == TRIT_N && d->t[1] == TRIT_N && d->t[2] == TRIT_N) {
        return 0;
    }

    /* Error state {c,u,n} */
    if (d->t[0] == TRIT_C && d->t[1] == TRIT_U && d->t[2] == TRIT_N) {
        return 0;
    }

    /* Select ES index via Fibonacci counter */
    int es_idx = counter_select_es_position();
    Trit es_val = d->t[es_idx];
    
    if (es_val == TRIT_N) {
        /* Verify no auto-reference */
        for (int i = 0; i < 3; i++) {
            int fo_idx_from_val = es_val_to_fo_idx(d->t[i]);
            if (fo_idx_from_val == es_idx) {
                return 0; /* Auto-reference detected */
            }
        }
        return 1;
    }

    int fo_idx = es_val_to_fo_idx(es_val);
    if (fo_idx == es_idx) return 0; /* ES cannot point to itself */
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
```

---

### § 10. Analizador de Error

**Spec:** Detect invalid structural state {0-1-2}  
- Signals when all trits simultaneously self-assign ES role
- No correction; marks operation as erroneous

**Implementation:**

```c
static int analizador_error(const Dimension* d) {
    return (d->t[0] == TRIT_C && d->t[1] == TRIT_U && d->t[2] == TRIT_N);
}
```

---

### § 4. Trigate (Átomo de Inteligencia)

**Spec:** 4-element reversible unit:
- Modes: Inference (0), Learning (1), Deduction (2)
- Operators: AND₃ (c=0), OR₃ (u=1), CONSENSUS (n=2)
- No stored state; all functions stateless

**Implementation:**

```c
/* Inference: (A,B,M) → R */
static Trit trit_infer(Trit a, Trit b, Trit m) {
    if (m == TRIT_C) return trit_and(a, b);
    if (m == TRIT_U) return trit_or(a, b);
    return trit_consensus(a, b);
}

/* Learning: (A,B,R) → M */
static Trit trit_learn(Trit a, Trit b, Trit r) {
    if (trit_and(a, b) == r) return TRIT_C;
    if (trit_or(a, b) == r) return TRIT_U;
    if (a != TRIT_N && a == b && r == a) return TRIT_N;
    return TRIT_N;
}

/* Deduction: (A,M,R) → B */
static Trit trit_deduce_b(Trit a, Trit m, Trit r) { /* ... */ }
```

---

### § 6. Función de Emergencia (Síntesis & Extensión)

**Spec:** Reversible function for information scaling:
- **Síntesis:** Dimensiones destiladas → Dimensión superior emergente + Memoria (me1, me2, me3)
- **Extensión:** Dimensión emergente → 3 Dimensiones destiladas (mediante breadcrumbs)

**Implementation:**

```c
typedef struct {
    Trit me[3]; /* Breadcrumbs: me1, me2, me3 */
} EmergencyMemory;

static void emergence_function(
    const Dimension d[3],
    Dimension* ds_out,
    EmergencyMemory* mem_out,
    VectorRole current_role
) {
    /* Triadic inference: FO-pairs, FN-pairs, ES-pairs */
    Trit rFO[3], rFN[3], rES[3];
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        rFO[i] = trit_infer(d[i].t[0], d[j].t[0], d[i].t[2]);
        rFN[i] = trit_infer(d[i].t[1], d[j].t[1], d[i].t[0]);
        rES[i] = trit_infer(d[i].t[2], d[j].t[2], d[i].t[1]);
    }

    /* Synthesis via triadic collapse */
    ds_out->t[0] = triadic_collapse(rFO[0], rFO[1], rFO[2]);
    ds_out->t[1] = triadic_collapse(rFN[0], rFN[1], rFN[2]);
    ds_out->t[2] = triadic_collapse(rES[0], rES[1], rES[2]);

    /* Store breadcrumbs */
    mem_out->me[0] = rFO[0];
    mem_out->me[1] = rFO[1];
    mem_out->me[2] = rFO[2];
}

static void extend_function(
    const Dimension* ds,
    const EmergencyMemory* mem,
    Dimension d_out[3]
) {
    /* Guided reconstruction using memory breadcrumbs */
    for (int i = 0; i < 3; i++) {
        Trit m_fo = (ds->t[1] != TRIT_N) ? ds->t[1] : TRIT_U;
        d_out[i].t[0] = trit_infer(ds->t[0], mem->me[i], m_fo);
        /* ... más reconstrucción ... */
    }
}
```

---

### § 12. Destilador (Homogeneización Estructural)

**Spec:** Process three dimensions → three **distilled dimensions** (all trits share same role):
- ES destilada: direct recopilation {ES₁, ES₂, ES₃}
- FO destilada: Trigates on (d₁–d₂), (d₂–d₃), (d₃–d₁)
- FN destilada: coupled with FO via learned modes

**Implementation:**

```c
typedef struct {
    Dimension fo_destilada;
    Dimension fn_destilada;
    Dimension es_destilada;
} DimensionesDestiladas;

static void destilador(const Dimension d[3], DimensionesDestiladas* out) {
    /* ES direct aggregation */
    out->es_destilada.t[0] = d[0].t[2];
    out->es_destilada.t[1] = d[1].t[2];
    out->es_destilada.t[2] = d[2].t[2];

    /* FO/FN Trigates on pairs */
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        Trit fo_a = d[i].t[0], fo_b = d[j].t[0];
        Trit fn_a = d[i].t[1], fn_b = d[j].t[1];
        
        /* Learn mode from FN */
        Trit m_fn = trit_infer(fn_a, fn_b, TRIT_N);
        Trit r_fo = trit_infer(fo_a, fo_b, m_fn);
        
        out->fo_destilada.t[i] = r_fo;
        out->fn_destilada.t[i] = m_fn;
    }
}
```

---

### § 13. Ciclo Emergente (Síntesis Multinivel)

**Spec:** Apply emergence functions to destilled dimensions:
- For each role (FO, FN, ES): generate superior dimension + knowledge tensor
- Compose final **vector emergente** + **vector conocimiento**

**Implementation:**

```c
typedef struct {
    Dimension ds;        /* Superior emergent dimension */
    Vector conocimiento; /* Knowledge breadcrumbs */
} ResultadoEmergencia;

static void ciclo_emergente(
    const DimensionesDestiladas* destiladas,
    ResultadoEmergencia* out
) {
    /* Process FO destilada */
    Dimension temp[3];
    EmergencyMemory mem[3];
    
    temp[0] = temp[1] = temp[2] = destiladas->fo_destilada;
    emergence_function(temp, &out->ds, &mem[0], ROLE_INFORMATIONAL);
    
    out->conocimiento.d[0].t[0] = mem[0].me[0];
    out->conocimiento.d[0].t[1] = mem[0].me[1];
    out->conocimiento.d[0].t[2] = mem[0].me[2];
    
    /* Repeat for FN and ES ... */
}
```

---

### § 15. Extensor (Desarrollador / Reconstrucción)

**Spec:** Reverse the emergence:
- Input: Dimensión emergente superior + vector conocimiento
- Output: 3 dimensiones destiladas (reconstruidas)
- Role rotation allowed (FO→FN, etc.)

**Implementation:**

```c
static void extensor(
    const Dimension* ds,
    const Vector* conocimiento,
    DimensionesDestiladas* out
) {
    int role_rotation = fib_select_role(&global_fib_counter) % 3;
    
    /* Reconstruir cada rol con rotación de conocimiento */
    int fo_src = role_rotation;
    EmergencyMemory mem;
    mem.me[0] = conocimiento->d[fo_src].t[0];
    mem.me[1] = conocimiento->d[fo_src].t[1];
    mem.me[2] = conocimiento->d[fo_src].t[2];
    
    Dimension temp_out[3];
    extend_function(ds, &mem, temp_out);
    out->fo_destilada = temp_out[0];
    /* ... más reconstrucción ... */
}
```

---

### § 16. Compositor (Reconstrucción Final de Dimensiones)

**Spec:** 6 Trigates (3 pares) resolving coherence vs creativity:
- Entrada: Dimensiones destiladas extendidas + conocimiento
- Salida: 3 dimensiones originales (reensam bladas)

**Implementation:**

```c
static void compositor(
    const DimensionesDestiladas* destiladas,
    const Vector* conocimiento,
    Dimension d_out[3]
) {
    for (int i = 0; i < 3; i++) {
        /* Trigate 1: Coherencia (r1,m1) */
        Trit r1 = destiladas->fo_destilada.t[i];
        Trit m1 = conocimiento->d[i].t[1];
        
        /* Trigate 2: Creatividad (r2,m2) */
        Trit r2 = conocimiento->d[i].t[0];
        Trit m2 = destiladas->fn_destilada.t[i];
        
        /* Buscar (A,B) tal que A⊗B(M1)→R1 y A⊗B(M2)→R2 */
        Trit best_a = r1, best_b = TRIT_C;
        float best_score = -1.0f;
        
        for (int ia = 0; ia < 3; ia++) {
            for (int ib = 0; ib < 3; ib++) {
                Trit a = (ia == 0 ? TRIT_C : ia == 1 ? TRIT_U : TRIT_N);
                Trit b = (ib == 0 ? TRIT_C : ib == 1 ? TRIT_U : TRIT_N);
                
                Trit r_coh = trit_infer(a, b, m1);
                if (r_coh != r1) continue;
                
                Trit r_crea = trit_infer(a, b, m2);
                if (r2 != TRIT_N && r_crea != r2) continue;
                
                float sc = score_candidate(a, b, r_crea, r2, destiladas->es_destilada.t[i]);
                if (sc > best_score) {
                    best_score = sc;
                    best_a = a;
                    best_b = b;
                }
            }
        }
        
        d_out[i].t[0] = best_a;
        d_out[i].t[1] = m2;
        d_out[i].t[2] = destiladas->es_destilada.t[i];
    }
}
```

---

### § 11. Lógica Combinacional

**Spec:** Deterministic function propagating command mode to all components:
- Decodifica trits de comando
- Genera señales: Inferencia / Aprendizaje / Razonamiento
- Distribuye a Trigates, funciones de emergencia, armonizadores

**Implementation:**

```c
typedef enum { CMD_MEM_APRENDER, CMD_MEM_COMPRENDER, CMD_MEM_ENTENDER } CommandMem;
typedef enum { CMD_FLOW_ACEPTAR, CMD_FLOW_REINTENTAR, CMD_FLOW_DESISTIR } CommandFlow;
typedef enum { CMD_MODE_INFERIR, CMD_MODE_DEDUCIR, CMD_MODE_APRENDER } CommandMode;

typedef struct {
    CommandMem mem_sel;
    CommandFlow flow;
    CommandMode mode;
} CommandDecoded;

static CommandDecoded decode_command(const Dimension* comando) {
    CommandDecoded c;
    c.mem_sel = (comando->t[2] == TRIT_U) ? CMD_MEM_COMPRENDER :
                (comando->t[2] == TRIT_N) ? CMD_MEM_ENTENDER : CMD_MEM_APRENDER;
    c.flow = (comando->t[0] == TRIT_U) ? CMD_FLOW_REINTENTAR :
             (comando->t[0] == TRIT_N) ? CMD_FLOW_DESISTIR : CMD_FLOW_ACEPTAR;
    c.mode = (comando->t[1] == TRIT_U) ? CMD_MODE_DEDUCIR :
             (comando->t[1] == TRIT_N) ? CMD_MODE_APRENDER : CMD_MODE_INFERIR;
    return c;
}

static PropagacionModo logica_combinacional(const Dimension* comando) {
    PropagacionModo p;
    p.decoded = decode_command(comando);
    return p;
}
```

---

### § 17. Reproductor

**Spec:** Transform emergent tensors to comprehensible output (tokens):
- Reverse translation mapping
- No new information; pure reversible translation

**Implementation:**

```c
typedef struct {
    Trit token_trits[9];
} TokenSalida;

static void reproductor(const Vector* emergente, TokenSalida* out) {
    int k = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            out->token_trits[k++] = emergente->d[i].t[j];
        }
    }
}
```

---

## Pipeline Integral (Transcender)

**Spec § 18-20:** Complete pipeline: Ventana → Destilador → Ciclo Emergente → Extensor → Compositor

**Implementation:**

```c
static void transcender(
    const Vector ventana[3],  /* T1, T2, T3 */
    TranscenderOutput* out
) {
    int attempts = 0;
    while (attempts < 2) {
        /* Fase 1: Destilador */
        DimensionesDestiladas dest[3];
        for (int i = 0; i < 3; i++) {
            Dimension combo[3];
            combo[0] = ventana[0].d[i];
            combo[1] = ventana[1].d[i];
            combo[2] = ventana[2].d[i];
            destilador(combo, &dest[i]);
        }

        /* Fase 2: Ciclo Emergente */
        ResultadoEmergencia emerg[3];
        for (int i = 0; i < 3; i++) {
            ciclo_emergente(&dest[i], &emerg[i]);
        }

        out->tensor_emergente.d[0] = emerg[0].ds;
        out->tensor_emergente.d[1] = emerg[1].ds;
        out->tensor_emergente.d[2] = emerg[2].ds;
        out->te1_sintesis = out->tensor_emergente;

        /* Generar comando */
        Dimension temp_cmd[3] = { emerg[0].ds, emerg[1].ds, emerg[2].ds };
        EmergencyMemory mem_cmd;
        emergence_function(temp_cmd, &out->comando, &mem_cmd, ROLE_COGNITIVE);

        CommandDecoded cmd = decode_command(&out->comando);
        
        /* Fase 3: Extensor */
        DimensionesDestiladas dest_rec[3];
        int mem_idx = (cmd.mem_sel == CMD_MEM_COMPRENDER) ? 1 : 
                      (cmd.mem_sel == CMD_MEM_ENTENDER ? 2 : 0);
        for (int i = 0; i < 3; i++) {
            extensor(&emerg[i].ds, &emerg[mem_idx].conocimiento, &dest_rec[i]);
        }

        /* Fase 4: Compositor */
        Dimension dims_out[3];
        for (int i = 0; i < 3; i++) {
            compositor(&dest_rec[i], &emerg[mem_idx].conocimiento, dims_out);
            out->tensor_emergente.d[i] = dims_out[i];
        }

        /* Aplicar modo y control de flujo */
        apply_mode_learning(cmd.mode, &emerg[mem_idx]);

        if (cmd.flow == CMD_FLOW_DESISTIR) {
            fib_init(&global_fib_counter);
            null_vector(&out->tensor_emergente);
            return;
        }
        if (cmd.flow == CMD_FLOW_REINTENTAR) {
            fib_next(&global_fib_counter);
            attempts++;
            continue;
        }
        break; /* Aceptado */
    }
}
```

---

## Knowledge Architecture (Spec § 19-24)

**Spec:** Three distributed memories corresponding to three Transcenders:

### Transcender A — Hechos (Arquetipos)

```c
typedef struct {
    Trit pattern[3];
    Trit fo_output;
    int support;
    unsigned long rev;
} Arquetipo;

static Arquetipo arquetipos[MAX_MEM];
static int n_arquetipos = 0;
```

### Transcender B — Razonamientos (Dinámicas)

```c
typedef struct {
    Trit state_before[3];
    Trit state_after[3];
    Trit fn_output;
    int support;
    unsigned long rev;
} Dinamica;

static Dinamica dinamicas[MAX_MEM];
static int n_dinamicas = 0;
```

### Transcender C — Relatores (Estructurales)

```c
typedef struct {
    Trit dim_a[3], dim_b[3];
    Trit mode[3];
    int support;
    unsigned long rev;
} Relator;

static Relator relatores[MAX_MEM];
static int n_relatores = 0;
```

---

## Complete Cycle (Spec § 25-27)

**Spec:** Information → Knowledge → Energy → Information

**Implementation:**

```c
static VectorRole next_role_in_cycle(VectorRole current) {
    switch(current) {
        case ROLE_INFORMATIONAL: return ROLE_COGNITIVE;
        case ROLE_COGNITIVE: return ROLE_ENERGETIC;
        case ROLE_ENERGETIC: return ROLE_INFORMATIONAL;
        default: return ROLE_INFORMATIONAL;
    }
}

static void process_complete_cycle(Dimension* input, int cycles) {
    fib_init(&global_fib_counter);

    VectorRole current_role = ROLE_INFORMATIONAL;
    Dimension current[3];
    Dimension superior;
    EmergencyMemory mem;
    
    /* Initialize with input */
    for (int i = 0; i < 3; i++) {
        copy_dim(&current[i], input);
    }
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        Vector v_temp = {{current[0], current[1], current[2]}};
        if (!validate_vector(&v_temp)) break;
        
        emergence_function(current, &superior, &mem, current_role);
        
        current_role = next_role_in_cycle(current_role);
        extend_function(&superior, &mem, current);
    }
}
```

---

## Persistence & Interactive Mode

**Spec:** (New in v3.1) Knowledge persistence + REPL loop

**Implementation:**

```c
static void save_knowledge(const char* filename) {
    FILE* f = fopen(filename, "wb");
    fwrite(&n_arquetipos, sizeof(int), 1, f);
    fwrite(&n_dinamicas, sizeof(int), 1, f);
    fwrite(&n_relatores, sizeof(int), 1, f);
    fwrite(arquetipos, sizeof(Arquetipo), n_arquetipos, f);
    fwrite(dinamicas, sizeof(Dinamica), n_dinamicas, f);
    fwrite(relatores, sizeof(Relator), n_relatores, f);
    fclose(f);
}

static void load_knowledge(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return;
    fread(&n_arquetipos, sizeof(int), 1, f);
    fread(&n_dinamicas, sizeof(int), 1, f);
    fread(&n_relatores, sizeof(int), 1, f);
    fread(arquetipos, sizeof(Arquetipo), n_arquetipos, f);
    fread(dinamicas, sizeof(Dinamica), n_dinamicas, f);
    fread(relatores, sizeof(Relator), n_relatores, f);
    fclose(f);
}

static void interactive_aurora_loop(void) {
    /* REPL loop with commands:
       - 'e <9 trits>' : Emergencia
       - 'c <3 trits>' : Ciclo completo
       - 's <file>'    : Guardar
       - 'l <file>'    : Cargar
       - 'i'           : Info
       - 'q'           : Salir
    */
}
```

---

## Compilation & Testing

**Build:**
```bash
gcc aurora_core_refactored.c -o aurora_core_refactored.exe
```

**Run:**
```bash
# Demo
./aurora_core_refactored.exe

# Interactive mode
./aurora_core_refactored.exe -i

# Load saved knowledge
./aurora_core_refactored.exe --load aurora_knowledge.bin
```

**Output:**
- Test emergence roundtrip (coherence verification)
- Phase 1: Pattern learning (arquetipos, dinámicas, relatores)
- Phase 1.1: Cluster tensorial with sliding windows
- Phase 2: Complete cycle (Information → Knowledge → Energy)
- Phase 3: Validation tests (ES.index ≠ FO.index)
- Save/restore knowledge persistence

---

## Key Design Principles

1. **Autosimilarity:** Same Trigate operations at all levels
2. **Fractal Efficiency:** Only compute active dimensions
3. **Structural Coherence:** ES prevents self-reference via deterministic counter
4. **Reversibility:** Emergence ↔ Extension preserve relational information
5. **Role Rotation:** Roles (FO/FN/ES) are contextual, not fixed
6. **Distributed Memory:** Three separate knowledge pyramids per Transcender
7. **No Semantics in Core:** All operations purely structural
8. **Closed-Loop Learning:** System evolves from emergences, no external heuristics

---

## File Structure

```
c/
  ├─ aurora_core_refactored.c    [THIS IMPLEMENTATION]
  ├─ aurora_core_refactored.exe   [Compiled binary]
  └─ aurora_knowledge.bin         [Persisted knowledge]
```

---

## Version History

- **v3.0:** Initial implementation with Trigate, Emergencia, Ciclo
- **v3.0.1:** ES validation + Fibonacci counter
- **v3.1:** Destilador, Ciclo Emergente, Extensor, Compositor (full pipeline)
- **v3.1.1:** Wrappers for Selector, Analizador, Lógica Combinacional, Reproductor

---

## References

- [operational-specification.md](./operational-specification.md) — Full formal spec
- [auroraprogrammodel.py.instructions.md](../.github/instructions/auroraprogrammodel.py.instructions.md) — Aurora model overview
- [Technical-Annex.instructions.md](../.github/instructions/Technical-Annex.instructions.md) — Energetic & coherence theory

---

**Document Generated:** 2026-01-25  
**Aurora Core Version:** 3.1.1 (Trigate-Pure, Technical Annex § Compliant)  
**License:** Apache 2.0 + CC BY 4.0
