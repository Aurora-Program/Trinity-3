# ===============================================================================
# BENCHMARK FRACTAL AURORA - allcode3.py
# ===============================================================================
import time
from allcode3 import FractalTensor, Evolver, Extender, FractalKnowledgeBase

# Inicialización
kb = FractalKnowledgeBase()
evolver = Evolver()
extender = Extender(kb)

results = {}

# FASE 1: ARQUETIPO
start = time.time()
familia_movimiento = [
    FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,0,0]]*9, nivel_27=[[0,0,1]]*27),
    FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,1,0]]*9, nivel_27=[[0,1,0]]*27),
    FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[0,1,1]]*9, nivel_27=[[1,1,1]]*27)
]
arquetipo_movimiento = evolver.compute_fractal_archetype(familia_movimiento)
kb.add_archetype('fisica_conceptual', 'movimiento_universal', arquetipo_movimiento)
results['arquetipo'] = time.time() - start

# FASE 2: DINÁMICA
start = time.time()
estado_t0 = FractalTensor.random()
estado_t1 = evolver.base_transcender.compute_full_fractal(estado_t0, estado_t0, FractalTensor.neutral())
estado_t2 = evolver.base_transcender.compute_full_fractal(estado_t1, estado_t1, FractalTensor.neutral())
secuencia_temporal_logica = [estado_t0, estado_t1, estado_t2]
firma_dinamica = evolver.analyze_fractal_dynamics(secuencia_temporal_logica)
kb.add_archetype('dinamicas_sistemas', 'evolucion_sistema_X', firma_dinamica)
results['dinamica'] = time.time() - start

# FASE 3: RELATOR
start = time.time()
concepto_base = FractalTensor.random()
concepto_fuerza = evolver.base_transcender.compute_full_fractal(concepto_base, FractalTensor.random(), FractalTensor.neutral())
concepto_energia = evolver.base_transcender.compute_full_fractal(concepto_base, concepto_fuerza, FractalTensor.neutral())
cluster_contextual = [concepto_base, concepto_fuerza, concepto_energia]
firma_relacional = evolver.analyze_fractal_relations(cluster_contextual)
kb.add_archetype('mapas_conceptuales', 'mecanica_basica', firma_relacional)
results['relator'] = time.time() - start

# FASE 4: EXTENSIÓN
start = time.time()
tensor_incompleto = FractalTensor(nivel_3=arquetipo_movimiento.nivel_3)
resultado_extension = extender.extend_fractal(
    tensor_incompleto,
    contexto={'space_id': 'fisica_conceptual'}
)
tensor_reconstruido = resultado_extension['reconstructed_tensor']
results['extension'] = time.time() - start

# Mostrar resultados
print("\n=== BENCHMARK FRACTAL AURORA ===")
for fase, t in results.items():
    print(f"Fase {fase:10}: {t:.6f} segundos")
print("==============================\n")
