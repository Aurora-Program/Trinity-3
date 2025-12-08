"""Carga rápida de modelo sentence-transformers para validar embeddings reales.

Usa un modelo pequeño primero (paraphrase-MiniLM-L3-v2). Si carga correctamente
imprime similitudes y luego intenta opcionalmente cargar el modelo estándar
all-MiniLM-L6-v2 (puede tardar más). Así evitamos esperas largas iniciales.

Reglas proyecto: solo código en newVersion, mínimo líneas, fractalidad.
"""

print("🔄 Paso 1: imports base...")
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
print("✅ numpy/sklearn OK")

print("🔄 Paso 2: import sentence_transformers...")
from sentence_transformers import SentenceTransformer
print("✅ sentence_transformers OK")

def cosine(a,b):
    return float(np.dot(a,b)/(np.linalg.norm(a)*np.linalg.norm(b)))

def prueba_modelo(nombre, frases):
    print(f"\n🚀 Cargando modelo: {nombre}")
    m = SentenceTransformer(nombre)
    emb = m.encode(frases, show_progress_bar=False)
    s01 = cosine(emb[0], emb[1])
    s23 = cosine(emb[2], emb[3])
    s02 = cosine(emb[0], emb[2])
    print(f"📊 {nombre} similitudes:")
    print(f"   par 1 sem: {s01:.4f}")
    print(f"   par 2 sem: {s23:.4f}")
    print(f"   cruzada   : {s02:.4f}")
    ok = s01 > s02 and s23 > s02
    print("✅ Semántica preservada" if ok else "⚠️ Patrón inesperado")
    return ok

frases = ["El gato duerme","Un felino descansa","El perro corre","Un can trota"]

print("\n🌱 Modelo pequeño primero...")
small_ok = prueba_modelo('paraphrase-MiniLM-L3-v2', frases)

if small_ok:
    print("\n⏳ Intentando modelo estándar (puede tardar)...")
    try:
        prueba_modelo('all-MiniLM-L6-v2', frases)
    except Exception as e:
        print(f"❌ Error en modelo grande: {e}")
else:
    print("⚠️ El modelo pequeño no preservó semántica; revisar entorno antes de continuar.")

print("\n🧪 Estado: listo para test FFE real si al menos un modelo dio OK.")