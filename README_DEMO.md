# 🌌 Aurora Interactive Demo - Guía Rápida

## 🚀 Ejecutar el Demo

### Opción 1: Con el entorno virtual activado
```bash
cd C:\Users\p_m_a\Aurora\Trinity-3
.venv\Scripts\activate
streamlit run demo_aurora_interactive.py
```

### Opción 2: Sin activar el entorno
```bash
cd C:\Users\p_m_a\Aurora\Trinity-3
C:/Users/p_m_a/Aurora/Trinity-3/.venv/Scripts/streamlit.exe run demo_aurora_interactive.py
```

El demo se abrirá automáticamente en tu navegador en `http://localhost:8501` (o el próximo puerto disponible).

---

## 📚 Módulos Disponibles

### 🏠 **Inicio**
- Introducción a Aurora
- Características clave
- Recursos y documentación

### 🔷 **Trigate Simulator**
**¿Qué hace?**
- Simula operaciones de lógica ternaria (0, 1, None)
- Operaciones: `learn(A, B, C) → Ms` y `infer(A, B, Ms) → Ss`

**Cómo usarlo:**
1. Selecciona valores para vectores A, B, C (cada uno de 3 trits)
2. O carga un preset predefinido
3. Haz clic en "🚀 Ejecutar Trigate"
4. Observa Ms (síntesis) y Ss (forma factual)

**Presets disponibles:**
- **Todo 1s**: `[1,1,1]` en todos los vectores
- **Todo 0s**: `[0,0,0]` en todos los vectores
- **Mix binario**: Combinación de 0s y 1s
- **Con Nones**: Incluye valores `None` (desconocido)
- **Patrón XOR**: Patrón exclusivo

### 🌐 **Fractal Tensor 3D**
**¿Qué hace?**
- Visualiza la estructura jerárquica 3×9×27
- Muestra conexiones entre niveles (nivel_3 → nivel_9 → nivel_27)

**Cómo usarlo:**
1. **Aleatorio**: Genera un tensor con probabilidad configurable de Nones
2. **Cargar ejemplo**: 
   - Patrón simple: Repetición ordenada
   - Alta fragmentación: ~50% Nones
   - Coherente completo: Sin Nones
3. Visualización 3D interactiva con Plotly
4. Métricas de fragmentación (conteo de Nones por nivel)

**Niveles del Fractal Tensor:**
- **🔴 Nivel 3 (Z=2)**: Abstracto - 3 nodos principales
- **🔵 Nivel 9 (Z=1)**: Intermedio - 9 nodos de concepto
- **🟢 Nivel 27 (Z=0)**: Concreto - 27 nodos base

### 🧠 **Transcender Explorer**
**¿Qué hace?**
- Ejecuta síntesis triple: `solve(M1, M2, M3)`
- Retorna `Ms` (síntesis), `Ss` (forma factual), y `MetaM` (audit trail)

**Cómo usarlo:**
1. Define 3 vectores M1, M2, M3 (cada uno de 3 trits)
2. Haz clic en "⚡ Ejecutar Triple Synthesis"
3. Observa los resultados:
   - **Ms**: Patrón sintetizado que maximiza coherencia
   - **Ss**: Forma inferida desde (M1, M2, Ms)
   - **MetaM**: [M1, M2, M3, Ms] - trazabilidad completa
4. Analiza el gráfico de coherencia entre vectores

**Métrica de Coherencia:**
- Score de 0.0 a 1.0
- Basado en similitud bit a bit (ignorando Nones)

### 📚 **Evolver Knowledge Base**
**¿Qué hace?**
- Muestra los 3 bancos de aprendizaje:
  1. **🔗 Relator**: Relaciones dimensionales
  2. **✨ Emergencia**: Patrones de síntesis
  3. **⏱️ Dinámicas**: Transiciones temporales

**Cómo usarlo:**
1. **Ver bancos**: Explora los top 5 patrones de cada banco
2. **Entrenar**: 
   - Haz clic en "🎲 Generar y entrenar patrón aleatorio"
   - El sistema genera M1, M2, M3 aleatorios
   - Ejecuta `solve()` para obtener Ms, Ss
   - Almacena en `relator_bank` y `emergence_bank`
3. **Limpiar**: Reinicia todos los bancos

**Estructura de los Bancos:**
```python
relator_bank: {key: (wiring, role) → proto, weight, count}
emergence_bank: {key: (M1, M2, M3) → Ms, Ss, count}
dynamics_bank: {key: (tag, t) → sequence, deltas}
```

### 🔧 **Harmonizer Inspector**
**¿Qué hace?**
- Simula conflictos (Nones, inconsistencias)
- Ejecuta el proceso de harmonización
- Muestra audit trail completo

**Tipos de Conflicto:**
1. **Nones en Ms_parent**: Padre con valores desconocidos
2. **Hijos inconsistentes**: Children con Nones y conflictos
3. **Fragmentación severa**: >60% Nones en todo el sistema

**Cómo usarlo:**
1. Selecciona un tipo de conflicto
2. Haz clic en "🚀 Ejecutar Harmonización"
3. Observa:
   - ✅ **Reparado**: Si se resolvió el conflicto
   - ⚠️ **Escalado**: Si requirió crear nuevo arquetipo
   - **Pasos**: Número de iteraciones (máx 5)
4. Revisa el **Audit Trail** detallado paso a paso

**Estrategias de Reparación:**
1. **Soft Re-rotation**: Ajustar hijos con hints de Ss
2. **Contextual Extension**: Llenar Nones desde KB
3. **Hard Inference**: Forzar coherencia con `infer()`
4. **Escalation**: Crear nuevo arquetipo si falla todo

### 🎮 **Full Pipeline Playground**
**¿Qué hace?**
- Ejecuta el pipeline completo de Aurora
- Ingestión → Transcender → Evolver → Storage

**Cómo usarlo:**
1. **Generar datos**: 
   - Número de muestras: 1-10
   - Probabilidad de None: 0-50%
   - Haz clic en "🎲 Generar e Ingerir"
2. **Ver estado**:
   - Tensors ingeridos
   - Patrones en Relator/Emergencia
3. **Explorar storage**:
   - Selecciona un tensor por tag
   - Visualiza en 3D
   - Inspecciona metadata

**Pipeline Flow:**
```
Input → FractalTensor → Transcender.solve() 
  → Evolver.observe_*() → Storage → Extender → Output
```

---

## 🔍 Características Técnicas

### **Lógica Ternaria**
- **0**: False/Absent
- **1**: True/Present
- **None**: Unknown/Uncertain

### **Operaciones Trigate**
```python
Ms = Trigate.learn(A, B, C)   # Síntesis
Ss = Trigate.infer(A, B, Ms)  # Inferencia
```

### **Estructura Fractal**
- **3 niveles jerárquicos**: 3 → 9 → 27
- **Síntesis bottom-up**: nivel_27 → nivel_9 → nivel_3
- **Coherencia top-down**: nivel_3 impone restricciones

### **Métricas**
- **Score**: Total de Nones en resultado (menor es mejor)
- **Coherencia**: Similitud entre vectores (0.0-1.0)
- **Fragmentación**: % de Nones en cada nivel

---

## 🐛 Troubleshooting

### Error: `cannot import name 'Trigate3'`
**Solución**: Las clases correctas son:
- `Trigate` (no `Trigate3`)
- `Transcender` (no `Transcender3`)
- `Evolver3` ✓ (correcto)

### Error: `'dict' object has no attribute 'Ms'`
**Solución**: `solve()` retorna un diccionario:
```python
result = transcender.solve(M1, M2, M3)
Ms = result["Ms"]  # ✓ Correcto
Ss = result["Ss"]  # ✓ Correcto
```

### Error: `'FractalTensor' has no attribute 'L3'`
**Solución**: Los atributos correctos son:
- `nivel_3` (no `L3`)
- `nivel_9` (no `L9`)
- `nivel_27` (no `L27`)

### El servidor no inicia
```bash
# Verificar que streamlit esté instalado
pip list | grep streamlit

# Si no está, instalar
pip install streamlit plotly numpy pandas
```

---

## 📊 Ejemplos de Uso

### Ejemplo 1: Explorar Trigate
```python
# En el simulador
A = [1, 0, 1]
B = [0, 1, 0]
C = [1, 1, 0]

# Resultado esperado:
Ms = [1, 1, 0]  # Patrón sintetizado
Ss = [0, 1, 1]  # Forma factual
```

### Ejemplo 2: Generar Fractal Tensor
```python
# En Fractal Tensor 3D
1. Modo: "Aleatorio"
2. Prob None: 20%
3. Click "Generar"

# Resultado:
- 27 vectores base generados
- Síntesis automática hacia arriba
- Visualización 3D interactiva
```

### Ejemplo 3: Entrenar Evolver
```python
# En Evolver KB
1. Click "🎲 Generar y entrenar"
2. Repetir 5-10 veces
3. Ver bancos poblados con patrones

# Efecto:
- relator_bank: 5+ patrones
- emergence_bank: 5+ síntesis
```

---

## 🎨 Personalización

### Cambiar colores del tema
Edita `demo_aurora_interactive.py`:
```python
# Línea ~57
.metric-card {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    # Cambia los colores aquí
}
```

### Ajustar límites
```python
# Línea ~346
st.session_state.evolver = Evolver3(Trigate, th_match=2)
# Cambia th_match para ajustar sensibilidad de matching
```

---

## 📝 Logs y Debugging

### Ver salida de terminal
```bash
# El servidor muestra:
  Local URL: http://localhost:8501
  Network URL: http://192.168.1.35:8501
  
# Si hay errores, aparecerán en el terminal
```

### Streamlit auto-recarga
- Cada vez que guardas `demo_aurora_interactive.py`
- Streamlit detecta cambios y recarga automáticamente
- Mensaje: "Source file changed, rerunning..."

---

## 🔗 Links Útiles

- **Paper**: `PAPER_Aurora_Fractal_Intelligence.md`
- **Verificación**: `VERIFICACION_DISENO_VS_IMPLEMENTACION.md`
- **Tests**: `v2.0/test_*.py`
- **Código fuente**: `v2.0/*.py`

---

## 🤝 Contribuir

Para mejorar el demo:
1. Fork el repositorio
2. Crea una rama: `git checkout -b feature/mi-mejora`
3. Commit: `git commit -m "Agrega nueva visualización"`
4. Push: `git push origin feature/mi-mejora`
5. Abre un Pull Request

---

**¡Disfruta explorando Aurora! 🌌**
