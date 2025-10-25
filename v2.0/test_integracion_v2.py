"""
TEST DE INTEGRACIÓN RÁPIDA - AURORA TRINITY 3 v2.0
====================================================

Suite simplificada que valida la API real del código v2.0:
- Usa la API real de cada módulo
- Tests end-to-end funcionales
- Enfocado en flujo operacional

Ejecutar: python test_integracion_v2.py
"""

import sys
from typing import List

# Imports del sistema Aurora v2.0
try:
    from Trigate import Trigate
    from Transceder import Transcender
    from FractalTensor import FractalTensor
    from Evolver import Evolver3
    from Extender import Extender
    from Harmonizer import Harmonizer
    from aurora_pipeline import AuroraPipeline
    print("✅ Todos los módulos v2.0 importados correctamente\n")
except ImportError as e:
    print(f"❌ Error de importación: {e}")
    sys.exit(1)


def test_1_trigate():
    """Test 1: Trigate - Lógica ternaria básica"""
    print("🧪 Test 1: Trigate - Lógica Ternaria")
    tg = Trigate()
    
    # Inferencia
    A = [0, 1, 0]
    B = [1, 0, 1]
    M = [1, 1, 1]
    R = tg.infer(A, B, M)
    assert R == [1, 1, 1], f"Esperado [1,1,1], obtenido {R}"
    print("  ✅ Trigate.infer funciona")
    
    # Aprendizaje
    M_learned = tg.learn([0, 1, 1], [1, 0, 1], [1, 1, 0])
    assert len(M_learned) == 3, "M debe tener 3 elementos"
    print("  ✅ Trigate.learn funciona")
    
    # NULL propagation
    R_null = tg.infer([0, None, 1], [1, 1, None], [1, 1, 1])
    assert None in R_null, "NULL no se propagó"
    print("  ✅ Trigate maneja NULL correctamente")
    

def test_2_transcender():
    """Test 2: Transcender - Síntesis con .solve()"""
    print("\n🧪 Test 2: Transcender - Síntesis Jerárquica")
    tg = Trigate()
    tc = Transcender(trigate_cls=tg)
    
    # Usar el método real: solve()
    A = [0, 1, 0]
    B = [1, 0, 1]
    C = [0, 1, 1]
    
    resultado = tc.solve(A, B, C)
    
    assert 'Ms' in resultado, "Debe devolver Ms"
    assert 'Ss' in resultado, "Debe devolver Ss"
    assert 'MetaM' in resultado, "Debe devolver MetaM"
    assert len(resultado['Ms']) == 3, "Ms debe ser lista de 3"
    
    print(f"  ✅ Transcender.solve() OK - Ms={resultado['Ms']}, Ss={resultado['Ss']}")


def test_3_fractaltensor():
    """Test 3: FractalTensor - Representación fractal"""
    print("\n🧪 Test 3: FractalTensor - Representación Fractal")
    
    # Crear con datos iniciales
    nivel_3 = [0, 1, 0]
    nivel_9 = [[0, 1, 0], [1, 0, 1], [0, 1, 1]]
    nivel_27 = [
        [[0, 1, 0], [1, 0, 1], [0, 1, 1]],
        [[1, 0, 0], [0, 1, 1], [1, 1, 0]],
        [[0, 0, 1], [1, 1, 1], [0, 0, 0]]
    ]
    
    ft = FractalTensor(nivel_3, nivel_9, nivel_27)
    
    assert ft.nivel_3 == nivel_3, "nivel_3 no coincide"
    assert ft.nivel_9 == nivel_9, "nivel_9 no coincide"
    assert ft.nivel_27 == nivel_27, "nivel_27 no coincide"
    
    print("  ✅ FractalTensor almacena niveles correctamente")


def test_4_evolver():
    """Test 4: Evolver3 - Sistema de aprendizaje"""
    print("\n🧪 Test 4: Evolver3 - Aprendizaje Tripartito")
    tg = Trigate()
    ev = Evolver3(trigate_cls=tg)
    
    # Verificar que los bancos existen
    assert hasattr(ev, 'store'), "Debe tener método store"
    assert hasattr(ev, 'retrieve'), "Debe tener método retrieve"
    
    # Registrar un patrón
    Ms_test = [1, 0, 1]
    MetaM_test = {
        'M1': [1, 1, 1],
        'M2': [0, 0, 0],
        'M3': [1, 0, 1],
        'Ms': [1, 0, 1]
    }
    Ss_test = [0, 1, 0]
    
    ev.store(Ms_test, MetaM_test, Ss_test)
    
    # Intentar recuperar
    recuperado = ev.retrieve(Ms_test)
    assert recuperado is not None, "Debe recuperar el patrón almacenado"
    
    print("  ✅ Evolver3 almacena y recupera patrones")


def test_5_extender():
    """Test 5: Extender - Reconstrucción top-down"""
    print("\n🧪 Test 5: Extender - Reconstrucción")
    tg = Trigate()
    ev = Evolver3(trigate_cls=tg)
    ext = Extender(kb=ev, evolver=ev)
    
    # Almacenar conocimiento
    Ms = [1, 0, 1]
    MetaM = {'M1': [1, 1, 1], 'M2': [0, 0, 0], 'M3': [1, 0, 1], 'Ms': [1, 0, 1]}
    Ss = [0, 1, 0]
    ev.store(Ms, MetaM, Ss)
    
    # Verificar que Extender puede acceder a la KB
    assert ext.kb is not None, "Extender debe tener referencia a KB"
    
    print("  ✅ Extender inicializado con KB")


def test_6_harmonizer():
    """Test 6: Harmonizer - Reparación configurableconfiguración"""
    print("\n🧪 Test 6: Harmonizer - Reparación")
    tg = Trigate()
    ev = Evolver3(trigate_cls=tg)
    
    harm = Harmonizer(
        trigate_cls=tg,
        evolver=ev,
        extender_cls=Extender,
        max_conflicts=10,
        max_null_fills=5,
        min_child_sim=0.6
    )
    
    assert harm.max_conflicts == 10, "Parámetro max_conflicts no se aplicó"
    assert harm.max_null_fills == 5, "Parámetro max_null_fills no se aplicó"
    assert harm.min_child_sim == 0.6, "Parámetro min_child_sim no se aplicó"
    
    print("  ✅ Harmonizer configurado con parámetros personalizados")


def test_7_pipeline_completo():
    """Test 7: Pipeline completo - End-to-end"""
    print("\n🧪 Test 7: Pipeline Completo - End-to-End")
    
    pipeline = AuroraPipeline()
    
    assert hasattr(pipeline, 'kb'), "Pipeline debe tener KB"
    assert hasattr(pipeline, 'evolver'), "Pipeline debe tener Evolver"
    assert hasattr(pipeline, 'fractal_evolver'), "Pipeline debe tener FractalEvolver"
    
    print("  ✅ Pipeline inicializado con todos los componentes")
    
    # Test de flujo básico si existe método ingest
    if hasattr(pipeline, 'ingest_fractal_tensor'):
        print("  ✅ Pipeline tiene método ingest_fractal_tensor")
    
    if hasattr(pipeline, 'complete_fractal_enhanced'):
        print("  ✅ Pipeline tiene método complete_fractal_enhanced")


def test_8_flujo_completo():
    """Test 8: Flujo operacional completo"""
    print("\n🧪 Test 8: Flujo Operacional Completo")
    
    # 1. Crear componentes
    tg = Trigate()
    tc = Transcender(trigate_cls=tg)
    ev = Evolver3(trigate_cls=tg)
    
    # 2. Sintetizar conocimiento
    A = [0, 1, 0]
    B = [1, 0, 1]
    C = [0, 1, 1]
    
    resultado = tc.solve(A, B, C)
    Ms = resultado['Ms']
    Ss = resultado['Ss']
    MetaM = resultado['MetaM']
    
    print(f"  → Síntesis: Ms={Ms}, Ss={Ss}")
    
    # 3. Almacenar en Evolver
    ev.store(Ms, MetaM, Ss)
    print("  → Almacenado en KB")
    
    # 4. Recuperar
    recuperado = ev.retrieve(Ms)
    assert recuperado is not None, "Debe recuperar el patrón"
    print("  → Recuperado de KB")
    
    # 5. Verificar coherencia
    assert recuperado['Ms'] == Ms, "Ms debe coincidir"
    assert recuperado['Ss'] == Ss, "Ss debe coincidir"
    
    print("  ✅ Flujo completo: Síntesis → Almacenamiento → Recuperación")


def main():
    """Ejecutar todos los tests"""
    print("="*70)
    print("🚀 TESTS DE INTEGRACIÓN - AURORA TRINITY 3 v2.0")
    print("="*70)
    
    tests = [
        test_1_trigate,
        test_2_transcender,
        test_3_fractaltensor,
        test_4_evolver,
        test_5_extender,
        test_6_harmonizer,
        test_7_pipeline_completo,
        test_8_flujo_completo
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"  ❌ FALLÓ: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "="*70)
    print("📊 RESUMEN")
    print("="*70)
    print(f"✅ Tests Pasados: {passed}/{len(tests)}")
    print(f"❌ Tests Fallidos: {failed}/{len(tests)}")
    
    porcentaje = (passed / len(tests) * 100)
    print(f"📈 Éxito: {porcentaje:.1f}%")
    
    if passed == len(tests):
        print("\n🎉 ¡PERFECTO! Todos los tests pasaron")
        return 0
    else:
        print(f"\n⚠️  {failed} test(s) fallaron")
        return 1


if __name__ == '__main__':
    sys.exit(main())