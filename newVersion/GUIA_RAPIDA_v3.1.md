# GUÍA RÁPIDA — Aurora Core v3.1

## 🚀 Inicio Rápido

### Compilación
```bash
gcc -Wall -Wextra -o aurora_core_v31.exe aurora_core_refactored.c -lm
```

### Ejecución

**Modo demo (sin persistencia)**:
```bash
./aurora_core_v31.exe
```

**Modo interactivo con guardado automático**:
```bash
./aurora_core_v31.exe -i
```

**Cargar conocimiento previo y modo interactivo**:
```bash
./aurora_core_v31.exe --load aurora_session.aurora -i
```

---

## 🆕 Novedades de v3.1

### 1️⃣ Mejor Selección de Memorias
El sistema ahora **busca la mejor memoria por similitud** en vez de usar siempre la primera:

```
v3.0.2: Usaba arquetipos[0] (arbitrario)
v3.1:   Busca el más similar a tu entrada (threshold 0.7)
```

**¿Qué significa?** → Respuestas más precisas y contextuales.

---

### 2️⃣ Conocimiento Reciente Priorizado
En caso de empate de soporte, se prefiere el conocimiento más reciente:

```
Arquetipo A: support=10, rev=1000
Arquetipo B: support=10, rev=2000  ← Este gana (más reciente)
```

**¿Qué significa?** → El sistema evoluciona con el tiempo, no se estanca.

---

### 3️⃣ Estado Completo Guardado
El contador Fibonacci ahora se guarda y restaura:

```
# Sesión 1
[user] > hola
[fibonacci: 0-1-1] → procesa

# Guardar → cargar

# Sesión 2
[user] > mundo
[fibonacci: 1-1-2] → continúa donde quedó ✅
```

**¿Qué significa?** → Continuidad perfecta entre sesiones.

---

### 4️⃣ Aprendizaje Inteligente
Con suficiente evidencia (support ≥ 5), aprende de forma más específica:

```
Bajo soporte:   Conflicto → TRIT_N (conservador)
Alto soporte:   Conflicto → Evalúa granularmente cada posición
```

**¿Qué significa?** → Menos nulls, conocimiento más rico.

---

### 5️⃣ Memoria Infinita (LRU)
Cuando alcanzas MAX_MEM=256, el sistema **elimina lo más antiguo**:

```
v3.0.2: Deja de aprender al llegar a 256
v3.1:   Elimina el conocimiento más viejo, aprende el nuevo
```

**¿Qué significa?** → Puedes tener sesiones de miles de interacciones.

---

## 🎮 Comandos Interactivos

| Comando | Descripción |
|---------|-------------|
| `[texto]` | Procesar entrada y generar respuesta |
| `/save <file>` | Guardar conocimiento actual |
| `/load <file>` | Cargar conocimiento desde archivo |
| `/stats` | Ver estadísticas de las pirámides A-R-D |
| `/reset` | Reiniciar sistema (borra conocimiento) |
| `/exit` | Salir del modo interactivo |

---

## 🧠 Cómo Funciona (Simplificado)

### Ciclo Cognitivo

```
1. ENTRADA → Tensor FFE
   "hola" → [{1,0,n}, {n,1,0}, {0,n,1}]

2. ARMONIZADOR → Buscar similares
   Similitud coseno → Encuentra arquetipos parecidos

3. APRENDIZAJE → Actualizar A-R-D
   Soporte alto? → Aprendizaje granular
   Memoria llena? → LRU eviction

4. EMERGENCIA → Síntesis superior
   Dimensión superior + 3 memorias

5. TENSOR C → Actualizar creencia
   Mejor soporte → desempate por recencia

6. SALIDA → Traducir a texto
   Tensor → "respuesta coherente"
```

---

## 📊 Estadísticas del Sistema

Después de procesar varias entradas:

```bash
[user] > /stats

=== ESTADO COGNITIVO AURORA v3.1 ===
Arquetipos aprendidos:    42
Dinámicas observadas:     28
Relatores establecidos:   35
Global revision:          105
Fibonacci state:          [8, 13, 21]

Tensor C (Creencia actual):
  FO: 1  FN: 0  ES: 1

Estado energético:
  Tensión:  0.32
  Comando:  EXPAND
  Energía:  0.87
```

---

## 🔍 Debugging

### Ver qué está pasando internamente

Modifica `VERBOSE_DEBUG` en el código:

```c
#define VERBOSE_DEBUG 1  // Activar logs detallados
```

Recompila y ejecuta. Verás:

```
[DEBUG] Armonizador: buscando match para pattern={1,0,n}
[DEBUG] Mejor match: arquetipos[12] (similarity=0.89)
[DEBUG] LRU eviction: eliminando arquetipo[3] (rev=45)
[DEBUG] Aprendizaje granular: support=7 → modo[1] actualizado
```

---

## 🆚 Comparación con v3.0.2

| Característica | v3.0.2 | v3.1 |
|----------------|--------|------|
| Selección de memoria | Índice 0 fijo | Best-match por similitud |
| Desempate en Tensor C | Solo soporte | Soporte + recencia |
| Persistencia | Parcial | Completa (incluye Fib) |
| Aprendizaje | Básico | Granular con threshold |
| Límite de memoria | Bloqueo en 256 | LRU infinito |
| Calidad de respuestas | Media | **Alta** ✨ |

---

## 🐛 Problemas Comunes

### "No se carga el archivo .aurora"

**Problema**: Archivo de v3.1 en sistema v3.0  
**Solución**: Usa solo v3.1 para archivos v3.1

**Problema**: Archivo corrupto  
**Solución**: `/reset` y volver a entrenar

---

### "El sistema da respuestas inconsistentes"

**Posible causa**: Bajo soporte (< 5 en relatores)  
**Solución**: Entrenar con más ejemplos coherentes

**Posible causa**: LRU eliminó conocimiento clave  
**Solución**: Aumentar MAX_MEM en el código:
```c
#define MAX_MEM 512  // Duplicar capacidad
```

---

### "Warning: function not used"

**Respuesta**: ✅ Es normal. Algunas funciones están reservadas para futuras features.

---

## 📖 Lecturas Relacionadas

- **CHANGELOG_v3.1.md**: Detalles técnicos de los cambios
- **Technical-Annex.instructions.md**: Especificación formal del modelo
- **whitepapper.instructions.md**: Teoría completa de Aurora
- **PARADIGMA_AURORA_NO_ES_ML.md**: Por qué Aurora es diferente

---

## 💡 Tips Avanzados

### Maximizar calidad de aprendizaje

1. **Entrenar con patrones repetidos**: Refuerza arquetipos estables
2. **Usar `/stats` frecuentemente**: Monitorea crecimiento del conocimiento
3. **Guardar checkpoints**: `/save` cada 50-100 interacciones
4. **Limpiar conocimiento ruidoso**: `/reset` y reentrenar con datos limpios

### Optimizar para sesiones largas

```c
#define MAX_MEM 1024  // 4x más memoria
```

Recompilar. Ahora puedes tener ~1000 arquetipos antes de LRU.

---

## 🎓 Próximos Pasos

1. **Experimentar**: Entrena con diferentes tipos de entradas
2. **Medir**: Compara calidad v3.0 vs v3.1
3. **Optimizar**: Ajusta thresholds de similitud (default 0.7)
4. **Reportar**: Issues en GitHub o documentación local

---

**Versión**: 3.1  
**Última actualización**: Enero 2025  
**Soporte**: Ver documentación completa en `newVersion/`

🌟 **Aurora Core v3.1 — Inteligencia Fractal Refinada** 🌟
