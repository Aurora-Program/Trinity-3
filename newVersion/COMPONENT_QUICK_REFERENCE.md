# Aurora Core — Quick Reference & Component Guide
## For Developers Extending the System

---

## Component Architecture at a Glance

```
╔════════════════════════════════════════════════════════════════════╗
║  PIPELINE: SLIDING WINDOW → DESTILADOR → CICLO → EXTENSOR → COMPOSITOR
╠════════════════════════════════════════════════════════════════════╣
║
║   INPUT (Cluster of Vectors)
║        ↓
║   [Selector] — Choose 3 consecutive vectors (T_i, T_{i+1}, T_{i+2})
║        ↓
║   [Validator] — Check ES.index ≠ FO.index (no self-reference)
║        ↓
║   [Counter] — Fibonacci base-3 for ES role position rotation
║        ↓
║   [Destilador] — Homogenize roles → 3 distilled dimensions
║        │         (FO, FN, ES each become homogeneous)
║        ↓
║   [Ciclo Emergente] — Apply emergence to each role
║        │              Generate: ds_superior + memories (me1, me2, me3)
║        ↓
║   [Evolver] — Decide which memory level to propagate:
║        │      - Memoria (first emergence)
║        │      - Razonamiento (second emergence)
║        │      - Intuición (third emergence)
║        ↓
║   [Extensor] — Reconstruct distilled dimensions (guided by breadcrumbs)
║        │       Allow role rotation (FO↔FN↔ES)
║        ↓
║   [Compositor] — Assemble final original dimensions (FO, FN, ES)
║        │         Resolve coherence vs creativity via Trigates
║        ↓
║   [Reproductor] — Convert emergent vector to output tokens
║        ↓
║   [Lógica Combinacional] — Decode command tensor → operation modes
║        │                   (Inferir/Deducir/Aprender)
║        │                   (Aceptar/Reintentar/Desistir)
║        ↓
║   OUTPUT (Te: emergent vector, Tk: knowledge vector, Tc: command)
║
╚════════════════════════════════════════════════════════════════════╝
```

---

## Data Structures Quick Ref

### Primitive

```c
Trit t;     /* -1=null, 0=false, 1=true */
```

### Dimension (3 trits)

```c
Dimension d;    /* d.t[0]=FO, d.t[1]=FN, d.t[2]=ES */
```

### Vector (3 dimensions)

```c
Vector v;       /* v.d[0], v.d[1], v.d[2] */
VectorRole role; /* INFORMATIONAL, COGNITIVE, ENERGETIC */
```

### EmergencyMemory (Breadcrumbs)

```c
EmergencyMemory mem;  /* mem.me[0], mem.me[1], mem.me[2] */
```

---

## Component Functions Quick Ref

### Selector (Window Selection)

```c
int selector(const Vector* cluster, int count, int start_idx, Vector window_out[3]);
/* Returns: 1 if valid window, 0 if index out of bounds */
```

### Counter (Fibonacci Base-3)

```c
FibCounter fc;
fib_init(&fc);              /* Initialize */
fib_next(&fc);              /* Advance to next state */
int es_pos = fib_select_role(&fc) % 3;  /* Select ES position (0,1,2) */
int pos = counter_select_es_position();  /* Deterministic position */
```

### Validator

```c
int validate_dimension(const Dimension* d);  /* Returns: 1 if valid, 0 if invalid */
int validate_vector(const Vector* v);        /* Returns: 1 if all dims valid */
int analizador_error(const Dimension* d);    /* Detects {c,u,n} error state */
```

### Trigate (Core Logic)

```c
Trit trit_infer(Trit a, Trit b, Trit m);           /* (A,B,M)→R */
Trit trit_learn(Trit a, Trit b, Trit r);           /* (A,B,R)→M */
Trit trit_deduce_b(Trit a, Trit m, Trit r);        /* (A,M,R)→B */
Trit trit_and(Trit a, Trit b);
Trit trit_or(Trit a, Trit b);
Trit trit_consensus(Trit a, Trit b);
```

### Emergence Functions

```c
/* Síntesis: 3 dimensions → 1 superior dimension + breadcrumbs */
void emergence_function(
    const Dimension d[3],
    Dimension* ds_out,
    EmergencyMemory* mem_out,
    VectorRole current_role
);

/* Extensión: 1 superior dimension + breadcrumbs → 3 dimensions */
void extend_function(
    const Dimension* ds,
    const EmergencyMemory* mem,
    Dimension d_out[3]
);
```

### Destilador

```c
typedef struct {
    Dimension fo_destilada;  /* FO roles only */
    Dimension fn_destilada;  /* FN roles only */
    Dimension es_destilada;  /* ES roles only */
} DimensionesDestiladas;

void destilador(const Dimension d[3], DimensionesDestiladas* out);
```

### Ciclo Emergente

```c
typedef struct {
    Dimension ds;            /* Superior emergent dimension */
    Vector conocimiento;     /* Knowledge breadcrumbs (3×3 trits) */
} ResultadoEmergencia;

void ciclo_emergente(
    const DimensionesDestiladas* destiladas,
    ResultadoEmergencia* out
);
```

### Extensor

```c
void extensor(
    const Dimension* ds,
    const Vector* conocimiento,
    DimensionesDestiladas* out  /* Reconstructed destilled dimensions */
);
```

### Compositor

```c
void compositor(
    const DimensionesDestiladas* destiladas,
    const Vector* conocimiento,
    Dimension d_out[3]  /* Reassembled original dimensions */
);
```

### Transcender (Full Pipeline)

```c
typedef struct {
    Vector tensor_emergente;   /* Final output vector */
    Vector conocimiento;       /* Final knowledge vector */
    Dimension comando;         /* Command tensor (decoded to modes) */
    Vector te1_sintesis;       /* Synthesis level 1 (bookmark) */
} TranscenderOutput;

void transcender(
    const Vector ventana[3],  /* Input window */
    TranscenderOutput* out
);
```

### Command Decoding

```c
typedef struct {
    CommandMem mem_sel;    /* APRENDER / COMPRENDER / ENTENDER */
    CommandFlow flow;      /* ACEPTAR / REINTENTAR / DESISTIR */
    CommandMode mode;      /* INFERIR / DEDUCIR / APRENDER */
} CommandDecoded;

CommandDecoded decode_command(const Dimension* comando);
PropagacionModo logica_combinacional(const Dimension* comando);
```

### Reproductor (Output)

```c
typedef struct {
    Trit token_trits[9];  /* 9 trits = 1 vector flattened */
} TokenSalida;

void reproductor(const Vector* emergente, TokenSalida* out);
```

### Knowledge Persistence

```c
void save_knowledge(const char* filename);
void load_knowledge(const char* filename);
```

### Interactive Mode

```c
void interactive_aurora_loop(void);  /* REPL: commands e, c, s, l, i, q */
```

---

## Common Patterns

### Creating a Test Dimension

```c
Dimension d = {{TRIT_U, TRIT_C, TRIT_U}};
```

### Creating a Test Vector

```c
Vector v = {{{TRIT_U, TRIT_C, TRIT_U},
             {TRIT_C, TRIT_U, TRIT_C},
             {TRIT_U, TRIT_U, TRIT_C}}};
```

### Running a Simple Emergence

```c
Dimension d[3] = {
    {{TRIT_U, TRIT_C, TRIT_N}},
    {{TRIT_C, TRIT_U, TRIT_U}},
    {{TRIT_U, TRIT_U, TRIT_C}}
};

Dimension ds;
EmergencyMemory mem;
emergence_function(d, &ds, &mem, ROLE_INFORMATIONAL);

printf("Synth: [%s,%s,%s]\n", ts(ds.t[0]), ts(ds.t[1]), ts(ds.t[2]));
printf("Mem:   [%s,%s,%s]\n", ts(mem.me[0]), ts(mem.me[1]), ts(mem.me[2]));
```

### Running a Complete Transcender

```c
Vector cluster[3] = {v1, v2, v3};
TranscenderOutput out;
transcender(cluster, &out);

CommandDecoded cmd = decode_command(&out.comando);
printf("Flow: %s\n", cmd.flow == CMD_FLOW_ACEPTAR ? "Accept" : "Retry/Discard");
```

---

## Role Rotation (Advanced)

Roles (FO/FN/ES) are NOT fixed to positions [0,1,2]. They rotate:

```c
/* During Destilador, roles from level above determine which */
/* dimensions combine in the Compositor */

/* Example: FO from level 1 may guide FN in level 2 */
```

This is controlled by:
- **Contador (Fibonacci):** Determines ES position each cycle
- **Asignador:** Inherits roles from parent level
- **Extensor:** Allows role rotation via `role_rotation` variable

---

## Memory (Knowledge) Architecture

### Three Pyramids (Distributed Knowledge)

1. **Arquetipos (Facts)** — Input patterns → observed outputs
   ```c
   Arquetipo {pattern[3], fo_output, support, rev}
   ```

2. **Dinámicas (Reasoning)** — State transitions
   ```c
   Dinamica {state_before[3], state_after[3], fn_output, support, rev}
   ```

3. **Relatores (Structural)** — Dimension relationships
   ```c
   Relator {dim_a[3], dim_b[3], mode[3], support, rev}
   ```

### Learning Functions

```c
void learn_arquetipo(const Trit pattern[3], Trit fo_out);
void learn_dinamica(const Trit before[3], const Trit after[3], Trit fn_out);
void learn_relator(const Trit a[3], const Trit b[3], const Trit mem[3]);
```

---

## Testing Checklist

- [ ] Dimension validation (ES ≠ FO auto-reference)
- [ ] Trigate inference/learning/deduction
- [ ] Emergence → Extension roundtrip (coherence)
- [ ] Destilador output (3 destilled dimensions)
- [ ] Ciclo emergente (synth + memories)
- [ ] Full Transcender pipeline
- [ ] Command decoding (3 sub-commands)
- [ ] Interactive mode commands (e, c, s, l, i)
- [ ] Knowledge persistence (save/load)

---

## Performance Notes

- **Trigate:** O(1) per operation
- **Emergence:** O(1) per dimension (3 operations)
- **Destilador:** O(3) pairs = O(1)
- **Compositor:** O(9) candidates per trit = O(1) worst-case
- **Cluster Pipeline:** O(N) windows × O(1) per window

---

## Debugging Hints

### "Vector inválido" Message

```c
if (!validate_vector(&v)) {
    printf("  ⚠️  Vector inválido (auto-referencia detectada)\n");
}
```

→ Check if any dimension has ES position pointing to itself.

### "Patrón inválido" Message

→ Dimension contains {0,1,-1} (c,u,n) which triggers error state detection.

### "Cluster insuficiente"

→ Need at least 3 vectors to form a sliding window.

### "Flow: desistir"

→ System decided to abort current processing. Check command decoding.

---

## Extensions & Hooks

### Adding New Components

1. Declare forward: `static void my_component(...);`
2. Define after Transcender section
3. Call from appropriate pipeline stage
4. Test with demo patterns

### Custom Role Behaviors

Modify `VectorRole next_role_in_cycle()` to change role sequence:
```c
/* Default: INFO → COGNITIVE → ENERGETIC → INFO */
/* Can customize for domain-specific reasoning */
```

### Custom Memory Learning

Hook into `emergence_function()` calls within each role:
```c
if (current_role == ROLE_ENERGETIC) {
    /* Custom energetic learning */
}
```

---

## References

- **File:** `aurora_core_refactored.c` (v3.1.1)
- **Spec:** `OPERATIONAL_SPEC_MAPPING.md` (this document)
- **Full Theory:** See `.github/instructions/` for technical annex & whitepaper

---

## Quick Start Commands

```bash
# Compile
gcc aurora_core_refactored.c -o aurora_core_refactored.exe

# Run demo
./aurora_core_refactored.exe

# Interactive mode
./aurora_core_refactored.exe -i
# Then type: e u c n c u u u c c
# Or: c u c n
# Or: s myknowledge.bin

# Load previously saved knowledge
./aurora_core_refactored.exe --load aurora_knowledge.bin
```

---

**Version:** 3.1.1  
**Last Updated:** 2026-01-25  
**License:** Apache 2.0 + CC BY 4.0
