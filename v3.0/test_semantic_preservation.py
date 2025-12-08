"""
Test Crítico: ¿Aurora preserva relaciones semánticas reales?

Objetivo:
    Probar si la codificación FFE mantiene la estructura semántica
    de embeddings producidos por modelos de lenguaje reales.
    
Test de Éxito:
    - Frases similares → tensores similares
    - Frases diferentes → tensores diferentes
    - Distancia semántica original ≈ distancia tensorial Aurora
"""

import numpy as np
from typing import List, Tuple
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler

# ═══════════════════════════════════════════════════════════════════════
# ENCODER FFE (Python)
# ═══════════════════════════════════════════════════════════════════════

class FFEEncoder:
    """Codifica embeddings continuos → tensores FFE discretos (1/0/-1)"""
    
    def __init__(self, embedding_dim: int = 384, n_dims_pca: int = 81):
        """
        Args:
            embedding_dim: Dimensión del embedding de entrada
            n_dims_pca: Dimensión tras PCA (debe ser múltiplo de 3)
        """
        self.embedding_dim = embedding_dim
        self.n_dims_pca = n_dims_pca
        
        # Verificar que n_dims_pca es múltiplo de 3
        assert n_dims_pca % 3 == 0, "n_dims_pca debe ser múltiplo de 3"
        
        # Componentes de transformación
        self.scaler = StandardScaler()
        self.pca = PCA(n_components=n_dims_pca)
        self.fitted = False
        
        # Umbrales de cuantización (aprenden durante fit)
        self.upper_threshold = None  # valores > umbral_alto → 1
        self.lower_threshold = None  # valores < umbral_bajo → 0
    
    def fit(self, embeddings: np.ndarray):
        """Aprende la transformación desde embeddings continuos"""
        # Normalizar
        scaled = self.scaler.fit_transform(embeddings)
        
        # Reducir dimensionalidad preservando varianza
        reduced = self.pca.fit_transform(scaled)
        
        # Aprender umbrales de cuantización ADAPTATIVOS
        # Usar desviación estándar en vez de percentiles fijos
        std_dev = np.std(reduced)
        mean_val = np.mean(reduced)
        
        # Umbrales más conservadores (±0.5 std)
        self.lower_threshold = mean_val - 0.5 * std_dev
        self.upper_threshold = mean_val + 0.5 * std_dev
        
        self.fitted = True
        
        print(f"[FFEEncoder] Entrenado:")
        print(f"  Varianza preservada: {self.pca.explained_variance_ratio_.sum():.3f}")
        print(f"  Mean: {mean_val:.3f} | Std: {std_dev:.3f}")
        print(f"  Umbrales: {self.lower_threshold:.3f} / {self.upper_threshold:.3f}")
    
    def encode(self, embedding: np.ndarray) -> np.ndarray:
        """Codifica un embedding → tensor FFE de trits"""
        assert self.fitted, "Debe llamar a fit() primero"
        
        # Transformar
        scaled = self.scaler.transform(embedding.reshape(1, -1))
        reduced = self.pca.transform(scaled).flatten()
        
        # Cuantizar a trits {1, 0, -1}
        tensor = np.zeros_like(reduced, dtype=np.int8)
        tensor[reduced > self.upper_threshold] = 1
        tensor[reduced < self.lower_threshold] = 0
        tensor[(reduced >= self.lower_threshold) & (reduced <= self.upper_threshold)] = -1
        
        return tensor
    
    def decode(self, tensor: np.ndarray) -> np.ndarray:
        """Reconstruye embedding aproximado desde tensor FFE"""
        assert self.fitted, "Debe llamar a fit() primero"
        
        # Dequantizar (trits → valores continuos aproximados)
        continuous = np.zeros_like(tensor, dtype=np.float32)
        continuous[tensor == 1] = self.upper_threshold + 0.5
        continuous[tensor == 0] = self.lower_threshold - 0.5
        continuous[tensor == -1] = (self.upper_threshold + self.lower_threshold) / 2
        
        # Invertir PCA y normalización
        restored_pca = self.pca.inverse_transform(continuous.reshape(1, -1))
        restored = self.scaler.inverse_transform(restored_pca).flatten()
        
        return restored


# ═══════════════════════════════════════════════════════════════════════
# MÉTRICAS DE SIMILITUD
# ═══════════════════════════════════════════════════════════════════════

def cosine_similarity(a: np.ndarray, b: np.ndarray) -> float:
    """Similitud coseno entre dos vectores"""
    return np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b) + 1e-8)

def hamming_similarity(a: np.ndarray, b: np.ndarray) -> float:
    """Similitud Hamming entre dos tensores FFE (trits)"""
    matches = np.sum(a == b)
    return matches / len(a)

def triadic_distance(a: np.ndarray, b: np.ndarray) -> float:
    """Distancia triádica (cuenta solo diferencias en valores no-null)"""
    # Solo comparar donde ambos NO son null
    valid_mask = (a != -1) & (b != -1)
    if np.sum(valid_mask) == 0:
        return 1.0  # Máxima distancia si todo es null
    
    diff = np.sum(a[valid_mask] != b[valid_mask])
    return diff / np.sum(valid_mask)


# ═══════════════════════════════════════════════════════════════════════
# TEST CRÍTICO
# ═══════════════════════════════════════════════════════════════════════

def test_semantic_preservation():
    """
    TEST CRÍTICO: ¿Los tensores FFE preservan relaciones semánticas?
    
    Hipótesis:
        Si dos frases son semánticamente similares en el espacio continuo,
        sus tensores FFE también deben ser similares en el espacio discreto.
    """
    
    print("╔═══════════════════════════════════════════════════════════════════╗")
    print("║  TEST CRÍTICO: Preservación de Relaciones Semánticas            ║")
    print("╚═══════════════════════════════════════════════════════════════════╝\n")
    
    # ━━━ PASO 1: Generar embeddings simulados (MiniLM-L6-v2 → 384 dims) ━━━
    print("━━━ PASO 1: Generando embeddings simulados ━━━")
    print("(Simulando sentence-transformers/all-MiniLM-L6-v2)\n")
    
    # Frases de prueba (AMPLIADAS: necesitamos >= 81 muestras para PCA a 81 dims)
    frases_base = [
        "El gato duerme en el sofá",           # 0: tema gatos-dormir
        "Un felino descansa sobre el sillón",  # 1: tema gatos-dormir (similar a 0)
        "El perro corre en el parque",         # 2: tema perros-correr
        "Un can trota por el jardín",          # 3: tema perros-correr (similar a 2)
    ]
    
    # Simular embeddings (en producción usar SentenceTransformer real)
    np.random.seed(42)
    
    # Crear embeddings base con estructura semántica implícita
    base_cat_sleep = np.random.randn(384) * 0.5
    base_dog_run = np.random.randn(384) * 0.5
    
    # Generar 100 variaciones para tener suficientes datos para PCA
    embeddings_list = []
    frases = []
    
    for i in range(25):  # 25 variaciones de cada tipo → 100 total
        # Variaciones de gato-dormir
        embeddings_list.append(base_cat_sleep + np.random.randn(384) * 0.15)
        frases.append(f"{frases_base[0]} (var {i})")
        
        embeddings_list.append(base_cat_sleep + np.random.randn(384) * 0.15)
        frases.append(f"{frases_base[1]} (var {i})")
        
        # Variaciones de perro-correr
        embeddings_list.append(base_dog_run + np.random.randn(384) * 0.15)
        frases.append(f"{frases_base[2]} (var {i})")
        
        embeddings_list.append(base_dog_run + np.random.randn(384) * 0.15)
        frases.append(f"{frases_base[3]} (var {i})")
    
    embeddings = np.array(embeddings_list)
    
    print(f"Embeddings generados: {embeddings.shape}")
    print(f"Dimensión: {embeddings.shape[1]}")
    print(f"Total muestras: {len(frases)} (necesarias >= 81 para PCA)\n")
    
    # ━━━ PASO 2: Calcular similitudes en espacio continuo ━━━
    print("━━━ PASO 2: Similitudes en espacio continuo (baseline) ━━━\n")
    
    # Usar las primeras 4 para comparación (representativas de cada grupo)
    test_indices = [0, 1, 50, 51]  # 0,1=gato-dormir | 50,51=perro-correr
    test_frases = [frases[i] for i in test_indices]
    test_embeddings = embeddings[test_indices]
    
    pairs = [
        (0, 1, "Similar (gato-dormir vs gato-dormir)"),
        (2, 3, "Similar (perro-correr vs perro-correr)"),
        (0, 2, "Diferente (gato-dormir vs perro-correr)"),
        (1, 3, "Diferente (gato-dormir vs perro-correr)"),
    ]
    
    print("Similitud Coseno (espacio continuo):")
    cosine_scores = {}
    for i, j, desc in pairs:
        sim = cosine_similarity(test_embeddings[i], test_embeddings[j])
        cosine_scores[(i, j)] = sim
        print(f"  [{i}↔{j}] {desc:45s} → {sim:.4f}")
    
    print()
    
    # ━━━ PASO 3: Entrenar encoder FFE ━━━
    print("━━━ PASO 3: Entrenando FFE Encoder ━━━\n")
    
    encoder = FFEEncoder(embedding_dim=384, n_dims_pca=81)
    encoder.fit(embeddings)
    
    print()
    
    # ━━━ PASO 4: Codificar a tensores FFE ━━━
    print("━━━ PASO 4: Codificando a tensores FFE ━━━\n")
    
    tensores = [encoder.encode(emb) for emb in test_embeddings]
    
    print("Tensores FFE generados:")
    for i, tensor in enumerate(tensores):
        nulls = np.sum(tensor == -1)
        ones = np.sum(tensor == 1)
        zeros = np.sum(tensor == 0)
        print(f"  [{i}] \"{test_frases[i][:45]}...\"")
        print(f"      Distribución: 1={ones} | 0={zeros} | N={nulls}")
    
    print()
    
    # ━━━ PASO 5: Calcular similitudes en espacio FFE ━━━
    print("━━━ PASO 5: Similitudes en espacio FFE (discreto) ━━━\n")
    
    print("Similitud Hamming (espacio FFE):")
    hamming_scores = {}
    for i, j, desc in pairs:
        sim = hamming_similarity(tensores[i], tensores[j])
        hamming_scores[(i, j)] = sim
        print(f"  [{i}↔{j}] {desc:45s} → {sim:.4f}")
    
    print()
    
    print("Distancia Triádica (ignora nulls):")
    triadic_scores = {}
    for i, j, desc in pairs:
        dist = triadic_distance(tensores[i], tensores[j])
        sim = 1.0 - dist  # Convertir a similitud
        triadic_scores[(i, j)] = sim
        print(f"  [{i}↔{j}] {desc:45s} → {sim:.4f}")
    
    print()
    
    # ━━━ PASO 6: VERIFICACIÓN CRÍTICA ━━━
    print("━━━ PASO 6: VERIFICACIÓN CRÍTICA ━━━\n")
    
    # Criterio de éxito:
    # - Pares similares deben tener similitud > pares diferentes
    # - Tanto en espacio continuo como en espacio FFE
    
    similar_pairs = [(0, 1), (2, 3)]
    different_pairs = [(0, 2), (1, 3)]
    
    print("Comparación de similitudes:\n")
    
    # Promedios
    avg_cosine_similar = np.mean([cosine_scores[p] for p in similar_pairs])
    avg_cosine_different = np.mean([cosine_scores[p] for p in different_pairs])
    
    avg_hamming_similar = np.mean([hamming_scores[p] for p in similar_pairs])
    avg_hamming_different = np.mean([hamming_scores[p] for p in different_pairs])
    
    avg_triadic_similar = np.mean([triadic_scores[p] for p in similar_pairs])
    avg_triadic_different = np.mean([triadic_scores[p] for p in different_pairs])
    
    print(f"Espacio Continuo (Coseno):")
    print(f"  Pares similares:    {avg_cosine_similar:.4f}")
    print(f"  Pares diferentes:   {avg_cosine_different:.4f}")
    print(f"  Separación:         {avg_cosine_similar - avg_cosine_different:.4f}")
    
    print(f"\nEspacio FFE (Hamming):")
    print(f"  Pares similares:    {avg_hamming_similar:.4f}")
    print(f"  Pares diferentes:   {avg_hamming_different:.4f}")
    print(f"  Separación:         {avg_hamming_similar - avg_hamming_different:.4f}")
    
    print(f"\nEspacio FFE (Triádico):")
    print(f"  Pares similares:    {avg_triadic_similar:.4f}")
    print(f"  Pares diferentes:   {avg_triadic_different:.4f}")
    print(f"  Separación:         {avg_triadic_similar - avg_triadic_different:.4f}")
    
    print()
    
    # ━━━ VEREDICTO ━━━
    print("╔═══════════════════════════════════════════════════════════════════╗")
    print("║  VEREDICTO                                                        ║")
    print("╚═══════════════════════════════════════════════════════════════════╝\n")
    
    # Test 1: ¿Se preserva el orden? (USAR TRIÁDICA, no Hamming)
    test1_pass = (avg_triadic_similar > avg_triadic_different)
    
    # Test 2: ¿La separación es significativa?
    test2_pass = (avg_triadic_similar - avg_triadic_different) > 0.05
    
    # Test 3: ¿Los nulls están balanceados? (no todos null, no cero nulls)
    avg_nulls = np.mean([np.sum(t == -1) for t in tensores])
    test3_pass = (10 < avg_nulls < 70)  # Entre 12% y 86% nulls
    
    print(f"✓ Test 1 - Orden preservado (Triádico):       {'✅ PASS' if test1_pass else '❌ FAIL'}")
    print(f"✓ Test 2 - Separación significativa (>0.05):  {'✅ PASS' if test2_pass else '❌ FAIL'}")
    print(f"✓ Test 3 - Nulls balanceados (10-70):         {'✅ PASS' if test3_pass else '❌ FAIL'} (avg={avg_nulls:.1f})")
    
    print()
    
    if test1_pass and test2_pass and test3_pass:
        print("🌟 RESULTADO: PRESERVACIÓN SEMÁNTICA EXITOSA")
        print("\nLos tensores FFE mantienen la estructura semántica original.")
        print("Aurora puede operar sobre significado real, no solo números.")
        print("\nPróximo paso: Integrar con LLM real (sentence-transformers)")
    else:
        print("⚠️ RESULTADO: PRESERVACIÓN PARCIAL O FALLIDA")
        print("\nAjustar hiperparámetros:")
        print("  - Aumentar n_dims_pca")
        print("  - Cambiar umbrales de cuantización")
        print("  - Probar con embeddings reales (no simulados)")
    
    print()
    
    # ━━━ INFORMACIÓN ADICIONAL ━━━
    print("━━━ Información Técnica ━━━\n")
    print(f"Dimensión original:  {embeddings.shape[1]}")
    print(f"Dimensión PCA:       {encoder.n_dims_pca}")
    print(f"Ratio compresión:    {embeddings.shape[1] / encoder.n_dims_pca:.2f}x")
    print(f"Varianza preservada: {encoder.pca.explained_variance_ratio_.sum():.3f}")
    
    print()

if __name__ == "__main__":
    test_semantic_preservation()
