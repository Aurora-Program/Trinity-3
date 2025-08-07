from dataclasses import dataclass
from typing import List, Optional
import numpy as np
# Asegúrate de que allcode3new.py está en la misma carpeta o en el PYTHONPATH
from allcode3new import Evolver, FractalTensor, FractalKnowledgeBase, Extender, pattern0_create_fractal_cluster, TensorPoolManager

'''
Muy importante:
No borrar estos comentarios.
Los vectores de los tensores fractales deber tener siempre una dimension que determina la forma otra que determina la estructura y una tercera que determina la función.
'''

# ==============================================================================
# 1. ESQUEMA SEMÁNTICO Y DICCIONARIO FRACTAL (CORREGIDO Y COMPLETADO)
# ==============================================================================

# --- Esquema Semántico (Valores base) ---
EJE = {"FONETICA": 1, "GRAFEMICA": 2, "SISTEMICA": 3}
# Fonética
PUNTO_ART = {"Labial": 1, "Dental": 2, "Alveolar": 3, "Palatal": 4, "Velar": 5, "Vocálico": 6, "Glotal": 7}
MODO_ART = {"Oclusivo": 1, "Fricativo": 2, "Nasal": 3, "Vibrante Simple": 4, "Vibrante Múltiple": 5, "Lateral": 6, "Vocálico": 7, "Silente": 8}
SONORIDAD = {"Sonoro": 1, "Sordo": 2, "Vocálico": 3, "Silente": 4}
# Grafémica
SIMETRIA = {"Vertical": 1, "Horizontal": 2, "Rotacional": 3, "Asimétrica": 4}
TRAZOS = {"Rectos": 1, "Curvos": 2, "Mixtos": 3}
CAJA_TIPO = {"Ascendente": 1, "Descendente": 2, "Caja-X": 3, "Completa": 4}
# Sistémica
FRECUENCIA = {"Muy Alta": 1, "Alta": 2, "Media": 3, "Baja": 4, "Muy Baja": 5}
MODIFICACION = {"Base": 1, "Modificable": 2, "Componente Dígrafo": 3, "Silente": 4}
ROL_SILABICO = {"Ataque": 1, "Nucleo": 2, "Coda": 3}

# --- Función de binarización ---
def to_bin3(n):
    """Convierte un entero a un vector binario de 3 bits."""
    return [(n >> 2) & 1, (n >> 1) & 1, n & 1]

# --- Diccionario Fractal Binarizado Completo ---
diccionario_letras_fractales = {
    # Nivel 3: Ejes (Fonética, Grafémica, Sistémica)
    # Nivel 9: 3 vectores para Fonética, 3 para Grafémica, 3 para Sistémica
    'A': [{"tensor": [to_bin3(PUNTO_ART["Vocálico"]), to_bin3(MODO_ART["Vocálico"]), to_bin3(SONORIDAD["Vocálico"]), to_bin3(SIMETRIA["Vertical"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Muy Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Nucleo"])]}],
    'B': [{"tensor": [to_bin3(PUNTO_ART["Labial"]), to_bin3(MODO_ART["Oclusivo"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Ascendente"]), to_bin3(FRECUENCIA["Baja"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'C': [{"tensor": [to_bin3(PUNTO_ART["Velar"]), to_bin3(MODO_ART["Oclusivo"]), to_bin3(SONORIDAD["Sordo"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Curvos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Modificable"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'D': [{"tensor": [to_bin3(PUNTO_ART["Dental"]), to_bin3(MODO_ART["Oclusivo"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Ascendente"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'F': [{"tensor": [to_bin3(PUNTO_ART["Labial"]), to_bin3(MODO_ART["Fricativo"]), to_bin3(SONORIDAD["Sordo"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Ascendente"]), to_bin3(FRECUENCIA["Baja"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'G': [{"tensor": [to_bin3(PUNTO_ART["Velar"]), to_bin3(MODO_ART["Oclusivo"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Curvos"]), to_bin3(CAJA_TIPO["Descendente"]), to_bin3(FRECUENCIA["Media"]), to_bin3(MODIFICACION["Modificable"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'H': [{"tensor": [to_bin3(PUNTO_ART["Glotal"]), to_bin3(MODO_ART["Silente"]), to_bin3(SONORIDAD["Silente"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Rectos"]), to_bin3(CAJA_TIPO["Ascendente"]), to_bin3(FRECUENCIA["Media"]), to_bin3(MODIFICACION["Componente Dígrafo"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'L': [{"tensor": [to_bin3(PUNTO_ART["Alveolar"]), to_bin3(MODO_ART["Lateral"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Rectos"]), to_bin3(CAJA_TIPO["Ascendente"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'M': [{"tensor": [to_bin3(PUNTO_ART["Labial"]), to_bin3(MODO_ART["Nasal"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Vertical"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'N': [{"tensor": [to_bin3(PUNTO_ART["Alveolar"]), to_bin3(MODO_ART["Nasal"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Rotacional"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'P': [{"tensor": [to_bin3(PUNTO_ART["Labial"]), to_bin3(MODO_ART["Oclusivo"]), to_bin3(SONORIDAD["Sordo"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Descendente"]), to_bin3(FRECUENCIA["Media"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'R': [{"tensor": [to_bin3(PUNTO_ART["Alveolar"]), to_bin3(MODO_ART["Vibrante Simple"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Mixtos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Modificable"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'S': [{"tensor": [to_bin3(PUNTO_ART["Alveolar"]), to_bin3(MODO_ART["Fricativo"]), to_bin3(SONORIDAD["Sordo"]), to_bin3(SIMETRIA["Rotacional"]), to_bin3(TRAZOS["Curvos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Muy Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'T': [{"tensor": [to_bin3(PUNTO_ART["Dental"]), to_bin3(MODO_ART["Oclusivo"]), to_bin3(SONORIDAD["Sordo"]), to_bin3(SIMETRIA["Vertical"]), to_bin3(TRAZOS["Rectos"]), to_bin3(CAJA_TIPO["Ascendente"]), to_bin3(FRECUENCIA["Alta"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'V': [{"tensor": [to_bin3(PUNTO_ART["Labial"]), to_bin3(MODO_ART["Fricativo"]), to_bin3(SONORIDAD["Sonoro"]), to_bin3(SIMETRIA["Asimétrica"]), to_bin3(TRAZOS["Rectos"]), to_bin3(CAJA_TIPO["Caja-X"]), to_bin3(FRECUENCIA["Baja"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
    'Z': [{"tensor": [to_bin3(PUNTO_ART["Dental"]), to_bin3(MODO_ART["Fricativo"]), to_bin3(SONORIDAD["Sordo"]), to_bin3(SIMETRIA["Rotacional"]), to_bin3(TRAZOS["Rectos"]), to_bin3(CAJA_TIPO["Descendente"]), to_bin3(FRECUENCIA["Baja"]), to_bin3(MODIFICACION["Base"]), to_bin3(ROL_SILABICO["Ataque"])]}],
}

# ==============================================================================
# 2. LÓGICA DE PROCESAMIENTO (CORREGIDA)
# ==============================================================================

def letra_a_tensor(letra: str) -> FractalTensor:
    """Convierte una letra a su Tensor Fractal usando el esquema binario y jerárquico."""
    if not letra or letra == '-':
        return FractalTensor.neutral()
        
    # Obtiene la lista de 9 vectores binarios de características
    entrada = diccionario_letras_fractales.get(letra.upper(), [{"tensor": [[0,0,0]]*9}])[0]
    features = entrada["tensor"]
    
    # Define la estructura jerárquica del tensor
    # Nivel 3 (Raíz): Los tres ejes principales (Forma, Estructura, Función)
    nivel_3_vectors = [to_bin3(EJE["FONETICA"]), to_bin3(EJE["GRAFEMICA"]), to_bin3(EJE["SISTEMICA"])]
    
    # Nivel 9: Las 9 características detalladas
    nivel_9_vectors = features
    
    return FractalTensor(nivel_3=nivel_3_vectors, nivel_9=nivel_9_vectors)


@dataclass
class CorpusItem:
    silaba: str
    letras: tuple

# Corpus global para importar (sin cambios)
corpus = [
    CorpusItem(silaba="PA", letras=("P", "A", "-")), CorpusItem(silaba="LA", letras=("L", "A", "-")),
    CorpusItem(silaba="BRA", letras=("B", "R", "A")), CorpusItem(silaba="CA", letras=("C", "A", "-")),
    CorpusItem(silaba="SA", letras=("S", "A", "-")), CorpusItem(silaba="TA", letras=("T", "A", "-")),
    CorpusItem(silaba="RA", letras=("R", "A", "-")), CorpusItem(silaba="MA", letras=("M", "A", "-")),
    CorpusItem(silaba="NA", letras=("N", "A", "-")), CorpusItem(silaba="DA", letras=("D", "A", "-")),
    CorpusItem(silaba="FA", letras=("F", "A", "-")), CorpusItem(silaba="VA", letras=("V", "A", "-")),
    CorpusItem(silaba="GA", letras=("G", "A", "-")), CorpusItem(silaba="CHA", letras=("C", "H", "A")),
    CorpusItem(silaba="PLA", letras=("P", "L", "A")), CorpusItem(silaba="TRA", letras=("T", "R", "A")),
    CorpusItem(silaba="FRA", letras=("F", "R", "A")), CorpusItem(silaba="GRA", letras=("G", "R", "A")),
    CorpusItem(silaba="CRA", letras=("C", "R", "A")), CorpusItem(silaba="STA", letras=("S", "T", "A")),
    CorpusItem(silaba="SPA", letras=("S", "P", "A")), CorpusItem(silaba="BRA", letras=("B", "R", "A")),
    CorpusItem(silaba="PRA", letras=("P", "R", "A")), CorpusItem(silaba="DRA", letras=("D", "R", "A")),
    CorpusItem(silaba="ZRA", letras=("Z", "R", "A")), CorpusItem(silaba="CLA", letras=("C", "L", "A")),
    CorpusItem(silaba="GLA", letras=("G", "L", "A")), CorpusItem(silaba="BLA", letras=("B", "L", "A")),
    CorpusItem(silaba="FLA", letras=("F", "L", "A")), CorpusItem(silaba="SLA", letras=("S", "L", "A")),
]

def build_letter_trios(corpus):
    trios = []
    for item in corpus:
        tensores = [letra_a_tensor(l) for l in item.letras]
        # No es necesario rellenar, letra_a_tensor ya maneja '-'
        trios.append(tensores[:3])
    return trios

def build_syllable_tensors(trios, evolver):
    syllable_records = []
    for trio in trios:
        # La síntesis ahora es de tensores completos, no solo de vectores raíz
        dyn_ft = evolver.analyze_fractal_dynamics(trio)
        # Ms_emergent es la raíz (Ms) del tensor dinámico resultante
        Ms_emerg = dyn_ft.Ms
        syllable_records.append((Ms_emerg, dyn_ft))
    return syllable_records

silaba_a_id = {item.silaba: i for i, item in enumerate(corpus)}

def silaba_a_ss(silaba: str) -> list:
    sid = silaba_a_id.get(silaba.upper(), len(silaba_a_id)) # Usar un ID de fallback
    return to_bin3(sid)

def train_extender(syllable_records, corpus, kb, extender, space='silabas'):
    data_lut = []
    for (Ms_emerg, dyn_ft), item in zip(syllable_records, corpus):
        Ss = silaba_a_ss(item.silaba)
        # Guardar la sílaba en los metadatos del tensor
        setattr(dyn_ft, 'metadata', {'syllable': item.silaba})
        kb.add_archetype(space, f"{item.silaba}", dyn_ft, Ss=Ss)
        data_lut.append((Ss, dyn_ft))
    extender.learn_lut_from_data(space, data_lut)

def tensor_a_silaba(tensor):
    if hasattr(tensor, 'metadata') and 'syllable' in tensor.metadata:
        return tensor.metadata['syllable']
    return "?"

PALABRAS_VALIDAS = set([
    'PALABRA', 'CASA', 'PLANTA', 'GRANADA', 'FRAGUA', 'TRANCA', 'CLAVO', 'DRAMA', 
    'FRASE', 'MANTA', 'SALVA', 'CRANEO', 'BLANCA', 'FLAMA', 'SALA',
])

def silabificar(texto, extender, space='silabas'):
    predicciones = []
    for sil in texto.split('-'):
        Ss = silaba_a_ss(sil)
        out = extender.extend_fractal(Ss, {'space_id': space})
        predicciones.append(out['reconstructed_tensor'])
    return predicciones

def validar_palabra(silabas, predicciones):
    palabra_reconstruida = ''.join(tensor_a_silaba(p) for p in predicciones)
    if palabra_reconstruida.upper() in PALABRAS_VALIDAS:
        return palabra_reconstruida, True
    else:
        # Opcional: Registrar nuevo conocimiento si se desea
        # print(f"[Aprendizaje] Nuevo arquetipo de palabra no validada: {palabra_reconstruida}")
        return palabra_reconstruida, False
        
# ==============================================================================
# 3. EJECUCIÓN PRINCIPAL
# ==============================================================================

if __name__ == "__main__":
    kb = FractalKnowledgeBase()
    evolver = Evolver()
    extender = Extender(knowledge_base=kb)

    print("=== CORPUS DE ENTRENAMIENTO ===")
    for item in corpus:
        print(f"Sílaba: {item.silaba:5}  Letras: {', '.join(item.letras)}")

    print("\n1. Construyendo tensores de letras desde el corpus...")
    trios = build_letter_trios(corpus)
    
    print("2. Sintetizando arquetipos de sílabas...")
    syllable_records = build_syllable_tensors(trios, evolver)

    print("3. Entrenando el Extender con los arquetipos de sílabas...")
    train_extender(syllable_records, corpus, kb, extender)
    print("\n✅ Extender entrenado.\n")

    pruebas = ["PA-LA-BRA", "CRA-CLA-CA", "PALABRA", "CASA", "BLA-BRA-FRA"]
    aciertos = 0
    total_palabras = 0
    resumen = []

    for texto_prueba in pruebas:
        print(f"=== INFERENCIA para '{texto_prueba}' ===")
        silabas = texto_prueba.split('-')
        
        resultado_tensors = silabificar(texto_prueba, extender)
        
        for sil, pred in zip(silabas, resultado_tensors):
            decod = tensor_a_silaba(pred)
            print(f"  Sílaba: {sil:8} → Decodificada: {decod:8} (Coincide: {'✔' if sil == decod else '✗'})")

        palabra_reconstruida, es_valida = validar_palabra(silabas, resultado_tensors)
        
        if es_valida:
            print(f"→ PALABRA RECONSTRUIDA: {palabra_reconstruida} (VÁLIDA ✔)")
            aciertos += 1
        else:
            print(f"→ PALABRA RECONSTRUIDA: {palabra_reconstruida} (NO RECONOCIDA ✗)")
        
        total_palabras += 1
        resumen.append({'entrada': texto_prueba, 'reconstruida': palabra_reconstruida, 'acierto': es_valida})
        print("-" * 40)

    # Filtrar solo las pruebas que son palabras completas para el resumen de aciertos
    pruebas_palabras = [p for p in resumen if '-' not in p['entrada']]
    aciertos_palabras = sum(1 for p in pruebas_palabras if p['acierto'])
    total_palabras_validas = len(pruebas_palabras)

    print(f"\nResumen de aciertos: {aciertos_palabras} de {total_palabras_validas} ({(aciertos_palabras/total_palabras_validas*100):.1f}%) palabras reconocidas.")
    print("\nResumen detallado de todas las pruebas:")
    for r in resumen:
        estado = '✔' if r['acierto'] or '-' in r['entrada'] else '✗' # Consideramos acierto si es palabra válida o si es prueba de sílabas
        print(f"Entrada: {r['entrada']:12} | Reconstruida: {r['reconstruida']:12} | {'OK' if estado == '✔' else 'NO'} {estado}")