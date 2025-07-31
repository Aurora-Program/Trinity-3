from dataclasses import dataclass
from typing import List, Optional
import numpy as np
from allcode3new import Evolver, FractalTensor, FractalKnowledgeBase, Extender



'''
Muy importante:
No borrar estos comentarios.
Los vectores de los tensores fractales deber tener siempre una dimension que determinaw la forma otra que determina la estructura y una tercera que determina la función.


'''

# Semantic schema for letters
EJE = {"FONETICA": 1, "GRAFEMICA": 2, "SISTEMICA": 3}
PUNTO_ART = {"Labial": 1, "Dental": 2, "Alveolar": 3, "Palatal": 4, "Velar": 5, "Vocálico": 6}
MODO_ART = {"Oclusivo": 1, "Fricativo": 2, "Nasal": 3, "Vibrante Simple": 4, "Vibrante Múltiple": 5, "Lateral": 6, "Vocálico": 7, "Silente": 8}
SONORIDAD = {"Sonoro": 1, "Sordo": 2, "Vocálico": 3, "Silente": 4}
SIMETRIA = {"Vertical": 1, "Horizontal": 2, "Rotacional": 3, "Asimétrica": 4}
TRAZOS = {"Rectos": 1, "Curvos": 2, "Mixtos": 3}
CAJA_TIPO = {"Ascendente": 1, "Descendente": 2, "Caja-X": 3, "Completa": 4}
FRECUENCIA = {"Muy Alta": 1, "Alta": 2, "Media": 3, "Baja": 4, "Muy Baja": 5}
MODIFICACION = {"Base": 1, "Modificable": 2, "Componente Dígrafo": 3, "Silente": 4}

# Dictionary from user input
diccionario_letras_fractales = {
}

# --- Asignar un vector raíz único a cada letra (one-hot simple para 29 letras) ---
_letras = list(diccionario_letras_fractales.keys())
for idx, letra in enumerate(_letras):
    for variante in diccionario_letras_fractales[letra]:
        root = [0, 0, 0]
        # Codificación simple: idx en binario en 3 bits (mod 8)
        code = idx % 8
        root[0] = (code >> 2) & 1
        root[1] = (code >> 1) & 1
        root[2] = code & 1
        variante["tensor"][0] = root

VOCALES = "AEIOUÁÉÍÓÚÜaeiouáéíóúü"
def vector_fractal_completo(letra, siguiente_letra=None):
    """Obtiene el vector fractal de 3 dimensiones para una letra, usando el esquema semántico."""
    entries = diccionario_letras_fractales.get(letra.upper(), [{"tensor": [[1,2,3], [0, 0, 0], [0, 0, 0], [0, 0]]}])
    tensor = entries[0]["tensor"]
    if letra.upper() in ['C', 'G', 'R', 'W', 'Y'] and siguiente_letra:
        if letra.upper() == 'C' and siguiente_letra.upper() in 'EI':
            tensor = entries[1]["tensor"]
        elif letra.upper() == 'G' and siguiente_letra.upper() in 'EI':
            tensor = entries[1]["tensor"]
        elif letra.upper() == 'R':
            tensor = entries[1]["tensor"] if siguiente_letra else entries[0]["tensor"]
        elif letra.upper() == 'Y' and siguiente_letra.upper() not in VOCALES:
            tensor = entries[0]["tensor"]
    fonetica = tensor[1]

# --- 0. Utilidades base (solo codificación dominio) ---
def letra_a_tensor(letra: str) -> FractalTensor:
    # Mapea la letra a su tensor fractal según el diccionario, siguiendo el principio de forma, estructura y función
    entrada = diccionario_letras_fractales.get(letra.upper(), [{"tensor": [[1,2,3], [0,0,0], [0,0,0], [0,0]]}])
    tensor = entrada[0]["tensor"]
    # El tensor raíz debe tener al menos 3 vectores: forma, estructura, función
    # Si hay un cuarto vector (frecuencia/modificación), se ignora para el FractalTensor principal
    nivel_3 = [tensor[0], tensor[1], tensor[2]]
    return FractalTensor(nivel_3=nivel_3)

def silaba_a_ss(silaba: str) -> tuple:
    # Mapeo inyectivo: usa el valor ASCII de los primeros tres caracteres (rellena con '-')
    s = silaba.upper().ljust(3, '-')
    return (ord(s[0]), ord(s[1]), ord(s[2]))

from dataclasses import dataclass


@dataclass
class CorpusItem:
    silaba: str
    letras: tuple

# --- 1. Generar tensores-letra ---
def build_letter_trios(corpus):
    trios = []
    for item in corpus:
        tensores = [letra_a_tensor(l) for l in item.letras]
        while len(tensores) < 3:
            tensores.append(FractalTensor.neutral())
        trios.append(tensores[:3])
    return trios

# Corpus global para importar
corpus = [
    CorpusItem(silaba="PA", letras=("P", "A", "-")),
    CorpusItem(silaba="LA", letras=("L", "A", "-")),
    CorpusItem(silaba="BRA", letras=("B", "R", "A")),
    CorpusItem(silaba="CA", letras=("C", "A", "-")),
    CorpusItem(silaba="SA", letras=("S", "A", "-")),
    CorpusItem(silaba="TA", letras=("T", "A", "-")),
    CorpusItem(silaba="RA", letras=("R", "A", "-")),
    CorpusItem(silaba="MA", letras=("M", "A", "-")),
    CorpusItem(silaba="NA", letras=("N", "A", "-")),
    CorpusItem(silaba="DA", letras=("D", "A", "-")),
    CorpusItem(silaba="FA", letras=("F", "A", "-")),
    CorpusItem(silaba="VA", letras=("V", "A", "-")),
    CorpusItem(silaba="GA", letras=("G", "A", "-")),
    CorpusItem(silaba="CHA", letras=("C", "H", "A")),
    CorpusItem(silaba="PLA", letras=("P", "L", "A")),
    CorpusItem(silaba="TRA", letras=("T", "R", "A")),
    CorpusItem(silaba="FRA", letras=("F", "R", "A")),
    CorpusItem(silaba="GRA", letras=("G", "R", "A")),
    CorpusItem(silaba="CRA", letras=("C", "R", "A")),
    CorpusItem(silaba="STA", letras=("S", "T", "A")),
    CorpusItem(silaba="SPA", letras=("S", "P", "A")),
    CorpusItem(silaba="BRA", letras=("B", "R", "A")),
    CorpusItem(silaba="PRA", letras=("P", "R", "A")),
    CorpusItem(silaba="DRA", letras=("D", "R", "A")),
    CorpusItem(silaba="ZRA", letras=("Z", "R", "A")),
    CorpusItem(silaba="CLA", letras=("C", "L", "A")),
    CorpusItem(silaba="GLA", letras=("G", "L", "A")),
    CorpusItem(silaba="BLA", letras=("B", "L", "A")),
    CorpusItem(silaba="FLA", letras=("F", "L", "A")),
    CorpusItem(silaba="SLA", letras=("S", "L", "A")),
]



 # --- LUT inversa global para decodificar tensores a sílabas ---
tensor2silaba = {}

# --- 2. Síntesis fractal y dinámica ---
def build_syllable_tensors(trios, evolver):
    syllable_records = []
    for trio in trios:
        res = evolver.base_transcender.compute_vector_trio(
            trio[0].nivel_3[0],
            trio[1].nivel_3[0],
            trio[2].nivel_3[0])
        Ms_emerg = res['M_emergent']
        dyn_ft = evolver.analyze_fractal_dynamics(trio)
        syllable_records.append((Ms_emerg, dyn_ft))
    return syllable_records

# --- 3. Entrenar Extender ---
def train_extender(syllable_records, corpus, kb, extender, space='silabas'):
    data_lut = []
    for (Ms_emerg, dyn_ft), item in zip(syllable_records, corpus):
        Ss = silaba_a_ss(item.silaba)
        kb.add_archetype(space, f"{item.silaba}", dyn_ft, Ss=Ss)
        data_lut.append((Ss, dyn_ft))
        # ---- Guarda el mapeo de dyn_ft a la sílaba ----
        tensor2silaba[str(dyn_ft)] = item.silaba
    extender.learn_lut_from_data(space, data_lut)

# --- Decodificador de tensor a sílaba ---
def tensor_a_silaba(tensor):
    return tensor2silaba.get(str(tensor), "?")

# --- 4. Inferencia ---

# --- 4. Inferencia con validación de palabra ---
PALABRAS_VALIDAS = set([
    'PALABRA', 'CASA', 'PLANTA', 'GRANADA', 'FRAGUA', 'TRANCA', 'CLAVO', 'DRAMA', 'FRASE', 'MANTA',
    'SALVA', 'CRANEO', 'BLANCA', 'FLAMA', 'SALA',
    # Puedes ampliar este set según tu corpus
])

def silabificar(texto, extender, space='silabas'):
    predicciones = []
    for sil in texto.split('-'):
        Ss = silaba_a_ss(sil)
        out = extender.extend_fractal(Ss, {'space_id': space})
        predicciones.append(out['reconstructed_tensor'])
    return predicciones

def validar_palabra(silabas, predicciones):
    # Decodifica cada tensor a string antes de unir
    palabra_reconstruida = ''.join(tensor_a_silaba(p) for p in predicciones)
    if palabra_reconstruida.upper() in PALABRAS_VALIDAS:
        return palabra_reconstruida, True
    else:
        print(f"[Aprendizaje] Nuevo arquetipo registrado: {palabra_reconstruida}")
        return palabra_reconstruida, False


if __name__ == "__main__":
    # Inicializar core
    evolver = Evolver()
    kb = FractalKnowledgeBase()
    extender = Extender(knowledge_base=kb)

    # 1. Mostrar corpus
    print("=== CORPUS DE ENTRENAMIENTO ===")
    for item in corpus:
        print(f"Sílaba: {item.silaba:5}  Letras: {', '.join(item.letras)}")


    # 2. Generar trios de tensores
    trios = build_letter_trios(corpus)

    # --- DEBUG: Mostrar tensores de letras por sílaba ---
    print("\n--- Tensores de letras por sílaba ---")
    for i, trio in enumerate(trios):
        print(f"{corpus[i].silaba}: {[t.nivel_3 for t in trio]}")

    # 3. Síntesis fractal y dinámica
    syllable_records = build_syllable_tensors(trios, evolver)

    # --- DEBUG: Mostrar tensores dinámicos por sílaba ---
    print("\n--- Tensores dinámicos por sílaba ---")
    for i, (Ms_emerg, dyn_ft) in enumerate(syllable_records):
        print(f"{corpus[i].silaba}: Ms_emerg={Ms_emerg}, dyn_ft={dyn_ft}")

    # 4. Entrenar Extender
    train_extender(syllable_records, corpus, kb, extender)
    print("\nExtender entrenado.\n")

    # 5. Inferencia de ejemplo
    pruebas = [
        # Pruebas de sílabas
        "PA-LA-BRA",
        "CA-SA-TA",
        "TRA-PLA-FRA",
        "STA-SPA-CLA",
        "BRA-DRA-ZRA",
        "GLA-BLA-FLA",
        "SLA-NA-MA",
        "CHA-GRA-CRA",
        "FA-VA-GA",
        "RA-DA-PA",
        # Pruebas de palabras completas
        "PALABRA",
        "CASA",
        "PLANTA",
        "GRANADA",
        "FRAGUA",
        "TRANCA",
        "CLAVO",
        "DRAMA",
        "FRASE",
        "MANTA",
        "SALVA",
        "CRANEO",
        "BLANCA",
        "FLAMA",
        "SALA",
    ]
    aciertos = 0
    total = 0
    resumen = []
    for texto_prueba in pruebas:
        print(f"=== INFERENCIA para '{texto_prueba}' ===")
        # Si la palabra no tiene guiones, la segmentamos en sílabas de 2-3 letras (simple, para demo)
        if '-' not in texto_prueba:
            # Segmentación simple: cada 2 o 3 letras (mejorable con un silabificador real)
            silabas = []
            i = 0
            while i < len(texto_prueba):
                if i+3 <= len(texto_prueba):
                    silabas.append(texto_prueba[i:i+3])
                    i += 3
                else:
                    silabas.append(texto_prueba[i:i+2])
                    i += 2
            texto_segmentado = '-'.join(silabas)
        else:
            texto_segmentado = texto_prueba
            silabas = texto_prueba.split('-')
        resultado = silabificar(texto_segmentado, extender)
        for sil, pred in zip(silabas, resultado):
            decod = tensor_a_silaba(pred)
            print(f"Sílaba: {sil:8}  →  Inferencia: {pred}")
            if hasattr(pred, 'nivel_3'):
                print(f"    [Tokenizado/root vector]: {pred.nivel_3}")
            else:
                print(f"    [Tokenizado/root vector]: {pred}")
            print(f"    [Sílaba decodificada]: {decod}")
        # Validación a nivel palabra
        palabra_reconstruida, es_valida = validar_palabra(silabas, resultado)
        if es_valida:
            print(f"→ PALABRA RECONSTRUIDA: {palabra_reconstruida} (válida)")
            aciertos += 1
        else:
            print(f"→ PALABRA RECONSTRUIDA: {palabra_reconstruida} (NO reconocida)")
        total += 1
        resumen.append({
            'entrada': texto_prueba,
            'reconstruida': palabra_reconstruida,
            'acierto': es_valida
        })
        print()
    print(f"Resumen de aciertos: {aciertos} de {total} ({(aciertos/total*100):.1f}%) palabras reconocidas.")
    print("\nResumen detallado:")
    for r in resumen:
        estado = '✔' if r['acierto'] else '✗'
        print(f"Entrada: {r['entrada']:12} | Reconstruida: {r['reconstruida']:12} | {'Acierto' if r['acierto'] else 'NO'} {estado}")
        print(f"Entrada: {r['entrada']:12} | Reconstruida: {r['reconstruida']:12} | {'Acierto' if r['acierto'] else 'NO'} {estado}")
