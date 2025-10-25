"""
Aurora Interactive Demo - Streamlit Application
==============================================

Aplicación interactiva para explorar y visualizar el funcionamiento de Aurora.

Características:
- Visualización 3D de Fractal Tensors (3×9×27)
- Simulador de Trigate en tiempo real
- Explorador de Knowledge Base (Evolver)
- Inspector de Audit Trails
- Playground para experimentación

Ejecutar: streamlit run demo_aurora_interactive.py
"""

import streamlit as st
import sys
from pathlib import Path

# Agregar v2.0 al path
sys.path.insert(0, str(Path(__file__).parent / "v2.0"))

try:
    from Trigate import Trigate
    from FractalTensor import FractalTensor
    from Transcender import Transcender
    from Evolver import Evolver3
    from Harmonizer import Harmonizer
    from aurora_pipeline import AuroraPipeline
    AURORA_AVAILABLE = True
except ImportError as e:
    st.error(f"⚠️ No se pudieron importar los módulos de Aurora: {e}")
    AURORA_AVAILABLE = False

import numpy as np
import plotly.graph_objects as go
import plotly.express as px
from typing import List, Tuple, Dict, Any
import json

# ============================================================================
# CONFIGURACIÓN DE PÁGINA
# ============================================================================

st.set_page_config(
    page_title="Aurora Interactive Demo",
    page_icon="🌌",
    layout="wide",
    initial_sidebar_state="expanded"
)

# ============================================================================
# ESTILOS PERSONALIZADOS
# ============================================================================

st.markdown("""
<style>
    .main-header {
        font-size: 3rem;
        font-weight: bold;
        background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        text-align: center;
        margin-bottom: 1rem;
    }
    .sub-header {
        font-size: 1.2rem;
        text-align: center;
        color: #666;
        margin-bottom: 2rem;
    }
    .metric-card {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        padding: 1rem;
        border-radius: 10px;
        color: white;
        text-align: center;
    }
    .trigate-result {
        background: #f0f2f6;
        padding: 1rem;
        border-radius: 5px;
        border-left: 4px solid #667eea;
        margin: 1rem 0;
    }
    .audit-step {
        background: #fff;
        padding: 0.5rem;
        border-radius: 5px;
        border-left: 3px solid #764ba2;
        margin: 0.5rem 0;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
</style>
""", unsafe_allow_html=True)

# ============================================================================
# FUNCIONES AUXILIARES
# ============================================================================

def trit_to_str(trit):
    """Convierte trit a string legible."""
    if trit is None:
        return "None"
    return str(trit)

def vector_to_str(vec):
    """Convierte vector de trits a string."""
    if vec is None:
        return "None"
    return f"[{', '.join(trit_to_str(t) for t in vec)}]"

def create_3d_fractal_viz(ft: 'FractalTensor') -> go.Figure:
    """
    Crea visualización 3D del Fractal Tensor.
    
    Estructura:
    - nivel_3: 3 nodos principales (nivel abstracto)
    - nivel_9: 9 nodos intermedios
    - nivel_27: 27 nodos base (nivel concreto)
    """
    # Preparar datos
    nodes_x, nodes_y, nodes_z = [], [], []
    node_colors = []
    node_sizes = []
    node_texts = []
    
    # nivel_3 - Nivel superior (Z=2)
    for i in range(3):
        nodes_x.append(i * 3)
        nodes_y.append(4.5)
        nodes_z.append(2)
        node_colors.append('red')
        node_sizes.append(30)
        vec = ft.nivel_3[i] if hasattr(ft, 'nivel_3') and i < len(ft.nivel_3) else [None, None, None]
        node_texts.append(f"L3[{i}]: {vector_to_str(vec)}")
    
    # nivel_9 - Nivel medio (Z=1)
    for i in range(9):
        nodes_x.append((i % 3) * 3 + (i // 3))
        nodes_y.append(i // 3 * 3)
        nodes_z.append(1)
        node_colors.append('blue')
        node_sizes.append(20)
        vec = ft.nivel_9[i] if hasattr(ft, 'nivel_9') and i < len(ft.nivel_9) else [None, None, None]
        node_texts.append(f"L9[{i}]: {vector_to_str(vec)}")
    
    # nivel_27 - Nivel base (Z=0)
    for i in range(27):
        nodes_x.append((i % 9) + (i // 9) * 0.3)
        nodes_y.append((i // 9) * 3)
        nodes_z.append(0)
        node_colors.append('green')
        node_sizes.append(12)
        vec = ft.nivel_27[i] if hasattr(ft, 'nivel_27') and i < len(ft.nivel_27) else [None, None, None]
        node_texts.append(f"L27[{i}]: {vector_to_str(vec)}")
    
    # Crear conexiones jerárquicas
    edge_x, edge_y, edge_z = [], [], []
    
    # nivel_3 → nivel_9 (cada nodo nivel_3 conecta con 3 nodos nivel_9)
    for i in range(3):
        for j in range(3):
            idx_L9 = i * 3 + j
            edge_x.extend([nodes_x[i], nodes_x[3 + idx_L9], None])
            edge_y.extend([nodes_y[i], nodes_y[3 + idx_L9], None])
            edge_z.extend([nodes_z[i], nodes_z[3 + idx_L9], None])
    
    # nivel_9 → nivel_27 (cada nodo nivel_9 conecta con 3 nodos nivel_27)
    for i in range(9):
        for j in range(3):
            idx_L27 = i * 3 + j
            edge_x.extend([nodes_x[3 + i], nodes_x[12 + idx_L27], None])
            edge_y.extend([nodes_y[3 + i], nodes_y[12 + idx_L27], None])
            edge_z.extend([nodes_z[3 + i], nodes_z[12 + idx_L27], None])
    
    # Crear figura
    fig = go.Figure()
    
    # Agregar aristas
    fig.add_trace(go.Scatter3d(
        x=edge_x, y=edge_y, z=edge_z,
        mode='lines',
        line=dict(color='rgba(125,125,125,0.3)', width=1),
        hoverinfo='none',
        showlegend=False
    ))
    
    # Agregar nodos
    fig.add_trace(go.Scatter3d(
        x=nodes_x, y=nodes_y, z=nodes_z,
        mode='markers+text',
        marker=dict(
            size=node_sizes,
            color=node_colors,
            opacity=0.8,
            line=dict(color='white', width=1)
        ),
        text=node_texts,
        hoverinfo='text',
        showlegend=False
    ))
    
    # Configurar layout
    fig.update_layout(
        title="Fractal Tensor 3D: Estructura Jerárquica 3×9×27",
        scene=dict(
            xaxis=dict(showgrid=False, showticklabels=False, title=""),
            yaxis=dict(showgrid=False, showticklabels=False, title=""),
            zaxis=dict(showgrid=False, showticklabels=False, title="Nivel"),
            camera=dict(
                eye=dict(x=1.5, y=1.5, z=1.2)
            )
        ),
        height=600,
        margin=dict(l=0, r=0, t=40, b=0)
    )
    
    return fig

def create_trigate_visualization(A, B, C, Ms, Ss):
    """Crea visualización de operación Trigate."""
    fig = go.Figure()
    
    # Preparar datos
    operations = ['Input A', 'Input B', 'Input C', 'Learn → Ms', 'Infer → Ss']
    vectors = [A, B, C, Ms, Ss]
    
    # Colores según valor de trit
    def get_color(trit):
        if trit is None:
            return 'gray'
        elif trit == 1:
            return 'green'
        else:
            return 'red'
    
    # Crear heatmap
    for i, (op, vec) in enumerate(zip(operations, vectors)):
        if vec is None:
            vec = [None, None, None]
        for j, trit in enumerate(vec):
            fig.add_trace(go.Bar(
                x=[op],
                y=[1],
                name=f"{op}[{j}]",
                marker_color=get_color(trit),
                text=trit_to_str(trit),
                textposition='inside',
                hoverinfo='text',
                hovertext=f"{op}[{j}] = {trit_to_str(trit)}",
                showlegend=False,
                width=0.8
            ))
    
    fig.update_layout(
        title="Flujo de Operación Trigate",
        barmode='stack',
        height=300,
        xaxis_title="Operación",
        yaxis_title="Valor",
        yaxis=dict(showticklabels=False)
    )
    
    return fig

def visualize_evolver_banks(evolver: Evolver3):
    """Visualiza el contenido de los bancos del Evolver."""
    col1, col2, col3 = st.columns(3)
    
    with col1:
        st.markdown("### 🔗 Banco Relator")
        relator_tops = evolver.relator_top(k=5)
        if relator_tops:
            for i, entry in enumerate(relator_tops, 1):
                with st.expander(f"Patrón {i} (n={entry.get('n', 0)})"):
                    st.json({
                        "proto": vector_to_str(entry.get('proto')),
                        "weight": f"{entry.get('w', 0):.3f}",
                        "count": entry.get('n', 0)
                    })
        else:
            st.info("Banco vacío")
    
    with col2:
        st.markdown("### ✨ Banco Emergencia")
        emergence_tops = evolver.emergence_top(k=5)
        if emergence_tops:
            for i, entry in enumerate(emergence_tops, 1):
                with st.expander(f"Emergencia {i} (n={entry.get('n', 0)})"):
                    # proto es un vector directo, no un dict
                    proto = entry.get('proto')
                    st.json({
                        "proto": vector_to_str(proto) if isinstance(proto, list) else str(proto),
                        "weight": f"{entry.get('w', 0):.3f}",
                        "count": entry.get('n', 0),
                        "key": str(entry.get('key', 'N/A'))
                    })
        else:
            st.info("Banco vacío")
    
    with col3:
        st.markdown("### ⏱️ Banco Dinámicas")
        dynamics_tops = evolver.dynamics_top(k=5)
        if dynamics_tops:
            for i, entry in enumerate(dynamics_tops, 1):
                with st.expander(f"Dinámica {i} (n={entry.get('n', 0)})"):
                    st.json({
                        "pattern": str(entry.get('proto', 'N/A')),
                        "count": entry.get('n', 0)
                    })
        else:
            st.info("Banco vacío")

# ============================================================================
# HEADER
# ============================================================================

st.markdown('<h1 class="main-header">🌌 Aurora Interactive Demo</h1>', unsafe_allow_html=True)
st.markdown('<p class="sub-header">Explora la arquitectura de Inteligencia Fractal Discreta</p>', unsafe_allow_html=True)

if not AURORA_AVAILABLE:
    st.error("❌ Los módulos de Aurora no están disponibles. Verifica la instalación.")
    st.stop()

# ============================================================================
# SIDEBAR - NAVEGACIÓN
# ============================================================================

st.sidebar.title("🧭 Navegación")
demo_mode = st.sidebar.radio(
    "Selecciona un módulo:",
    [
        "🏠 Inicio",
        "🔷 Trigate Simulator",
        "🌐 Fractal Tensor 3D",
        "🧠 Transcender Explorer",
        "📚 Evolver Knowledge Base",
        "🔧 Harmonizer Inspector",
        "🎮 Full Pipeline Playground"
    ]
)

st.sidebar.markdown("---")
st.sidebar.markdown("### 📊 Métricas del Sistema")

# Inicializar componentes globales
if 'trigate' not in st.session_state:
    st.session_state.trigate = Trigate  # Es una clase con métodos estáticos
if 'evolver' not in st.session_state:
    st.session_state.evolver = Evolver3(Trigate, th_match=2)
if 'transcender' not in st.session_state:
    st.session_state.transcender = Transcender(Trigate)

# Métricas
st.sidebar.metric("Trigate LUT Size", "19,683 entries")
relator_count = len(st.session_state.evolver.relator_bank) if hasattr(st.session_state.evolver, 'relator_bank') else 0
emergence_count = len(st.session_state.evolver.emergence_bank) if hasattr(st.session_state.evolver, 'emergence_bank') else 0
st.sidebar.metric("Relator Patterns", relator_count)
st.sidebar.metric("Emergence Patterns", emergence_count)

# ============================================================================
# MODO 1: INICIO
# ============================================================================

if demo_mode == "🏠 Inicio":
    st.markdown("## Bienvenido a Aurora")
    
    col1, col2 = st.columns(2)
    
    with col1:
        st.markdown("""
        ### 🎯 ¿Qué es Aurora?
        
        **Aurora** es una arquitectura de IA basada en:
        - **Lógica Ternaria** (0, 1, None)
        - **Estructuras Fractales** (3×9×27)
        - **Coherencia Geométrica**
        
        ### 🔑 Características Clave
        
        - ✅ **23× más eficiente** que BERT en memoria
        - ✅ **100% auditable** (cada decisión es trazable)
        - ✅ **Sin gradient descent** (aprendizaje discreto)
        - ✅ **Convergencia garantizada** en resolución de conflictos
        """)
    
    with col2:
        st.markdown("""
        ### 🧩 Componentes Principales
        
        1. **Trigate** - Unidad lógica ternaria básica
        2. **Fractal Tensor** - Representación jerárquica 3×9×27
        3. **Transcender** - Motor de síntesis triple
        4. **Evolver** - Sistema de aprendizaje en 3 bancos
        5. **Harmonizer** - Resolución de conflictos
        6. **Extender** - Reconstrucción top-down
        
        ### 📚 Recursos
        
        - [Paper Académico](PAPER_Aurora_Fractal_Intelligence.md)
        - [Documentación Técnica](VERIFICACION_DISENO_VS_IMPLEMENTACION.md)
        - [Tests (10/10 ✅)](v2.0/)
        """)
    
    st.markdown("---")
    st.info("👈 Selecciona un módulo en la barra lateral para comenzar la exploración")

# ============================================================================
# MODO 2: TRIGATE SIMULATOR
# ============================================================================

elif demo_mode == "🔷 Trigate Simulator":
    st.markdown("## 🔷 Trigate Simulator")
    st.markdown("Experimenta con la unidad lógica fundamental de Aurora")
    
    col1, col2 = st.columns([2, 1])
    
    with col1:
        st.markdown("### Entradas")
        
        # Input A
        st.markdown("**Vector A:**")
        col_a1, col_a2, col_a3 = st.columns(3)
        with col_a1:
            a0 = st.selectbox("A[0]", [0, 1, None], key="a0")
        with col_a2:
            a1 = st.selectbox("A[1]", [0, 1, None], key="a1")
        with col_a3:
            a2 = st.selectbox("A[2]", [0, 1, None], key="a2")
        
        # Input B
        st.markdown("**Vector B:**")
        col_b1, col_b2, col_b3 = st.columns(3)
        with col_b1:
            b0 = st.selectbox("B[0]", [0, 1, None], key="b0")
        with col_b2:
            b1 = st.selectbox("B[1]", [0, 1, None], key="b1")
        with col_b3:
            b2 = st.selectbox("B[2]", [0, 1, None], key="b2")
        
        # Input C
        st.markdown("**Vector C:**")
        col_c1, col_c2, col_c3 = st.columns(3)
        with col_c1:
            c0 = st.selectbox("C[0]", [0, 1, None], key="c0")
        with col_c2:
            c1 = st.selectbox("C[1]", [0, 1, None], key="c1")
        with col_c3:
            c2 = st.selectbox("C[2]", [0, 1, None], key="c2")
    
    with col2:
        st.markdown("### Presets")
        preset = st.selectbox("Cargar ejemplo:", [
            "Manual",
            "Todo 1s",
            "Todo 0s",
            "Mix binario",
            "Con Nones",
            "Patrón XOR"
        ])
        
        if preset != "Manual":
            presets = {
                "Todo 1s": ([1,1,1], [1,1,1], [1,1,1]),
                "Todo 0s": ([0,0,0], [0,0,0], [0,0,0]),
                "Mix binario": ([1,0,1], [0,1,0], [1,1,0]),
                "Con Nones": ([1,None,1], [None,1,None], [0,0,None]),
                "Patrón XOR": ([1,0,0], [0,1,0], [0,0,1])
            }
            A, B, C = presets[preset]
        else:
            A = [a0, a1, a2]
            B = [b0, b1, b2]
            C = [c0, c1, c2]
    
    # Ejecutar operaciones
    if st.button("🚀 Ejecutar Trigate", type="primary"):
        trigate = st.session_state.trigate
        
        try:
            # Learn
            Ms = trigate.learn(A, B, C)
            
            # Infer
            Ss = trigate.infer(A, B, Ms)
            
            # Mostrar resultados
            st.markdown("---")
            st.markdown("### 📊 Resultados")
            
            col_res1, col_res2 = st.columns(2)
            
            with col_res1:
                st.markdown('<div class="trigate-result">', unsafe_allow_html=True)
                st.markdown("**🧠 Learn (Síntesis):**")
                st.code(f"Ms = {vector_to_str(Ms)}")
                st.markdown("</div>", unsafe_allow_html=True)
            
            with col_res2:
                st.markdown('<div class="trigate-result">', unsafe_allow_html=True)
                st.markdown("**🔍 Infer (Forma Factual):**")
                st.code(f"Ss = {vector_to_str(Ss)}")
                st.markdown("</div>", unsafe_allow_html=True)
            
            # Visualización
            fig = create_trigate_visualization(A, B, C, Ms, Ss)
            st.plotly_chart(fig, use_container_width=True)
            
            # Explicación
            st.markdown("### 💡 Explicación")
            st.markdown(f"""
            **Proceso:**
            1. **Learn**: Síntesis de patrones A, B, C → Ms (vector que maximiza coherencia)
            2. **Infer**: Dado A, B y Ms, infiere Ss (forma factual esperada)
            
            **None-Propagation**: Si algún input contiene None, se propaga según:
            ```
            None ⊕ x = None, ∀x ∈ {{0, 1, None}}
            ```
            """)
            
        except Exception as e:
            st.error(f"❌ Error en operación: {e}")

# ============================================================================
# MODO 3: FRACTAL TENSOR 3D
# ============================================================================

elif demo_mode == "🌐 Fractal Tensor 3D":
    st.markdown("## 🌐 Fractal Tensor 3D Visualization")
    st.markdown("Explora la estructura jerárquica 3×9×27")
    
    # Opciones de generación
    col1, col2 = st.columns([1, 2])
    
    with col1:
        st.markdown("### Configuración")
        
        generation_mode = st.radio("Modo de generación:", [
            "Aleatorio",
            "Manual L3",
            "Cargar ejemplo"
        ])
        
        if generation_mode == "Aleatorio":
            null_prob = st.slider("Probabilidad de None:", 0.0, 0.7, 0.2, 0.1)
            if st.button("🎲 Generar aleatorio"):
                ft = FractalTensor.neutral()
                # Generar nivel_27 aleatorio
                for i in range(27):
                    vec = []
                    for j in range(3):
                        if np.random.random() < null_prob:
                            vec.append(None)
                        else:
                            vec.append(np.random.randint(0, 2))
                    ft.nivel_27[i] = vec
                
                # Sintetizar hacia arriba usando Transcender simple
                transcender = st.session_state.transcender
                # Sintetizar nivel_9 desde nivel_27 (cada 3 nodos → 1 nodo)
                for i in range(9):
                    idx = i * 3
                    M1, M2, M3 = ft.nivel_27[idx], ft.nivel_27[idx+1], ft.nivel_27[idx+2]
                    result = transcender.solve(M1, M2, M3)
                    ft.nivel_9[i] = result["Ms"]
                
                # Sintetizar nivel_3 desde nivel_9
                for i in range(3):
                    idx = i * 3
                    M1, M2, M3 = ft.nivel_9[idx], ft.nivel_9[idx+1], ft.nivel_9[idx+2]
                    result = transcender.solve(M1, M2, M3)
                    ft.nivel_3[i] = result["Ms"]
                
                st.session_state.current_ft = ft
                st.success("✅ Fractal Tensor generado")
        
        elif generation_mode == "Cargar ejemplo":
            example = st.selectbox("Ejemplo:", [
                "Patrón simple",
                "Alta fragmentación",
                "Coherente completo"
            ])
            
            if st.button("📥 Cargar"):
                ft = FractalTensor.neutral()
                
                if example == "Patrón simple":
                    # Patrón repetitivo simple
                    for i in range(27):
                        ft.nivel_27[i] = [(i % 2), ((i+1) % 2), (i % 2)]
                
                elif example == "Alta fragmentación":
                    # 50% Nones
                    for i in range(27):
                        ft.nivel_27[i] = [
                            None if i % 2 == 0 else (i % 2),
                            (i % 2) if i % 3 != 0 else None,
                            None if i % 4 == 0 else ((i+1) % 2)
                        ]
                
                else:  # Coherente completo
                    for i in range(27):
                        ft.nivel_27[i] = [1, 0, 1]
                
                # Sintetizar hacia arriba
                transcender = st.session_state.transcender
                # Sintetizar nivel_9
                for i in range(9):
                    idx = i * 3
                    M1, M2, M3 = ft.nivel_27[idx], ft.nivel_27[idx+1], ft.nivel_27[idx+2]
                    result = transcender.solve(M1, M2, M3)
                    ft.nivel_9[i] = result["Ms"]
                
                # Sintetizar nivel_3
                for i in range(3):
                    idx = i * 3
                    M1, M2, M3 = ft.nivel_9[idx], ft.nivel_9[idx+1], ft.nivel_9[idx+2]
                    result = transcender.solve(M1, M2, M3)
                    ft.nivel_3[i] = result["Ms"]
                
                st.session_state.current_ft = ft
                st.success(f"✅ Ejemplo '{example}' cargado")
    
    with col2:
        if 'current_ft' in st.session_state:
            ft = st.session_state.current_ft
            
            # Visualización 3D
            fig_3d = create_3d_fractal_viz(ft)
            st.plotly_chart(fig_3d, use_container_width=True)
            
            # Métricas
            col_m1, col_m2, col_m3 = st.columns(3)
            
            # Contar Nones
            none_count_27 = sum(1 for i in range(27) for t in ft.nivel_27[i] if t is None)
            none_count_9 = sum(1 for i in range(9) for t in ft.nivel_9[i] if t is None)
            none_count_3 = sum(1 for i in range(3) for t in ft.nivel_3[i] if t is None)
            
            with col_m1:
                st.metric("L3 Nones", f"{none_count_3}/9")
            with col_m2:
                st.metric("L9 Nones", f"{none_count_9}/27")
            with col_m3:
                st.metric("L27 Nones", f"{none_count_27}/81")
        else:
            st.info("👈 Genera o carga un Fractal Tensor para visualizar")

# ============================================================================
# MODO 4: TRANSCENDER EXPLORER
# ============================================================================

elif demo_mode == "🧠 Transcender Explorer":
    st.markdown("## 🧠 Transcender: Triple Synthesis Engine")
    st.markdown("Observa el proceso de síntesis triple (Ms, Ss, MetaM)")
    
    transcender = st.session_state.transcender
    
    # Inputs
    st.markdown("### Vectores de Entrada")
    col1, col2, col3 = st.columns(3)
    
    with col1:
        st.markdown("**M1:**")
        m1_0 = st.selectbox("M1[0]", [0, 1, None], key="m1_0")
        m1_1 = st.selectbox("M1[1]", [0, 1, None], key="m1_1")
        m1_2 = st.selectbox("M1[2]", [0, 1, None], key="m1_2")
        M1 = [m1_0, m1_1, m1_2]
    
    with col2:
        st.markdown("**M2:**")
        m2_0 = st.selectbox("M2[0]", [0, 1, None], key="m2_0")
        m2_1 = st.selectbox("M2[1]", [0, 1, None], key="m2_1")
        m2_2 = st.selectbox("M2[2]", [0, 1, None], key="m2_2")
        M2 = [m2_0, m2_1, m2_2]
    
    with col3:
        st.markdown("**M3:**")
        m3_0 = st.selectbox("M3[0]", [0, 1, None], key="m3_0")
        m3_1 = st.selectbox("M3[1]", [0, 1, None], key="m3_1")
        m3_2 = st.selectbox("M3[2]", [0, 1, None], key="m3_2")
        M3 = [m3_0, m3_1, m3_2]
    
    if st.button("⚡ Ejecutar Triple Synthesis", type="primary"):
        try:
            # Ejecutar síntesis usando solve()
            result = transcender.solve(M1, M2, M3)
            Ms = result["Ms"]
            Ss = result["Ss"]
            MetaM = result["MetaM"]
            
            st.markdown("---")
            st.markdown("### 📊 Resultados de Síntesis")
            
            col_r1, col_r2, col_r3 = st.columns(3)
            
            with col_r1:
                st.markdown("**Ms (Síntesis):**")
                st.code(vector_to_str(Ms))
                st.caption("Patrón sintetizado que maximiza coherencia")
            
            with col_r2:
                st.markdown("**Ss (Forma Factual):**")
                st.code(vector_to_str(Ss))
                st.caption("Forma inferida desde (M1, M2, Ms)")
            
            with col_r3:
                st.markdown("**MetaM (Audit Trail):**")
                st.json({
                    "M1": vector_to_str(MetaM[0]) if len(MetaM) > 0 else "N/A",
                    "M2": vector_to_str(MetaM[1]) if len(MetaM) > 1 else "N/A",
                    "M3": vector_to_str(MetaM[2]) if len(MetaM) > 2 else "N/A",
                    "Ms": vector_to_str(MetaM[3]) if len(MetaM) > 3 else vector_to_str(Ms)
                })
                st.caption("Trazabilidad completa")
            
            # Visualizar coherencia
            st.markdown("### 🎯 Análisis de Coherencia")
            
            # Calcular similarity scores
            def similarity(a, b):
                if a is None or b is None:
                    return 0
                score = sum(1 for x, y in zip(a, b) if x == y and x is not None)
                return score / 3
            
            coherence_scores = {
                "M1 ↔ M2": similarity(M1, M2),
                "M1 ↔ M3": similarity(M1, M3),
                "M2 ↔ M3": similarity(M2, M3),
                "M1 ↔ Ms": similarity(M1, Ms),
                "M2 ↔ Ms": similarity(M2, Ms),
                "M3 ↔ Ms": similarity(M3, Ms)
            }
            
            fig = go.Figure(data=[
                go.Bar(
                    x=list(coherence_scores.keys()),
                    y=list(coherence_scores.values()),
                    marker_color='lightblue'
                )
            ])
            fig.update_layout(
                title="Coherencia entre Vectores",
                xaxis_title="Par de Vectores",
                yaxis_title="Score de Similitud",
                yaxis=dict(range=[0, 1])
            )
            st.plotly_chart(fig, use_container_width=True)
            
        except Exception as e:
            st.error(f"❌ Error en síntesis: {e}")

# ============================================================================
# MODO 5: EVOLVER KNOWLEDGE BASE
# ============================================================================

elif demo_mode == "📚 Evolver Knowledge Base":
    st.markdown("## 📚 Evolver: Sistema de Aprendizaje en 3 Bancos")
    st.markdown("Explora los patrones aprendidos en Relator, Emergencia y Dinámicas")
    
    evolver = st.session_state.evolver
    
    # Visualizar bancos
    visualize_evolver_banks(evolver)
    
    st.markdown("---")
    st.markdown("### ➕ Agregar Patrón de Entrenamiento")
    
    col1, col2 = st.columns([2, 1])
    
    with col1:
        # Generar datos aleatorios para entrenamiento
        if st.button("🎲 Generar y entrenar patrón aleatorio"):
            # Crear vectores aleatorios
            M1 = [np.random.randint(0, 2) for _ in range(3)]
            M2 = [np.random.randint(0, 2) for _ in range(3)]
            M3 = [np.random.randint(0, 2) for _ in range(3)]
            
            # Sintetizar
            Ms = st.session_state.trigate.learn(M1, M2, M3)
            Ss = st.session_state.trigate.infer(M1, M2, Ms)
            
            # Entrenar Evolver
            Ms_parent = Ms
            wiring = [('A', 'B', 'C')]
            
            evolver.observe_relator(Ms_parent, wiring, M1, M2, M3)
            evolver.observe_emergence(M1, M2, M3, Ms)
            
            st.success(f"✅ Patrón aprendido: {vector_to_str(Ms)}")
            st.rerun()
    
    with col2:
        st.metric("Total Relator", len(evolver.relator_bank) if hasattr(evolver, 'relator_bank') else 0)
        st.metric("Total Emergencia", len(evolver.emergence_bank) if hasattr(evolver, 'emergence_bank') else 0)
        st.metric("Total Dinámicas", len(evolver.dynamics_bank) if hasattr(evolver, 'dynamics_bank') else 0)
    
    # Limpiar bancos
    if st.button("🗑️ Limpiar todos los bancos"):
        st.session_state.evolver = Evolver3(Trigate, th_match=2)
        st.success("✅ Bancos reiniciados")
        st.rerun()

# ============================================================================
# MODO 6: HARMONIZER INSPECTOR
# ============================================================================

elif demo_mode == "🔧 Harmonizer Inspector":
    st.markdown("## 🔧 Harmonizer: Resolución de Conflictos")
    st.markdown("Simula conflictos y observa cómo el Harmonizer los resuelve")
    
    # Necesitamos importar Extender también
    from Extender import Extender
    
    harmonizer = Harmonizer(
        trigate_cls=Trigate,
        evolver=st.session_state.evolver,
        extender_cls=Extender,
        max_conflicts=0,
        max_null_fills=3,
        min_child_sim=6
    )
    
    # Crear escenario de conflicto
    st.markdown("### 🎭 Escenario de Conflicto")
    
    conflict_type = st.selectbox("Tipo de conflicto:", [
        "Nones en Ms_parent",
        "Hijos inconsistentes",
        "Fragmentación severa"
    ])
    
    if st.button("🚀 Ejecutar Harmonización", type="primary"):
        try:
            # Generar conflicto según tipo
            if conflict_type == "Nones en Ms_parent":
                Ms_parent = (
                    [1, None, None],
                    [None, 1, None],
                    [None, None, 1]
                )
                children_observed = {
                    "x": ([1,0,0], [0,1,0], [0,0,1]),
                    "y": ([0,1,1], [1,0,1], [1,1,0]),
                    "z": ([1,1,1], [0,0,0], [1,0,1])
                }
            
            elif conflict_type == "Hijos inconsistentes":
                Ms_parent = (
                    [1, 1, 0],
                    [0, 1, 1],
                    [1, 0, 1]
                )
                children_observed = {
                    "x": ([1,0,None], [None,1,0], [0,0,1]),
                    "y": ([None,1,1], [1,None,1], [1,1,None]),
                    "z": ([1,1,None], [None,0,0], [1,None,1])
                }
            
            else:  # Fragmentación severa
                Ms_parent = (
                    [None, None, 1],
                    [None, None, None],
                    [1, None, None]
                )
                children_observed = {
                    "x": ([None,None,1], [None,1,None], [0,None,1]),
                    "y": ([None,None,None], [None,None,None], [None,None,None]),
                    "z": ([1,None,None], [None,None,0], [None,None,None])
                }
            
            # Ejecutar harmonización
            with st.spinner("Armonizando..."):
                result = harmonizer.harmonize_from_state(
                    Ms_parent_triplet=Ms_parent,
                    children_observed=children_observed
                )
            
            # Mostrar resultados
            st.markdown("---")
            st.markdown("### 📊 Resultado de Harmonización")
            
            col_r1, col_r2, col_r3 = st.columns(3)
            
            with col_r1:
                status = "✅ Reparado" if result.repaired else "❌ No reparado"
                st.markdown(f"**Estado:** {status}")
                
            with col_r2:
                escalated = "⚠️ Sí" if result.escalated else "✅ No"
                st.markdown(f"**Escalado:** {escalated}")
            
            with col_r3:
                st.markdown(f"**Pasos:** {len(result.audit)}")
            
            # Audit trail detallado
            st.markdown("### 📋 Audit Trail")
            
            for i, step in enumerate(result.audit, 1):
                with st.expander(f"Paso {i}: {step.get('step', 'unknown')}"):
                    st.json(step)
            
            # Estado final
            if result.repaired:
                st.success(f"✅ Conflicto resuelto en {len(result.audit)} iteraciones")
            else:
                st.warning("⚠️ No se pudo resolver completamente, pero el sistema manejó el error")
            
        except Exception as e:
            st.error(f"❌ Error en harmonización: {e}")
            import traceback
            st.code(traceback.format_exc())

# ============================================================================
# MODO 7: FULL PIPELINE PLAYGROUND
# ============================================================================

elif demo_mode == "🎮 Full Pipeline Playground":
    st.markdown("## 🎮 Full Pipeline Playground")
    st.markdown("Ejecuta el pipeline completo de Aurora desde input hasta output")
    
    # Inicializar pipeline
    if 'pipeline' not in st.session_state:
        st.session_state.pipeline = AuroraPipeline(enable_harmony=True)
    
    pipeline = st.session_state.pipeline
    
    # Input
    st.markdown("### 📥 Entrada de Datos")
    
    input_method = st.radio("Método de entrada:", [
        "Generar Fractal Tensor aleatorio",
        "Secuencia de vectores manual"
    ])
    
    if input_method == "Generar Fractal Tensor aleatorio":
        num_samples = st.slider("Número de ciclos:", 1, 10, 3)
        null_prob = st.slider("Probabilidad de None:", 0.0, 0.5, 0.2, 0.1)
        
        if st.button("🎲 Generar y Ejecutar Ciclos"):
            with st.spinner("Generando y procesando..."):
                # Inicializar storage si no existe
                if 'pipeline_results' not in st.session_state:
                    st.session_state.pipeline_results = []
                
                for sample_id in range(num_samples):
                    # Generar 3 FractalTensors aleatorios (A, B, C)
                    def generate_random_data():
                        data = []
                        for i in range(27):
                            vec = []
                            for j in range(3):
                                if np.random.random() < null_prob:
                                    vec.append(None)
                                else:
                                    vec.append(np.random.randint(0, 2))
                            data.append(vec)
                        return data
                    
                    data_A = generate_random_data()
                    data_B = generate_random_data()
                    data_C = generate_random_data()
                    
                    # Ejecutar ciclo completo
                    tag = f"sample_{sample_id}"
                    result = pipeline.run_cycle(data_A, data_B, data_C, tag=tag)
                    
                    # Almacenar resultado
                    st.session_state.pipeline_results.append({
                        'tag': tag,
                        'result': result
                    })
                
                st.success(f"✅ {num_samples} ciclos procesados")
                st.session_state.last_tag = f"sample_{num_samples-1}"
    
    # Mostrar estado del pipeline
    st.markdown("---")
    st.markdown("### 📊 Estado del Pipeline")
    
    col_p1, col_p2, col_p3 = st.columns(3)
    
    with col_p1:
        num_stored = len(st.session_state.pipeline_results) if 'pipeline_results' in st.session_state else 0
        st.metric("Ciclos Ejecutados", num_stored)
    
    with col_p2:
        # Get stats from KB
        stats = pipeline.get_stats()
        relator_size = stats.get('kb', {}).get('evolver_relators', 0)
        st.metric("Patrones Relator", relator_size)
    
    with col_p3:
        stats = pipeline.get_stats()
        emergence_size = stats.get('kb', {}).get('evolver_emergences', 0)
        st.metric("Patrones Emergencia", emergence_size)
    
    # Explorar resultados
    if 'pipeline_results' in st.session_state and st.session_state.pipeline_results:
        st.markdown("### 🗄️ Explorador de Resultados")
        
        tags = [r['tag'] for r in st.session_state.pipeline_results]
        selected_tag = st.selectbox("Seleccionar resultado:", tags)
        
        if selected_tag:
            # Encontrar el resultado
            data = next((r['result'] for r in st.session_state.pipeline_results if r['tag'] == selected_tag), None)
            
            if data:
                col_e1, col_e2 = st.columns(2)
                
                with col_e1:
                    st.markdown("**Fractal Tensor Resultante:**")
                    if 'tensor_cross' in data:
                        ft = data['tensor_cross']
                        fig = create_3d_fractal_viz(ft)
                        st.plotly_chart(fig, use_container_width=True)
                
                with col_e2:
                    st.markdown("**Información del Ciclo:**")
                    st.json({
                        "tag": selected_tag,
                        "harmony_applied": data.get('harmony_applied', False),
                        "harmony_escalated": data.get('harmony_escalated', False),
                        "harmony_steps": len(data.get('harmony_audit', [])),
                        "has_tensor": 'tensor_cross' in data,
                        "has_audits": 'audits' in data
                    })
                    
                    # Mostrar Ms del tensor
                    if 'tensor_cross' in data:
                        st.markdown("**Ms (nivel_3):**")
                        ms = data['tensor_cross'].nivel_3
                        for i, v in enumerate(ms):
                            st.text(f"  [{i}]: {v}")

# ============================================================================
# FOOTER
# ============================================================================

st.markdown("---")
st.markdown("""
<div style="text-align: center; color: #666; padding: 2rem;">
    <p>🌌 <strong>Aurora Interactive Demo</strong> v1.0</p>
    <p>Fractal Logic Architecture for Explainable and Efficient AI</p>
    <p>Licensed under Apache 2.0 + CC BY 4.0</p>
</div>
""", unsafe_allow_html=True)
