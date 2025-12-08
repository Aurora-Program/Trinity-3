"""
test_semantic_preservation_REAL.py

Test DEFINITIVO con embeddings REALES de sentence-transformers
- Usa MiniLM-L6-v2 (384 dims)
- Frases del whitepaper Aurora
- Valida preservación semántica con FFE encoding
"""

import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from typing import List, Tuple

try:
    from sentence_transformers import SentenceTransformer
    REAL_EMBEDDINGS_AVAILABLE = True
except ImportError:
    REAL_EMBEDDINGS_AVAILABLE = False
    print("⚠️  sentence-transformers no disponible, abortando")
    exit(1)

# ═══════════════════════════════════════════════════════════════════════
# FFE ENCODER: De embeddings continuos → tensores ternarios
# ═══════════════════════════════════════════════════════════════════════

class FFEEncoder:
    """
    Encoder de embeddings continuos a tensores FFE ternarios
    
    Pipeline:
    1. Normalización (StandardScaler)
    2. Reducción dimensional (PCA)
    3. Cuantización adaptativa a trits {1, 0, -1}
    """
    
    def __init__(self, embedding_dim=384, n_dims_pca=81):
        self.embedding_dim = embedding_dim
        self.n_dims_pca = n_dims_pca
        self.scaler = StandardScaler()
        self.pca = PCA(n_components=n_dims_pca)
        self.mean = None
        self.std = None
        self.thresh_low = None
        self.thresh_high = None
    
    def fit(self, embeddings: np.ndarray) -> float:
        """Entrena el encoder con un conjunto de embeddings"""
        # Normalizar
        scaled = self.scaler.fit_transform(embeddings)
        
        # PCA
        reduced = self.pca.fit_transform(scaled)
        variance_preserved = self.pca.explained_variance_ratio_.sum()
        
        # Calcular thresholds adaptativos (±0.5σ)
        self.mean = reduced.mean()
        self.std = reduced.std()
        self.thresh_low = self.mean - 0.5 * self.std
        self.thresh_high = self.mean + 0.5 * self.std
        
        return variance_preserved
    
    def encode(self, embedding: np.ndarray) -> List[int]:
        """Codifica un embedding a tensor FFE ternario"""
        # Normalizar y reducir
        scaled = self.scaler.transform(embedding.reshape(1, -1))
        reduced = self.pca.transform(scaled).flatten()
        
        # Cuantizar a trits
        tensor = []
        for val in reduced:
            if val < self.thresh_low:
                tensor.append(-1)  # null
            elif val > self.thresh_high:
                tensor.append(1)   # high/true
            else:
                tensor.append(0)   # low/false
        
        return tensor

# ═══════════════════════════════════════════════════════════════════════
# MÉTRICAS DE SIMILITUD
# ═══════════════════════════════════════════════════════════════════════

def cosine_similarity(a: np.ndarray, b: np.ndarray) -> float:
    """Similitud coseno para vectores continuos"""
    return np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b))

def triadic_distance(a: List[int], b: List[int]) -> float:
    """
    Distancia triádica: IGNORA NULLS, solo cuenta 1/0
    
    Esta es LA métrica correcta para Aurora porque:
    - Nulls = "desconocido", no "diferente"
    - Solo importan las posiciones donde ambos tienen opinión (1 o 0)
    """
    valid_pairs = [(x, y) for x, y in zip(a, b) if x != -1 and y != -1]
    if not valid_pairs:
        return 0.0
    
    matches = sum(1 for x, y in valid_pairs if x == y)
    return matches / len(valid_pairs)

# ═══════════════════════════════════════════════════════════════════════
# GENERACIÓN DE EMBEDDINGS REALES
# ═══════════════════════════════════════════════════════════════════════

def generate_real_embeddings():
    """Genera embeddings REALES usando MiniLM-L6-v2"""
    print("✨ Cargando modelo MiniLM-L6-v2...")
    model = SentenceTransformer('all-MiniLM-L6-v2')
    
    # Frases exactas del whitepaper Aurora
    base_frases = [
        "El gato duerme en el sofá",
        "Un felino descansa sobre el sillón",
        "El perro corre en el parque",
        "Un can trota por el jardín"
    ]
    
    print(f"📝 Frases base (del whitepaper):")
    for i, frase in enumerate(base_frases):
        print(f"  [{i}] {frase}")
    
    # Generar variaciones para tener 100 samples (necesario para PCA 81 dims)
    print("\n🔄 Generando variaciones semánticas (100 total para PCA)...")
    frases_expandidas = []
    
    for frase in base_frases:
        frases_expandidas.append(frase)
        
        # Variaciones de artículos/adjetivos
        frases_expandidas.append(frase.replace("El ", "El pequeño "))
        frases_expandidas.append(frase.replace("Un ", "Un gran "))
        frases_expandidas.append(frase.replace("El ", "Este "))
        
        # Variaciones de preposiciones
        if "en el" in frase:
            frases_expandidas.append(frase.replace("en el", "sobre el"))
            frases_expandidas.append(frase.replace("en el", "en un"))
        
        # Variaciones léxicas
        if "gato" in frase:
            frases_expandidas.append(frase.replace("gato", "minino"))
            frases_expandidas.append(frase.replace("gato", "gatito"))
        if "perro" in frase:
            frases_expandidas.append(frase.replace("perro", "cachorro"))
            frases_expandidas.append(frase.replace("perro", "perrito"))
        if "duerme" in frase:
            frases_expandidas.append(frase.replace("duerme", "dormita"))
            frases_expandidas.append(frase.replace("duerme", "reposa"))
        if "corre" in frase:
            frases_expandidas.append(frase.replace("corre", "galopa"))
            frases_expandidas.append(frase.replace("corre", "avanza"))
    
    # Completar hasta 100 con repeticiones
    while len(frases_expandidas) < 100:
        frases_expandidas.extend(base_frases)
    
    frases_expandidas = frases_expandidas[:100]
    
    print(f"✅ Total de frases: {len(frases_expandidas)}")
    print("🧠 Generando embeddings con MiniLM-L6-v2...")
    
    embeddings = model.encode(frases_expandidas, show_progress_bar=False)
    
    print(f"✅ Embeddings generados: shape={embeddings.shape}")
    
    # Retornar embeddings completos + los 4 de test
    test_embeddings = model.encode(base_frases, show_progress_bar=False)
    
    return embeddings, test_embeddings

# ═══════════════════════════════════════════════════════════════════════
# TEST PRINCIPAL
# ═══════════════════════════════════════════════════════════════════════

def test_semantic_preservation_real():
    print("\n" + "="*70)
    print(" TEST DE PRESERVACIÓN SEMÁNTICA - EMBEDDINGS REALES")
    print(" Modelo: MiniLM-L6-v2 (sentence-transformers)")
    print("="*70)
    
    # ━━━ PASO 1: Generar embeddings REALES ━━━
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("PASO 1: Generar embeddings REALES")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    all_embeddings, test_embeddings = generate_real_embeddings()
    
    # ━━━ PASO 2: Calcular similitudes baseline (espacio continuo) ━━━
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("PASO 2: Similitudes baseline (Coseno)")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    # Similares: [0↔1] y [2↔3]
    # Diferentes: [0↔2], [0↔3], [1↔2], [1↔3]
    
    sim_01 = cosine_similarity(test_embeddings[0], test_embeddings[1])
    sim_23 = cosine_similarity(test_embeddings[2], test_embeddings[3])
    
    sim_02 = cosine_similarity(test_embeddings[0], test_embeddings[2])
    sim_03 = cosine_similarity(test_embeddings[0], test_embeddings[3])
    sim_12 = cosine_similarity(test_embeddings[1], test_embeddings[2])
    sim_13 = cosine_similarity(test_embeddings[1], test_embeddings[3])
    
    avg_similar_coseno = (sim_01 + sim_23) / 2
    avg_different_coseno = (sim_02 + sim_03 + sim_12 + sim_13) / 4
    separation_coseno = avg_similar_coseno - avg_different_coseno
    
    print(f"  [0↔1] Similar (gato/felino):     {sim_01:.4f}")
    print(f"  [2↔3] Similar (perro/can):       {sim_23:.4f}")
    print(f"  [0↔2] Diferente (gato/perro):    {sim_02:.4f}")
    print(f"  [1↔3] Diferente (felino/can):    {sim_13:.4f}")
    print(f"\n  📊 Promedio similares:    {avg_similar_coseno:.4f}")
    print(f"  📊 Promedio diferentes:   {avg_different_coseno:.4f}")
    print(f"  📊 Separación:            {separation_coseno:.4f} ✅")
    
    # ━━━ PASO 3: Entrenar FFE encoder ━━━
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("PASO 3: Entrenar FFE Encoder")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    encoder = FFEEncoder(embedding_dim=384, n_dims_pca=81)
    variance = encoder.fit(all_embeddings)
    
    print(f"  ✅ PCA: 384 → 81 dims")
    print(f"  ✅ Varianza preservada: {variance*100:.1f}%")
    print(f"  ✅ Thresholds adaptativos:")
    print(f"     Mean: {encoder.mean:.3f}")
    print(f"     Std:  {encoder.std:.3f}")
    print(f"     Low:  {encoder.thresh_low:.3f}")
    print(f"     High: {encoder.thresh_high:.3f}")
    
    # ━━━ PASO 4: Codificar a tensores FFE ━━━
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("PASO 4: Codificar a tensores FFE")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    tensors = [encoder.encode(emb) for emb in test_embeddings]
    
    for i, tensor in enumerate(tensors):
        ones = tensor.count(1)
        zeros = tensor.count(0)
        nulls = tensor.count(-1)
        null_pct = (nulls / len(tensor)) * 100
        
        print(f"  Tensor [{i}]: 1={ones:2d}, 0={zeros:2d}, N={nulls:2d} ({null_pct:.0f}% nulls)")
    
    # ━━━ PASO 5: Similitudes FFE (triádico) ━━━
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("PASO 5: Similitudes FFE (Distancia Triádica)")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    sim_01_ffe = triadic_distance(tensors[0], tensors[1])
    sim_23_ffe = triadic_distance(tensors[2], tensors[3])
    
    sim_02_ffe = triadic_distance(tensors[0], tensors[2])
    sim_03_ffe = triadic_distance(tensors[0], tensors[3])
    sim_12_ffe = triadic_distance(tensors[1], tensors[2])
    sim_13_ffe = triadic_distance(tensors[1], tensors[3])
    
    avg_similar_ffe = (sim_01_ffe + sim_23_ffe) / 2
    avg_different_ffe = (sim_02_ffe + sim_03_ffe + sim_12_ffe + sim_13_ffe) / 4
    separation_ffe = avg_similar_ffe - avg_different_ffe
    
    print(f"  [0↔1] Similar (gato/felino):     {sim_01_ffe:.4f}")
    print(f"  [2↔3] Similar (perro/can):       {sim_23_ffe:.4f}")
    print(f"  [0↔2] Diferente (gato/perro):    {sim_02_ffe:.4f}")
    print(f"  [1↔3] Diferente (felino/can):    {sim_13_ffe:.4f}")
    print(f"\n  📊 Promedio similares:    {avg_similar_ffe:.4f}")
    print(f"  📊 Promedio diferentes:   {avg_different_ffe:.4f}")
    print(f"  📊 Separación:            {separation_ffe:.4f}")
    
    # ━━━ PASO 6: Verificación ━━━
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("PASO 6: Verificación de criterios")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    test1_pass = avg_similar_ffe > avg_different_ffe
    test2_pass = separation_ffe > 0.05
    
    avg_nulls = sum(t.count(-1) for t in tensors) / len(tensors)
    avg_nulls_pct = (avg_nulls / 81) * 100
    test3_pass = 10 <= avg_nulls_pct <= 70
    
    print(f"\n✅ Test 1 (Orden):      {avg_similar_ffe:.4f} > {avg_different_ffe:.4f} → {'✅ PASS' if test1_pass else '❌ FAIL'}")
    print(f"✅ Test 2 (Separación): {separation_ffe:.4f} > 0.05 → {'✅ PASS' if test2_pass else '❌ FAIL'}")
    print(f"✅ Test 3 (Nulls):      {avg_nulls_pct:.1f}% promedio (rango esperado: 10-70%) → {'✅ PASS' if test3_pass else '❌ FAIL'}")
    
    # ━━━ VEREDICTO FINAL ━━━
    print("\n" + "="*70)
    print("VEREDICTO FINAL - Embeddings REALES (MiniLM-L6-v2)")
    print("="*70)
    
    if test1_pass and test2_pass and test3_pass:
        print("\n🎉 PRESERVACIÓN SEMÁNTICA EXITOSA")
        print("\n✨ Aurora puede operar sobre significado REAL, no solo números")
        print("✨ El FFE encoding mantiene las relaciones semánticas")
        print("✨ Los tensores ternarios preservan el conocimiento continuo")
        print("\n🚀 Aurora está lista para integración con LLMs reales")
    else:
        print("\n⚠️  PRESERVACIÓN PARCIAL O FALLIDA")
        print("Revisar thresholds o aumentar dimensiones PCA")
    
    print("\n" + "="*70)
    
    return test1_pass and test2_pass and test3_pass

# ═══════════════════════════════════════════════════════════════════════
# MAIN
# ═══════════════════════════════════════════════════════════════════════

if __name__ == "__main__":
    success = test_semantic_preservation_real()
    exit(0 if success else 1)
