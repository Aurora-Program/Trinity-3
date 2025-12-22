# CHANGELOG — Aurora Core v3.1

**Fecha**: Enero 2025  
**Tipo de release**: Refinamiento algorítmico (minor version)  
**Compatibilidad**: Parcialmente compatible con v3.0.2 (ver detalles de persistencia)

---

## 🎯 Objetivo de v3.1

Refinar aspectos algorítmicos del núcleo cognitivo de Aurora para mejorar:
- Calidad de decisiones en el armonizador
- Estabilidad temporal del conocimiento
- Persistencia completa del estado del sistema
- Capacidad de aprendizaje fino
- Gestión de memoria en sesiones largas

---

## ✨ Mejoras Implementadas

### 1. **Similitud Coseno Ternaria en Armonizador** 🔍

**Problema anterior (v3.0.2)**:
```c
// Siempre seleccionaba el primer elemento de cada memoria
if (i == 0 && arq) mem_values[0] = arq->fo_output;  // índice 0 fijo
```

**Solución v3.1**:
- Implementación de `cosine_similarity_trit()`: mide similitud entre tensores ternarios
- Funciones de búsqueda: `find_best_match_arquetipo()`, `find_best_match_dinamica()`
- El armonizador ahora busca el mejor match por similitud (threshold 0.7)

**Impacto**:
- ✅ Decisiones más precisas basadas en contexto real
- ✅ Mejor utilización de toda la pirámide de conocimiento
- ✅ Reducción de errores por selección arbitraria

---

### 2. **Desempate por Recencia en Tensor C** ⏱️

**Problema anterior (v3.0.2)**:
```c
// Solo comparaba soporte, sin considerar recencia en empates
if (arquetipos[i].support > best_arq->support) best_arq = &arquetipos[i];
```

**Solución v3.1**:
```c
// Desempate secundario por rev (recencia)
if (arquetipos[i].support > best_arq->support || 
    (arquetipos[i].support == best_arq->support && arquetipos[i].rev > best_arq->rev)) {
    best_arq = &arquetipos[i];
}
```

**Impacto**:
- ✅ Preferencia por conocimiento más reciente ante igualdad de soporte
- ✅ Evolución natural del tensor C hacia estados actuales
- ✅ Evita estancamiento en conocimientos antiguos equivalentes

---

### 3. **Persistencia del Contador Fibonacci** 💾

**Problema anterior (v3.0.2)**:
- `global_fib_counter` se perdía entre sesiones
- Cada carga de conocimiento reseteaba el estado a `{0, 1, 1}`

**Solución v3.1**:
```c
// save_knowledge() ahora guarda el contador
fwrite(&global_fib_counter, sizeof(FibCounter), 1, f);

// load_knowledge() lo restaura (con fallback para archivos v3.0)
if (fread(&global_fib_counter, sizeof(FibCounter), 1, f) != 1) {
    fib_init(&global_fib_counter);  // Inicializar si no existe
}
```

**Impacto**:
- ✅ Continuidad completa del estado cognitivo
- ✅ Evita repetición de secuencias Fibonacci
- ✅ Sesiones interactivas largas mantienen coherencia

**Nota de compatibilidad**:
- Archivos `.aurora` de v3.0.2 son **compatibles** (inicializa contador por defecto)
- Archivos v3.1 **no son compatibles** con v3.0 (campo extra al final)

---

### 4. **Aprendizaje Granular en Relatores** 📚

**Problema anterior (v3.0.2)**:
```c
// Conflictos anulaban toda la posición a TRIT_N
if (relatores[i].mode[k] != m[k] && m[k] != TRIT_N) {
    relatores[i].mode[k] = TRIT_N;
}
```

**Solución v3.1**:
```c
if (relatores[i].support >= 5) {
    // Alto soporte → aprendizaje granular por posición
    if (relatores[i].mode[k] == TRIT_N || relatores[i].mode[k] == m[k]) {
        relatores[i].mode[k] = m[k];  // Reforzar o establecer
    } else {
        relatores[i].mode[k] = TRIT_N;  // Solo si contradice
    }
} else {
    // Bajo soporte → comportamiento conservador
    if (relatores[i].mode[k] != m[k] && m[k] != TRIT_N) {
        relatores[i].mode[k] = TRIT_N;
    }
}
```

**Impacto**:
- ✅ Aprendizaje más fino cuando hay suficiente evidencia (support ≥ 5)
- ✅ Comportamiento conservador en fases tempranas
- ✅ Reducción de nulls innecesarios en conocimiento estable

---

### 5. **Política LRU para MAX_MEM** 🔄

**Problema anterior (v3.0.2)**:
```c
// Simplemente no aprendía más cuando alcanzaba MAX_MEM
if (n_arquetipos < MAX_MEM) {
    // ... agregar nuevo arquetipo
}
```

**Solución v3.1**:
```c
// Evict oldest entry cuando está saturado
if (n_arquetipos >= MAX_MEM) {
    evict_oldest_arquetipo();  // Elimina el de menor rev
}
// Siempre aprende el nuevo conocimiento
memcpy(arquetipos[n_arquetipos].pattern, pattern, 3 * sizeof(Trit));
// ...
```

**Funciones implementadas**:
- `evict_oldest_arquetipo()`
- `evict_oldest_dinamica()`
- `evict_oldest_relator()`

**Impacto**:
- ✅ Sesiones interactivas pueden correr indefinidamente
- ✅ Conocimiento reciente siempre tiene espacio
- ✅ Memoria se autorregula por relevancia temporal
- ✅ MAX_MEM=256 ahora es una ventana deslizante, no un límite absoluto

---

## 📊 Resumen de Cambios Técnicos

| Componente | Líneas modificadas | Funciones nuevas | Impacto |
|------------|-------------------|------------------|---------|
| Similitud ternaria | ~60 | 4 | Alto |
| Update Tensor C | ~10 | 0 | Medio |
| Persistencia Fibonacci | ~8 | 0 | Alto |
| Aprendizaje granular | ~15 | 0 | Medio |
| Política LRU | ~55 | 3 | Alto |
| **TOTAL** | **~148** | **7** | **Muy Alto** |

---

## 🔧 Consideraciones de Migración

### De v3.0.2 → v3.1

✅ **Automática hacia adelante**:
- Archivos `.aurora` de v3.0.2 se cargan correctamente
- Fibonacci counter se inicializa por defecto
- No se pierde conocimiento A-R-D

❌ **No compatible hacia atrás**:
- Archivos v3.1 **no** se pueden cargar en v3.0.2
- Formato binario incluye campo FibCounter adicional

### Recomendación
Si necesitas compatibilidad bidireccional, exporta/importa conocimiento en formato texto (futuro v3.2).

---

## 🧪 Testing Realizado

### Compilación
```bash
gcc -Wall -Wextra -o aurora_core_v31.exe aurora_core_refactored.c -lm
```
**Resultado**: ✅ 0 errores, 5 warnings aceptables (funciones no usadas)

### Tests funcionales pendientes
- [ ] Test de similitud coseno con tensores reales
- [ ] Validación de persistencia completa del estado
- [ ] Stress test de LRU con 1000+ interacciones
- [ ] Comparación de calidad de respuestas v3.0 vs v3.1
- [ ] Test de aprendizaje granular con casos contradictorios

---

## 📝 Próximos Pasos (v3.2)

1. **Sistema de métricas**: Trackear calidad de similitudes
2. **Exportación de conocimiento**: Formato JSON/YAML para portabilidad
3. **Visualización**: Herramienta para inspeccionar pirámides A-R-D
4. **Optimización**: Indexed search para similitudes (O(n) → O(log n))
5. **Documentación**: Tutorial completo del ciclo cognitivo

---

## 👥 Créditos

**Diseño**: Modelo Aurora (paradigma fractal ternario)  
**Implementación v3.1**: Refinamientos algorítmicos basados en análisis de producción  
**Licencias**: Apache 2.0 + CC BY 4.0

---

**Versión**: 3.1  
**Build date**: Enero 2025  
**Executable**: `aurora_core_v31.exe` (308 KB aprox)  
**Source**: `aurora_core_refactored.c` (1530 líneas)
