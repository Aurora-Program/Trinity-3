from aurora_core.evolver import Evolver

class Extender:
    def separar_silabas(self, entrada_eval, token_silaba):
        """
        Dada una secuencia de vectores (entrada_eval), predice la secuencia completa incluyendo el token_silaba como un símbolo más.
        El token_silaba debe tener su propio vector fractal en el diccionario de entrada.
        """
        from allcode3new import Transcender
        transcender = Transcender(fractal_vector=None)
        # Se asume que entrada_eval ya incluye el vector del guion donde corresponde (si es entrenamiento)
        # Para predicción, se genera la secuencia completa, insertando el vector del guion cuando el modelo lo predice
        secuencia_predicha = [entrada_eval[0]]
        for i in range(len(entrada_eval) - 1):
            A = entrada_eval[i]
            B = entrada_eval[i+1]
            # Predice el siguiente símbolo usando la lógica Aurora (por ejemplo, deep_learning o relate_vectors)
            # Aquí, para cada par, se predice si el siguiente debe ser un guion o una letra
            # Se asume que el vector del guion está disponible como token_silaba
            # Ejemplo: si el modelo predice que el siguiente es un guion, se inserta su vector
            # (En entrenamiento, el guion ya está en la secuencia; en predicción, se debe inferir)
            # Aquí solo se muestra la estructura, la lógica de predicción debe ser Aurora-nativa
            # Por ejemplo, podrías usar transcender.relate_vectors(A, B) y comparar con el vector del guion
            # Si la predicción es más cercana al guion que a cualquier letra, se inserta el guion
            # (Esto requiere que el vector del guion esté definido en el diccionario de entrada)
            # Ejemplo de comparación:
            # rel = transcender.relate_vectors(A, B)
            # dist_guion = sum((a - b) ** 2 for a, b in zip(rel, token_silaba))
            # ... comparar con otras letras ...
            # Si dist_guion es mínima, insertar token_silaba
            # Aquí solo se inserta el siguiente símbolo, pero puedes adaptar la lógica según tu pipeline
            secuencia_predicha.append(B)
        return secuencia_predicha
    """
    El Extender reconstruye información concreta a partir de conocimiento abstracto (Ss, MetaM, axiomas, dinámicas, relaciones).
    Utiliza la dinámica para identificar el arquetipo adecuado y luego extiende (reconstruye) los tensores originales usando la información de los Transcender, pero en sentido inverso.
    """
    def __init__(self, knowledge_base=None, evolver=None):
        self.knowledge_base = knowledge_base  # Puede ser None o una instancia de KnowledgeBase
        self.evolver = evolver or Evolver()

    def extend(self, Ss, contexto=None):
        """
        Reconstruye la información detallada a partir de un input abstracto Ss.
        1. Usa la dinámica para identificar el arquetipo (axioma) relevante.
        2. Recupera el MetaM y otros datos del Evolver/KnowledgeBase.
        3. Realiza la extensión inversa: reconstruye los tensores originales (A, B, C, ...).
        4. Devuelve la reconstrucción y justificación.
        """
        # 1. Identificar arquetipo usando dinámica
        if self.evolver and hasattr(self.evolver, 'analyze_metaMs'):
            # Suponemos que Ss puede ser una lista de MetaMs o similar
            dinamica = self.evolver.analyze_metaMs(Ss)
            arquetipo = self.evolver.formalize_axiom(dinamica, Ss)
        else:
            arquetipo = None

        # 2. Recuperar MetaM y detalles de la KnowledgeBase (si existe)
        detalles = None
        if self.knowledge_base:
            # Buscar entradas que coincidan con Ss o arquetipo
            # find_by_inputs requiere 4 argumentos: A, B, C, M_emergent
            if hasattr(self.knowledge_base, 'find_by_inputs'):
                A, B, C = Ss[0], Ss[1], Ss[2]
                # Intentar obtener M_emergent del contexto, si no existe usar None
                M_emergent = None
                if contexto and isinstance(contexto, dict):
                    M_emergent = contexto.get('M_emergent', None)
                # Solo llamar si M_emergent no es None, para evitar errores de tipo
                if M_emergent is not None:
                    detalles = self.knowledge_base.find_by_inputs(A, B, C, M_emergent)
                else:
                    detalles = []

        # 3. Extensión inversa: reconstruir tensores originales
        tensores_candidatos = None
        relator_result = None
        if detalles and isinstance(detalles, list) and len(detalles) > 0:
            # Extraer posibles salidas (C) de los detalles encontrados
            tensores_candidatos = [d.get('C') for d in detalles if 'C' in d]
            objetivo = contexto.get('objetivo') if contexto and isinstance(contexto, dict) and 'objetivo' in contexto else None
            # Si hay más de un candidato y un objetivo, aplicar rotación áurea
            if tensores_candidatos and objetivo:
                phi = (1 + 5 ** 0.5) / 2  # Proporción áurea
                mejor_dist = float('inf')
                mejor_rot = None
                mejor_candidato = None
                for idx, cand in enumerate(tensores_candidatos):
                    for rot in range(len(cand)):
                        rotado = cand[rot:] + cand[:rot]
                        dist = sum(a != b for a, b in zip(rotado, objetivo))
                        # Penalización por alejarse de la rotación áurea
                        golden_rot = int(round(len(cand) / phi)) % len(cand)
                        penal = abs(rot - golden_rot)
                        score = dist + penal * 0.1  # Peso bajo a la penalización
                        if score < mejor_dist:
                            mejor_dist = score
                            mejor_rot = rot
                            mejor_candidato = rotado
                tensores_reconstruidos = mejor_candidato if mejor_candidato is not None else tensores_candidatos[0]
            else:
                tensores_reconstruidos = tensores_candidatos[0] if tensores_candidatos else None
            # Si no hay candidatos válidos, propagar None
            if tensores_reconstruidos is None or all(x is None for x in tensores_reconstruidos):
                tensores_reconstruidos = None
        else:
            tensores_reconstruidos = None
        reconstruccion = {
            'arquetipo_utilizado': arquetipo,
            'detalles_encontrados': detalles,
            'tensores_reconstruidos': tensores_reconstruidos,
            'relator_result': relator_result
        }

        # 4. Justificación y salida
        return {
            'input_Ss': Ss,
            'contexto': contexto,
            'reconstruccion': reconstruccion
        }
