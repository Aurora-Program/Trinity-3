#!/usr/bin/env python3
"""
Demo Completo del Sistema Entrópico Aurora v2.1
Pipeline end-to-end: Embeddings → Trits → Aprendizaje → Inferencia
"""

import numpy as np
import os
import subprocess
import sys

def print_header(title):
    print("\n" + "═"*60)
    print(f"  {title}")
    print("═"*60 + "\n")

def print_section(title):
    print("\n" + "─"*60)
    print(f"  {title}")
    print("─"*60)

def demo_entropic_system():
    print_header("🌌 DEMO SISTEMA ENTRÓPICO AURORA v2.1")
    
    # Verificar imports
    print("📦 Verificando dependencias...")
    try:
        from ffe_generator import FFEGenerator, generate_synthetic_embeddings
        from sentence_transformers import SentenceTransformer
        print("   ✅ ffe_generator")
        print("   ✅ sentence_transformers")
    except ImportError as e:
        print(f"   ❌ Error: {e}")
        print("   Instalar: pip install sentence-transformers numpy scikit-learn")
        return False
    
    # PASO 1: Generar embeddings
    print_section("PASO 1: Generar Embeddings Sintéticos")
    
    print("Generando 100 embeddings 384D...")
    embeddings, labels = generate_synthetic_embeddings(100, 384)
    print(f"✅ Generados {len(embeddings)} embeddings")
    print(f"   Dimensión: {embeddings.shape}")
    print(f"   Ejemplos: {labels[:5]}")
    
    # PASO 2: Cuantización entrópica
    print_section("PASO 2: Cuantización Entrópica (FFE)")
    
    gen = FFEGenerator()
    print("Aplicando PCA 384D → 81D...")
    print("Cuantizando a trits entrópicos {1=false, 2=true, 3=null}...")
    
    trits = gen.encode(embeddings)
    print(f"\n✅ Cuantización completada")
    print(f"   Shape: {trits.shape} (100 tensores × 81 trits)")
    
    # Mostrar distribución
    count_1 = np.sum(trits == 1)
    count_2 = np.sum(trits == 2)
    count_3 = np.sum(trits == 3)
    total = trits.size
    
    print(f"\n📊 Distribución entrópica:")
    print(f"   1 (false): {count_1:5d} ({100*count_1/total:5.1f}%) ← Orden negativo")
    print(f"   2 (true):  {count_2:5d} ({100*count_2/total:5.1f}%) ← Orden positivo")
    print(f"   3 (null):  {count_3:5d} ({100*count_3/total:5.1f}%) ← Máxima entropía")
    
    # Guardar para C
    print("\n💾 Guardando tensores para C...")
    gen.save_for_c(trits, 'demo_tensors_entropic.txt', labels)
    print("   ✅ demo_tensors_entropic.txt")
    
    # PASO 3: Mostrar ejemplo de tensor
    print_section("PASO 3: Anatomía de un Tensor FFE")
    
    print(f"Tensor ejemplo ('{labels[0]}'):")
    print(f"   Primeras 9 dimensiones (de 27 totales):")
    for i in range(min(9, trits.shape[1]//3)):
        dim_start = i * 3
        dim = trits[0, dim_start:dim_start+3]
        
        # Interpretar roles FFE
        fo_val = dim[0]  # Forma
        fn_val = dim[1]  # Función
        es_val = dim[2]  # Estructura
        
        fo_str = {1: "false", 2: "true", 3: "null"}[fo_val]
        fn_str = {1: "AND", 2: "OR", 3: "CONSENSUS"}[fn_val]
        es_str = {1: "orden1", 2: "orden2", 3: "orden3"}[es_val]
        
        print(f"   Dim {i:02d}: [{fo_val},{fn_val},{es_val}] = FO:{fo_str:5s} FN:{fn_str:9s} ES:{es_str}")
    
    # PASO 4: Compilar C
    print_section("PASO 4: Compilar Aurora (C)")
    
    if not os.path.exists('aurora_awaken.c'):
        print("⚠️  aurora_awaken.c no encontrado en directorio actual")
        print("   Asegúrate de estar en newVersion/")
        return False
    
    print("Compilando aurora_awaken.c...")
    result = subprocess.run(
        ['gcc', '-O3', '-o', 'aurora_awaken_demo.exe', 'aurora_awaken.c'],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        print(f"❌ Error de compilación:")
        print(result.stderr)
        return False
    
    print("✅ aurora_awaken_demo.exe")
    
    print("\nCompilando aurora_inference.c...")
    result = subprocess.run(
        ['gcc', '-O3', '-o', 'aurora_inference_demo.exe', 'aurora_inference.c'],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        print(f"❌ Error de compilación:")
        print(result.stderr)
        return False
    
    print("✅ aurora_inference_demo.exe")
    
    # PASO 5: Entrenar
    print_section("PASO 5: Entrenamiento (Learning)")
    
    print("Ejecutando: aurora_awaken_demo.exe demo_tensors_entropic.txt demo_knowledge.dat\n")
    result = subprocess.run(
        ['./aurora_awaken_demo.exe', 'demo_tensors_entropic.txt', 'demo_knowledge.dat'],
        capture_output=False,  # Mostrar output directo
        text=True
    )
    
    if result.returncode != 0:
        print("❌ Error en entrenamiento")
        return False
    
    # PASO 6: Inferencia
    print_section("PASO 6: Inferencia Generativa (sin transformer)")
    
    print("Ejecutando: aurora_inference_demo.exe demo_knowledge.dat\n")
    result = subprocess.run(
        ['./aurora_inference_demo.exe', 'demo_knowledge.dat'],
        capture_output=False,
        text=True
    )
    
    if result.returncode != 0:
        print("❌ Error en inferencia")
        return False
    
    # PASO 7: Resumen
    print_section("RESUMEN DEL DEMO")
    
    print("""
✅ Pipeline completo ejecutado exitosamente:

   1. Embeddings 384D generados (sentence-transformers)
   2. Reducción PCA → 81D
   3. Cuantización entrópica → trits {1,2,3}
   4. Entrenamiento → Arquetipos, Dinámicas, Relatores
   5. Inferencia → Nuevos tensores sin transformer

🌟 SISTEMA ENTRÓPICO OPERATIVO

   Principio: 1=false (orden-), 2=true (orden+), 3=null (entropía máxima)
   
   Base teórica:
   • Shannon: H(null) > H(false) = H(true)
   • Boltzmann: S(null) > S(false) = S(true)
   • von Neumann: S(superposición) > S(estado puro)
   
🎯 Próximos pasos:
   • Entrenar con corpus real (español)
   • Medir similitudes coseno
   • Benchmark vs transformers
   • Implementar tetraedro trimodal completo
    """)
    
    return True

def main():
    try:
        success = demo_entropic_system()
        
        if success:
            print("\n" + "═"*60)
            print("  🎉 DEMO COMPLETADO EXITOSAMENTE")
            print("═"*60)
            print("\n🌌 'El orden emerge del caos, la inteligencia de la entropía'\n")
            return 0
        else:
            print("\n⚠️  Demo incompleto - revisar errores arriba")
            return 1
            
    except KeyboardInterrupt:
        print("\n\n⚠️  Demo interrumpido por usuario")
        return 1
    except Exception as e:
        print(f"\n❌ Error inesperado: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
