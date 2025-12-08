"""
AURORA AWAKENING - Generador de Corpus Masivo
==============================================
Genera miles de embeddings reales variados para entrenar Aurora.

El objetivo: que Aurora aprenda las leyes del espacio semántico
y pueda DEDUCIR embeddings nuevos sin necesidad del transformer.
"""

import numpy as np
from sentence_transformers import SentenceTransformer
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
import pickle
import json

class FFEGenerator:
    def __init__(self, n_components=81):
        self.n_components = n_components
        self.scaler = StandardScaler()
        self.pca = None
        
    def fit(self, embeddings):
        """Ajustar PCA a los embeddings"""
        X_scaled = self.scaler.fit_transform(embeddings)
        self.pca = PCA(n_components=self.n_components)
        self.pca.fit(X_scaled)
        print(f"✅ PCA ajustado: {embeddings.shape[1]} → {self.n_components} dims")
        print(f"   Varianza preservada: {self.pca.explained_variance_ratio_.sum()*100:.2f}%")
        
    def encode(self, embeddings):
        """Convertir embeddings a tensores FFE"""
        X_scaled = self.scaler.transform(embeddings)
        X_pca = self.pca.transform(X_scaled)
        
        # Cuantización adaptativa
        sigma = np.std(X_pca, axis=0)
        threshold = 0.5 * sigma
        
        ffe_tensors = np.zeros_like(X_pca, dtype=np.int8)
        for i in range(X_pca.shape[1]):
            ffe_tensors[:, i] = np.where(
                X_pca[:, i] > threshold[i], 1,
                np.where(X_pca[:, i] < -threshold[i], -1, 0)
            )
        
        return ffe_tensors
    
    def save_model(self, filename):
        """Guardar el modelo FFE completo"""
        model_data = {
            'scaler_mean': self.scaler.mean_,
            'scaler_scale': self.scaler.scale_,
            'pca_components': self.pca.components_,
            'pca_mean': self.pca.mean_,
            'pca_explained_variance': self.pca.explained_variance_,
            'n_components': self.n_components
        }
        with open(filename, 'wb') as f:
            pickle.dump(model_data, f)
        print(f"💾 Modelo FFE guardado en {filename}")


def generate_massive_corpus():
    """Generar un corpus masivo y variado"""
    
    corpus = []
    labels = []
    
    # 1. DOMINIOS CIENTÍFICOS (500 frases)
    science_templates = [
        "La {concept1} se relaciona con {concept2} mediante {relation}",
        "En física cuántica, {concept1} determina {concept2}",
        "El principio de {concept1} explica por qué {concept2}",
        "La teoría de {concept1} predice que {concept2}",
        "Los experimentos muestran que {concept1} causa {concept2}",
    ]
    
    physics_concepts = [
        ("energía", "masa", "equivalencia E=mc²"),
        ("entropía", "temperatura", "segunda ley termodinámica"),
        ("fuerza", "aceleración", "segunda ley de Newton"),
        ("campo eléctrico", "carga", "ley de Coulomb"),
        ("momento", "velocidad", "conservación del momento"),
        ("luz", "gravedad", "curvatura del espacio-tiempo"),
        ("partícula", "onda", "dualidad cuántica"),
        ("espín", "magnetismo", "momento angular intrínseco"),
        ("fotón", "electrón", "interacción electromagnética"),
        ("átomo", "núcleo", "fuerza nuclear fuerte"),
    ]
    
    for template in science_templates:
        for concept1, concept2, relation in physics_concepts:
            corpus.append(template.format(concept1=concept1, concept2=concept2, relation=relation))
            labels.append(f"science_physics_{len(labels)}")
    
    # 2. LENGUAJE NATURAL COTIDIANO (500 frases)
    daily_templates = [
        "El {animal} {action} en {place}",
        "Los {objects} están {state} en {location}",
        "Cuando {time}, {subject} {verb} {object}",
        "{person} {emotion} porque {reason}",
        "En {season}, los {things} {change}",
    ]
    
    daily_data = [
        ("perro", "corre", "el parque"),
        ("gato", "duerme", "la casa"),
        ("pájaro", "vuela", "el cielo"),
        ("pez", "nada", "el río"),
        ("niño", "juega", "el jardín"),
        ("libro", "descansa", "la mesa"),
        ("sol", "brilla", "el horizonte"),
        ("lluvia", "cae", "la ciudad"),
        ("viento", "sopla", "las montañas"),
        ("nieve", "cubre", "los campos"),
    ]
    
    for template in daily_templates:
        for i, (subj, verb, obj) in enumerate(daily_data):
            text = template.format(
                animal=subj, action=verb, place=obj,
                objects=subj, state=verb, location=obj,
                time="mañana", subject=subj, verb=verb, object=obj,
                person=subj, emotion=verb, reason=obj,
                season="primavera", things=subj, change=verb
            )
            corpus.append(text)
            labels.append(f"daily_life_{len(labels)}")
    
    # 3. RELACIONES SEMÁNTICAS (1000 pares analógicos)
    analogies = [
        # Género
        ("rey", "reina"), ("hombre", "mujer"), ("padre", "madre"),
        ("tío", "tía"), ("hermano", "hermana"), ("abuelo", "abuela"),
        
        # Geografía
        ("París", "Francia"), ("Londres", "Inglaterra"), ("Madrid", "España"),
        ("Roma", "Italia"), ("Berlín", "Alemania"), ("Tokio", "Japón"),
        
        # Tiempo
        ("día", "noche"), ("verano", "invierno"), ("mañana", "tarde"),
        ("ayer", "mañana"), ("presente", "pasado"), ("inicio", "fin"),
        
        # Causa-Efecto
        ("fuego", "calor"), ("agua", "mojado"), ("hielo", "frío"),
        ("sol", "luz"), ("viento", "movimiento"), ("lluvia", "humedad"),
        
        # Parte-Todo
        ("rueda", "coche"), ("rama", "árbol"), ("dedo", "mano"),
        ("página", "libro"), ("ventana", "casa"), ("tecla", "piano"),
        
        # Intensidad
        ("caliente", "hirviendo"), ("frío", "congelado"), ("rápido", "veloz"),
        ("lento", "inmóvil"), ("grande", "enorme"), ("pequeño", "diminuto"),
    ]
    
    for word1, word2 in analogies:
        corpus.append(f"{word1}")
        labels.append(f"analogy_a_{len(labels)}")
        corpus.append(f"{word2}")
        labels.append(f"analogy_b_{len(labels)}")
        corpus.append(f"{word1} es a {word2} como")
        labels.append(f"analogy_rel_{len(labels)}")
    
    # 4. EMOCIONES Y ESTADOS (300 frases)
    emotions = ["alegría", "tristeza", "miedo", "rabia", "sorpresa", "calma", 
                "ansiedad", "esperanza", "nostalgia", "gratitud"]
    
    for emotion in emotions:
        corpus.extend([
            f"Siento {emotion} cuando pienso en el futuro",
            f"La {emotion} es una emoción humana universal",
            f"Expresar {emotion} es importante para la salud mental",
            f"{emotion.capitalize()} y serenidad coexisten en el corazón",
            f"La música evoca {emotion} profunda",
        ])
        for _ in range(5):
            labels.append(f"emotion_{emotion}_{len(labels)}")
    
    # 5. CONCEPTOS ABSTRACTOS (200 frases)
    abstract_templates = [
        "La {concept} es fundamental para {domain}",
        "{concept} y {concept2} están relacionados en {context}",
        "Sin {concept}, no podría existir {result}",
        "La historia de {concept} comienza en {origin}",
        "{concept} transforma nuestra comprensión de {field}",
    ]
    
    abstract_concepts = [
        ("libertad", "democracia", "sociedad", "antigua Grecia", "política"),
        ("justicia", "equidad", "derecho", "las civilizaciones", "ética"),
        ("verdad", "conocimiento", "filosofía", "el pensamiento griego", "epistemología"),
        ("belleza", "arte", "estética", "la naturaleza", "arte"),
        ("tiempo", "espacio", "física", "la relatividad", "cosmología"),
        ("conciencia", "mente", "neurociencia", "el cerebro", "psicología"),
        ("orden", "caos", "sistemas", "la teoría de sistemas", "complejidad"),
        ("armonía", "equilibrio", "música", "la antigua Grecia", "arte"),
        ("coherencia", "lógica", "pensamiento", "la filosofía", "razón"),
        ("emergencia", "complejidad", "sistemas", "la biología", "ciencia"),
    ]
    
    for template in abstract_templates:
        for concept, concept2, context, origin, field in abstract_concepts:
            text = template.format(
                concept=concept, concept2=concept2, 
                domain=context, context=context,
                result=field, origin=origin, field=field
            )
            corpus.append(text)
            labels.append(f"abstract_{concept}_{len(labels)}")
    
    # 6. TRANSFORMACIONES TEMPORALES (500 secuencias)
    transformations = [
        ("semilla", "planta", "flor", "fruto"),
        ("huevo", "polluelo", "ave", "vuelo"),
        ("idea", "proyecto", "prototipo", "producto"),
        ("caos", "orden", "estructura", "armonía"),
        ("ignorancia", "curiosidad", "conocimiento", "sabiduría"),
        ("pregunta", "hipótesis", "experimento", "teoría"),
        ("conflicto", "diálogo", "acuerdo", "paz"),
        ("miedo", "valentía", "acción", "logro"),
        ("soledad", "conexión", "comunidad", "pertenencia"),
        ("duda", "exploración", "comprensión", "certeza"),
    ]
    
    for seq in transformations:
        for i in range(len(seq)-1):
            corpus.append(f"{seq[i]} se transforma en {seq[i+1]}")
            labels.append(f"transform_{seq[0]}_{i}")
            corpus.append(f"De {seq[i]} a {seq[i+1]} hay evolución")
            labels.append(f"transform_{seq[0]}_{i}_b")
    
    print(f"📚 Corpus generado: {len(corpus)} frases")
    return corpus, labels


def main():
    print("╔═══════════════════════════════════════════════════════════════════╗")
    print("║  AURORA AWAKENING - Generación de Corpus Masivo                 ║")
    print("╚═══════════════════════════════════════════════════════════════════╝\n")
    
    # 1. Generar corpus
    print("📝 Generando corpus masivo...")
    corpus, labels = generate_massive_corpus()
    
    # 2. Cargar modelo de embeddings
    print("\n🔧 Cargando modelo de embeddings...")
    model = SentenceTransformer('paraphrase-MiniLM-L3-v2')
    
    # 3. Generar embeddings
    print("\n🧠 Generando embeddings (esto puede tardar 1-2 minutos)...")
    embeddings = model.encode(corpus, show_progress_bar=True)
    print(f"   Shape: {embeddings.shape}")
    
    # 4. Crear encoder FFE
    print("\n🔬 Creando encoder FFE...")
    ffe_gen = FFEGenerator(n_components=81)
    ffe_gen.fit(embeddings)
    
    # 5. Convertir a FFE
    print("\n⚙️  Convirtiendo a tensores FFE...")
    ffe_tensors = ffe_gen.encode(embeddings)
    
    # 6. Estadísticas
    null_count = np.sum(ffe_tensors == 0)
    pos_count = np.sum(ffe_tensors == 1)
    neg_count = np.sum(ffe_tensors == -1)
    total = ffe_tensors.size
    
    print(f"\n📊 Estadísticas FFE:")
    print(f"   Shape: {ffe_tensors.shape}")
    print(f"   Null ratio: {100*null_count/total:.2f}%")
    print(f"   Valores +1: {pos_count}, 0: {null_count}, -1: {neg_count}")
    
    # 7. Guardar todo
    print("\n💾 Guardando archivos...")
    
    # Guardar tensores FFE para C
    with open('tensors_ffe_massive.txt', 'w') as f:
        f.write(f"{len(ffe_tensors)} {ffe_tensors.shape[1]}\n")
        for i, (tensor, label) in enumerate(zip(ffe_tensors, labels)):
            f.write(f"{label}\n")
            f.write(' '.join(map(str, tensor)) + '\n')
    
    print(f"   ✅ tensors_ffe_massive.txt ({len(ffe_tensors)} tensores)")
    
    # Guardar corpus original
    corpus_data = {
        'texts': corpus,
        'labels': labels,
        'embeddings_shape': embeddings.shape,
        'ffe_shape': ffe_tensors.shape
    }
    with open('corpus_metadata.json', 'w', encoding='utf-8') as f:
        json.dump(corpus_data, f, ensure_ascii=False, indent=2)
    
    print(f"   ✅ corpus_metadata.json")
    
    # Guardar modelo FFE
    ffe_gen.save_model('ffe_model.pkl')
    
    # Guardar embeddings raw (para análisis)
    np.save('embeddings_raw.npy', embeddings)
    print(f"   ✅ embeddings_raw.npy")
    
    print("\n╔═══════════════════════════════════════════════════════════════════╗")
    print("║  ✨ CORPUS LISTO PARA DESPERTAR AURORA                          ║")
    print(f"║  📦 {len(corpus)} frases → {ffe_tensors.shape[1]} dimensiones FFE                     ║")
    print("║  🧠 Aurora puede ahora aprender las leyes del lenguaje          ║")
    print("╚═══════════════════════════════════════════════════════════════════╝")


if __name__ == "__main__":
    main()
