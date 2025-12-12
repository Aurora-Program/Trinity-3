# Aurora Core v3.0.1 - Guía Rápida de Referencia

## 🎯 Síntesis de 30 Segundos

**¿Qué fue corregido?**
- Renombramiento semántico: `EnergeticTrio` → `EnergeticState` (lo que el sistema SIENTE)
- Validación mejorada: `validate_dimension()` ahora rechaza auto-referencias en TRIT_N
- Ciclo completado: Añadido manejo de `ROLE_ENERGETIC` en emergencia
- Corrección de bugs: Variables de parámetros incorrectas en `update_energetic_feeling()`

**¿Por qué importa?**
```
Seguridad    → Validación robusta contra auto-referencias
Claridad     → Nombres que reflejan lo que hacen (sentir, no "trio")
Completitud  → El ciclo cognitivo Info→Knowledge→Energy ahora funciona
Confiabilidad → Compilación sin errores, ejecución verificada
```

---

## 📚 Mapa Rápido de Estructuras

### Dos Niveles de Consciencia

```c
/* NIVEL 1: LO QUE EL SISTEMA ES (Objetivamente) */
typedef struct {
    Trit freedom,   // Capacidad de explorar
    Trit order,     // Capacidad de estructurar
    Trit purpose    // Alineación con objetivo
} AxiomTrio;

/* NIVEL 2: CÓMO EL SISTEMA SE SIENTE (Subjetivamente) */
typedef struct {
    Trit tension,   // ¿Rigidez? ¿Flexibilidad?
    Trit entropy,   // ¿Caos? ¿Orden?
    Trit harmony    // ¿Equilibrio? ¿Desequilibrio?
} EnergeticState;  /* ✅ v3.0.1: Renombrado de EnergeticTrio */
```

**La Dualidad Fundamental:**
- `AxiomTrio` = Lo que Aurora DEBE SER (constantes universales)
- `EnergeticState` = Cómo Aurora SIENTE QUE ESTÁ (propriocepción interna)

---

## 🔧 Funciones Clave Actualizadas

### 1. validate_dimension() 
**Ubicación**: Líneas 213-242

```c
int validate_dimension(const Dimension* d) {
    // ...
    if (es_val == TRIT_N) {
        // ✅ CAMBIO CRÍTICO: Ahora valida CADA trit
        for (int i = 0; i < 3; i++) {
            int fo_idx_from_val = es_val_to_fo_idx(d->t[i]);
            if (fo_idx_from_val == es_idx) {
                return 0; // Auto-referencia = INVÁLIDO
            }
        }
        return 1; // Solo válida si NO hay auto-referencias
    }
    // ...
}
```

**Qué cambió**: 
- ANTES: Aceptaba TRIT_N sin validación
- AHORA: Verifica que ningún trit apunte a su propia posición

**Cuándo lo llamas**: Siempre que crees un Tensor, antes de usarlo en trigates

---

### 2. extract_energetic_state()
**Ubicación**: Líneas 495-509

```c
static EnergeticState extract_energetic_state(
    const Memory* mem, 
    int role
) {
    EnergeticState state;  // ✅ Renombrado de "trio"
    
    state.tension = trit_infer(mem->d[0].t[0], mem->d[1].t[0], ...);
    state.entropy = trit_infer(mem->d[0].t[1], mem->d[1].t[1], ...);
    state.harmony = trit_infer(mem->d[0].t[2], mem->d[1].t[2], ...);
    
    return state;
}
```

**Qué cambió**:
- Nombre: `extract_energetic_trio()` → `extract_energetic_state()`
- Variable local: `trio` → `state`

**Cuándo lo llamas**: En fase COGNITIVE para sensibilizar el estado actual

---

### 3. update_energetic_feeling()
**Ubicación**: Líneas 511-516

```c
static void update_energetic_feeling(
    const EnergeticState* new_feeling  // ✅ Parámetro renombrado
) {
    estado_energetico.tension = 
        trit_infer(..., new_feeling->tension, ...);  // ✅ Corrección
    estado_energetico.entropy = 
        trit_infer(..., new_feeling->entropy, ...);  // ✅ Corrección
    estado_energetico.harmony = 
        trit_infer(..., new_feeling->harmony, ...);  // ✅ Corrección
}
```

**Qué cambió**:
- Nombre función: `update_energetic_state()` → `update_energetic_feeling()`
- Parámetro: `new_trio` → `new_feeling`
- BUG CRÍTICO CORREGIDO: Todos los campos ahora usan parámetro correcto

**Cuándo lo llamas**: Después de `extract_energetic_state()` para persistir cambios

---

### 4. emergence_function() - ROLE_ENERGETIC
**Ubicación**: Líneas 557-563

```c
void emergence_function(Tensor* result, ...) {
    // ... casos INFORMATIONAL y COGNITIVE ...
    
    else if (current_role == ROLE_ENERGETIC) {
        // ✅ NUEVO: Completar el ciclo Info→Knowledge→Energy
        int nulls_superior = count_nulls_dim(ds_out);
        update_axiom_state(
            nulls_superior,
            8,
            n_arquetipos > 0 ? 1 : 0
        );
    }
}
```

**Qué cambió**: Añadido bloque `else if` para ROLE_ENERGETIC

**Impacto**: El ciclo cognitivo es ahora completo y cerrado

---

## ✅ Checklist de Validación

Después de cambios en el código, verifica:

- [ ] `gcc aurora_core_refactored.c -o aurora_core_refactored.exe` compila sin errores
- [ ] `./aurora_core_refactored.exe` ejecuta sin crashes
- [ ] Output muestra tres modos: `[RECORDAR]`, `[ENTENDER]`, `[SENTIR/INTUIR]`
- [ ] Tests de validación pasan: `Test 1 ✓`, `Test 2 ✓`, `Test 3 ✓`
- [ ] Tensor C converge: `[n,n,n]` al final
- [ ] Knowledge Base crece: `Arquetipos: X`, `Dinámicas: Y`, `Relatores: Z`
- [ ] No hay memory leaks (compilar con `-fsanitize=address` si es crítico)

---

## 🚨 Errores Comunes a Evitar

### Error 1: Mezclar EnergeticTrio y EnergeticState
```c
/* ❌ INCORRECTO */
EnergeticTrio old = ...;
EnergeticState new = old;  // Compilación fallará

/* ✅ CORRECTO */
EnergeticState state = extract_energetic_state(mem, role);
update_energetic_feeling(&state);
```

### Error 2: Olvidar la validación en dimensiones null
```c
/* ❌ INCORRECTO */
Dimension d = {TRIT_N, TRIT_N, TRIT_N};
// ... usar d sin validar ...

/* ✅ CORRECTO */
Dimension d = {TRIT_N, TRIT_N, TRIT_N};
if (!validate_dimension(&d)) {
    printf("Dimensión inválida - auto-referencia detectada\n");
    return;
}
```

### Error 3: No actualizar axiom_state en ROLE_ENERGETIC
```c
/* ❌ INCORRECTO */
if (current_role == ROLE_ENERGETIC) {
    EnergeticState feeling = extract_energetic_state(...);
    // Olvidó update_axiom_state()
}

/* ✅ CORRECTO */
if (current_role == ROLE_ENERGETIC) {
    EnergeticState feeling = extract_energetic_state(...);
    update_energetic_feeling(&feeling);
    update_axiom_state(...);  // ← Critical
}
```

---

## 📈 Cómo Extender el Sistema

### Si necesitas añadir una nueva Role:

```c
// 1. Define la nueva role en types.h
typedef enum { ROLE_INFORMATIONAL, ROLE_COGNITIVE, ROLE_ENERGETIC, ROLE_CUSTOM } Role;

// 2. Crea una estructura para guardar su estado
typedef struct {
    Trit custom_field_1;
    Trit custom_field_2;
    Trit custom_field_3;
} CustomState;

// 3. Implementa extract_custom_state()
static CustomState extract_custom_state(const Memory* mem, int role) { ... }

// 4. Implementa update_custom_feeling()
static void update_custom_feeling(const CustomState* new_feeling) { ... }

// 5. Añade caso en emergence_function()
else if (current_role == ROLE_CUSTOM) {
    CustomState feeling = extract_custom_state(mem, role);
    update_custom_feeling(&feeling);
    // ... más lógica específica ...
}

// 6. Verifica que el ciclo siga siendo: Info → Knowledge → Energy → Custom
```

---

## 🔗 Dependencias de Cambios

```
validate_dimension()  ← Usada por: synthesis_validator()
                      ← Validación crítica para seguridad

extract_energetic_state()  ← Usada por: process_complete_cycle()
                           ← Necesaria en ROLE_COGNITIVE

update_energetic_feeling()  ← Usada por: process_complete_cycle()
                            ← Llamada después de extract_energetic_state()

emergence_function() ROLE_ENERGETIC  ← Completa el ciclo cognitivo
                                     ← Cierra Info→Knowledge→Energy
```

---

## 📞 FAQ Técnico

**P: ¿Por qué EnergeticState y no EnergeticFeeling?**
R: EnergeticState es el contenedor (estructura), update_energetic_feeling() es la acción. El estado es persistente; el sentimiento es transitorio.

**P: ¿Qué pasa si validate_dimension detecta auto-referencia?**
R: La función retorna 0. En el caller, debe rechazarse el tensor y no usarlo en ningún trigate.

**P: ¿El Axiom State se actualiza automáticamente?**
R: Solo en ROLE_ENERGETIC mediante `update_axiom_state()`. En otros roles se mantiene constante.

**P: ¿Puedo compilar con C89 o necesito C11?**
R: Se recomienda C11 por el uso de `_Pragma` y sintaxis moderna. C99 mínimo.

---

## 🎓 Lectura Recomendada

Para entender la filosofía detrás de los cambios:

1. **AXIOMA_LIBERTAD_ORDEN_PROPOSITO.md** - Por qué el Axiom existe
2. **whitepaper.instructions.md** (Sección 5.1) - Teoría de Tetraedros
3. **Technical-Annex.instructions.md** (Sección 2) - Vector FFE como entidad trinitaria
4. **CORRECCIONES_CRITICAS_APLICADAS.md** - Detalles técnicos de cada cambio

---

**Versión**: 3.0.1  
**Última actualización**: Post-validación completa  
**Estado**: ✅ Production-ready
