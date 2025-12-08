"""Test rápido con embeddings reales - versión simplificada"""

import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA

print("🔄 Importando sentence-transformers...")
from sentence_transformers import SentenceTransformer

print("✅ Importación exitosa")
print("🔄 Cargando modelo MiniLM-L6-v2 (puede tardar ~1 min en primera ejecución)...")

model = SentenceTransformer('all-MiniLM-L6-v2')

print("✅ Modelo cargado")
print("\n" + "="*70)
print(" TEST PRESERVACIÓN SEMÁNTICA - EMBEDDINGS REALES")
print("="*70)

# Frases del whitepaper
frases = [
    "El gato duerme en el sofá",       # 0
    "Un felino descansa sobre el sillón",  # 1 (similar a 0)
    "El perro corre en el parque",     # 2
    "Un can trota por el jardín"       # 3 (similar a 2)
]

print("\n📝 Frases:")
for i, f in enumerate(frases):
    print(f"  [{i}] {f}")

print("\n🧠 Generando embeddings...")
embeddings = model.encode(frases, show_progress_bar=False)
print(f"✅ Shape: {embeddings.shape}")

# Cosine similarity baseline
def cosine_sim(a, b):
    return np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b))

print("\n📊 Similitudes baseline (Coseno):")
sim_01 = cosine_sim(embeddings[0], embeddings[1])
sim_23 = cosine_sim(embeddings[2], embeddings[3])
sim_02 = cosine_sim(embeddings[0], embeddings[2])
sim_13 = cosine_sim(embeddings[1], embeddings[3])

print(f"  [0↔1] gato/felino:    {sim_01:.4f} ✅ (esperado: alto)")
print(f"  [2↔3] perro/can:      {sim_23:.4f} ✅ (esperado: alto)")
print(f"  [0↔2] gato/perro:     {sim_02:.4f} (esperado: bajo)")
print(f"  [1↔3] felino/can:     {sim_13:.4f} (esperado: bajo)")

avg_similar = (sim_01 + sim_23) / 2
avg_different = (sim_02 + sim_13) / 2

print(f"\n  Avg similar:    {avg_similar:.4f}")
print(f"  Avg diferente:  {avg_different:.4f}")
print(f"  Separación:     {avg_similar - avg_different:.4f}")

# FFE Encoding con solo 4 samples (PCA a 3 dims para simplificar)
print("\n🔄 FFE Encoding (PCA a 9 dims para 4 samples)...")

scaler = StandardScaler()
scaled = scaler.fit_transform(embeddings)

pca = PCA(n_components=9)  # Máximo 4-1=3 realmente útiles
reduced = pca.fit_transform(scaled)

print(f"  Varianza preservada: {pca.explained_variance_ratio_.sum()*100:.1f}%")

# Quantizar
mean = reduced.mean()
std = reduced.std()
thresh_low = mean - 0.5 * std
thresh_high = mean + 0.5 * std

tensors = []
for row in reduced:
    tensor = []
    for val in row:
        if val < thresh_low:
            tensor.append(-1)
        elif val > thresh_high:
            tensor.append(1)
        else:
            tensor.append(0)
    tensors.append(tensor)

print("\n📊 Tensores FFE:")
for i, t in enumerate(tensors):
    nulls = t.count(-1)
    print(f"  [{i}] Nulls: {nulls}/9 ({nulls/9*100:.0f}%)")

# Distancia triádica
def triadic_dist(a, b):
    valid = [(x, y) for x, y in zip(a, b) if x != -1 and y != -1]
    if not valid:
        return 0.0
    matches = sum(1 for x, y in valid if x == y)
    return matches / len(valid)

print("\n📊 Similitudes FFE (Triádico):")
sim_01_ffe = triadic_dist(tensors[0], tensors[1])
sim_23_ffe = triadic_dist(tensors[2], tensors[3])
sim_02_ffe = triadic_dist(tensors[0], tensors[2])
sim_13_ffe = triadic_dist(tensors[1], tensors[3])

print(f"  [0↔1] gato/felino:    {sim_01_ffe:.4f}")
print(f"  [2↔3] perro/can:      {sim_23_ffe:.4f}")
print(f"  [0↔2] gato/perro:     {sim_02_ffe:.4f}")
print(f"  [1↔3] felino/can:     {sim_13_ffe:.4f}")

avg_similar_ffe = (sim_01_ffe + sim_23_ffe) / 2
avg_different_ffe = (sim_02_ffe + sim_13_ffe) / 2

print(f"\n  Avg similar:    {avg_similar_ffe:.4f}")
print(f"  Avg diferente:  {avg_different_ffe:.4f}")
print(f"  Separación:     {avg_similar_ffe - avg_different_ffe:.4f}")

# Veredicto
print("\n" + "="*70)
print("VEREDICTO")
print("="*70)

test1 = avg_similar_ffe > avg_different_ffe
test2 = (avg_similar_ffe - avg_different_ffe) > 0.05

print(f"\n✅ Test 1 (Orden): {avg_similar_ffe:.4f} > {avg_different_ffe:.4f} → {'✅ PASS' if test1 else '❌ FAIL'}")
print(f"✅ Test 2 (Sep):   {avg_similar_ffe - avg_different_ffe:.4f} > 0.05 → {'✅ PASS' if test2 else '❌ FAIL'}")

if test1 and test2:
    print("\n🎉 PRESERVACIÓN SEMÁNTICA EXITOSA CON EMBEDDINGS REALES")
    print("✨ Aurora puede operar sobre significado REAL de sentence-transformers")
else:
    print("\n⚠️  Preservación parcial - ajustar parámetros")

print("\n" + "="*70)
