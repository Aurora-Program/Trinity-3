"""FFE Generator: Genera tensores FFE desde embeddings y los exporta para C.

Pipeline completo:
1. Genera embeddings (sintéticos o reales)
2. Reduce dimensionalidad con PCA
3. Cuantiza a trits entrópicos {1=false, 2=true, 3=null}
4. Exporta formato simple para los módulos Aurora en C.

Principio entrópico:
 1 = estado definido (false, baja entropía)
 2 = estado definido opuesto (true, baja entropía)
 3 = estado indeterminado (null, máxima entropía)

Cumple: sólo carpeta newVersion, mínimo código, autosimilitud.
"""

import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA

class FFEGenerator:
    def __init__(self, n_components=81):
        self.scaler = StandardScaler()
        self.pca = PCA(n_components=n_components, random_state=42)
        self.fitted = False
    
    def fit(self, embeddings):
        """Entrena PCA con embeddings base."""
        scaled = self.scaler.fit_transform(embeddings)
        self.pca.fit(scaled)
        self.fitted = True
        return self
    
    def encode(self, embeddings):
        """Convierte embeddings a tensores FFE (81 dims → 27 dimensiones de 3 trits).
        
        Sistema entrópico: {1=false, 2=true, 3=null}
        """
        if not self.fitted:
            self.fit(embeddings)
        
        scaled = self.scaler.transform(embeddings)
        reduced = self.pca.transform(scaled)
        
        # Cuantización adaptativa ±0.5σ → SISTEMA ENTRÓPICO {1,2,3}
        std = np.std(reduced, axis=0)
        std[std == 0] = 1.0
        
        trits = np.zeros_like(reduced, dtype=np.int8)
        trits[reduced > 0.5 * std] = 2       # true (orden positivo)
        trits[reduced < -0.5 * std] = 1      # false (orden negativo)
        trits[(reduced >= -0.5 * std) & (reduced <= 0.5 * std)] = 3  # null (máxima entropía)
        
        return trits
    
    def save_for_c(self, trits, filename, labels=None):
        """Guarda tensores FFE en formato legible por C.
        
        Formato:
        <n_tensors> <dims_per_tensor>
        label1
        t0 t1 t2 ... t80
        label2
        t0 t1 t2 ... t80
        """
        n_tensors, n_dims = trits.shape
        
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(f"{n_tensors} {n_dims}\n")
            
            for i, tensor in enumerate(trits):
                label = labels[i] if labels else f"tensor_{i}"
                f.write(f"{label}\n")
                # Escribir todos los trits en UNA línea separados por espacios
                trit_strs = [str(int(t)) for t in tensor]
                f.write(" ".join(trit_strs) + "\n")
        
        print(f"✅ Guardados {n_tensors} tensores FFE en {filename}")
        return filename

def generate_synthetic_embeddings(n_samples=20, dim=384):
    """Genera embeddings sintéticos con clusters semánticos."""
    clusters = []
    
    # Cluster 1: "animales pequeños"
    for _ in range(n_samples // 4):
        base = np.random.randn(dim) * 0.3
        base[:10] += 2.0  # Marca semántica
        clusters.append(base)
    
    # Cluster 2: "animales grandes"
    for _ in range(n_samples // 4):
        base = np.random.randn(dim) * 0.3
        base[:10] -= 2.0
        clusters.append(base)
    
    # Cluster 3: "objetos"
    for _ in range(n_samples // 4):
        base = np.random.randn(dim) * 0.3
        base[10:20] += 2.0
        clusters.append(base)
    
    # Cluster 4: "acciones"
    for _ in range(n_samples // 4):
        base = np.random.randn(dim) * 0.3
        base[10:20] -= 2.0
        clusters.append(base)
    
    embeddings = np.array(clusters)
    
    labels = (
        [f"animal_peq_{i}" for i in range(n_samples // 4)] +
        [f"animal_gra_{i}" for i in range(n_samples // 4)] +
        [f"objeto_{i}" for i in range(n_samples // 4)] +
        [f"accion_{i}" for i in range(n_samples // 4)]
    )
    
    return embeddings, labels

if __name__ == "__main__":
    print("🧬 FFE Generator - Embeddings → Trits Entrópicos → C")
    print("=" * 60)

    embeddings, labels = generate_synthetic_embeddings(n_samples=100, dim=384)
    print(f"📊 Embeddings sintéticos: {embeddings.shape}")

    gen = FFEGenerator(n_components=81)
    trits = gen.encode(embeddings)
    print(f"🎯 Tensores FFE: {trits.shape} (81 trits → 27 dimensiones)")

    count_false = (trits == 1).sum()
    count_true  = (trits == 2).sum()
    count_null  = (trits == 3).sum()
    total = trits.size
    print("📊 Distribución:")
    print(f"  false(1): {count_false} ({100*count_false/total:5.1f}%)")
    print(f"  true (2): {count_true} ({100*count_true/total:5.1f}%)")
    print(f"  null (3): {count_null} ({100*count_null/total:5.1f}%)")

    output_file = "c:/Users/p_m_a/Aurora/Trinity-3/newVersion/tensors_ffe.txt"
    gen.save_for_c(trits, output_file, labels)

    print("\n📈 Resumen:")
    print(f"  Tensores: {len(trits)}")
    print(f"  Trits por tensor: {trits.shape[1]}")
    print(f"  Dimensiones FFE: {trits.shape[1] // 3}")
    print(f"  Entropía (ratio null): {count_null/total:.2%}")
    print(f"\n✨ Archivo listo: {output_file}")
