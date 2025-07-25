# ===============================================================================
# LEYENDA DEL ESQUEMA FRACTAL PARA LETRAS (VERSIÓN EXTENDIDA)
# ===============================================================================

# --- NIVEL 1: EJES PRINCIPALES ---
EJE = { "FONETICA": 1, "GRAFEMICA": 2, "SISTEMICA": 3 }

# --- NIVEL 2: SUBDIMENSIONES (AMPLIADAS PARA CUBRIR TODO EL ALFABETO) ---
# 1. Articulación Fonética
PUNTO_ART = {"Labial": 1, "Dental": 2, "Alveolar": 3, "Palatal": 4, "Velar": 5, "Vocálico": 6}
MODO_ART = {"Oclusivo": 1, "Fricativo": 2, "Nasal": 3, "Vibrante Simple": 4, "Vibrante Múltiple": 5, "Lateral": 6, "Vocálico": 7, "Silente": 8}
SONORIDAD = {"Sonoro": 1, "Sordo": 2, "Vocálico": 3, "Silente": 4}

# 2. Estructura Grafémica
SIMETRIA = {"Vertical": 1, "Horizontal": 2, "Rotacional": 3, "Asimétrica": 4}
TRAZOS = {"Rectos": 1, "Curvos": 2, "Mixtos": 3}
CAJA_TIPO = {"Ascendente": 1, "Descendente": 2, "Caja-X": 3, "Completa": 4}

# 3. Función Sistémica
FRECUENCIA = {"Muy Alta": 1, "Alta": 2, "Media": 3, "Baja": 4, "Muy Baja": 5}
MODIFICACION = {"Base": 1, "Modificable": 2, "Componente Dígrafo": 3, "Silente": 4}

# ===============================================================================
# DICCIONARIO FRACTAL COMPLETO
# ===============================================================================

diccionario_letras_fractales = {
    '-': [{
        "descripcion": "Separador de sílaba",
        "tensor": [
            [1,2,3],
            [0, 0, 0],  # Sin articulación fonética
            [SIMETRIA["Horizontal"], TRAZOS["Rectos"], CAJA_TIPO["Completa"]],
            [FRECUENCIA["Muy Baja"], MODIFICACION["Base"]]
        ]
    }],
    # --- VOCALES ---
    'A': [{"descripcion": "Vocal abierta", "tensor": [[1,2,3], [PUNTO_ART["Vocálico"], MODO_ART["Vocálico"], SONORIDAD["Vocálico"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Muy Alta"], MODIFICACION["Modificable"]]]}],
    'E': [{"descripcion": "Vocal media anterior", "tensor": [[1,2,3], [PUNTO_ART["Vocálico"], MODO_ART["Vocálico"], SONORIDAD["Vocálico"]], [SIMETRIA["Horizontal"], TRAZOS["Mixtos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Muy Alta"], MODIFICACION["Modificable"]]]}],
    'I': [{"descripcion": "Vocal cerrada anterior", "tensor": [[1,2,3], [PUNTO_ART["Vocálico"], MODO_ART["Vocálico"], SONORIDAD["Vocálico"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Modificable"]]]}],
    'O': [{"descripcion": "Vocal media posterior", "tensor": [[1,2,3], [PUNTO_ART["Vocálico"], MODO_ART["Vocálico"], SONORIDAD["Vocálico"]], [SIMETRIA["Horizontal"], TRAZOS["Curvos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Muy Alta"], MODIFICACION["Modificable"]]]}],
    'U': [{"descripcion": "Vocal cerrada posterior", "tensor": [[1,2,3], [PUNTO_ART["Vocálico"], MODO_ART["Vocálico"], SONORIDAD["Vocálico"]], [SIMETRIA["Vertical"], TRAZOS["Curvos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Media"], MODIFICACION["Modificable"]]]}],

    # --- CONSONANTES ---
    'B': [{"descripcion": "Oclusiva bilabial sonora", "tensor": [[1,2,3], [PUNTO_ART["Labial"], MODO_ART["Oclusivo"], SONORIDAD["Sonoro"]], [SIMETRIA["Horizontal"], TRAZOS["Mixtos"], CAJA_TIPO["Ascendente"]], [FRECUENCIA["Media"], MODIFICACION["Base"]]]}],
    'C': [
        {"descripcion": "Sonido /k/ (casa)", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Oclusivo"], SONORIDAD["Sordo"]], [SIMETRIA["Horizontal"], TRAZOS["Curvos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Componente Dígrafo"]]]},
        {"descripcion": "Sonido /s/ (cielo)", "tensor": [[1,2,3], [PUNTO_ART["Dental"], MODO_ART["Fricativo"], SONORIDAD["Sordo"]], [SIMETRIA["Horizontal"], TRAZOS["Curvos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Componente Dígrafo"]]]}
    ],
    'D': [{"descripcion": "Oclusiva dental sonora", "tensor": [[1,2,3], [PUNTO_ART["Dental"], MODO_ART["Oclusivo"], SONORIDAD["Sonoro"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Ascendente"]], [FRECUENCIA["Alta"], MODIFICACION["Base"]]]}],
    'F': [{"descripcion": "Fricativa labiodental sorda", "tensor": [[1,2,3], [PUNTO_ART["Labial"], MODO_ART["Fricativo"], SONORIDAD["Sordo"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Completa"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]}],
    'G': [
        {"descripcion": "Sonido /g/ (gato)", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Oclusivo"], SONORIDAD["Sonoro"]], [SIMETRIA["Asimétrica"], TRAZOS["Curvos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Media"], MODIFICACION["Base"]]]},
        {"descripcion": "Sonido /x/ (gente)", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Fricativo"], SONORIDAD["Sordo"]], [SIMETRIA["Asimétrica"], TRAZOS["Curvos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Media"], MODIFICACION["Base"]]]}
    ],
    'H': [{"descripcion": "Letra silente", "tensor": [[1,2,3], [0, MODO_ART["Silente"], SONORIDAD["Silente"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Ascendente"]], [FRECUENCIA["Media"], MODIFICACION["Silente"]]]}],
    'J': [{"descripcion": "Fricativa velar sorda", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Fricativo"], SONORIDAD["Sordo"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]}],
    'K': [{"descripcion": "Oclusiva velar sorda (extranjerismos)", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Oclusivo"], SONORIDAD["Sordo"]], [SIMETRIA["Asimétrica"], TRAZOS["Rectos"], CAJA_TIPO["Ascendente"]], [FRECUENCIA["Muy Baja"], MODIFICACION["Base"]]]}],
    'L': [{"descripcion": "Lateral alveolar sonora", "tensor": [[1,2,3], [PUNTO_ART["Alveolar"], MODO_ART["Lateral"], SONORIDAD["Sonoro"]], [SIMETRIA["Asimétrica"], TRAZOS["Rectos"], CAJA_TIPO["Ascendente"]], [FRECUENCIA["Alta"], MODIFICACION["Componente Dígrafo"]]]}],
    'M': [{"descripcion": "Nasal bilabial sonora", "tensor": [[1,2,3], [PUNTO_ART["Labial"], MODO_ART["Nasal"], SONORIDAD["Sonoro"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Base"]]]}],
    'N': [{"descripcion": "Nasal alveolar sonora", "tensor": [[1,2,3], [PUNTO_ART["Alveolar"], MODO_ART["Nasal"], SONORIDAD["Sonoro"]], [SIMETRIA["Rotacional"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Base"]]]}],
    'Ñ': [{"descripcion": "Nasal palatal sonora", "tensor": [[1,2,3], [PUNTO_ART["Palatal"], MODO_ART["Nasal"], SONORIDAD["Sonoro"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]}],
    'P': [{"descripcion": "Oclusiva bilabial sorda", "tensor": [[1,2,3], [PUNTO_ART["Labial"], MODO_ART["Oclusivo"], SONORIDAD["Sordo"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Alta"], MODIFICACION["Base"]]]}],
    'Q': [{"descripcion": "Oclusiva velar sorda (en 'que','qui')", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Oclusivo"], SONORIDAD["Sordo"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Media"], MODIFICACION["Base"]]]}],
    'R': [
        {"descripcion": "Vibrante simple (caro)", "tensor": [[1,2,3], [PUNTO_ART["Alveolar"], MODO_ART["Vibrante Simple"], SONORIDAD["Sonoro"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Componente Dígrafo"]]]},
        {"descripcion": "Vibrante múltiple (carro, rosa)", "tensor": [[1,2,3], [PUNTO_ART["Alveolar"], MODO_ART["Vibrante Múltiple"], SONORIDAD["Sonoro"]], [SIMETRIA["Asimétrica"], TRAZOS["Mixtos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Alta"], MODIFICACION["Componente Dígrafo"]]]}
    ],
    'S': [{"descripcion": "Fricativa alveolar sorda", "tensor": [[1,2,3], [PUNTO_ART["Alveolar"], MODO_ART["Fricativo"], SONORIDAD["Sordo"]], [SIMETRIA["Rotacional"], TRAZOS["Curvos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Muy Alta"], MODIFICACION["Base"]]]}],
    'T': [{"descripcion": "Oclusiva dental sorda", "tensor": [[1,2,3], [PUNTO_ART["Dental"], MODO_ART["Oclusivo"], SONORIDAD["Sordo"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Ascendente"]], [FRECUENCIA["Alta"], MODIFICACION["Base"]]]}],
    'V': [{"descripcion": "Oclusiva bilabial sonora (igual que B)", "tensor": [[1,2,3], [PUNTO_ART["Labial"], MODO_ART["Oclusivo"], SONORIDAD["Sonoro"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Media"], MODIFICACION["Base"]]]}],
    'W': [
        {"descripcion": "Sonido /b/ (wolframio)", "tensor": [[1,2,3], [PUNTO_ART["Labial"], MODO_ART["Oclusivo"], SONORIDAD["Sonoro"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Muy Baja"], MODIFICACION["Base"]]]},
        {"descripcion": "Sonido /gu/ (whisky)", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Oclusivo"], SONORIDAD["Sonoro"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Muy Baja"], MODIFICACION["Base"]]]}
    ],
    'X': [{"descripcion": "Sonido /ks/", "tensor": [[1,2,3], [PUNTO_ART["Velar"], MODO_ART["Oclusivo"], SONORIDAD["Sordo"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]}],
    'Y': [
        {"descripcion": "Sonido consonántico (yema)", "tensor": [[1,2,3], [PUNTO_ART["Palatal"], MODO_ART["Fricativo"], SONORIDAD["Sonoro"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]},
        {"descripcion": "Sonido vocálico (ley)", "tensor": [[1,2,3], [PUNTO_ART["Vocálico"], MODO_ART["Vocálico"], SONORIDAD["Vocálico"]], [SIMETRIA["Vertical"], TRAZOS["Rectos"], CAJA_TIPO["Descendente"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]}
    ],
    'Z': [{"descripcion": "Fricativa dental sorda (sonido /s/ en LatAm)", "tensor": [[1,2,3], [PUNTO_ART["Dental"], MODO_ART["Fricativo"], SONORIDAD["Sordo"]], [SIMETRIA["Rotacional"], TRAZOS["Rectos"], CAJA_TIPO["Caja-X"]], [FRECUENCIA["Baja"], MODIFICACION["Base"]]]}],





}

# Puedes probarlo con la función del ejemplo anterior:
# mostrar_info_letra('G')
# mostrar_info_letra('R')