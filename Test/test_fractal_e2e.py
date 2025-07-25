import time
from allcode3 import FractalTensor, TensorPoolManager, Evolver, Extender, FractalKnowledgeBase

def test_end_to_end_fractal_cycle():
    # 1. Inicialización de componentes
    t0 = time.time()
    kb = FractalKnowledgeBase()
    evolver = Evolver()
    extender = Extender(kb, evolver)
    pool = TensorPoolManager()
    t1 = time.time()

    # 2. Pool: Añadir tensores al pool
    t2 = time.time()
    t1_obj = FractalTensor.random()
    t2_obj = FractalTensor.random()
    t3_obj = FractalTensor.random()
    pool.add_tensor(t1_obj)
    pool.add_tensor(t2_obj)
    pool.add_tensor(t3_obj)
    t3 = time.time()

    # 3. Trío: Selección de trío optimizado
    trio = pool.get_tensor_trio()
    assert len(trio) == 3
    t4 = time.time()

    # 4. Síntesis fractal: Unificar el trío en un tensor representativo
    tensor_sintetizado = evolver.base_transcender.compute_full_fractal(*trio)
    assert isinstance(tensor_sintetizado, FractalTensor)
    t5 = time.time()

    # 5. Almacenamiento en KB
    nombre = f"arquetipo_e2e_{int(time.time())}"
    ok = kb.add_archetype('e2e_test', nombre, tensor_sintetizado)
    assert ok
    encontrado = kb.find_archetype_by_name('e2e_test', nombre)
    assert encontrado is not None
    t6 = time.time()

    # 6. Reconstrucción: Extender un tensor incompleto usando la KB
    tensor_incompleto = FractalTensor(nivel_3=tensor_sintetizado.nivel_3)
    resultado = extender.extend_fractal(tensor_incompleto, {'space_id': 'e2e_test'})
    reconstruido = resultado['reconstructed_tensor']
    assert reconstruido.nivel_9 == tensor_sintetizado.nivel_9
    assert reconstruido.nivel_27 == tensor_sintetizado.nivel_27
    t7 = time.time()

    # 7. Métricas: Validar coherencia y tiempo de cada etapa
    print("\n==== MÉTRICAS FLUJO E2E ====")
    print(f"Inicialización: {t1-t0:.6f}s")
    print(f"Pool (add): {t3-t2:.6f}s | Tamaño pool: {sum(len(pool.pools[k]) for k in pool.pools)}")
    print(f"Selección trío: {t4-t3:.6f}s | Trío: {[repr(t) for t in trio]}")
    print(f"Síntesis fractal: {t5-t4:.6f}s | Tensor sintetizado: {repr(tensor_sintetizado)}")
    print(f"Almacenamiento KB: {t6-t5:.6f}s | Nombre: {nombre}")
    print(f"Reconstrucción: {t7-t6:.6f}s | Reconstruido: {repr(reconstruido)}")
    print(f"✔️ Flujo E2E Aurora Trinity-3 completado. Tiempo total: {t7-t0:.6f}s")
    print(f"Reconstrucción coherente: {reconstruido is not None}")

if __name__ == "__main__":
    test_end_to_end_fractal_cycle()
