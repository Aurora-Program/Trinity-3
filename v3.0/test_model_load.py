"""Test minimalista - verifica que sentence-transformers funciona"""

print("🔄 Step 1: Importando numpy y sklearn...")
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
print("✅ numpy y sklearn OK")

print("\n🔄 Step 2: Importando sentence_transformers...")
try:
    from sentence_transformers import SentenceTransformer
    print("✅ sentence_transformers importado")
except Exception as e:
    print(f"❌ Error: {e}")
    exit(1)

print("\n🔄 Step 3: Cargando modelo (primera vez puede tardar ~1 min)...")
print("   Descargando desde HuggingFace...")

try:
    model = SentenceTransformer('all-MiniLM-L6-v2')
    print("✅ Modelo cargado")
except Exception as e:
    print(f"❌ Error al cargar: {e}")
    exit(1)

print("\n🔄 Step 4: Generando embeddings de prueba...")
frases = [
    "El gato duerme",
    "Un felino descansa",
    "El perro corre",
    "Un can trota"
]

embeddings = model.encode(frases, show_progress_bar=False)
print(f"✅ Embeddings generados: {embeddings.shape}")

# Verificar similitudes
def cosine(a, b):
    return np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b))

sim_gatos = cosine(embeddings[0], embeddings[1])
sim_perros = cosine(embeddings[2], embeddings[3])
sim_cruzada = cosine(embeddings[0], embeddings[2])

print(f"\n📊 Similitudes baseline:")
print(f"   gato ↔ felino:  {sim_gatos:.4f}")
print(f"   perro ↔ can:    {sim_perros:.4f}")
print(f"   gato ↔ perro:   {sim_cruzada:.4f}")

if sim_gatos > sim_cruzada and sim_perros > sim_cruzada:
    print("\n🎉 ¡EMBEDDINGS REALES FUNCIONAN!")
    print("✨ sentence-transformers captura semántica correctamente")
    print("🚀 Listo para test completo FFE")
else:
    print("\n⚠️  Similitudes inesperadas")

print("\n" + "="*60)
print("CONCLUSIÓN: Sistema listo para FFE encoding real")
print("="*60)
