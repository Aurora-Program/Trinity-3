# Aurora v3.0 - Arquitectura Modular

## Organización del Código

Esta versión separa el **núcleo Aurora** (reutilizable) del **código de demostración** (específico de aplicación).

### Estructura de Archivos

```
v3.0/
├── aurora_core.h      # Header del núcleo Aurora (tipos + API)
├── aurora_core.c      # Implementación del núcleo (trigate, síntesis, harmonizer, LOP, etc.)
├── aurora_core.o      # Objeto compilado del núcleo
├── syllables_demo.c   # Demo específico: silabificación
└── syllables_demo.exe # Ejecutable del demo
```

---

## Núcleo Aurora (`aurora_core.h` + `aurora_core.c`)

### Responsabilidad
Implementa **toda la lógica universal** del Modelo Aurora sin conocimiento de dominio:

#### 1. **Trigate: Átomo de Inteligencia**
- `trigate_infer(a, b, m)` → Inferencia ternaria
- `trigate_learn(a, b, r)` → Aprendizaje del modo M
- Operaciones: `trit_and`, `trit_or`, `trit_consensus`

#### 2. **Tensores Fractales (1→3→9)**
- `DimensionFFE` (3 trits)
- `VectorFFE_Fractal` (3 dimensiones)
- `TensorFFE_Fractal` (3 vectores)
- `TensorFFE` (plano legacy: FO/FN/ES)

Constructores:
- `make_dim(a, b, c)`
- `make_vec_f(d0, d1, d2)`
- `make_tensor_f(v0, v1, v2)`
- `fractal_to_flat(tf)`

#### 3. **Pirámides de Conocimiento**
- **Relatores** (`Rule`): relaciones aprendidas (a, b, M)
- **Arquetipos** (`Archetype`): patrones emergentes
- **Dinámicas** (`DynRule`, `DynArchetype`): reglas temporales

Funciones:
- `upsert_rule_mem()` - Almacenar reglas con consensus
- `synthesize_archetypes()` - Síntesis de patrones emergentes
- `upsert_dyn_rule_mem()` / `synthesize_dyn_archetypes()`

#### 4. **Algoritmo de Dios: Armonizador**
- `harmonize_with_fibonacci(t)` - Minimización geométrica de nulls
- `harmonize_guided(t, C)` - Armonización con ancla C

#### 5. **Creencia C: Tensor de Referencia**
- `build_creencia_tensor_from_pyramids(VR, VA, VD)` - Síntesis triádica
- `anneal_creencia_tensor(C, temp)` - Refinamiento por temperatura
- `extract_Cref_from_C(C)` - Valor escalar de referencia

#### 6. **Diagnóstico Meta-Cognitivo**
- `diagnose_rules()` → `DiagnosticMetrics` (consistency, separability, convergence)
- `build_diagnostic_tensor(d, Cref)` - Tensor de diagnóstico
- `build_lop_tensor(d, acc, Cref)` - Tensor LOP (Libertad-Orden-Propósito)

#### 7. **Causas Emergentes**
- `emergent_cause_with_lop()` - Clasificación de error (falta info / mínimo local / base incorrecta)
- `emergent_cluster_cause()` - Síntesis triádica R+A+D

#### 8. **Extender (Aprendizaje desde Salida)**
- `ExtenderRule` - Paradigma output-first
- `upsert_extender_rule()` - Entrenar desde secuencias de salida

---

## Demo (`syllables_demo.c`)

### Responsabilidad
Implementa **la aplicación específica** de silabificación:

1. **Datos de entrenamiento** (`Example`)
2. **Encoding de caracteres** (3 versiones: fonética, silábica, binaria)
3. **Entrenamiento tradicional** (pares de entrada)
4. **Evaluación comparativa**
5. **Pipeline de ejecución**:
   - Fase 1: Comparar encodings
   - Fase 2: Sintetizar arquetipos
   - Fase 3: Construir Creencia C
   - Fase 4: Diagnóstico LOP + causas emergentes
   - Fase 5: Multi-época (si aplica)
   - Predicciones demo

---

## Compilación

### Paso 1: Compilar el núcleo (una sola vez)
```bash
gcc -std=c11 -c -O2 -Wall aurora_core.c -o aurora_core.o
```

### Paso 2: Compilar el demo (enlazando con el núcleo)
```bash
gcc -std=c11 -O2 -Wall -o syllables_demo syllables_demo.c aurora_core.o
```

### Paso 3: Ejecutar
```bash
./syllables_demo        # 1 época (default)
./syllables_demo 3      # 3 épocas
```

---

## Ventajas de la Arquitectura Modular

### ✅ **Reutilización**
El núcleo (`aurora_core.o`) puede usarse en **múltiples demos**:
- Silabificación (`syllables_demo.c`)
- Clasificación de texto
- Análisis de sentimiento
- Cualquier aplicación de inferencia ternaria

### ✅ **Mantenibilidad**
- **Núcleo**: lógica universal (invariante)
- **Demo**: lógica de dominio (variable)
- Cambios en uno no afectan al otro

### ✅ **Claridad Conceptual**
Respeta el principio del whitepaper:
> "La inteligencia NO está en el código, está en la GEOMETRÍA del tensor"

El núcleo solo implementa **operaciones genéricas** (trigate, síntesis, harmonización).
La semántica viene de los **tensores** (definidos en el demo).

### ✅ **Facilita Refactors Mayores**
- **Extender paradigma**: solo cambiar `syllables_demo.c`
- **Tensores fractales completos**: solo modificar encoding en demo
- **Eliminar hardcoded thresholds**: refactor en `aurora_core.c` sin tocar demos

---

## Próximos Pasos (Whitepaper Alignment)

### 1. **Integrar Extender Learning** (demo)
- Entrenar desde "ca-sa" completa (output-first)
- Usar TOKEN_SEP '-' como tensor especial
- Implementar `train_from_output_sequences()`

### 2. **Tensores Fractales Full** (core + demo)
- Refactorizar `Rule`/`Archetype` para operar con `TensorFFE_Fractal`
- Encoding jerárquico 1→3→9 donde nivel superior determina espacio inferior
- Eliminar wrapper `fractal_to_flat()`

### 3. **Eliminar Hardcoded Thresholds** (core)
- Reemplazar `metric_to_trit(0.66/0.33)` con emergencia geométrica
- Reemplazar `count>=2` con coherencia fractal
- Thresholds emergen de dimensión superior

### 4. **Paralelización** (opcional, core)
- OpenMP para síntesis simultánea de R/VA/VD/baselines

---

## Filosofía del Diseño

```
┌─────────────────────────────────────────────────┐
│  AURORA CORE (aurora_core.h/.c)                 │
│  ─────────────────────────────────────────────  │
│  • Trigate (AND/OR/CONSENSUS)                   │
│  • Tensores fractales (1→3→9)                   │
│  • Armonizador (Fibonacci + colapso triádico)   │
│  • Síntesis (archetipos, C, LOP)                │
│  • Diagnóstico + causas emergentes              │
│                                                  │
│  → Código UNIVERSAL (sin dominio)               │
│  → "Algoritmo de Dios": proceso fijo            │
└─────────────────────────────────────────────────┘
                      ▲
                      │ usa (link)
                      │
┌─────────────────────────────────────────────────┐
│  SYLLABLES DEMO (syllables_demo.c)              │
│  ─────────────────────────────────────────────  │
│  • Datos de entrenamiento (ejemplos)            │
│  • Encoding de caracteres (V1/V2/V3)            │
│  • Pipeline específico (fases 1-5)              │
│  • Evaluación de silabificación                 │
│                                                  │
│  → Código de APLICACIÓN (con dominio)           │
│  → "Conocimiento": tensores específicos         │
└─────────────────────────────────────────────────┘
```

---

## Licencias

- **Apache 2.0**: Permite uso, modificación, redistribución (mantener avisos)
- **CC BY 4.0**: Requiere atribución al proyecto Aurora

---

**Contacto**: Aurora Intelligence Project
**Versión**: 3.0 - Modular Core (Jan 2025)
