# Ejecutar la demostración fractal completa
import sys
import os   
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from allcode3new import *

if __name__ == "__main__":
    print("🌌 DEMOSTRACIÓN FRACTAL AURORA: Arquetipos, Dinámicas y Relatores 🌌")
    print("=" * 80)
    print("Análisis de conocimiento desde tres perspectivas con datos coherentes.")
    print("=" * 80)

    # === INICIALIZACIÓN DEL ECOSISTEMA AURORA ===
    kb = FractalKnowledgeBase()
    evolver = Evolver()
    extender = Extender(kb)
    pool_manager = TensorPoolManager()

    # === FASE 1: ANÁLISIS DE ARQUETIPOS ===
    print("\n🏛️ FASE 1: ANÁLISIS DE ARQUETIPOS")
    print("-" * 50)
    familia_movimiento = [
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,0,0]]*9, nivel_27=[[0,0,1]]*27),
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,1,0]]*9, nivel_27=[[0,1,0]]*27),
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[0,1,1]]*9, nivel_27=[[1,1,1]]*27)
    ]
    for t in familia_movimiento: pool_manager.add_tensor(t)
    
    trio_para_arquetipo = pool_manager.get_tensor_trio('arquetipo')
    arquetipo_movimiento = evolver.compute_fractal_archetype(trio_para_arquetipo)
    print(f"• Analizando {len(trio_para_arquetipo)} conceptos de 'movimiento'...")
    print(f"• ARQUETIPO resultante: {arquetipo_movimiento}")
    # Extraer Ss del tensor raíz del arquetipo (ejemplo: primer vector de nivel_3)
    Ss_movimiento = arquetipo_movimiento.nivel_3[0] if hasattr(arquetipo_movimiento, 'nivel_3') else [0,0,0]
    kb.add_archetype('fisica_conceptual', 'movimiento_universal', arquetipo_movimiento, Ss=Ss_movimiento)
    print("  └─ Arquetipo almacenado en el espacio 'fisica_conceptual'.")

    # === FASE 2: ANÁLISIS DE DINÁMICAS ===
    print("\n⚡ FASE 2: ANÁLISIS DE DINÁMICAS")
    print("-" * 50)
    
    estado_t0 = FractalTensor.random()
    estado_t1 = evolver.base_transcender.compute_full_fractal(estado_t0, estado_t0, FractalTensor.neutral())
    estado_t2 = evolver.base_transcender.compute_full_fractal(estado_t1, estado_t1, FractalTensor.neutral())
    secuencia_temporal_logica = [estado_t0, estado_t1, estado_t2]
    
    print(f"• Analizando secuencia temporal de {len(secuencia_temporal_logica)} estados.")
    firma_dinamica = evolver.analyze_fractal_dynamics(secuencia_temporal_logica)
    print(f"• DINÁMICA resultante: {firma_dinamica}")
    Ss_dinamica = firma_dinamica.nivel_3[0] if hasattr(firma_dinamica, 'nivel_3') else [0,0,0]
    kb.add_archetype('dinamicas_sistemas', 'evolucion_sistema_X', firma_dinamica, Ss=Ss_dinamica)
    print("  └─ Dinámica almacenada en 'dinamicas_sistemas'.")

    # === FASE 3: ANÁLISIS DE RELATORES ===
    print("\n🔗 FASE 3: ANÁLISIS DE RELATORES")
    print("-" * 50)
    
    concepto_base = FractalTensor.random()
    concepto_fuerza = evolver.base_transcender.compute_full_fractal(concepto_base, FractalTensor.random(), FractalTensor.neutral())
    concepto_energia = evolver.base_transcender.compute_full_fractal(concepto_base, concepto_fuerza, FractalTensor.neutral())
    cluster_contextual = [concepto_base, concepto_fuerza, concepto_energia]
    
    print(f"• Analizando clúster de {len(cluster_contextual)} conceptos relacionados.")
    firma_relacional = evolver.analyze_fractal_relations(cluster_contextual)
    print(f"• RELATOR resultante: {firma_relacional}")
    Ss_relator = firma_relacional.nivel_3[0] if hasattr(firma_relacional, 'nivel_3') else [0,0,0]
    kb.add_archetype('mapas_conceptuales', 'mecanica_basica', firma_relacional, Ss=Ss_relator)
    print("  └─ Relator almacenado en 'mapas_conceptuales'.")

    # === FASE 4: EXTENSIÓN BASADA EN CONOCIMIENTO ===
    print("\n🧩 FASE 4: EXTENSIÓN POR ARQUETIPO")
    print("-" * 50)
    
    # Construir el tensor incompleto solo con la raíz (primer vector de nivel_3)
    tensor_incompleto = FractalTensor(nivel_3=[arquetipo_movimiento.nivel_3[0], [None, None, None], [None, None, None]])
    print(f"• Tensor a extender (solo con raíz): {tensor_incompleto}")

    # Extensión robusta: la función copiará todos los niveles del arquetipo encontrado
    resultado_extension = extender.extend_fractal(
        tensor_incompleto,
        contexto={'space_id': 'fisica_conceptual'}
    )

    tensor_reconstruido = resultado_extension['reconstructed_tensor']
    print(f"• Método de reconstrucción: {resultado_extension['reconstruction_method']}")
    print(f"• Tensor reconstruido: {tensor_reconstruido}")
    print("  └─ Los niveles 3, 9 y 27 se han rellenado desde la KB.")

    print("\n" + "=" * 80)
    print("🎯 DEMOSTRACIÓN FINALIZADA.")
    print("=" * 80)