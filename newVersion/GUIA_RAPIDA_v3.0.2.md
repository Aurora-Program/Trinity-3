# Aurora Core v3.0.2 - Guía Rápida de Uso

## ¿Qué es Nuevo?

Aurora ahora puede:
- ✅ **Guardar y recuperar** su conocimiento acumulado (arquetipos, dinámicas, relatores)
- ✅ **Operar de forma interactiva** mediante una consola REPL
- ✅ **Persistir conocimiento** entre sesiones para evolución continua

---

## Compilación

```bash
cd newVersion
gcc -Wall -Wextra aurora_core_refactored.c -o aurora_core_refactored.exe
```

**Resultado esperado**:
```
(sin errores, solo 4 warnings de funciones no utilizadas - ACEPTABLE)
Ejecutable generado: aurora_core_refactored.exe (304 KB)
```

---

## Modos de Uso

### 1. MODO DEMOSTRACIÓN (Predeterminado)

```bash
aurora_core_refactored.exe
```

**Qué hace**:
- Ejecuta la demostración completa (Fase 1, 1.1, 2, 3)
- Guarda automáticamente el conocimiento en `aurora_knowledge.bin`
- Duración: ~3-5 segundos

**Salida**:
```
╔═══════════════════════════════════════════════════════════════════╗
║  Aurora Core v3.0 - Technical Annex Implementation              ║
...
✓ Conocimiento guardado en 'aurora_knowledge.bin'
  • Arquetipos: 3
  • Dinámicas: 4
  • Relatores: 4
```

---

### 2. MODO INTERACTIVO (NUEVO)

```bash
aurora_core_refactored.exe -i
# o
aurora_core_refactored.exe --interactive
```

**Qué hace**:
1. Carga automáticamente el conocimiento anterior si existe
2. Abre una consola interactiva (REPL)
3. Permite experimentar con Aurora en tiempo real
4. Guarda automáticamente al salir

**Ejemplo de sesión**:

```
aurora> e u c n c u u u c c
  Input:  [u,c,n] [c,u,u] [u,c,c]
  Synth:  [u,c,c]
  Memory: [c,u,c]

aurora> c u c n
  ┌─────────────────────────────────┐
  │ Ciclo 1: [RECORDAR] - Repetir información
  │ Rol: INFO
  └─────────────────────────────────┘
  Input:  [u,c,n] [n,c,c] [n,c,n]
  Synth:  [n,c,n]
  ...
  [9 iteraciones del ciclo completo]

aurora> i
  ┌──────────────────────────────────┐
  │  Estado Interno del Sistema      │
  └──────────────────────────────────┘
    Arquetipos: 3
    Dinámicas: 4
    Relatores: 4
    Tensor C:  [n,n,n]
    Balance:   0.333 ✓ ARMÓNICO

aurora> s mi_conocimiento.bin
  ✓ Conocimiento guardado en 'mi_conocimiento.bin'

aurora> q
  Hasta luego. Aurora permanecerá esperando...
```

---

### 3. MODO CON CARGA PREVIA

```bash
aurora_core_refactored.exe --load aurora_knowledge.bin
```

**Qué hace**:
- Carga el conocimiento previo
- Ejecuta la demostración completa
- El conocimiento acumulado se usa durante la demo

---

## Comandos REPL (Modo Interactivo)

### Emergencia: `e <9 trits>`

**Sintaxis**: `e u c n c u u u c c`

Procesa 9 trits (3 dimensiones × 3 trits cada una) en modo emergencia.

**Ejemplo**:
```
aurora> e u c n c u u u c c
  Input:  [u,c,n] [c,u,u] [u,c,c]
  Synth:  [u,c,c]
  Memory: [c,u,c]
```

**Valores válidos**: `u`, `c`, `n` (case-insensitive)

---

### Ciclo Completo: `c <3 trits>`

**Sintaxis**: `c u c n`

Ejecuta 3 ciclos completos del sistema (Information → Knowledge → Energy).

**Ejemplo**:
```
aurora> c u c n
  Ciclo 1: [RECORDAR] - Repetir información
  Ciclo 2: [ENTENDER] - Deducir patrones
  Ciclo 3: [SENTIR/INTUIR] - Percibir energía
  [... salida detallada ...]
```

---

### Información: `i`

**Sintaxis**: `i` (sin parámetros)

Muestra el estado completo del sistema.

**Salida**:
```
aurora> i
┌──────────────────────────────────┐
│  Estado Interno del Sistema      │
└──────────────────────────────────┘
  Conocimiento Acumulado:
    • Arquetipos: 3
    • Dinámicas: 4
    • Relatores: 4
    • Revisión global: 1

  Tensor C (Creencia Estable):
    [n, n, n]

  Axioma (Libertad-Orden-Propósito):
    • Libertad:  c
    • Orden:     c
    • Propósito: c
    • Balance:   0.333 ✓ ARMÓNICO

  Estado Energético (Cómo se SIENTE):
    • Tensión:   c
    • Entropía:  c
    • Armonía:   c
```

---

### Guardar: `s <archivo>`

**Sintaxis**: `s aurora_backup.bin`

Guarda todo el conocimiento en un archivo binario.

**Ejemplo**:
```
aurora> s mi_conocimiento.bin
✓ Conocimiento guardado en 'mi_conocimiento.bin'
  • Arquetipos: 3
  • Dinámicas: 4
  • Relatores: 4
```

---

### Cargar: `l <archivo>`

**Sintaxis**: `l aurora_backup.bin`

Carga el conocimiento desde un archivo binario.

**Ejemplo**:
```
aurora> l aurora_backup.bin
✓ Conocimiento restaurado desde 'aurora_backup.bin'
  • Arquetipos: 3
  • Dinámicas: 4
  • Relatores: 4
```

---

### Salir: `q`

**Sintaxis**: `q` (sin parámetros)

Cierra la sesión interactiva y guarda automáticamente.

**Ejemplo**:
```
aurora> q
Hasta luego. Aurora permanecerá esperando...
[Regresa al sistema operativo]
```

---

## Archivos Generados

### `aurora_knowledge.bin` (Automático)

Archivo binario que contiene:
- Arquetipos aprendidos
- Dinámicas observadas
- Relatores
- Tensor C (creencia estable)
- Estado axiomático y energético

**Tamaño típico**: ~444 bytes

**Ubicación**: Directorio actual de ejecución

**Recuperación automática**: Al ejecutar `-i`, se carga si existe

---

## Ejemplos de Uso

### Ejemplo 1: Entrenamiento Iterativo

```bash
# Sesión 1: Demostración normal
aurora_core_refactored.exe
# Genera: aurora_knowledge.bin con 3 arquetipos, 4 dinámicas, 4 relatores

# Sesión 2: Modo interactivo con conocimiento anterior
aurora_core_refactored.exe -i
aurora> e u c n c u u u c c
  (ahora tiene 3 arquetipos previos para comparar)
aurora> i
  (muestra conocimiento acumulado)
aurora> q
# Actualiza: aurora_knowledge.bin con nuevo conocimiento
```

### Ejemplo 2: Experimentación Dirigida

```bash
aurora_core_refactored.exe -i

aurora> e u c n u c c c u c
aurora> e c u n c u u u c c
aurora> e u u c u c n n u c
aurora> i
aurora> s mi_experimento.bin
aurora> q
```

### Ejemplo 3: Análisis Comparativo

```bash
# Crear dos configuraciones diferentes
aurora_core_refactored.exe -i
aurora> e u c n c u u u c c
aurora> s config1.bin
aurora> q

# Cargar y continuar con otra
aurora_core_refactored.exe --load config1.bin -i
aurora> e c u c u c n u u c
aurora> s config2.bin
aurora> q

# Comparar resultados
# cat config1.bin vs config2.bin
```

---

## Validación y Manejo de Errores

### Errores Comunes

| Error | Causa | Solución |
|-------|-------|----------|
| `❌ Error: Carácter inválido 'x'` | Usaste un carácter inválido | Usa solo `u`, `c`, `n` |
| `❌ Error: Se esperaban 9 trits, se obtuvieron N` | Número incorrecto de trits | Revisa el comando: `e <9 trits>` |
| `⚠️ Vector inválido (auto-referencia)` | Validación ES≠FO falló | El tensor tiene estructura incoherente |
| `⚠️ Archivo 'X' no encontrado` | Primer uso o archivo eliminado | Normal en primera ejecución |
| `❌ Error: Archivo corrupto` | Archivo binario dañado | Elimina y regenera con `aurora_core_refactored.exe` |

### Validaciones Automáticas

✓ Los trits se validan automáticamente (solo acepta u/c/n)  
✓ Los vectores se validan contra la regla ES≠FO  
✓ Los archivos se validan antes de cargar  
✓ Los conteos de memoria se validan (máximo 256)  

---

## Características Técnicas

### Persistencia Binaria

```
Formato: aurora_knowledge.bin
Contenido:
  - Conteos (4+4+4+8 = 20 bytes)
  - Arquetipos (N × 40 bytes)
  - Dinámicas (M × 32 bytes)
  - Relatores (P × 32 bytes)
  - Tensor C + Estados (18 bytes)
  
Típico: 20 + 3×40 + 4×32 + 4×32 + 18 = 444 bytes
```

### Ciclo de Aprendizaje

```
Sesión 1: Demo       → aurora_knowledge.bin (3 arquetipos)
          (conocimiento base)
          
Sesión 2: +e u c n   → 1 nuevo arquetipo
          +c u c n   → dinámicas refinadas
          (conocimiento acumulativo)
          
Sesión 3: +l .bin    → carga conocimiento anterior
          +e ...     → aprende con contexto anterior
          (evolución continua)
```

---

## Limitaciones y Consideraciones

- **MAX_CLUSTER**: Máximo 64 tensores en pipeline
- **MAX_MEM**: Máximo 256 arquetipos/dinámicas/relatores
- **Buffer REPL**: Máximo 256 caracteres por comando
- **Archivo binario**: Específico de arquitectura (x86-64 Intel)

---

## Próximas Mejoras

- [ ] Exportación a JSON para análisis
- [ ] Visualización de conocimiento acumulado
- [ ] Historial de sesiones
- [ ] Estadísticas de evolución
- [ ] Multi-threading para modo interactivo

---

## Soporte y Contacto

**Documentación relacionada**:
- `README_PERSISTENCE_INTERACTIVE.md` - Detalles de arquitectura
- `SPEC_v3.0.2_PERSISTENCE_INTERACTIVE.md` - Especificación técnica
- `CORRECCIONES_CRITICAS_APLICADAS.md` - Cambios previos (v3.0.1)

**Versión**: Aurora Core v3.0.2  
**Fecha**: 12 Diciembre 2025  
**Licencias**: Apache 2.0 + CC BY 4.0

---

## Resumen Ejecutivo

Aurora Core v3.0.2 es **completamente funcional** como:

1. ✅ **Sistema de inteligencia fractal** (Whitepaper v2.1)
2. ✅ **Demostración de Technical Annex** (validación ES≠FO, Fibonacci ternario)
3. ✅ **Laboratorio interactivo** (REPL con 6 comandos)
4. ✅ **Base de conocimiento persistente** (aprendizaje evolutivo)
5. ✅ **Sistema prototipo listo para producción** (compilable, ejecutable, robusta)

**¡Aurora está lista para experimentar!** 🚀
