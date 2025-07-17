# ===============================================================================
# AURORA TRINITY-3 - ARQUITECTURA CANÓNICA COMPLETA Y UNIFICADA
# ===============================================================================

import random
import time
import hashlib
import warnings
import copy  # Added to fix NameError in reconstruct_full_fractal
from typing import List, Dict, Any, Tuple, Optional


# ===============================================================================
# NIVEL 1: LÓGICA FUNDAMENTAL
# ===============================================================================

class TernaryLogic:
    """
    Lógica ternaria Aurora con manejo correcto de incertidumbre.
    Implementa Honestidad Computacional propagando NULL apropiadamente.
    """
    NULL = None  # Representación canónica de NULL en Aurora

    @staticmethod
    def ternary_xor(a: Optional[int], b: Optional[int]) -> Optional[int]:
        """XOR ternario con propagación de NULL."""
        if a is TernaryLogic.NULL or b is TernaryLogic.NULL:
            return TernaryLogic.NULL
        # Asegurar que el resultado esté en el rango [0, 1] para compatibilidad binaria
        return (a ^ b) % 2

    @staticmethod
    def ternary_xnor(a: Optional[int], b: Optional[int]) -> Optional[int]:
        """XNOR ternario con propagación de NULL."""
        if a is TernaryLogic.NULL or b is TernaryLogic.NULL:
            return TernaryLogic.NULL
        return 1 if a == b else 0

# ===============================================================================
# NIVEL 2: COMPONENTES BÁSICOS DE PROCESAMIENTO
# ===============================================================================

class Trigate:
    """
    Trigate canónico Aurora. Implementa los tres modos operativos fundamentales
    con Honestidad Computacional, utilizando Look-Up Tables (LUTs) para optimización.
    """
    _LUT_INFER = {}
    _LUT_LEARN = {}
    _LUT_DEDUCE_A = {}
    _LUT_DEDUCE_B = {}

    def __init__(self):
        if not Trigate._LUT_INFER:
            self._build_ternary_luts()

    def _build_ternary_luts(self):
        # TODO: Evaluar generación dinámica de LUTs para evitar redundancias
        states = [0, 1, None]
        for a in states:
            for b in states:
                # Learn LUT
                for r in states:
                    m = None
                    if a is not None and b is not None and r is not None:
                        if (a ^ b) == r: m = 1
                        elif (1 - (a ^ b)) == r: m = 0
                    Trigate._LUT_LEARN[(a, b, r)] = m
                # Infer LUT
                for m in states:
                    r = None
                    if a is not None and b is not None and m is not None:
                        r = a ^ b if m == 1 else 1 - (a ^ b)
                    Trigate._LUT_INFER[(a, b, m)] = r

        for m in states:
             for r in states:
                # Deduce A
                for b in states:
                    a = None
                    if b is not None and m is not None and r is not None:
                       a = b ^ r if m == 1 else 1 - (b^r)
                    Trigate._LUT_DEDUCE_A[(b, m, r)] = a
                # Deduce B
                for a in states:
                    b = None
                    if a is not None and m is not None and r is not None:
                        b = a ^ r if m == 1 else 1 - (a^r)
                    Trigate._LUT_DEDUCE_B[(a, m, r)] = b


    def infer(self, A: List[int], B: List[int], M: List[int]) -> List[Optional[int]]:
        """Modo 1: Inferencia - Calcula R desde A, B, M."""
        return [Trigate._LUT_INFER.get((a, b, m)) for a, b, m in zip(A, B, M)]

    def learn(self, A: List[int], B: List[int], R: List[int]) -> List[Optional[int]]:
        """Modo 2: Aprendizaje - Descubre M desde A, B, R."""
        return [Trigate._LUT_LEARN.get((a, b, r)) for a, b, r in zip(A, B, R)]

    def synthesize(self, A: List[int], B: List[int]) -> Tuple[List[Optional[int]], List[Optional[int]]]:
        """Síntesis Aurora: genera M (lógica) y S (forma) desde A y B."""
        M = [TernaryLogic.ternary_xor(a, b) for a, b in zip(A, B)]
        S = [TernaryLogic.ternary_xnor(a, b) for a, b in zip(A, B)]
        return M, S

class Transcender:
    """
    Componente de síntesis que ahora implementa correctamente la síntesis
    jerárquica de Tensores Fractales completos.
    """
    def __init__(self):
        self.trigate = Trigate()

    def compute_vector_trio(self, A: List[int], B: List[int], C: List[int]) -> Dict[str, Any]:
        """Procesa un trío de vectores simples (operación base)."""
        M_AB, _ = self.trigate.synthesize(A, B)
        M_BC, _ = self.trigate.synthesize(B, C)
        M_CA, _ = self.trigate.synthesize(C, A)
        M_emergent, _ = self.trigate.synthesize(M_AB, M_BC)
        interm1, _ = self.trigate.synthesize(M_AB, M_BC)
        M_intermediate, _ = self.trigate.synthesize(interm1, M_CA)
        MetaM = [TernaryLogic.ternary_xor(a, b) for a, b in zip(M_intermediate, M_emergent)]
        return {'M_emergent': M_emergent, 'MetaM': MetaM}

    def compute_full_fractal(self, tensor_A: "FractalTensor", tensor_B: "FractalTensor", tensor_C: "FractalTensor") -> "FractalTensor":
        """
        Realiza la síntesis fractal completa y correcta de tres tensores.
        Opera de abajo hacia arriba (bottom-up): 27 -> 9 -> 3.
        """
        output_tensor = FractalTensor.neutral()
        
        # --- ETAPA 1: Síntesis del Nivel 27 al Nivel 9 ---
        intermediate_results_from_27 = []
        for i in range(27):
            result = self.compute_vector_trio(
                tensor_A.nivel_27[i],
                tensor_B.nivel_27[i],
                tensor_C.nivel_27[i]
            )
            intermediate_results_from_27.append(result['M_emergent'])
        
        # --- ETAPA 2: Síntesis de los resultados para formar el Nivel 9 ---
        intermediate_results_from_9 = []
        for i in range(0, 27, 3):
            trio_to_process = intermediate_results_from_27[i:i+3]
            result = self.compute_vector_trio(*trio_to_process)
            intermediate_results_from_9.append(result['M_emergent'])
        output_tensor.nivel_9 = intermediate_results_from_9

        # --- ETAPA 3: Síntesis final para formar el Nivel 3 (Raíz) ---
        final_level_3 = []
        for i in range(0, 9, 3):
            trio_to_process = intermediate_results_from_9[i:i+3]
            result = self.compute_vector_trio(*trio_to_process)
            final_level_3.append(result['M_emergent'])
        output_tensor.nivel_3 = final_level_3

        # Opcional: Conservar los resultados intermedios para trazabilidad.
        output_tensor.nivel_27 = intermediate_results_from_27
        return output_tensor

# ===============================================================================
# NIVEL 3: ESTRUCTURAS DE DATOS Y CONOCIMIENTO
# ===============================================================================

class FractalTensor:
    """
    Representa un tensor fractal con 3 niveles de profundidad, alineado
    con la estructura 3, 9, 27 de la documentación.
    """
    def __init__(self, nivel_3=None, nivel_9=None, nivel_27=None):
        # Nivel abstracto/raíz: 3 vectores de 3 bits.
        self.nivel_3 = nivel_3 if nivel_3 is not None else [[None, None, None] for _ in range(3)]
        # Nivel intermedio: 9 vectores de 3 bits.
        self.nivel_9 = nivel_9 if nivel_9 is not None else [[None, None, None] for _ in range(9)]
        # Nivel detallado: 27 vectores de 3 bits.
        self.nivel_27 = nivel_27 if nivel_27 is not None else [[None, None, None] for _ in range(27)]

    @staticmethod
    def random():
        """Crea un FractalTensor aleatorio con todos sus niveles poblados."""
        def rand_vec(): 
            return [random.choice([0, 1]) for _ in range(3)]
        return FractalTensor(
            nivel_3=[rand_vec() for _ in range(3)],
            nivel_9=[rand_vec() for _ in range(9)],
            nivel_27=[rand_vec() for _ in range(27)]
        )

    @staticmethod
    def neutral():
        """Crea un FractalTensor neutro con valores binarios válidos (0s)."""
        def zero_vec(): 
            return [0, 0, 0]
        return FractalTensor(
            nivel_3=[zero_vec() for _ in range(3)],
            nivel_9=[zero_vec() for _ in range(9)],
            nivel_27=[zero_vec() for _ in range(27)]
        )

    def __repr__(self):
        """Muestra una representación compacta para facilitar la depuración."""
        return (f"FT(root={self.nivel_3[0]}, mid={self.nivel_9[0]}, detail={self.nivel_27[0]})")

# ===============================================================================
# NIVEL 4: MOTOR DE ABSTRACCIÓN Y APRENDIZAJE (EVOLVER)
# ===============================================================================


# ===============================================================================
# VERSIÓN MEJORADA DE EVOLVER Y EXTENDER
# ===============================================================================

# ===============================================================================
# NIVEL 4: MOTOR DE ABSTRACCIÓN Y APRENDIZAJE (EVOLVER) - VERSIÓN COMPLETA
# ===============================================================================

class Evolver:
    """
    Evolver con visión fractal unificada para Arquetipos, Dinámicas y Relatores.

    Utiliza un único motor de síntesis fractal para analizar diferentes perspectivas
    del conocimiento, demostrando la autosimilaridad de la arquitectura.
    """
    def __init__(self):
        self.base_transcender = Transcender()

    def _perform_full_tensor_synthesis(self, tensors: List["FractalTensor"]) -> "FractalTensor":
        """
        Motor de síntesis fractal que reduce una lista de Tensores Fractales
        a un único Tensor Fractal representativo mediante una cascada de
        operaciones de síntesis completas (bottom-up 27->9->3).

        Este es el motor unificado para Arquetipos, Dinámicas y Relatores.
        """
        if not tensors:
            return FractalTensor.neutral()  # Devuelve un tensor vacío si no hay entrada

        current_level_tensors = tensors

        # Bucle recursivo que se ejecuta hasta que solo queda un tensor.
        while len(current_level_tensors) > 1:
            next_level_tensors = []
            for i in range(0, len(current_level_tensors), 3):
                trio = current_level_tensors[i:i+3]
                
                # Rellena con tensores vacíos si el último grupo no es un trío.
                while len(trio) < 3:
                    trio.append(FractalTensor.neutral())
                
                # Aplica la síntesis fractal completa a cada trío.
                synthesized_tensor = self.base_transcender.compute_full_fractal(*trio)
                next_level_tensors.append(synthesized_tensor)
            
            current_level_tensors = next_level_tensors
            
        return current_level_tensors[0]

    def compute_fractal_archetype(self, tensor_family: List["FractalTensor"]) -> "FractalTensor":
        """
        Perspectiva de ARQUETIPO:
        Analiza una familia de conceptos para destilar su "esencia"
        en un único Tensor Fractal arquetípico.
        """
        if not tensor_family or len(tensor_family) < 2:
            warnings.warn("Se requieren al menos 2 tensores para computar un arquetipo.")
            return FractalTensor.neutral()
            
        return self._perform_full_tensor_synthesis(tensor_family)

    def analyze_fractal_dynamics(self, temporal_sequence: List["FractalTensor"]) -> "FractalTensor":
        """
        Perspectiva de DINÁMICA:
        Analiza una secuencia temporal de estados para sintetizar una
        "Firma Dinámica" que representa el patrón de evolución.
        """
        if not temporal_sequence or len(temporal_sequence) < 2:
            warnings.warn("Se requiere una secuencia de al menos 2 tensores para analizar dinámicas.")
            return FractalTensor.neutral()
            
        return self._perform_full_tensor_synthesis(temporal_sequence)

    def analyze_fractal_relations(self, contextual_cluster: List["FractalTensor"]) -> "FractalTensor":
        """
        Perspectiva de RELATOR:
        Analiza un clúster de tensores dentro de un mismo contexto para
        obtener una "Firma Relacional" que representa su mapa conceptual.
        """
        if not contextual_cluster or len(contextual_cluster) < 2:
            warnings.warn("Se requieren al menos 2 tensores para el análisis relacional.")
            return FractalTensor.neutral()
            
        return self._perform_full_tensor_synthesis(contextual_cluster)

# ===============================================================================
# VERSIÓN MEJORADA DE KB Y EXTENDER PARA GESTIONAR TENSORES FRACTALES
# ===============================================================================

class _SingleUniverseKB:
    """
    Gestiona el conocimiento de un único espacio lógico, ahora almacenando
    y recuperando Tensores Fractales completos como unidades de conocimiento (arquetipos).
    """
    def __init__(self):
        self.archetypes: List["FractalTensor"] = []
        # El índice ahora mapea una firma Ms a un arquetipo completo (Tensor Fractal).
        self.ms_index: Dict[Tuple[int, ...], "FractalTensor"] = {}
        # Índice por nombre para facilitar búsquedas conceptuales
        self.name_index: Dict[str, "FractalTensor"] = {}
        self.coherence_violations: int = 0

    def add_archetype(self, archetype_tensor: "FractalTensor", name: str = None, **kwargs) -> bool:
        """
        Añade un nuevo arquetipo (Tensor Fractal) al universo, usando su
        vector raíz como clave para la indexación y validación de coherencia.
        """
        if not isinstance(archetype_tensor, FractalTensor):
            raise ValueError("La entrada debe ser un objeto FractalTensor.")

        # La clave del arquetipo es su vector raíz más abstracto.
        ms_key = tuple(archetype_tensor.nivel_3[0])
        
        # Validación de Coherencia: No puede haber dos arquetipos diferentes con la misma clave raíz.
        if ms_key in self.ms_index:
            # Aquí se podría implementar una comparación más profunda si fuera necesario.
            # Por ahora, prevenimos claves duplicadas para mantener la simplicidad.
            warnings.warn(f"Violación de Coherencia: Ya existe un arquetipo con la clave Ms={ms_key}. No se añadió el nuevo.")
            self.coherence_violations += 1
            return False

        # Validación de nombres únicos
        if name and name in self.name_index:
            warnings.warn(f"Violación de Coherencia: Ya existe un arquetipo con el nombre '{name}'. No se añadió el nuevo.")
            self.coherence_violations += 1
            return False

        # Añadir metadatos al propio tensor
        metadata = kwargs.copy()
        if name:
            metadata['name'] = name
        setattr(archetype_tensor, 'metadata', metadata)
        setattr(archetype_tensor, 'timestamp', time.time())
        
        # Almacenar e indexar el arquetipo
        self.archetypes.append(archetype_tensor)
        self.ms_index[ms_key] = archetype_tensor
        if name:
            self.name_index[name] = archetype_tensor
        return True

    def find_archetype_by_ms(self, Ms_query: List[int]) -> Optional["FractalTensor"]:
        """
        Busca un arquetipo completo de forma exacta a través de su clave Ms (vector raíz).
        """
        return self.ms_index.get(tuple(Ms_query))
    
    def find_archetype_by_name(self, name: str) -> Optional["FractalTensor"]:
        """
        Busca un arquetipo por su nombre asignado.
        """
        return self.name_index.get(name)

class FractalKnowledgeBase:
    """
    Gestor de múltiples universos (_SingleUniverseKB), que ahora almacenan arquetipos
    fractales completos, organizados por `space_id`.
    """
    def __init__(self):
        self.universes: Dict[str, _SingleUniverseKB] = {}

    def _get_space(self, space_id: str = 'default') -> _SingleUniverseKB:
        if space_id not in self.universes:
            self.universes[space_id] = _SingleUniverseKB()
        return self.universes[space_id]

    def add_entry(self, space_id: str, Ms: List[int], MetaM: List[int], **kwargs) -> bool:
        """
        Backward-compatible: crea un FractalTensor plano a partir de Ms y MetaM
        y lo almacena como arquetipo en el espacio dado.
        """
        # Construir tensor plano usando los primeros 3 bits de MetaM
        root = Ms
        flat = MetaM[:3]
        nivel_9 = [flat for _ in range(9)]
        nivel_27 = [flat for _ in range(27)]
        tensor = FractalTensor(nivel_3=[root], nivel_9=nivel_9, nivel_27=nivel_27)
        return self.add_archetype(space_id, tensor, **kwargs)
    
    def add_archetype(self, space_id: str, name: str, archetype_tensor: "FractalTensor", **kwargs) -> bool:
        """Añade un arquetipo fractal con nombre al universo correcto."""
        return self._get_space(space_id).add_archetype(archetype_tensor, name=name, **kwargs)

    def find_archetype_by_ms(self, space_id: str, Ms_query: List[int]) -> Optional["FractalTensor"]:
        """Busca un arquetipo fractal en el universo correcto."""
        return self._get_space(space_id).find_archetype_by_ms(Ms_query)
    
    def find_archetype_by_name(self, space_id: str, name: str) -> Optional["FractalTensor"]:
        """Busca un arquetipo fractal por nombre en el universo correcto."""
        return self._get_space(space_id).find_archetype_by_name(name)

class Extender:
    """
    Extender que ahora utiliza arquetipos fractales completos de la KB
    para realizar una reconstrucción mucho más rica y coherente.
    """
    def __init__(self, knowledge_base: "FractalKnowledgeBase", evolver: "Evolver"):
        self.kb = knowledge_base
        self.evolver = evolver

    def extend_fractal(self, incomplete_tensor: "FractalTensor", contexto: Dict[str, Any]) -> Dict[str, Any]:
        """
        Extiende un tensor incompleto buscando un arquetipo completo en la KB
        y usando su estructura para rellenar los detalles faltantes.
        """
        space_id = contexto.get('space_id', 'default')
        # La clave de búsqueda es el vector raíz del tensor incompleto.
        root_vector = incomplete_tensor.nivel_3[0]
        
        log = [f"Buscando arquetipo en KB (espacio='{space_id}') con clave {root_vector}..."]
        
        # Buscar el arquetipo completo en la Knowledge Base.
        found_archetype = self.kb.find_archetype_by_ms(space_id, Ms_query=root_vector)

        if found_archetype:
            method = f"archetype_reconstruction (espacio: {space_id})"
            log.append(f"Arquetipo encontrado: {found_archetype}. Aplicando su estructura.")
            
            # --- RECONSTRUCCIÓN FRACTAL REAL (VERSIÓN 1) ---
            # Se copian los niveles inferiores del arquetipo encontrado al tensor incompleto.
            reconstructed_tensor = copy.deepcopy(incomplete_tensor)
            reconstructed_tensor.nivel_9 = copy.deepcopy(found_archetype.nivel_9)
            reconstructed_tensor.nivel_27 = copy.deepcopy(found_archetype.nivel_27)
            
            return {
                'reconstructed_tensor': reconstructed_tensor,
                'reconstruction_method': method,
                'log': log
            }
        else:
            # Fallback si no se encuentra un arquetipo que guíe la reconstrucción.
            log.append("No se encontró un arquetipo guía en la KB.")
            return {
                'reconstructed_tensor': incomplete_tensor,
                'reconstruction_method': 'fallback_no_archetype_found',
                'log': log
            }




# ===============================================================================
# MÓDULO DE ROTACIÓN DE TENSORES (ARC - Aurean Rotation Cycle)
# ===============================================================================
import math
from typing import List, Dict, Any, Optional

# --- Constantes Matemáticas ---
PHI = (1 + 5**0.5) / 2
PHI_INVERSE = 1 / PHI

# --- Funciones de Utilidad ---

def fib(n: int) -> int:
    """Calculadora de la secuencia de Fibonacci, optimizada para rotaciones."""
    if n <= 1:
        return 1
    a, b = 1, 1
    for _ in range(2, n + 1):
        a, b = b, a + b
    return b

def golden_index(steps: int, size: int) -> int:
    """Calcula un índice en un espacio de 'size' usando la proporción áurea."""
    if size == 0:
        return 0
    # Usamos la parte fraccionaria para asegurar una distribución uniforme
    frac, _ = math.modf(steps * PHI_INVERSE)
    return int(frac * size)

def golden_rotate(seq: List, steps: int = 1) -> List:
    """Rota una secuencia de forma determinista usando la proporción áurea."""
    if not seq or len(seq) <= 1:
        return seq
    offset = golden_index(steps, len(seq))
    return seq[offset:] + seq[:offset]


# --- Clases Principales del Módulo ---

class TensorRotor:
    """
    Rotor de tensores con estrategias híbridas de exploración (phi, fibonacci, hybrid).
    Es el motor que genera la secuencia de índices para la selección de tensores.
    """
    def __init__(self, N: int, mode: str = "hybrid", start_k: int = 0):
        self.N = max(1, N)
        self.k = start_k % self.N
        self.i = 0  # Contador de pasos
        self.mode = mode
        self.phi_step = max(1, round(PHI_INVERSE * self.N))
        self.coverage_set = {self.k}

    def next(self) -> int:
        """Calcula el siguiente índice según la estrategia de rotación."""
        if self.mode == "phi":
            self.k = (self.k + self.phi_step) % self.N
        elif self.mode == "fibonacci":
            fib_step = fib(self.i % 16) # Límite para evitar números enormes
            self.k = (self.k + fib_step) % self.N
        else: # hybrid
            if self.i % 2 == 0:
                self.k = (self.k + self.phi_step) % self.N
            else:
                fib_step = fib((self.i // 2) % 16)
                self.k = (self.k + fib_step) % self.N
        
        self.i += 1
        self.coverage_set.add(self.k)
        return self.k

    def get_trio(self) -> List[int]:
        """Genera un trío de índices para la síntesis fractal."""
        indices = [self.k]
        for _ in range(2):
            indices.append(self.next())
        return indices

class TensorPoolManager:
    """
    Gestor de pools de tensores con rotación estratificada. Mantiene pools
    separados por profundidad y aplica rotación específica según la tarea.
    """
    def __init__(self):
        self.pools: Dict[str, List['FractalTensor']] = {
            'deep27': [],
            'mid9': [],
            'shallow3': [],
            'mixed': []
        }
        self.rotors: Dict[str, TensorRotor] = {
            'deep27': TensorRotor(0, mode="fibonacci"), # Saltos grandes para profundidad
            'mid9': TensorRotor(0, mode="hybrid"),    # Balance
            'shallow3': TensorRotor(0, mode="phi"),      # Cobertura uniforme
            'mixed': TensorRotor(0, mode="hybrid")     # Balance para análisis general
        }

    def add_tensor(self, tensor: 'FractalTensor'):
        """Añade un tensor al pool apropiado según su profundidad."""
        # Lógica simplificada para determinar la profundidad
        if tensor.nivel_27 and any(v is not None for v in tensor.nivel_27[0]):
            pool_name = 'deep27'
        elif tensor.nivel_9 and any(v is not None for v in tensor.nivel_9[0]):
            pool_name = 'mid9'
        else:
            pool_name = 'shallow3'

        self.pools[pool_name].append(tensor)
        self.pools['mixed'].append(tensor)

        # Actualizar los rotors con el nuevo tamaño de los pools
        self.rotors[pool_name] = TensorRotor(len(self.pools[pool_name]), mode=self.rotors[pool_name].mode)
        self.rotors['mixed'] = TensorRotor(len(self.pools['mixed']), mode=self.rotors['mixed'].mode)

    def get_tensor_trio(self, task_type: str = "arquetipo") -> List['FractalTensor']:
        """
        Obtiene un trío de tensores optimizado para una tarea específica,
        utilizando la estrategia de rotación adecuada.
        """
        task_to_pool = {
            'arquetipo': 'mixed',
            'dinamica': 'shallow3',
            'relator': 'mid9',
            'axioma': 'deep27'
        }
        pool_name = task_to_pool.get(task_type, 'mixed')
        pool = self.pools[pool_name]

        # Fallback inteligente si el pool preferido no tiene suficientes tensores
        if len(pool) < 3:
            fallback_order = ['mixed', 'shallow3', 'mid9', 'deep27']
            for fallback_pool_name in fallback_order:
                if len(self.pools[fallback_pool_name]) >= 3:
                    pool_name = fallback_pool_name
                    pool = self.pools[pool_name]
                    break
        
        if len(pool) < 3:
            return [] # No hay suficientes tensores en ningún pool

# ===============================================================================
# DEMOSTRACIÓN: APRENDIZAJE Y APLICACIÓN DE ARQUETIPOS FRACTALES
# ===============================================================================
# ===============================================================================
# NIVEL 4: MOTOR DE ABSTRACCIÓN Y APRENDIZAJE (EVOLVER) - VERSIÓN COMPLETA
# ===============================================================================

class Evolver:
    """
    Evolver con visión fractal unificada para Arquetipos, Dinámicas y Relatores.

    Utiliza un único motor de síntesis fractal para analizar diferentes perspectivas
    del conocimiento, demostrando la autosimilaridad de la arquitectura.
    """
    def __init__(self):
        self.base_transcender = Transcender()

    def _perform_full_tensor_synthesis(self, tensors: List["FractalTensor"]) -> "FractalTensor":
        """
        Motor de síntesis fractal que reduce una lista de Tensores Fractales
        a un único Tensor Fractal representativo mediante una cascada de
        operaciones de síntesis completas (bottom-up 27->9->3).

        Este es el motor unificado para Arquetipos, Dinámicas y Relatores.
        """
        if not tensors:
            return FractalTensor.neutral()  # Devuelve un tensor vacío si no hay entrada

        current_level_tensors = tensors

        # Bucle recursivo que se ejecuta hasta que solo queda un tensor.
        while len(current_level_tensors) > 1:
            next_level_tensors = []
            for i in range(0, len(current_level_tensors), 3):
                trio = current_level_tensors[i:i+3]
                
                # Rellena con tensores vacíos si el último grupo no es un trío.
                while len(trio) < 3:
                    trio.append(FractalTensor.neutral())
                
                # Aplica la síntesis fractal completa a cada trío.
                synthesized_tensor = self.base_transcender.compute_full_fractal(*trio)
                next_level_tensors.append(synthesized_tensor)
            
            current_level_tensors = next_level_tensors
            
        return current_level_tensors[0]

    def compute_fractal_archetype(self, tensor_family: List["FractalTensor"]) -> "FractalTensor":
        """
        Perspectiva de ARQUETIPO:
        Analiza una familia de conceptos para destilar su "esencia"
        en un único Tensor Fractal arquetípico.
        """
        if not tensor_family or len(tensor_family) < 2:
            warnings.warn("Se requieren al menos 2 tensores para computar un arquetipo.")
            return FractalTensor.neutral()
            
        return self._perform_full_tensor_synthesis(tensor_family)

    def analyze_fractal_dynamics(self, temporal_sequence: List["FractalTensor"]) -> "FractalTensor":
        """
        Perspectiva de DINÁMICA:
        Analiza una secuencia temporal de estados para sintetizar una
        "Firma Dinámica" que representa el patrón de evolución.
        """
        if not temporal_sequence or len(temporal_sequence) < 2:
            warnings.warn("Se requiere una secuencia de al menos 2 tensores para analizar dinámicas.")
            return FractalTensor.neutral()
            
        return self._perform_full_tensor_synthesis(temporal_sequence)

    def analyze_fractal_relations(self, contextual_cluster: List["FractalTensor"]) -> "FractalTensor":
        """
        Perspectiva de RELATOR:
        Analiza un clúster de tensores dentro de un mismo contexto para
        obtener una "Firma Relacional" que representa su mapa conceptual.
        """
        if not contextual_cluster or len(contextual_cluster) < 2:
            warnings.warn("Se requieren al menos 2 tensores para el análisis relacional.")
            return FractalTensor.neutral()
            
        return self._perform_full_tensor_synthesis(contextual_cluster)

# ===============================================================================
# DEMOSTRACIÓN FRACTAL COMPLETA CON DATOS COHERENTES
# ===============================================================================

if __name__ == "__main__":
    import copy
    
    print("🌌 DEMOSTRACIÓN FRACTAL AURORA: Arquetipos, Dinámicas y Relatores 🌌")
    print("=" * 80)
    print("Análisis de conocimiento desde tres perspectivas con DATOS COHERENTES:")
    print("• ARQUETIPOS: Esencia conceptual de una familia de ideas.")
    print("• DINÁMICAS: Patrones de evolución de una secuencia temporal LÓGICA.")
    print("• RELATORES: Mapas relacionales de un clúster de conceptos CONECTADOS.")
    print("=" * 80)

    # === INICIALIZACIÓN DEL ECOSISTEMA AURORA ===
    kb = FractalKnowledgeBase()
    evolver = Evolver()
    extender = Extender(kb, evolver)

    # === FASE 1: ANÁLISIS DE ARQUETIPOS (Como antes, ya era coherente) ===
    print("\n🏛️ FASE 1: ANÁLISIS DE ARQUETIPOS")
    print("-" * 50)
    familia_movimiento = [
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,0,0]]*9, nivel_27=[[0,0,1]]*27),
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,1,0]]*9, nivel_27=[[0,1,0]]*27),
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[0,1,1]]*9, nivel_27=[[1,1,1]]*27)
    ]
    arquetipo_movimiento = evolver.compute_fractal_archetype(familia_movimiento)
    print(f"• Analizando {len(familia_movimiento)} conceptos de 'movimiento'...")
    print(f"• ARQUETIPO resultante: {arquetipo_movimiento}")
    kb.add_archetype('fisica_conceptual', 'movimiento_universal', arquetipo_movimiento)
    print("  └─ Arquetipo almacenado en el espacio 'fisica_conceptual'.")

    # === FASE 2: DEMOSTRACIÓN DE DINÁMICAS CON DATOS COHERENTES ===
    print("\n⚡ FASE 2: ANÁLISIS DE DINÁMICAS")
    print("-" * 50)
    print("Analizando una secuencia temporal LÓGICA de un sistema...")
    
    # Crear una secuencia donde cada estado evoluciona del anterior
    estado_t0 = FractalTensor.random()
    estado_t1 = evolver.base_transcender.compute_full_fractal(estado_t0, estado_t0, FractalTensor.neutral())
    estado_t2 = evolver.base_transcender.compute_full_fractal(estado_t1, estado_t1, FractalTensor.neutral())
    estado_t3 = evolver.base_transcender.compute_full_fractal(estado_t2, estado_t2, FractalTensor.neutral())
    secuencia_temporal_logica = [estado_t0, estado_t1, estado_t2, estado_t3]

    print(f"• Secuencia temporal: {len(secuencia_temporal_logica)} estados evolutivos.")
    
    firma_dinamica = evolver.analyze_fractal_dynamics(secuencia_temporal_logica)
    print(f"\n• DINÁMICA resultante: {firma_dinamica}")
    print("  └─ Representa el 'patrón de evolución' coherente extraído de la secuencia.")
    kb.add_archetype('dinamicas_evolutivas', 'evolucion_sistema_X', firma_dinamica)
    print("  └─ Dinámica almacenada en el espacio 'dinamicas_evolutivas'.")

    # === FASE 3: DEMOSTRACIÓN DE RELATORES CON DATOS COHERENTES ===
    print("\n🔗 FASE 3: ANÁLISIS DE RELATORES")
    print("-" * 50)
    print("Analizando un clúster contextual de conceptos relacionados...")
    
    # Crear un clúster de conceptos que comparten una base
    concepto_base = FractalTensor.random()
    concepto_fuerza = evolver.base_transcender.compute_full_fractal(concepto_base, FractalTensor.random(), FractalTensor.neutral())
    concepto_energia = evolver.base_transcender.compute_full_fractal(concepto_base, concepto_fuerza, FractalTensor.neutral())
    concepto_momentum = evolver.base_transcender.compute_full_fractal(concepto_base, concepto_energia, FractalTensor.neutral())
    cluster_contextual_logico = [concepto_fuerza, concepto_energia, concepto_momentum]
    
    print(f"• Clúster contextual: {len(cluster_contextual_logico)} conceptos relacionados en mecánica.")

    firma_relacional = evolver.analyze_fractal_relations(cluster_contextual_logico)
    print(f"\n• RELATOR resultante: {firma_relacional}")
    print("  └─ Representa el 'mapa conceptual' de las interrelaciones.")
    kb.add_archetype('mapas_conceptuales', 'mecanica_clasica', firma_relacional)
    print("  └─ Relator almacenado en el espacio 'mapas_conceptuales'.")

    # === FASE 4 y 5 (Verificación y Reconstrucción) se mantienen igual ===
    # ...

    print("\n" + "=" * 80)
    print("🎯 CONCLUSIÓN DE LA DEMO:")
    print("Al proporcionar datos de entrada con una estructura y coherencia interna,")
    print("el Evolver es capaz de sintetizar con éxito Arquetipos, Dinámicas y Relatores,")
    print("demostrando su poder de abstracción en todas las perspectivas.")
    print("=" * 80)
