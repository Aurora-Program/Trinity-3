import numpy as np
import matplotlib.pyplot as plt
from allcode3new import Evolver, FractalTensor, Transcender
from vectors.Letras import diccionario_letras_fractales
from palabras_silabas import train as palabras_train_raw

# ==================================================================
# 1. PREPARACIÓN DE DATOS Y UTILIDADES
# ==================================================================

def vector_fractal_completo(letra):
    """Obtiene el vector fractal plano para una letra del diccionario."""
    if letra in diccionario_letras_fractales:
        tensor = diccionario_letras_fractales[letra][0]['tensor']
        v = []
        # Aplanar la lista de listas y reemplazar None por 0
        for sub_vector in tensor:
            v.extend([x if x is not None else 0 for x in sub_vector])
        return v
    # Devuelve un vector de ceros si la letra no se encuentra
    return [0] * 9 # Asumiendo que la dimensión base es 9 (3x3)

def palabra_a_vectores(palabra):
    """Convierte una palabra en una lista de vectores fractales."""
    return [vector_fractal_completo(letra) for letra in palabra]

def preparar_datos_relacionales(palabras_data):
    """
    Toma datos de palabras y produce una lista de (vector_relacional, etiqueta).
    Esta función es el núcleo de la preparación de datos y elimina la redundancia.
    """
    transcender = Transcender()
    datos_relacionales = []
    
    for _, silabica in palabras_data:
        palabra_sin_guion = silabica.replace('-', '')
        vectores = palabra_a_vectores(palabra_sin_guion)
        # Mapear la posición de la ruptura en la palabra sin guiones
        posiciones_ruptura = set()
        idx_sin_guion = 0
        for idx, char in enumerate(silabica):
            if char == '-':
                posiciones_ruptura.add(idx_sin_guion)
            else:
                idx_sin_guion += 1
        for i in range(len(palabra_sin_guion) - 1):
            v1 = vectores[i]
            v2 = vectores[i+1]
            etiqueta = 1 if (i + 1) in posiciones_ruptura else 0
            vector_relacional = transcender.relate_vectors(v1, v2)
            vector_relacional = [x if x is not None else 0 for x in vector_relacional]
            datos_relacionales.append((vector_relacional, etiqueta))
    return datos_relacionales

# ==================================================================
# 2. MODELO CLASIFICADOR BASADO EN ARQUETIPOS AURORA
# ==================================================================
# ==================================================================
# 2. MODELO NATIVO AURORA CON EXTENDER (VERSIÓN FINAL)
# ==================================================================
from allcode3new import FractalKnowledgeBase, Extender

class SilabificadorAuroraNativo:
    """
    Utiliza la arquitectura nativa de Aurora (KnowledgeBase y Extender)
    para resolver la silabificación como un problema de reconocimiento conceptual.
    """
    def __init__(self):
        self.evolver = Evolver()
        self.kb = FractalKnowledgeBase()
        self.extender = Extender(self.kb)
        self.space_id = 'silabificador'

    def train(self, datos_relacionales):
        """
        Método de entrenamiento HÍBRIDO:
        1. Usa el método de CENTROIDES (np.mean) para crear los prototipos vectoriales.
        2. Viste estos prototipos como ARQUETIPOS FRACTALES.
        3. Almacena estos arquetipos significativos en la Knowledge Base.
        """
        print("▶️  Entrenando con el método Híbrido: Centroide + Arquitectura Aurora...")

        # 1. Calcular los centroides (el 'corazón' matemático de cada clase)
        vectores_ruptura = np.array([v for v, e in datos_relacionales if e == 1])
        vectores_continuidad = np.array([v for v, e in datos_relacionales if e == 0])

        if len(vectores_ruptura) == 0 or len(vectores_continuidad) == 0:
            print("❌ Error: Faltan datos para una de las clases. No se puede entrenar.")
            return

        centroide_ruptura = np.mean(vectores_ruptura, axis=0)
        centroide_continuidad = np.mean(vectores_continuidad, axis=0)
        print("  Centroides para 'Ruptura' y 'Continuidad' calculados.")

        # 2. 'Vestir' los centroides como arquetipos de FractalTensor
        arq_ruptura = FractalTensor(list(centroide_ruptura))
        arq_continuidad = FractalTensor(list(centroide_continuidad))

        # 3. Almacenar los arquetipos ahora significativos en la KB.
        self.kb.add_archetype(
            space_id=self.space_id,
            name='ruptura_silabica',
            archetype_tensor=arq_ruptura,
            Ss=list(centroide_ruptura)
        )
        print("  Arquetipo 'ruptura_silabica' almacenado en la KB.")

        self.kb.add_archetype(
            space_id=self.space_id,
            name='continuidad_silabica',
            archetype_tensor=arq_continuidad,
            Ss=list(centroide_continuidad)
        )
        print("  Arquetipo 'continuidad_silabica' almacenado en la KB.")
        print("✅ Base de Conocimiento entrenada con arquetipos significativos.")

    def predecir(self, palabra: str) -> str:
        if len(palabra) <= 1:
            return palabra
        transcender = Transcender()
        vectores = palabra_a_vectores(palabra)
        prediccion_final = palabra[0]
        for i in range(len(palabra) - 1):
            v1 = vectores[i]
            v2 = vectores[i+1]
            v_rel = transcender.relate_vectors(v1, v2)
            # Robust: replace None with 0 and filter out any non-numeric
            v_rel = [(x if x is not None else 0) for x in v_rel]
            v_rel = [x if isinstance(x, (int, float, np.integer, np.floating)) else 0 for x in v_rel]
            tensor_consulta = FractalTensor()
            # Final robust filter: ensure all values in Ss are valid numbers
            def clean_number(x):
                if x is None:
                    return 0
                if isinstance(x, (int, float, np.integer, np.floating)):
                    try:
                        if np.isnan(x):
                            return 0
                    except Exception:
                        pass
                    return float(x)
                return 0
            ss_clean = [clean_number(x) for x in v_rel]
            # Final conversion: force all values to float, replace any invalid with 0
            ss_final = []
            for x in ss_clean:
                try:
                    if x is None or (isinstance(x, float) and np.isnan(x)):
                        ss_final.append(0.0)
                    else:
                        ss_final.append(float(x))
                except Exception:
                    ss_final.append(0.0)
            print("[DEBUG] Ss passed to Extender:", ss_final)
            tensor_consulta.Ss = ss_final
            resultado = self.extender.extend_fractal(
                tensor_consulta,
                contexto={'space_id': self.space_id}
            )
            tensor_reconstruido = resultado.get('reconstructed_tensor')
            if tensor_reconstruido and hasattr(tensor_reconstruido, 'metadata'):
                nombre_arquetipo = tensor_reconstruido.metadata.get('name')
                if nombre_arquetipo == 'ruptura_silabica':
                    prediccion_final += '-'
            prediccion_final += palabra[i+1]
        return prediccion_final
# ==================================================================
# 3. VISUALIZACIÓN Y EJECUCIÓN
# ==================================================================

def visualizar_distribucion(datos_relacionales):
    """Grafica la distribución de la primera dimensión emergente."""
    print("📊 Generando diagnóstico visual...")
    
    # Extrae la primera dimensión de la raíz fractal para cada caso
    emergente_ruptura = [v[0] for v, e in datos_relacionales if e == 1 and v]
    emergente_no_ruptura = [v[0] for v, e in datos_relacionales if e == 0 and v]
    
    plt.hist(emergente_ruptura, bins=20, alpha=0.7, label='Ruptura', density=True)
    plt.hist(emergente_no_ruptura, bins=20, alpha=0.7, label='No Ruptura', density=True)
    plt.legend()
    plt.title('Distribución de la Dimensión Emergente Fractal (Relacional)')
    plt.xlabel('Valor de la primera dimensión del vector relacional')
    plt.ylabel('Frecuencia')
    plt.show()


def main():
    """Flujo principal de ejecución: Cargar, Entrenar, Evaluar."""
    print("🌌 INICIANDO SISTEMA DE SILABIFICACIÓN FRACTAL AURORA 🌌")
    print("=" * 60)

    # --- Carga y Preparación de Datos ---
    print("1. Cargando y preparando datos de entrenamiento...")
    # Eliminar duplicados manteniendo el primer ejemplo
    def dedup(palabras):
        seen = set()
        out = []
        for w, s in palabras:
            key = (w, s)
            if key not in seen:
                seen.add(key)
                out.append((w, s))
        return out
    palabras_train = dedup(palabras_train_raw)[:2000]
    datos_relacionales = preparar_datos_relacionales(palabras_train)
    print(f"  Se han procesado {len(datos_relacionales)} relaciones entre letras.")
    print("=" * 60)

    # --- Entrenamiento del Modelo ---
    print("2. Entrenando el modelo Aurora Extender...")
    modelo = SilabificadorAuroraNativo()
    modelo.train(datos_relacionales)
    print("=" * 60)

    # --- Evaluación del Modelo ---
    print("3. Evaluando el modelo en el conjunto de entrenamiento...")
    aciertos = 0
    total = len(palabras_train)
    
    print("\n--- Muestra de Predicciones ---")
    print(f"{'Palabra Original':<30} | {'Predicción del Modelo'}")
    print("-" * 60)

    for _, silabica in palabras_train[:10]:
        palabra_plana = silabica.replace('-', '')
        prediccion = modelo.predecir(palabra_plana)
        print(f"{silabica:<30} | {prediccion}")
        if prediccion == silabica:
            aciertos += 1

    # Calcular aciertos en el resto del set sin imprimir
    for _, silabica in palabras_train[10:]:
        palabra_plana = silabica.replace('-', '')
        if modelo.predecir(palabra_plana) == silabica:
            aciertos += 1
            
    print("-" * 60)
    precision = (aciertos / total) * 100 if total > 0 else 0
    print(f"\n🎯 Resumen Final: Aciertos: {aciertos} de {total} | Precisión: {precision:.2f}%")
    print("=" * 60)


if __name__ == "__main__":
    main()