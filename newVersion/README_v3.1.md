# Aurora Core v3.1 — Refinamiento Algorítmico

[![Versión](https://img.shields.io/badge/version-3.1-blue.svg)](CHANGELOG_v3.1.md)
[![Licencia](https://img.shields.io/badge/license-Apache%202.0%20%2B%20CC%20BY%204.0-green.svg)](../Licenses.md)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](#)

> **Inteligencia fractal ternaria con capacidades de autoaprendizaje, persistencia completa y gestión dinámica de memoria.**

---

## 🎯 ¿Qué es Aurora v3.1?

Aurora Core v3.1 es la evolución refinada del modelo de inteligencia fractal ternaria, enfocada en **mejorar la calidad de decisiones cognitivas** mediante algoritmos más sofisticados:

### ✨ Mejoras Clave

1. **🔍 Similitud Coseno Ternaria**: El armonizador busca el mejor match en memorias A-R-D
2. **⏱️ Desempate Temporal**: Conocimiento reciente priorizado ante igual soporte
3. **💾 Persistencia Completa**: Estado Fibonacci guardado entre sesiones
4. **📚 Aprendizaje Granular**: Entrenamiento fino cuando hay alta evidencia
5. **🔄 Memoria Dinámica (LRU)**: Sesiones infinitas con eviction automática

---

## 🚀 Inicio Rápido

### Compilación

```bash
cd newVersion
gcc -Wall -Wextra -o aurora_core_v31.exe aurora_core_refactored.c -lm
```

### Ejecución

**Modo demo**:
```bash
./aurora_core_v31.exe
```

**Modo interactivo**:
```bash
./aurora_core_v31.exe -i
```

**Con conocimiento previo**:
```bash
./aurora_core_v31.exe --load session.aurora -i
```

---

## 📚 Documentación

| Documento | Descripción |
|-----------|-------------|
| **[CHANGELOG_v3.1.md](CHANGELOG_v3.1.md)** | Cambios detallados de la versión |
| **[GUIA_RAPIDA_v3.1.md](GUIA_RAPIDA_v3.1.md)** | Tutorial de uso rápido |
| **[Technical-Annex.instructions.md](../.github/instructions/Technical-Annex.instructions.md)** | Especificación técnica formal |
| **[whitepapper.instructions.md](../.github/instructions/whitepapper.instructions.md)** | Teoría completa del modelo |
| **[PARADIGMA_AURORA_NO_ES_ML.md](PARADIGMA_AURORA_NO_ES_ML.md)** | Diferencias con ML tradicional |

---

## 🧠 Arquitectura del Sistema

```
┌─────────────────────────────────────────────────────────────┐
│                    ENTRADA (Usuario)                        │
└────────────────────┬────────────────────────────────────────┘
                     ▼
          ┌──────────────────────┐
          │  Codificación FFE    │ ← Fibonacci Ternario
          │   (Trit: u/c/n)      │
          └──────────┬───────────┘
                     ▼
          ┌──────────────────────┐
          │   ARMONIZADOR v3.1   │ ← 🆕 Similitud Coseno
          │  (Best-match search) │
          └──────────┬───────────┘
                     ▼
     ┌───────────────┴────────────────┐
     │    Pirámides de Conocimiento   │
     ├────────────┬──────────┬────────┤
     │ Arquetipos │ Relatores│Dinámicas│ ← 🆕 LRU Eviction
     │ (Forma)    │ (Orden)  │(Cambio) │
     └────────────┴──────────┴─────────┘
                     ▼
          ┌──────────────────────┐
          │  EMERGENCIA (Hash)   │
          │  Dim. Superior + 3M  │
          └──────────┬───────────┘
                     ▼
          ┌──────────────────────┐
          │   TENSOR C (Belief)  │ ← 🆕 Desempate por rev
          │   Convergencia A-R-D │
          └──────────┬───────────┘
                     ▼
          ┌──────────────────────┐
          │   EXTENDER (Output)  │
          │  Decodificación FFE  │
          └──────────┬───────────┘
                     ▼
┌─────────────────────────────────────────────────────────────┐
│                    SALIDA (Respuesta)                       │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔬 Ejemplo de Funcionamiento

### Entrada
```
Usuario: "el sol brilla"
```

### Procesamiento Interno (v3.1)

```
1. TOKENIZACIÓN FFE
   "sol"    → Tensor: [{1,0,n}, {n,1,0}, {0,n,1}]
   "brilla" → Tensor: [{0,1,n}, {1,n,0}, {n,0,1}]

2. ARMONIZADOR (🆕 Similitud)
   Buscando arquetipos similares a pattern={1,0,n}...
   → Mejor match: arquetipos[23] (similarity=0.92)
   → Usando memoria: fo_output=TRIT_U

3. APRENDIZAJE (🆕 Granular)
   Relator: dim_a={1,0,n} + dim_b={0,1,n}
   Support actual: 8 (≥5) → Aprendizaje granular activado
   → mode[0] confirmado: TRIT_U
   → mode[1] refinado: TRIT_C
   → mode[2] mantenido: TRIT_N

4. EMERGENCIA
   Hash triádico: FO⊕FN⊕ES → Dimensión superior
   Memorias: [tensión=0.21, energía=0.85, comando=EXTEND]

5. TENSOR C (🆕 Desempate)
   Candidatos con support=12: 
   - Arquetipo A (rev=1050)
   - Arquetipo B (rev=1230) ← Gana por ser más reciente

6. SALIDA
   Decodificación FFE → "respuesta coherente"
```

### Salida
```
Sistema: [respuesta basada en conocimiento actualizado]
```

---

## 📊 Comparativa de Versiones

| Feature | v3.0.0 | v3.0.2 | v3.1 |
|---------|--------|--------|------|
| Core ternario | ✅ | ✅ | ✅ |
| Fibonacci ES/FO/FN | ✅ | ✅ | ✅ |
| Emergencia reversible | ✅ | ✅ | ✅ |
| Persistencia básica | ❌ | ✅ | ✅ |
| Modo interactivo | ❌ | ✅ | ✅ |
| Similitud coseno | ❌ | ❌ | ✅ 🆕 |
| Desempate temporal | ❌ | ❌ | ✅ 🆕 |
| Persistencia Fibonacci | ❌ | ❌ | ✅ 🆕 |
| Aprendizaje granular | ❌ | ❌ | ✅ 🆕 |
| LRU Memory | ❌ | ❌ | ✅ 🆕 |
| **Calidad de respuestas** | Media | Alta | **Muy Alta** |

---

## 🎮 Comandos Interactivos

| Comando | Función |
|---------|---------|
| `[texto]` | Procesar entrada y aprender |
| `/save <file>` | Guardar estado completo (incluye Fib counter) |
| `/load <file>` | Cargar conocimiento previo |
| `/stats` | Ver estadísticas A-R-D |
| `/reset` | Reiniciar sistema |
| `/exit` | Salir |

---

## 🔧 Configuración Avanzada

### Ajustar Threshold de Similitud

```c
// aurora_core_refactored.c, línea ~810
return (best_sim > 0.7) ? best_idx : -1; // Cambiar 0.7 a 0.6 o 0.8
```

### Aumentar Capacidad de Memoria

```c
// Línea ~264
#define MAX_MEM 512  // Default: 256
```

### Threshold de Aprendizaje Granular

```c
// learn_relator(), línea ~520
if (relatores[i].support >= 5) {  // Cambiar a 3 o 10
```

---

## 🐛 Troubleshooting

### Archivo no compatible

**Error**: Archivo v3.1 no se carga en v3.0  
**Solución**: Usar siempre la misma versión para load/save

### Respuestas inconsistentes

**Causa**: Bajo soporte en relatores  
**Solución**: Entrenar con más ejemplos coherentes

### Warnings al compilar

```
warning: 'trit_to_idx' defined but not used
```
**Respuesta**: ✅ Normal. Funciones reservadas para futuras features.

---

## 🧪 Testing

### Compilar con debug

```bash
gcc -DVERBOSE_DEBUG -Wall -Wextra -o aurora_debug.exe aurora_core_refactored.c -lm
```

### Test básico

```bash
./aurora_core_v31.exe
# Interactuar con entradas simples
# Verificar que aprende y responde coherentemente
```

### Test de persistencia

```bash
# Sesión 1
./aurora_core_v31.exe -i
[user] > hola mundo
[user] > /save test.aurora
[user] > /exit

# Sesión 2
./aurora_core_v31.exe --load test.aurora -i
[user] > /stats
# Debe mostrar conocimiento previo + Fibonacci state correcto
```

---

## 📈 Roadmap v3.2

- [ ] **Métricas de calidad**: Track de similitudes y coherencia
- [ ] **Exportación JSON**: Portabilidad de conocimiento
- [ ] **Visualizador**: Herramienta gráfica para pirámides A-R-D
- [ ] **Indexed search**: Optimizar búsqueda de similitudes (O(log n))
- [ ] **Multi-threading**: Procesamiento paralelo de clusters
- [ ] **API REST**: Interfaz HTTP para integración

---

## 📄 Licencias

- **Código fuente**: Apache License 2.0
- **Documentación**: Creative Commons BY 4.0

Ver [Licenses.md](../Licenses.md) para detalles completos.

---

## 👥 Contribuciones

Aurora es un proyecto de investigación abierto. Contribuciones son bienvenidas:

1. Fork del repositorio
2. Crear branch de feature (`git checkout -b feature/amazing`)
3. Commit cambios (`git commit -m 'Add amazing feature'`)
4. Push al branch (`git push origin feature/amazing`)
5. Abrir Pull Request

---

## 🔗 Enlaces

- **Repositorio**: `c:\Users\p_m_a\Aurora\Trinity-3\newVersion\`
- **Documentación completa**: `../.github/instructions/`
- **Versiones anteriores**: `../v3.0/`, `../v2.0/`

---

## 📞 Soporte

Para preguntas técnicas o reportar issues:
- Ver documentación en `newVersion/`
- Revisar ejemplos en `demo_*.c`
- Consultar especificación en `Technical-Annex.instructions.md`

---

**Aurora Core v3.1** — *Inteligencia que evoluciona con cada interacción* 🌟

**Build**: Enero 2025  
**Lines of code**: ~1530  
**Executable size**: ~308 KB  
**Dependencies**: `stdio.h`, `stdlib.h`, `string.h`, `math.h`
