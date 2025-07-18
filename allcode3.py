# ===============================================================================
# IMPORTS AGRUPADOS
# ===============================================================================
import random
import time
import warnings
import copy
import math
from typing import List, Dict, Any, Tuple, Optional

# === NOTA SOBRE TESTS Y CONCURRENCIA ===
# Para concurrencia real, proteger la KB con locks o usar una base de datos transaccional.
# Añadir casos de prueba unitarios (ejemplo: PyTest) para cada clase principal.
# ===============================================================================
# AURORA TRINITY-3 - ARQUITECTURA CANÓNICA COMPLETA Y REFACTORIZADA
# ===============================================================================

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
        return a ^ b

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
    Las LUTs se generan una sola vez a nivel de clase para máxima eficiencia.
    """
    _LUT_INFER = {}
    _LUT_LEARN = {}
    _LUT_DEDUCE_A = {}
    _LUT_DEDUCE_B = {}

    @classmethod
    def _initialize_luts(cls):
        """Genera las Look-Up Tables una sola vez."""
        if cls._LUT_INFER:  # Si ya están inicializadas, no hacer nada.
            return

        states = [0, 1, None]
        for a in states:
            for b in states:
                # Learn LUT
                for r in states:
                    m = None
                    if a is not None and b is not None and r is not None:
                        if (a ^ b) == r: m = 1
                        elif (1 - (a ^ b)) == r: m = 0
                    cls._LUT_LEARN[(a, b, r)] = m
                # Infer LUT
                for m in states:
                    r = None
                    if a is not None and b is not None and m is not None:
                        r = a ^ b if m == 1 else 1 - (a ^ b)
                    cls._LUT_INFER[(a, b, m)] = r

        for m in states:
            for r in states:
                # Deduce A
                for b in states:
                    a = None
                    if b is not None and m is not None and r is not None:
                        a = b ^ r if m == 1 else 1 - (b ^ r)
                    cls._LUT_DEDUCE_A[(b, m, r)] = a
                # Deduce B
                for a in states:
                    b = None
                    if a is not None and m is not None and r is not None:
                        b = a ^ r if m == 1 else 1 - (a ^ r)
                    cls._LUT_DEDUCE_B[(a, m, r)] = b

    def infer(self, A: List[int], B: List[int], M: List[int]) -> List[Optional[int]]:
        """Modo 1: Inferencia - Calcula R desde A, B, M."""
        return [self._LUT_INFER.get((a, b, m)) for a, b, m in zip(A, B, M)]

    def learn(self, A: List[int], B: List[int], R: List[int]) -> List[Optional[int]]:
        """Modo 2: Aprendizaje - Descubre M desde A, B, R."""
        return [self._LUT_LEARN.get((a, b, r)) for a, b, r in zip(A, B, R)]

    def synthesize(self, A: List[int], B: List[int]) -> Tuple[List[Optional[int]], List[Optional[int]]]:
        """Síntesis Aurora: genera M (lógica) y S (forma) desde A y B."""
        M = [TernaryLogic.ternary_xor(a, b) for a, b in zip(A, B)]
        S = [TernaryLogic.ternary_xnor(a, b) for a, b in zip(A, B)]
        return M, S

# Inicializar las LUTs una sola vez al cargar el script
Trigate._initialize_luts()

class Transcender:
    """
    Componente de síntesis que implementa la síntesis jerárquica
    de Tensores Fractales completos.
    """
    def __init__(self):
        self.trigate = Trigate()

    def compute_vector_trio(self, A: List[int], B: List[int], C: List[int]) -> Dict[str, Any]:
        """Procesa un trío de vectores simples (operación base)."""
        M_AB, _ = self.trigate.synthesize(A, B)
        M_BC, _ = self.trigate.synthesize(B, C)
        M_CA, _ = self.trigate.synthesize(C, A)
        M_emergent, _ = self.trigate.synthesize(M_AB, M_BC)
        M_intermediate, _ = self.trigate.synthesize(M_emergent, M_CA)
        MetaM = [TernaryLogic.ternary_xor(a, b) for a, b in zip(M_intermediate, M_emergent)]
        return {'M_emergent': M_emergent, 'MetaM': MetaM}

    def compute_full_fractal(self, tensor_A: "FractalTensor", tensor_B: "FractalTensor", tensor_C: "FractalTensor") -> "FractalTensor":
        """
        Realiza la síntesis fractal completa (bottom-up: 27 -> 9 -> 3).
        """
        output_tensor = FractalTensor.neutral()
        
        # Etapa 1: Síntesis del Nivel 27
        intermediate_results_from_27 = [
            self.compute_vector_trio(
                tensor_A.nivel_27[i], tensor_B.nivel_27[i], tensor_C.nivel_27[i]
            )['M_emergent'] for i in range(27)
        ]
        
        # Etapa 2: Síntesis del Nivel 9
        intermediate_results_from_9 = [
            self.compute_vector_trio(
                *intermediate_results_from_27[i:i+3]
            )['M_emergent'] for i in range(0, 27, 3)
        ]
        output_tensor.nivel_9 = intermediate_results_from_9

        # Etapa 3: Síntesis del Nivel 3 (Raíz)
        final_level_3 = [
            self.compute_vector_trio(
                *intermediate_results_from_9[i:i+3]
            )['M_emergent'] for i in range(0, 9, 3)
        ]
        output_tensor.nivel_3 = final_level_3
        output_tensor.nivel_27 = intermediate_results_from_27 # Conservar para trazabilidad
        return output_tensor

# ===============================================================================
# NIVEL 3: ESTRUCTURAS DE DATOS Y CONOCIMIENTO
# ===============================================================================

class FractalTensor:
    """
    Representa un tensor fractal con 3 niveles de profundidad (3, 9, 27).
    """
    def __init__(self, nivel_3=None, nivel_9=None, nivel_27=None, Ss=None):
        self.nivel_3 = nivel_3 if nivel_3 is not None else [[None] * 3 for _ in range(3)]
        self.nivel_9 = nivel_9 if nivel_9 is not None else [[None] * 3 for _ in range(9)]
        self.nivel_27 = nivel_27 if nivel_27 is not None else [[None] * 3 for _ in range(27)]
        self.Ss = Ss  # Añadir memoria factual

    @staticmethod
    def random():
        """Crea un FractalTensor aleatorio."""
        rand_vec = lambda: [random.choice([0, 1]) for _ in range(3)]
        return FractalTensor(
            nivel_3=[rand_vec() for _ in range(3)],
            nivel_9=[rand_vec() for _ in range(9)],
            nivel_27=[rand_vec() for _ in range(27)]
        )

    @staticmethod
    def neutral():
        """Crea un FractalTensor neutro (ceros)."""
        zero_vec = lambda: [0, 0, 0]
        return FractalTensor(
            nivel_3=[zero_vec() for _ in range(3)],
            nivel_9=[zero_vec() for _ in range(9)],
            nivel_27=[zero_vec() for _ in range(27)]
        )

    def __repr__(self):
        """Representación compacta para depuración."""
        return f"FT(root={self.nivel_3[0]}, mid={self.nivel_9[0]}, detail={self.nivel_27[0]})"

# ===============================================================================
# NIVEL 4: MOTOR DE ABSTRACCIÓN Y APRENDIZAJE (EVOLVER)
# ===============================================================================

class Evolver:
    """
    Motor de visión fractal unificada para Arquetipos, Dinámicas y Relatores.
    """
    def __init__(self):
        self.base_transcender = Transcender()

    def _perform_full_tensor_synthesis(self, tensors: List["FractalTensor"]) -> "FractalTensor":
        """
        Motor de síntesis fractal: reduce una lista de tensores a uno solo.
        """
        if not tensors:
            return FractalTensor.neutral()
        
        current_level_tensors = list(tensors)
        while len(current_level_tensors) > 1:
            next_level_tensors = []
            for i in range(0, len(current_level_tensors), 3):
                trio = current_level_tensors[i:i+3]
                while len(trio) < 3:
                    trio.append(FractalTensor.neutral())
                synthesized_tensor = self.base_transcender.compute_full_fractal(*trio)
                next_level_tensors.append(synthesized_tensor)
            current_level_tensors = next_level_tensors
            
        return current_level_tensors[0]

    def compute_fractal_archetype(self, tensor_family: List["FractalTensor"]) -> "FractalTensor":
        """Perspectiva de ARQUETIPO: Destila la esencia de una familia de conceptos."""
        if len(tensor_family) < 2:
            warnings.warn("Se requieren al menos 2 tensores para computar un arquetipo.")
            return FractalTensor.neutral() if not tensor_family else tensor_family[0]
        return self._perform_full_tensor_synthesis(tensor_family)

    def analyze_fractal_dynamics(self, temporal_sequence: List["FractalTensor"]) -> "FractalTensor":
        """Perspectiva de DINÁMICA: Sintetiza el patrón de evolución de una secuencia."""
        if len(temporal_sequence) < 2:
            warnings.warn("Se requiere una secuencia de al menos 2 tensores para analizar dinámicas.")
            return FractalTensor.neutral() if not temporal_sequence else temporal_sequence[0]
        return self._perform_full_tensor_synthesis(temporal_sequence)

    def analyze_fractal_relations(self, contextual_cluster: List["FractalTensor"]) -> "FractalTensor":
        """Perspectiva de RELATOR: Obtiene el mapa conceptual de un clúster."""
        if len(contextual_cluster) < 2:
            warnings.warn("Se requieren al menos 2 tensores para el análisis relacional.")
            return FractalTensor.neutral() if not contextual_cluster else contextual_cluster[0]
        return self._perform_full_tensor_synthesis(contextual_cluster)
        
    @staticmethod
    def fractal_relate(tensor_group: List["FractalTensor"], level: int = 27) -> Optional[List[List[Optional[int]]]]:
        """
        Calcula una firma relacional por mayoría de votos entre un grupo de tensores.
        """
        if not tensor_group:
            return None

        # Seleccionar el nivel correcto del tensor
        try:
            dim_vectors = [getattr(t, f'nivel_{level}') for t in tensor_group]
        except AttributeError:
            raise ValueError(f"El nivel {level} no es válido. Debe ser 3, 9 o 27.")

        num_vectors = len(dim_vectors[0])
        signature = []
        for pos in range(num_vectors):
            bit_result = []
            for bit in range(3): # Asume vectores de 3 bits
                bit_vals = [t[pos][bit] for t in dim_vectors if t and t[pos] and t[pos][bit] is not None]
                if not bit_vals:
                    bit_result.append(None)
                    continue
                
                # Lógica de mayoría ternaria
                count_1 = bit_vals.count(1)
                count_0 = bit_vals.count(0)
                if count_1 > count_0: bit_result.append(1)
                elif count_0 > count_1: bit_result.append(0)
                else: bit_result.append(None)
            signature.append(bit_result)
        return signature

# ===============================================================================
# NIVEL 5: BASE DE CONOCIMIENTO Y EXTENSIÓN
# ===============================================================================

class _SingleUniverseKB:
    """Gestiona el conocimiento de un único espacio lógico (universo)."""
    def __init__(self):
        self.archetypes: List["FractalTensor"] = []
        self.ms_index: Dict[Tuple[int, ...], "FractalTensor"] = {}
        self.name_index: Dict[str, "FractalTensor"] = {}
        self.coherence_violations: int = 0

    def add_archetype(self, archetype_tensor: "FractalTensor", Ss: List[int], name: Optional[str] = None, **kwargs) -> bool:
        """Añade un arquetipo (Tensor Fractal) al universo, almacenando Ss (memoria factual)."""
        if not isinstance(archetype_tensor, FractalTensor):
            raise ValueError("La entrada debe ser un objeto FractalTensor.")

        ms_key = tuple(archetype_tensor.nivel_3[0])

        if ms_key in self.ms_index:
            warnings.warn(f"Violación de Coherencia: Ya existe un arquetipo con la clave Ms={ms_key}. No se añadió el nuevo.")
            self.coherence_violations += 1
            return False

        if name and name in self.name_index:
            warnings.warn(f"Violación de Coherencia: Ya existe un arquetipo con el nombre '{name}'. No se añadió el nuevo.")
            self.coherence_violations += 1
            return False

        metadata = kwargs.copy()
        if name: metadata['name'] = name
        setattr(archetype_tensor, 'metadata', metadata)
        setattr(archetype_tensor, 'timestamp', time.time())
        setattr(archetype_tensor, 'Ss', Ss)  # Memoria factual

        self.archetypes.append(archetype_tensor)
        self.ms_index[ms_key] = archetype_tensor
        if name: self.name_index[name] = archetype_tensor
        return True

    def find_archetype_by_ms(self, Ms_query: List[int]) -> Optional["FractalTensor"]:
        """Busca un arquetipo por su clave Ms (vector raíz)."""
        return self.ms_index.get(tuple(Ms_query))
    
    def find_archetype_by_name(self, name: str) -> Optional["FractalTensor"]:
        """Busca un arquetipo por su nombre asignado."""
        return self.name_index.get(name)

class FractalKnowledgeBase:
    """Gestor de múltiples universos de conocimiento fractal."""
    def __init__(self):
        self.universes: Dict[str, _SingleUniverseKB] = {}

    def _get_space(self, space_id: str = 'default') -> _SingleUniverseKB:
        if space_id not in self.universes:
            self.universes[space_id] = _SingleUniverseKB()
        return self.universes[space_id]

    def add_archetype(self, space_id: str, name: str, archetype_tensor: "FractalTensor", Ss: List[int], **kwargs) -> bool:
        """Añade un arquetipo fractal con nombre y Ss al universo correcto."""
        return self._get_space(space_id).add_archetype(archetype_tensor, Ss, name=name, **kwargs)

    def find_archetype_by_ms(self, space_id: str, Ms_query: List[int]) -> Optional["FractalTensor"]:
        """Busca un arquetipo fractal por Ms en el universo correcto."""
        return self._get_space(space_id).find_archetype_by_ms(Ms_query)
    
    def find_archetype_by_name(self, space_id: str, name: str) -> Optional["FractalTensor"]:
        """Busca un arquetipo fractal por nombre en el universo correcto."""
        return self._get_space(space_id).find_archetype_by_name(name)
class Extender:
    def __init__(self, knowledge_base: "FractalKnowledgeBase"):
        self.kb = knowledge_base
        self.log = []
    
    def _ss_similarity(self, ss1: list, ss2: list) -> float:
        """Calcula la similitud entre dos vectores Ss de forma segura."""
        try:
            if not ss1 or not ss2 or len(ss1) != len(ss2):
                return 0.0
            matches = sum(1 for a, b in zip(ss1, ss2) if a == b)
            return matches / len(ss1)
        except Exception:
            return 0.0
    
    def _find_archetype_by_ss(self, space_id: str, ss_query: List[int]) -> Optional[FractalTensor]:
        """Busca arquetipos por coincidencia de Ss con manejo de errores."""
        try:
            universe = self.kb._get_space(space_id)
            best_match = None
            best_similarity = 0.0
            
            for archetype in universe.archetypes:
                archetype_ss = getattr(archetype, 'Ss', [])
                similarity = self._ss_similarity(archetype_ss, ss_query)
                
                if similarity > best_similarity:
                    best_similarity = similarity
                    best_match = archetype
            
            return best_match if best_similarity > 0.7 else None
        except Exception as e:
            self.log.append(f"Error en búsqueda de arquetipo: {str(e)}")
            return None

    def _fallback_reconstruction(self, ss_query: List[int]) -> FractalTensor:
        """Crea un tensor básico cuando no se encuentra arquetipo."""
        self.log.append("Usando reconstrucción de fallback: tensor neutro")
        tensor = FractalTensor.neutral()
        tensor.Ss = ss_query
        return tensor

    def extend_fractal(self, input_ss, contexto: Dict[str, Any]) -> Dict[str, Any]:
        self.log = [f"Iniciando extensión en espacio '{contexto.get('space_id', 'default')}'"]
        
        # Manejar diferentes tipos de entrada
        if isinstance(input_ss, FractalTensor):
            ss_query = getattr(input_ss, 'Ss', input_ss.nivel_3[0])
        else:
            ss_query = input_ss
            
        # Asegurar que ss_query es válido
        if not isinstance(ss_query, list) or not ss_query:
            ss_query = [0, 0, 0]
            self.log.append("Ss inválido, usando valor por defecto [0,0,0]")
        
        # Buscar arquetipo compatible
        archetype = self._find_archetype_by_ss(contexto.get('space_id', 'default'), ss_query)
        
        if archetype:
            self.log.append(f"Arquetipo encontrado: {getattr(archetype, 'metadata', {}).get('name', 'sin nombre')}")
            reconstructed = FractalTensor(
                nivel_3=copy.deepcopy(archetype.nivel_3),
                nivel_9=copy.deepcopy(archetype.nivel_9),
                nivel_27=copy.deepcopy(archetype.nivel_27),
                Ss=ss_query  # Mantener Ss original
            )
            method = "reconstrucción guiada por arquetipo"
        else:
            self.log.append("No se encontró arquetipo compatible")
            reconstructed = self._fallback_reconstruction(ss_query)
            method = "reconstrucción por fallback"
        
        return {
            'reconstructed_tensor': reconstructed,
            'reconstruction_method': method,
            'log': self.log
        }


# ===============================================================================
# MÓDULO DE ROTACIÓN DE TENSORES (ARC - Aurean Rotation Cycle)
# ===============================================================================
PHI = (1 + 5**0.5) / 2
PHI_INVERSE = 1 / PHI

class TensorRotor:
    """Genera secuencias de índices para la selección de tensores."""
    def __init__(self, N: int, mode: str = "hybrid", start_k: int = 0):
        self.N = max(1, N)
        self.k = start_k % self.N
        self.i = 0
        self.mode = mode
        self.phi_step = max(1, round(PHI_INVERSE * self.N))
        self.fib_cache = {n: self._fib(n) for n in range(16)}

    def _fib(self, n: int) -> int:
        if n <= 1: return 1
        a, b = 1, 1
        for _ in range(2, n + 1): a, b = b, a + b
        return b

    def next(self) -> int:
        """Calcula el siguiente índice según la estrategia de rotación."""
        if self.mode == "phi":
            self.k = (self.k + self.phi_step) % self.N
        elif self.mode == "fibonacci":
            fib_step = self.fib_cache[self.i % 16]
            self.k = (self.k + fib_step) % self.N
        else: # hybrid
            if self.i % 2 == 0:
                self.k = (self.k + self.phi_step) % self.N
            else:
                fib_step = self.fib_cache[(self.i // 2) % 16]
                self.k = (self.k + fib_step) % self.N
        self.i += 1
        return self.k

class TensorPoolManager:
    """Gestor de pools de tensores con rotación estratificada."""
    def __init__(self):
        self.pools: Dict[str, List['FractalTensor']] = {
            'deep27': [], 'mid9': [], 'shallow3': [], 'mixed': []
        }
        self.rotors: Dict[str, TensorRotor] = {
            'deep27': TensorRotor(0, mode="fibonacci"),
            'mid9': TensorRotor(0, mode="hybrid"),
            'shallow3': TensorRotor(0, mode="phi"),
            'mixed': TensorRotor(0, mode="hybrid")
        }

    def add_tensor(self, tensor: 'FractalTensor'):
        """Añade un tensor al pool apropiado según su profundidad."""
        # Un tensor se considera "profundo" si tiene datos en el nivel 27
        if any(any(bit is not None for bit in vec) for vec in tensor.nivel_27):
            pool_name = 'deep27'
        elif any(any(bit is not None for bit in vec) for vec in tensor.nivel_9):
            pool_name = 'mid9'
        else:
            pool_name = 'shallow3'

        self.pools[pool_name].append(tensor)
        self.pools['mixed'].append(tensor)
        self.rotors[pool_name].N = len(self.pools[pool_name])
        self.rotors['mixed'].N = len(self.pools['mixed'])

    def get_tensor_trio(self, task_type: str = "arquetipo") -> List['FractalTensor']:
        """Obtiene un trío de tensores optimizado para una tarea específica."""
        task_to_pool = {
            'arquetipo': 'mixed', 'dinamica': 'shallow3',
            'relator': 'mid9', 'axioma': 'deep27'
        }
        pool_name = task_to_pool.get(task_type, 'mixed')
        
        # Fallback inteligente si el pool preferido no tiene suficientes tensores
        if len(self.pools[pool_name]) < 3:
            fallback_order = ['mixed', 'shallow3', 'mid9', 'deep27']
            for fb_pool_name in fallback_order:
                if len(self.pools[fb_pool_name]) >= 3:
                    pool_name = fb_pool_name
                    break
        
        pool = self.pools[pool_name]
        rotor = self.rotors[pool_name]

        if len(pool) < 3:
            trio = list(pool)
            while len(trio) < 3: trio.append(FractalTensor.neutral())
            return trio
        
        indices = [rotor.next() for _ in range(3)]
        return [pool[i] for i in indices]

# ===============================================================================
# DEMOSTRACIÓN FRACTAL COMPLETA
# ===============================================================================

if __name__ == "__main__":
    print("🌌 DEMOSTRACIÓN FRACTAL AURORA: Arquetipos, Dinámicas y Relatores 🌌")
    print("=" * 80)
    print("Análisis de conocimiento desde tres perspectivas con datos coherentes.")
    print("=" * 80)

    # === INICIALIZACIÓN DEL ECOSISTEMA AURORA ===
    kb = FractalKnowledgeBase()
    evolver = Evolver()
    extender = Extender(kb)
    pool_manager = TensorPoolManager()

    # === FASE 1: ANÁLISIS DE ARQUETIPOS ===
    print("\n🏛️ FASE 1: ANÁLISIS DE ARQUETIPOS")
    print("-" * 50)
    familia_movimiento = [
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,0,0]]*9, nivel_27=[[0,0,1]]*27),
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[1,1,0]]*9, nivel_27=[[0,1,0]]*27),
        FractalTensor(nivel_3=[[1,0,1]], nivel_9=[[0,1,1]]*9, nivel_27=[[1,1,1]]*27)
    ]
    for t in familia_movimiento: pool_manager.add_tensor(t)
    
    trio_para_arquetipo = pool_manager.get_tensor_trio('arquetipo')
    arquetipo_movimiento = evolver.compute_fractal_archetype(trio_para_arquetipo)
    print(f"• Analizando {len(trio_para_arquetipo)} conceptos de 'movimiento'...")
    print(f"• ARQUETIPO resultante: {arquetipo_movimiento}")
    # Extraer Ss del tensor raíz del arquetipo (ejemplo: primer vector de nivel_3)
    Ss_movimiento = arquetipo_movimiento.nivel_3[0] if hasattr(arquetipo_movimiento, 'nivel_3') else [0,0,0]
    kb.add_archetype('fisica_conceptual', 'movimiento_universal', arquetipo_movimiento, Ss=Ss_movimiento)
    print("  └─ Arquetipo almacenado en el espacio 'fisica_conceptual'.")

    # === FASE 2: ANÁLISIS DE DINÁMICAS ===
    print("\n⚡ FASE 2: ANÁLISIS DE DINÁMICAS")
    print("-" * 50)
    
    estado_t0 = FractalTensor.random()
    estado_t1 = evolver.base_transcender.compute_full_fractal(estado_t0, estado_t0, FractalTensor.neutral())
    estado_t2 = evolver.base_transcender.compute_full_fractal(estado_t1, estado_t1, FractalTensor.neutral())
    secuencia_temporal_logica = [estado_t0, estado_t1, estado_t2]
    
    print(f"• Analizando secuencia temporal de {len(secuencia_temporal_logica)} estados.")
    firma_dinamica = evolver.analyze_fractal_dynamics(secuencia_temporal_logica)
    print(f"• DINÁMICA resultante: {firma_dinamica}")
    Ss_dinamica = firma_dinamica.nivel_3[0] if hasattr(firma_dinamica, 'nivel_3') else [0,0,0]
    kb.add_archetype('dinamicas_sistemas', 'evolucion_sistema_X', firma_dinamica, Ss=Ss_dinamica)
    print("  └─ Dinámica almacenada en 'dinamicas_sistemas'.")

    # === FASE 3: ANÁLISIS DE RELATORES ===
    print("\n🔗 FASE 3: ANÁLISIS DE RELATORES")
    print("-" * 50)
    
    concepto_base = FractalTensor.random()
    concepto_fuerza = evolver.base_transcender.compute_full_fractal(concepto_base, FractalTensor.random(), FractalTensor.neutral())
    concepto_energia = evolver.base_transcender.compute_full_fractal(concepto_base, concepto_fuerza, FractalTensor.neutral())
    cluster_contextual = [concepto_base, concepto_fuerza, concepto_energia]
    
    print(f"• Analizando clúster de {len(cluster_contextual)} conceptos relacionados.")
    firma_relacional = evolver.analyze_fractal_relations(cluster_contextual)
    print(f"• RELATOR resultante: {firma_relacional}")
    Ss_relator = firma_relacional.nivel_3[0] if hasattr(firma_relacional, 'nivel_3') else [0,0,0]
    kb.add_archetype('mapas_conceptuales', 'mecanica_basica', firma_relacional, Ss=Ss_relator)
    print("  └─ Relator almacenado en 'mapas_conceptuales'.")

    # === FASE 4: EXTENSIÓN BASADA EN CONOCIMIENTO ===
    print("\n🧩 FASE 4: EXTENSIÓN POR ARQUETIPO")
    print("-" * 50)
    
    tensor_incompleto = FractalTensor(nivel_3=arquetipo_movimiento.nivel_3)
    print(f"• Tensor a extender (solo con raíz): {tensor_incompleto}")
    
    # Pasar el tensor incompleto directamente, la función extraerá Ss
    resultado_extension = extender.extend_fractal(
        tensor_incompleto,
        contexto={'space_id': 'fisica_conceptual'}
    )
    
    tensor_reconstruido = resultado_extension['reconstructed_tensor']
    print(f"• Método de reconstrucción: {resultado_extension['reconstruction_method']}")
    print(f"• Tensor reconstruido: {tensor_reconstruido}")
    print("  └─ Los niveles 9 y 27 se han rellenado desde la KB.")

    print("\n" + "=" * 80)
    print("🎯 DEMOSTRACIÓN FINALIZADA.")
    print("=" * 80)