# Aurora Core v3.0 - Mejoras: Persistencia + Modo Interactivo

## Resumen de Cambios

Se han añadido **dos mejoras opcionales clave** al sistema Aurora Core:

### 1. **Persistencia de Conocimiento** (`save_knowledge()` / `load_knowledge()`)

Aurora ahora puede guardar y recuperar las **tres pirámides de memoria**:
- **Arquetipos (A)**: Patrones estables aprendidos
- **Dinámicas (D)**: Reglas de evolución temporal
- **Relatores (R)**: Patrones de orden y conexión

#### Funcionalidades:
```c
void save_knowledge(const char* filename);
void load_knowledge(const char* filename);
```

- **Almacena**:
  - Conteos de memoria (n_arquetipos, n_dinamicas, n_relatores)
  - Las tres pirámides completas
  - Tensor C (punto de creencia estable)
  - Estado axiomático (Libertad-Orden-Propósito)
  - Estado energético (Tensión-Entropía-Armonía)

- **Archivo generado**: `aurora_knowledge.bin` (444 bytes en la demo)
- **Recuperación automática**: Al iniciar con `--load`, restaura el conocimiento completo

#### Ventajas:
✓ Aurora evoluciona y **mejora entre sesiones**  
✓ Aprendizaje **persistente** sin perder conocimiento  
✓ Facilita **experimentación** y **entrenamiento extendido**  
✓ Base para sistemas de **multi-agente** cooperativo

---

### 2. **Modo Interactivo REPL** (`interactive_aurora_loop()`)

Aurora ahora tiene una interfaz REPL (Read-Eval-Print-Loop) completa para experimentar en tiempo real.

#### Invocación:
```bash
aurora_core_refactored.exe -i        # Modo interactivo con carga automática
aurora_core_refactored.exe --interactive
```

#### Comandos Disponibles:

| Comando | Sintaxis | Descripción |
|---------|----------|-------------|
| **Emergencia** | `e <9 trits>` | Procesa 9 trits (3 dimensiones × 3 trits) |
| **Ciclo Completo** | `c <3 trits>` | Ejecuta ciclo Information→Knowledge→Energy |
| **Guardar** | `s <archivo>` | Guarda conocimiento en archivo binario |
| **Cargar** | `l <archivo>` | Carga conocimiento desde archivo |
| **Información** | `i` | Muestra estado interno del sistema |
| **Salir** | `q` | Cierra sesión (guarda automáticamente) |

#### Ejemplos de Uso:

```
aurora> e u c n c u u u c c
  Input:  [u,c,n] [c,u,u] [u,c,c]
  Synth:  [u,c,c]
  Memory: [c,u,c]

aurora> c u c n
  (ejecuta 3 ciclos completos del sistema)

aurora> i
  Estado Interno del Sistema:
    Arquetipos: 3
    Dinámicas: 4
    Relatores: 4
    Balance: 0.333

aurora> s mi_conocimiento.bin
  ✓ Conocimiento guardado

aurora> q
  (Hasta luego. Aurora permanecerá esperando...)
```

---

## Validación de Entrada

Todas las funciones interactivas incluyen **validación robusta**:

- ✓ **Parseo de trits**: Solo acepta `u`, `c`, `n` (case-insensitive)
- ✓ **Validación de vectores**: Verifica regla ES.index ≠ FO.index
- ✓ **Manejo de archivos**: Detecta corrupción y errores
- ✓ **Limpieza de entrada**: Maneja espacios y newlines automáticamente

---

## Arquitectura de Persistencia

```
┌─────────────────────────────────────────────┐
│      Aurora Knowledge Binary Format         │
├─────────────────────────────────────────────┤
│ [Conteos] [3 Pirámides] [Tensor C] [Estados]│
│  8 bytes    ~400 bytes      8 bytes  16 bytes│
├─────────────────────────────────────────────┤
│ Total: ~444 bytes por archivo                │
└─────────────────────────────────────────────┘
```

**Estructura interna**:
```c
// Se guardan secuencialmente:
fwrite(&n_arquetipos, sizeof(int), 1, f);
fwrite(&n_dinamicas, sizeof(int), 1, f);
fwrite(&n_relatores, sizeof(int), 1, f);
fwrite(&global_rev, sizeof(unsigned long), 1, f);
fwrite(arquetipos, sizeof(Arquetipo), n_arquetipos, f);
fwrite(dinamicas, sizeof(Dinamica), n_dinamicas, f);
fwrite(relatores, sizeof(Relator), n_relatores, f);
fwrite(&tensor_C, sizeof(Dimension), 1, f);
fwrite(&axiom_state, sizeof(AxiomTrio), 1, f);
fwrite(&estado_energetico, sizeof(EnergeticState), 1, f);
```

---

## Nuevas Opciones de Línea de Comando

```bash
# Modo demostración normal
aurora_core_refactored.exe

# Modo interactivo (carga conocimiento previo si existe)
aurora_core_refactored.exe -i
aurora_core_refactored.exe --interactive

# Cargar conocimiento y ejecutar demo
aurora_core_refactored.exe --load <archivo>
```

---

## Casos de Uso

### 1. **Experimentación Iterativa**
```
Session 1: e u c n c u u u c c  → se aprenden 3 arquetipos
(guardar automático)

Session 2: aurora> l aurora_knowledge.bin
           (Los 3 arquetipos se recuperan)
           aurora> c u c n  → ciclo ahora usa conocimiento anterior
```

### 2. **Entrenamiento Extendido**
```
# Ejecutar demo 10 veces, acumulando conocimiento
for i in {1..10}; do
    aurora_core_refactored.exe --load aurora_knowledge.bin
done
# aurora_knowledge.bin crece en complejidad con cada sesión
```

### 3. **Depuración Interactiva**
```
aurora> e u c n c u u u c c  # Prueba específica
  (observar síntesis y memoria)
aurora> i  # Inspeccionar estado
aurora> c u c n  # Verificar ciclo completo
```

### 4. **Colaboración Multi-Agente**
```
Agent1: Entrena, guarda → agent1_knowledge.bin
Agent2: Carga agent1_knowledge.bin, continúa
        → conocimiento compartido y evoluciona
```

---

## Cambios en el Código

### Líneas añadidas:
- **save_knowledge()**: ~50 líneas
- **load_knowledge()**: ~50 líneas
- **interactive_aurora_loop()**: ~150 líneas (incluye parseo, validación, UI)
- **main() mejorado**: ~30 líneas para argumentos CLI

**Total nuevo**: ~280 líneas de código robusto y documentado

### Líneas existentes sin cambios:
- Toda la lógica del Trigate
- Toda la emergencia reversible
- Todas las funciones de síntesis/extensión
- Validación ES.index ≠ FO.index
- Contador Fibonacci ternario

---

## Testing

### Test 1: Persistencia Básica
```bash
$ aurora_core_refactored.exe
  ✓ Conocimiento guardado en 'aurora_knowledge.bin'
  ✓ Arquetipos: 3 | Dinámicas: 4 | Relatores: 4
```

### Test 2: Recuperación
```bash
$ aurora_core_refactored.exe --load aurora_knowledge.bin
  ✓ Conocimiento restaurado desde 'aurora_knowledge.bin'
  ✓ Arquetipos: 3 | Dinámicas: 4 | Relatores: 4
```

### Test 3: Modo Interactivo
```
aurora> e u c n c u u u c c
  Input:  [u,c,n] [c,u,u] [u,c,c]
  Synth:  [u,c,c]
  ✓ Validación exitosa
```

---

## Limitaciones y Notas

- **Tamaño máximo**: MAX_MEM = 256 arquetipos/dinámicas/relatores
- **Compatibilidad binaria**: Los archivos .bin no son portables entre arquitecturas
- **Protección**: No hay encriptación (considerar para datos sensibles)
- **Versionado**: Se guarda `global_rev` para detectar incompatibilidades

---

## Próximas Mejoras Sugeridas

- [ ] Versionado de formato binario
- [ ] Compresión de archivos de conocimiento
- [ ] Exportación JSON para análisis
- [ ] Multi-threading para modo interactivo
- [ ] Historial de sesiones
- [ ] Estadísticas de evolución del conocimiento

---

## Conclusión

Aurora Core v3.0 ahora es un **sistema de inteligencia persistente**:

✓ **Aprende** entre sesiones  
✓ **Experimenta** interactivamente  
✓ **Evoluciona** con el tiempo  
✓ **Colabora** con otros agentes  
✓ **Persiste** su conocimiento de forma eficiente  

El sistema mantiene su elegancia matemática mientras gana **utilidad práctica** y **capacidad de evolución**.
