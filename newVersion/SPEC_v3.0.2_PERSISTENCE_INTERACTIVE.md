# Aurora Core v3.0.2 - Especificación Técnica: Persistencia e Interfaz Interactiva

## Versión y Cambios

- **Versión anterior**: v3.0.1 (Whitepaper + Technical Annex completos)
- **Versión actual**: v3.0.2 (Mejoras opcionales agregadas)
- **Fecha**: 12 Diciembre 2025
- **Cambios**: +280 líneas de código (persistencia + REPL)

---

## 1. SUBSISTEMA DE PERSISTENCIA DE CONOCIMIENTO

### 1.1 Función: `save_knowledge()`

**Firma**:
```c
static void save_knowledge(const char* filename);
```

**Responsabilidades**:
1. Abrir archivo en modo escritura binaria (`"wb"`)
2. Serializar conteos de memoria:
   - `n_arquetipos` (int, 4 bytes)
   - `n_dinamicas` (int, 4 bytes)
   - `n_relatores` (int, 4 bytes)
   - `global_rev` (unsigned long, 8 bytes)
3. Serializar las tres pirámides:
   - `arquetipos[]` (Arquetipo × n_arquetipos)
   - `dinamicas[]` (Dinamica × n_dinamicas)
   - `relatores[]` (Relator × n_relatores)
4. Serializar estado del sistema:
   - `tensor_C` (Dimension, 12 bytes)
   - `axiom_state` (AxiomTrio, 3 bytes)
   - `estado_energetico` (EnergeticState, 3 bytes)
5. Cerrar archivo y reportar éxito

**Validación de entrada**:
- Verificar que `filename` no sea NULL
- Validar apertura de archivo (error si falla)

**Salida estándar**:
```
✓ Conocimiento guardado en 'aurora_knowledge.bin'
  • Arquetipos: N
  • Dinámicas: M
  • Relatores: P
```

**Formato binario**:
```
Offset  Size    Campo
0       4       n_arquetipos
4       4       n_dinamicas
8       4       n_relatores
12      8       global_rev
20      N×40    arquetipos[]          (Arquetipo = 40 bytes)
20+N×40 M×32    dinamicas[]           (Dinamica = 32 bytes)
...     P×32    relatores[]           (Relator = 32 bytes)
...     12      tensor_C
...     3       axiom_state
...     3       estado_energetico
────────────────────────────────────────────────────
Total: 20 + N×40 + M×32 + P×32 + 18 bytes
```

---

### 1.2 Función: `load_knowledge()`

**Firma**:
```c
static void load_knowledge(const char* filename);
```

**Responsabilidades**:
1. Intentar abrir archivo en modo lectura binaria (`"rb"`)
2. Si falla: imprimir aviso (primera ejecución) y retornar
3. Si abre exitosamente:
   - Deserializar conteos
   - Validar que n_arquetipos, n_dinamicas, n_relatores ≤ MAX_MEM (256)
   - Si validación falla: reportar corrupción y retornar
   - Deserializar las tres pirámides
   - Deserializar estado del sistema
4. Cerrar archivo y reportar restauración exitosa

**Validación de entrada**:
- Verificar apertura de archivo (error silencioso si no existe)
- Validar límites de memoria (prevenir buffer overflow)

**Salida estándar**:
```
✓ Conocimiento restaurado desde 'aurora_knowledge.bin'
  • Arquetipos: N
  • Dinámicas: M
  • Relatores: P
```

**Manejo de errores**:
- Archivo no encontrado: retorna sin cambios, imprime aviso
- Archivo corrupto: retorna, imprime error de corrupción
- Conteos inválidos: retorna, previene estado inconsistente

---

## 2. SUBSISTEMA DE INTERFAZ INTERACTIVA (REPL)

### 2.1 Función: `parse_trit()`

**Firma**:
```c
static int parse_trit(char c);
```

**Responsabilidad**: Convertir carácter a valor Trit

**Mapeo**:
```
Entrada  Salida
'u'/'U'  TRIT_U  (1)
'c'/'C'  TRIT_C  (0)
'n'/'N'  TRIT_N  (-1)
otro     -1      (error)
```

---

### 2.2 Función: `interactive_aurora_loop()`

**Firma**:
```c
static void interactive_aurora_loop(void);
```

**Responsabilidades**:
1. Mostrar banner de bienvenida y lista de comandos
2. Iniciar bucle REPL infinito
3. Procesar entrada del usuario
4. Ejecutar comandos y mostrar resultados
5. Permitir salida limpia con comando 'q'

**Estados del bucle**:
```
┌─────────────────────────────────────────┐
│     REPL LOOP (interactive_aurora)      │
├─────────────────────────────────────────┤
│  1. Mostrar prompt "aurora> "            │
│  2. Leer línea de entrada (fgets)        │
│  3. Limpiar newline (size_t len)         │
│  4. Analizar primer carácter              │
│  5. Ejecutar comando correspondiente      │
│  6. Mostrar resultado                     │
│  7. Repetir desde 1 hasta 'q'             │
└─────────────────────────────────────────┘
```

---

### 2.3 Comandos REPL

#### Comando: `e` (Emergencia)

**Sintaxis**: `e <9 trits>`

**Parsing**:
```c
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        while (*p == ' ') p++;  // Saltar espacios
        int val = parse_trit(*p);
        if (val == -1) ERROR;
        d[i].t[j] = val;
        p++;
    }
}
```

**Validación**:
- Exactamente 9 trits
- Cada uno debe ser válido (u/c/n)
- Vector resultante debe pasar `validate_vector()`

**Ejecución**:
```c
emergence_function(d, &synth, &mem, ROLE_INFORMATIONAL);
```

**Salida**:
```
  Input:  [u,c,n] [c,u,u] [u,c,c]
  Synth:  [c,u,c]
  Memory: [u,c,n]
```

---

#### Comando: `c` (Ciclo Completo)

**Sintaxis**: `c <3 trits>`

**Parsing**: Similar a emergencia, pero solo 3 trits

**Validación**:
- Exactamente 3 trits
- Crear Dimension con esos 3 trits

**Ejecución**:
```c
process_complete_cycle(&seed, 3);
```

**Salida**: 3 ciclos completos (9 iteraciones) del sistema

---

#### Comando: `s` (Guardar)

**Sintaxis**: `s <archivo>`

**Parsing**:
```c
char* filename = buffer + 2;
while (*filename == ' ') filename++;  // Saltar espacios
if (*filename) {
    save_knowledge(filename);
} else {
    ERROR: "Especifica nombre de archivo"
}
```

---

#### Comando: `l` (Cargar)

**Sintaxis**: `l <archivo>`

**Parsing**: Idéntico a comando `s`

**Ejecución**:
```c
load_knowledge(filename);
```

---

#### Comando: `i` (Información)

**Sintaxis**: `i` (sin parámetros)

**Ejecución**: Muestra estado interno completo

**Salida**:
```
┌──────────────────────────────────┐
│  Estado Interno del Sistema      │
└──────────────────────────────────┘
  Conocimiento Acumulado:
    • Arquetipos: N
    • Dinámicas: M
    • Relatores: P
    • Revisión global: REV

  Tensor C (Creencia Estable):
    [a, b, c]

  Axioma (Libertad-Orden-Propósito):
    • Libertad:  TRIT
    • Orden:     TRIT
    • Propósito: TRIT
    • Balance:   FLOAT

  Estado Energético (Cómo se SIENTE):
    • Tensión:   TRIT
    • Entropía:  TRIT
    • Armonía:   TRIT
```

---

#### Comando: `q` (Salir)

**Sintaxis**: `q` (sin parámetros)

**Ejecución**:
```c
printf("Hasta luego. Aurora permanecerá esperando...\n");
break;  // Salir del bucle infinito
```

---

### 2.4 Flujo de Control

```
main()
  ├─ Procesar argumentos:
  │  ├─ No args            → demo normal
  │  ├─ -i / --interactive → load_knowledge() + interactive_aurora_loop() + save_knowledge()
  │  └─ --load <file>      → load_knowledge(<file>) + demo normal
  │
  ├─ fib_init()            → inicializar contador Fibonacci
  │
  ├─ Fase 1: Aprendizaje   → learn_arquetipo/dinamica/relator()
  ├─ Fase 1.1: Cluster    → cluster_pipeline()
  ├─ Fase 2: Ciclo        → process_complete_cycle()
  ├─ Fase 3: Validación   → validate_dimension()
  │
  └─ save_knowledge()      → guardar state.bin
```

---

## 3. VALIDACIÓN Y SEGURIDAD

### 3.1 Validaciones en Persistencia

```c
// Validación de apertura
if (!f) {
    printf("❌ Error: No se puede abrir %s\n", filename);
    return;
}

// Validación de límites
if (n_arquetipos > MAX_MEM || ...) {
    printf("❌ Error: Archivo corrupto\n");
    fclose(f);
    return;
}
```

### 3.2 Validaciones en REPL

```c
// Parseo de trit
int val = parse_trit(*p);
if (val == -1) {
    printf("❌ Error: Carácter inválido '%c'\n", *p);
    goto next_cmd;  // Saltar comando
}

// Conteo de trits
if (idx != 9) {
    printf("❌ Error: Se esperaban 9 trits, se obtuvieron %d\n", idx);
    goto next_cmd;
}

// Validación vectorial
if (!validate_vector(&v_test)) {
    printf("⚠️ Vector inválido (auto-referencia detectada)\n");
    goto next_cmd;
}
```

---

## 4. ESTRUCTURAS DE DATOS

### 4.1 Tamaños en Bytes

```
Arquetipo:     40 bytes (patrón[3] + fo_output + support + rev)
Dinamica:      32 bytes (state_before[3] + state_after[3] + fn_output + support + rev)
Relator:       32 bytes (dim_a[3] + dim_b[3] + mode[3] + support + rev)
AxiomTrio:      3 bytes (3 × Trit)
EnergeticState: 3 bytes (3 × Trit)
Dimension:     12 bytes (3 × Trit)
────────────────────────────────────
Por sesión típica (3 arquetipos, 4 dinámicas, 4 relatores):
    20 + 3×40 + 4×32 + 4×32 + 18 = 444 bytes
```

---

## 5. COMPATIBILIDAD Y LIMITACIONES

### 5.1 Limitaciones Conocidas

| Limitación | Valor | Impacto |
|-----------|-------|--------|
| MAX_CLUSTER | 64 | Máximo 64 tensores en pipeline |
| MAX_MEM | 256 | Máximo 256 arquetipos/dinámicas/relatores |
| Buffer interactivo | 256 bytes | Máximo 256 caracteres por comando |
| Archivo binario | 4 GB teórico | No probado con archivos grandes |

### 5.2 Consideraciones de Portabilidad

- **Endianness**: Binario es específico de arquitectura (Intel x86-64)
- **Padding de struct**: Puede variar entre compiladores
- **Tamaño de tipos**: `int` puede ser 16, 32 o 64 bits
- **Solución**: Convertir a JSON o usar formato marshalling explícito

---

## 6. TESTING Y VALIDACIÓN

### Test Suite Sugerida

```c
// test_persistence.c
void test_save_load_roundtrip(void) {
    /* Guardar conocimiento */
    save_knowledge("test.bin");
    
    /* Resetear memoria */
    n_arquetipos = 0; n_dinamicas = 0; n_relatores = 0;
    
    /* Cargar y verificar */
    load_knowledge("test.bin");
    assert(n_arquetipos == 3);
    assert(n_dinamicas == 4);
    assert(n_relatores == 4);
}

void test_interactive_parse(void) {
    assert(parse_trit('u') == TRIT_U);
    assert(parse_trit('c') == TRIT_C);
    assert(parse_trit('n') == TRIT_N);
    assert(parse_trit('x') == -1);
}

void test_cli_arguments(void) {
    // Simular: aurora_core -i
    // Verificar que load_knowledge() se llama
    // Verificar que interactive_aurora_loop() se llama
}
```

---

## 7. INTEGRACIÓN CON WHITEPAPER

### Conexión con Technical Annex

Las funciones de persistencia e interacción **NO modifican** el núcleo científico:

✓ **Trigate** - Sin cambios  
✓ **Emergencia reversible** - Sin cambios  
✓ **Ciclo Info→Knowledge→Energy** - Sin cambios  
✓ **Validación ES≠FO** - Sin cambios  
✓ **Fibonacci ternario** - Sin cambios  

**Solo se añaden**:
- UI para experimentación humana
- Persistencia para evolución entre sesiones
- Argumentos CLI para facilidad de uso

---

## 8. ESPECIFICACIONES FINALES

### Líneas de Código

```
Original (v3.0.1):     1058 líneas
Nuevo (v3.0.2):      +280 líneas
Total:                1338 líneas
────────────────────────────────
Incremento:           26.5%
```

### Compilación

```bash
$ gcc -Wall -Wextra -g aurora_core_refactored.c -o aurora_core_refactored.exe
Warnings: 4 (funciones no utilizadas - ACEPTABLE)
Errors: 0
Size: 304.63 KB (executable)
```

### Ejecución

```bash
$ aurora_core_refactored.exe
[Demo normal: 3-5 segundos]

$ aurora_core_refactored.exe -i
aurora> [interactivo: indefinido]

$ aurora_core_refactored.exe --load aurora_knowledge.bin
[Demo con conocimiento previo: 3-5 segundos]
```

---

## Conclusión

Las mejoras v3.0.2 añaden **capacidades prácticas** sin comprometer la **elegancia matemática** de Aurora Core. El sistema ahora es:

✓ **Experimentable** interactivamente  
✓ **Persistente** entre sesiones  
✓ **Evolucionable** mediante aprendizaje acumulativo  
✓ **Robusto** con validación completa  
✓ **Fácil de usar** con interfaz REPL  

Manteniendo su posición como sistema de inteligencia fractal basado en los principios del Whitepaper v2.1.
