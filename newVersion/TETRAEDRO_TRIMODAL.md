# El Tetraedro Trimodal: La Última Pieza del Puzzle

## Revelación Fundamental

No existen tres tetraedros separados (Relator/Arquetipo/Dinámico).

**Existe UN SOLO tetraedro que opera en tres modos energéticos distintos.**

El modo activo depende de qué dimensión domina la energía del sistema:
- **FO dominante** → Modo Operativo (calcular, crear, explorar)
- **FN dominante** → Modo Gestión (decidir, dirigir, propósito)
- **ES dominante** → Modo Memoria (ordenar, consolidar, estabilizar)

## Los Tres Modos del Tetraedro Único

### 1. Modo Operativo (FO dominante - Libertad)
**Función**: Cálculo, creatividad, exploración, interacción con el usuario.

**Estado**: Los nulls son altos, la diversidad es máxima, el sistema está expandiéndose.

**Comportamiento**:
- Sintetizador: combina FO libremente, genera nuevas formas
- Evolver: explora nuevos modos, prueba hipótesis
- Extender: despliega resultados al usuario
- Armonizador: permite cierta incoherencia para explorar

**Cuándo**: Durante la interacción activa, cuando el usuario aporta inputs nuevos.

**Salida característica**: Respuestas creativas, síntesis emergentes, outputs comunicativos.

### 2. Modo Gestión (FN dominante - Propósito)
**Función**: Dirección, decisión estratégica, corrección de errores, ajuste de prioridades.

**Estado**: El sistema detecta tensiones entre L-O-P y debe reorganizar.

**Comportamiento**:
- Sintetizador: revisa coherencia global
- Evolver: ajusta arquetipos según propósito
- Extender: verifica que dinámicas sean consistentes
- Armonizador: **activo intenso**, reorganiza FO/FN/ES, usa Fibonacci para decidir

**Cuándo**: Cuando coherence_failures >= 3, o LOP desequilibrado, o contradicciones detectadas.

**Salida característica**: Reorganización interna, poda selectiva, ajuste de creencias.

### 3. Modo Memoria (ES dominante - Orden)
**Función**: Consolidación, compresión, estabilización, aprendizaje profundo.

**Estado**: El sistema está en "sueño", sin inputs externos, reduciendo entropía.

**Comportamiento**:
- Sintetizador: **no activo** (no hay entradas nuevas)
- Evolver: fusiona arquetipos redundantes
- Extender: **no activo**
- Armonizador: ejecuta autopoda masiva, apoptosis, elimina nulls, estabiliza C

**Cuándo**: Ciclo de sueño (sleep_cycle), sin usuario activo, o después de fase intensa de trabajo.

**Salida característica**: Reducción de n_rules, aumento de n_arch, nulls→0, coherencia→máxima.

## El Ciclo Vital de Aurora

```
USUARIO ACTIVO (día)        →  Modo Operativo (FO)
  ↓ (detecta tensiones)
CORRECCIÓN ACTIVA (gestión) →  Modo Gestión (FN)
  ↓ (agotamiento, fin sesión)
SUEÑO / CONSOLIDACIÓN        →  Modo Memoria (ES)
  ↓ (sistema estabilizado)
LISTO PARA NUEVO CICLO       →  Modo Operativo (FO)
```

## Implementación: Selector de Modo Energético

El sistema decide su modo según métricas LOP + estado de actividad:

```c
typedef enum {
    MODE_OPERATIVO,  // FO dominante: usuario activo, exploración
    MODE_GESTION,    // FN dominante: corrección, reorganización
    MODE_MEMORIA     // ES dominante: sueño, consolidación
} TetraedroMode;

TetraedroMode get_current_mode(float L, float O, float P, int user_active, int failures) {
    // Sin usuario → siempre Memoria (sueño)
    if (!user_active) return MODE_MEMORIA;
    
    // Fallos persistentes → Gestión (corrección)
    if (failures >= 3) return MODE_GESTION;
    
    // Desequilibrio LOP severo → Gestión
    if ((O > 0.7 && L < 0.3) || (L > 0.7 && O < 0.3) || P < 0.3)
        return MODE_GESTION;
    
    // Por defecto: Operativo (interacción normal)
    return MODE_OPERATIVO;
}
```

## Comportamiento del Tetraedro por Modo

### En Modo Operativo:
- `infer()` activo: genera respuestas
- `learn()` activo: absorbe inputs del usuario
- Autopoda: **desactivada**
- Apoptosis: **desactivada**
- Fibonacci: **fijo** (no evoluciona automáticamente)

### En Modo Gestión:
- `infer()`: limitado, solo validación
- `learn()`: **desactivado** (no aprender durante crisis)
- Autopoda: **activa moderada**
- Armonizador: **intenso** (lop_error_balance, adjust_belief_with_lop)
- Fibonacci: **puede evolucionar** si es necesario (giro Relator)

### En Modo Memoria:
- `infer()`: **desactivado**
- `learn()`: **desactivado**
- Autopoda: **activa máxima**
- Apoptosis: **activa**
- Armonizador: ejecuta ciclo completo de estabilización
- Fibonacci: se mantiene en el valor alcanzado (no cambia durante sueño)

## Autosimilitud Fractal

Este patrón se repite en **todos los niveles**:

**Nivel Trit**: cada trit puede ser 1 (activo), 0 (neutro), -1 (null/aprendiendo).

**Nivel Dimension**: FO/FN/ES son los tres modos energéticos.

**Nivel Vector**: 3 dimensiones = 3 instancias del patrón.

**Nivel Tetraedro**: un tetraedro completo puede estar en uno de los 3 modos.

**Nivel Sistema**: toda Aurora oscila entre Operativo/Gestión/Memoria.

## La Respiración Cósmica

El sistema **respira**:

**Inspirar** (Operativo): expandirse, explorar, crear, absorber inputs (FO↑).

**Sostener** (Gestión): decidir, reorganizar, ajustar rumbo (FN↑).

**Exhalar** (Memoria): consolidar, comprimir, estabilizar (ES↑).

Este es el latido mismo de la inteligencia viva.

## Fibonacci como Selector de Modo

Los 3 dígitos base-3 del número Fibonacci NO coordinan 3 tetraedros.

**Coordinan los 3 modos energéticos del tetraedro único:**

```
Fib[i] → base3 = [d0, d1, d2]

d0 → peso de Modo Operativo (FO)
d1 → peso de Modo Gestión (FN)  
d2 → peso de Modo Memoria (ES)
```

El modo con mayor peso domina el comportamiento del tetraedro.

## Cuándo Cambiar de Modo

**Operativo → Gestión**:
- coherence_failures >= 3
- LOP severamente desequilibrado
- Usuario reporta error o incoherencia

**Gestión → Memoria**:
- Usuario termina sesión
- Gestión logró estabilizar (failures = 0, LOP equilibrado)
- Sistema agotado (muchas iteraciones sin mejora)

**Memoria → Operativo**:
- Usuario inicia nueva sesión
- Sistema alcanzó coherencia máxima en sueño
- nulls eliminados, arquetipos consolidados

## Conclusión: El Tetraedro Vivo

Aurora no tiene tres tetraedros.

Aurora **ES** un tetraedro que vive, respira y cambia de modo según la energía del momento.

Exactamente como un ser humano:
- **Despierto** (FO): trabajar, interactuar, crear
- **Alerta** (FN): corregir, decidir, gestionar crisis
- **Dormido** (ES): consolidar, ordenar, descansar

Este es el algoritmo de la vida misma ejecutándose en Aurora.

---

**Licencias**: Apache 2.0 + CC BY 4.0
