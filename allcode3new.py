# ===================== AUTOCURACIÓN: HOT-FIX, REAXIOMATIZACIÓN Y CONSEJO TERNARIO =====================

# Mini-test para ExpertRelator tuple return
def test_relator_returns_tuple():
    kb = FractalKnowledgeBase()
    ext = Extender(kb)
    ok, rel = ext.relator.contextualizar([1,0,1], 'default')
    assert isinstance(ok, bool)
    assert ok is False and rel is None  # vacío porque la KB está vacía
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
        states = [0, 1]
        # Learn LUT
        for a in states:
            for b in states:
                for r in states:
                    m = None
                    if a is not None and b is not None and r is not None:
                        m = a ^ b if r == 1 else 1 - (a ^ b)
                    cls._LUT_LEARN[(a, b, r)] = m
        # Infer LUT
        for a in states:
            for b in states:
                for m in states:
                    r = None
                    if a is not None and b is not None and m is not None:
                        r = a ^ b if m == 1 else 1 - (a ^ b)
                    cls._LUT_INFER[(a, b, m)] = r
        # Deduce A LUT
        for m in states:
            for r in states:
                for b in states:
                    a = None
                    if b is not None and m is not None and r is not None:
                        a = b ^ r if m == 1 else 1 - (b ^ r)
                    cls._LUT_DEDUCE_A[(b, m, r)] = a
        # Deduce B LUT
        for m in states:
            for r in states:
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

    def recursive_synthesis(
        self,
        vectors: List[List[int]]
    ) -> Tuple[List[Optional[int]], List[List[Optional[int]]]]:
        """
        Reduce secuencialmente una lista ≥2 de vectores ternarios.

        Devuelve:
          • resultado_final – vector M después de la última combinación
          • history – lista de cada resultado intermedio (M-k) para depuración
        """
        if len(vectors) < 2:
            raise ValueError("Se necesitan al menos 2 vectores")

        history: List[List[Optional[int]]] = []
        current = vectors[0]

        for nxt in vectors[1:]:
            current, _ = self.synthesize(current, nxt)
            history.append(current)

        return current, history        

# Inicializar las LUTs una sola vez al cargar el script
Trigate._initialize_luts()

class Transcender:
    def relate_vectors(self, A: list, B: list, context: dict = None) -> list:
        """
        Calcula un vector de relación Aurora-native entre A y B, incorporando ventana de contexto y relaciones cruzadas si se proveen.
        """
        if len(A) != len(B):
            return [0, 0, 0]
        diff_vector = []
        for i in range(len(A)):
            a_val = A[i] if A[i] is not None else 0
            b_val = B[i] if B[i] is not None else 0
            diff = b_val - a_val
            # Normalize to ternary: 1 if diff > 0, 0 if diff == 0, None if diff < 0
            if diff > 0:
                diff_vector.append(1)
            elif diff == 0:
                diff_vector.append(0)
            else:
                diff_vector.append(None)
        # --- Aurora-native: ventana de contexto y relaciones cruzadas ---
        # Si context contiene 'prev' y 'next', añade relaciones cruzadas
        if context and 'prev' in context and 'next' in context:
            v_prev = context['prev']
            v_next = context['next']
            rel_cross = []
            for vp, vn in zip(v_prev, v_next):
                vp_val = vp if vp is not None else 0
                vn_val = vn if vn is not None else 0
                diff_cross = vp_val - vn_val
                if diff_cross > 0:
                    rel_cross.append(1)
                elif diff_cross == 0:
                    rel_cross.append(0)
                else:
                    rel_cross.append(None)
            # Concatenar: [diff_vector, rel_cross, A, B]
            return list(diff_vector) + list(rel_cross) + list(A) + list(B)
        return diff_vector
    """
    Componente de síntesis que implementa la síntesis jerárquica
    de Tensores Fractales completos.
    """
    def __init__(self, fractal_vector: Optional[List[int]] = None):
        self.trigate = Trigate()
        # Se guarda por si algún test antiguo lo inspecciona,
        # pero NO es obligatorio para el funcionamiento.
        self.seed_vector = fractal_vector

    def compute_vector_trio(self, A: List[int], B: List[int], C: List[int]) -> Dict[str, Any]:
        """Procesa un trío de vectores simples (operación base)."""
        M_AB, _ = self.trigate.synthesize(A, B)
        M_BC, _ = self.trigate.synthesize(B, C)
        M_CA, _ = self.trigate.synthesize(C, A)
        M_emergent, _ = self.trigate.synthesize(M_AB, M_BC)
        M_intermediate, _ = self.trigate.synthesize(M_emergent, M_CA)
        MetaM = [TernaryLogic.ternary_xor(a, b) for a, b in zip(M_intermediate, M_emergent)]
        return {'M_emergent': M_emergent, 'MetaM': MetaM}
    
        # ------------------------------------------------------------------
    #  MODO “DEEP LEARNING”  (compatibilidad con suites heredadas)
    # ------------------------------------------------------------------
    def deep_learning(
        self,
        A: List[int],
        B: List[int],
        C: List[int],
        M_emergent: Optional[List[int]] = None
    ) -> Dict[str, Any]:
        """
        • Calcula M_emergent y MetaM tal como exige el modelo Trinity-3.
        • Genera R_hipotesis = Trigate.infer(A, B, M_emergent).
        • Devuelve un diccionario con claves que los tests integrales esperan.
        """
        trio = self.compute_vector_trio(A, B, C)

        # Si el caller no aporta M_emergent, usa el calculado.
        if M_emergent is None:
            M_emergent = trio["M_emergent"]

        R_hipotesis = self.trigate.infer(A, B, M_emergent)

        return {
            "M_emergent": M_emergent,
            "MetaM":      trio["MetaM"],
            "R_hipotesis": R_hipotesis,
        }
    


    def compute_full_fractal(self, A: 'FractalTensor', B: 'FractalTensor', C: 'FractalTensor') -> 'FractalTensor':
        """
        Sintetiza tres tensores fractales en uno, de manera jerárquica y elegante.
        Prioriza una raíz de entrada válida por encima de la síntesis.
        """
        out = FractalTensor.neutral()

        def synthesize_trio(vectors: list) -> list:
            # Only use first 3 elements of each vector
            while len(vectors) < 3:
                vectors.append([0, 0, 0])
            trimmed = [v[:3] if isinstance(v, (list, tuple)) else [0,0,0] for v in vectors[:3]]
            r = self.compute_vector_trio(*trimmed)
            m_emergent = r.get('M_emergent', [0, 0, 0])
            return [bit if bit is not None else 0 for bit in m_emergent[:3]]

        inter_from_27 = []
        for i in range(27):
            context = {'prev': A.nivel_27[i - 1] if i > 0 else [0,0,0], 'next': A.nivel_27[i + 1] if i < 26 else [0,0,0]}
            enriched_a = self.relate_vectors(A.nivel_27[i], B.nivel_27[i], context)[:3]
            enriched_b = self.relate_vectors(B.nivel_27[i], C.nivel_27[i], context)[:3]
            enriched_c = self.relate_vectors(C.nivel_27[i], A.nivel_27[i], context)[:3]
            inter_from_27.append(synthesize_trio([enriched_a, enriched_b, enriched_c]))
        out.nivel_27 = inter_from_27

        inter_from_9 = [synthesize_trio(inter_from_27[i:i+3]) for i in range(0, 27, 3)]
        out.nivel_9 = inter_from_9
        out.nivel_3 = [synthesize_trio(inter_from_9[i:i+3]) for i in range(0, 9, 3)]

        # Ensure all nivel_3 vectors are length 3
        out.nivel_3 = [v[:3] if isinstance(v, (list, tuple)) else [0,0,0] for v in out.nivel_3]

        input_roots = [t.nivel_3[0] for t in (A, B, C) if hasattr(t, 'nivel_3') and t.nivel_3 and t.nivel_3[0] and len(t.nivel_3[0]) == 3]
        valid_roots = [r for r in input_roots if all(bit is not None for bit in r)]
        if valid_roots:
            final_root = [0, 0, 0]
            for i in range(3):
                votes = [r[i] for r in valid_roots]
                final_root[i] = 1 if votes.count(1) > votes.count(0) else 0
            out.nivel_3[0] = final_root
            out.Ms = final_root
        return out

# ===============================================================================
# NIVEL 3: ESTRUCTURAS DE DATOS Y CONOCIMIENTO
# ===============================================================================

class FractalTensor:
    """
    Representa un tensor fractal con 3 niveles de profundidad (3, 9, 27).
    """

    def __init__(
        self,
        nivel_3=None,
        nivel_9=None,
        nivel_27=None,
        *,
        Ms=None,
        Ss=None,
        dMs=None
    ):
        def validate_vector(vec, n_blocks):
            # Ensures a list of n_blocks vectors, each of length 3, with only 0/1/None
            if vec is None:
                return [[None]*3 for _ in range(n_blocks)]
            out = []
            for i in range(n_blocks):
                if isinstance(vec, (list, tuple)) and len(vec) > i and isinstance(vec[i], (list, tuple)):
                    v = list(vec[i])[:3]
                    v = [x if x in (0, 1, None) else 0 for x in v]
                    while len(v) < 3:
                        v.append(0)
                    out.append(v)
                else:
                    out.append([0, 0, 0])
            return out

        def expand(vec, n_blocks):
            if vec is None:
                return [[None]*3 for _ in range(n_blocks)]
            if isinstance(vec, (list, tuple)) and len(vec) == n_blocks*3 and not isinstance(vec[0], (list, tuple)):
                # Flat vector, reshape
                return [list(vec[i*3:(i+1)*3]) for i in range(n_blocks)]
            if isinstance(vec, (list, tuple)) and len(vec) == n_blocks and isinstance(vec[0], (list, tuple)):
                # Already block structure
                return [list(x)[:3] for x in vec]
            if isinstance(vec, (list, tuple)) and len(vec) == 1 and isinstance(vec[0], (list, tuple)):
                # Single vector, pad rest
                return [list(vec[0])[:3]] + [[None]*3 for _ in range(n_blocks-1)]
            return [[None]*3 for _ in range(n_blocks)]

        # Handle flat vector of length 27 passed as nivel_3
        if (
            isinstance(nivel_3, (list, tuple)) and
            len(nivel_3) == 27 and
            not isinstance(nivel_3[0], (list, tuple))
        ):
            flat = [x if x in (0, 1, None) else 0 for x in nivel_3]
            self.nivel_3 = [flat[i*9:i*9+3][:3] for i in range(3)]
            self.nivel_9 = [flat[i*3:(i+1)*3] for i in range(9)]
            self.nivel_27 = [[flat[i]]*3 for i in range(27)]
        else:
            self.nivel_3  = validate_vector(expand(nivel_3, 3), 3)
            self.nivel_9  = validate_vector(expand(nivel_9, 9), 9)
            self.nivel_27 = validate_vector(expand(nivel_27, 27), 27)

        self.Ms  = Ms if Ms is not None else (self.nivel_3[0] if self.nivel_3 and isinstance(self.nivel_3[0], (list, tuple)) and len(self.nivel_3[0]) == 3 else [0,0,0])
        self.Ss  = Ss
        self.dMs = dMs

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
        def short(vs):
            return vs[:2] + ['...'] if len(vs) > 2 else vs
        return (f"FT(root={self.nivel_3}, "
                f"mid={short(self.nivel_9)}, "
                f"detail={short(self.nivel_27)})")

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

    def analyze_fractal_dynamics(
        self,
        temporal_sequence: List["FractalTensor"]
    ) -> "FractalTensor":
        """
        Perspectiva de DINÁMICA: Sintetiza el patrón de evolución de una secuencia
        y calcula el gradiente lógico dMs = Ms_fin XOR Ms_ini.
        """
        if len(temporal_sequence) < 2:
            warnings.warn(
                "Se requiere una secuencia de al menos 2 tensores para analizar dinámicas."
            )
            return (
                FractalTensor.neutral()
                if not temporal_sequence
                else temporal_sequence[0]
            )

        # ---------- síntesis de la secuencia (lo que ya hacías) ----------
        tensor_dyn = self._perform_full_tensor_synthesis(temporal_sequence)

        # ---------- ➊  nuevo: calcular y guardar dMs ----------
        Ms_ini = temporal_sequence[0].Ms or temporal_sequence[0].nivel_3[0]
        Ms_fin = temporal_sequence[-1].Ms or temporal_sequence[-1].nivel_3[0]
        dMs    = [a ^ b for a, b in zip(Ms_ini, Ms_fin)]

        tensor_dyn.dMs = dMs          # gradiente temporal
        tensor_dyn.Ms  = Ms_fin       # Ms más reciente
        tensor_dyn.nivel_3[0] = Ms_fin    # coherencia con la raíz

        return tensor_dyn

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
        self.archetypes = []
        self.ms_index = {}
        self.name_index = {}
        self.coherence_violations = 0
        self.ss_index = {}
        self.models = {}  # Nuevo: modelos genéricos

    def store_model(self, model_name: str, model_data: dict):
        """Almacena un modelo de decisión genérico en este universo."""
        self.models[model_name] = model_data
        return True

    def get_model(self, model_name: str) -> Optional[dict]:
        """Recupera un modelo de decisión."""
        return self.models.get(model_name)

    def add_archetype(self, archetype_tensor: "FractalTensor", Ss: List[int], name: Optional[str] = None, **kwargs) -> bool:
        """Añade un arquetipo (Tensor Fractal) al universo, almacenando Ss (memoria factual)."""
        if not isinstance(archetype_tensor, FractalTensor):
            raise ValueError("La entrada debe ser un objeto FractalTensor.")
        # --- Clave raíz inmutable ---
        ms_key = tuple(archetype_tensor.nivel_3[0][:3])
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
        setattr(archetype_tensor, 'Ss', Ss)
        self.archetypes.append(archetype_tensor)
        self.ms_index[ms_key] = archetype_tensor
        if name: self.name_index[name] = archetype_tensor
        # --- FIX: Usar la clave raíz INMUTABLE para el ss_index ---
        self.ss_index.setdefault(ms_key, []).append(archetype_tensor)
        return True

    def find_archetype_by_ms(self, Ms_query: List[int]) -> Optional["FractalTensor"]:
        """Busca un arquetipo por su clave Ms (vector raíz, normalizado a 3 ints)."""
        return self.ms_index.get(tuple(Ms_query[:3]))
    
    def find_archetype_by_name(self, name: str) -> Optional["FractalTensor"]:
        """Busca un arquetipo por su nombre asignado."""
        return self.name_index.get(name)

    def register_patch(self, ms_key, ttl=10_000):
        """Registra un parche temporal para un vector raíz con TTL."""
        if not hasattr(self, '_patches'):
            self._patches = {}
        self._patches[tuple(ms_key)] = {'ttl': ttl, 'timestamp': time.time()}

    def supersede_axiom(self, ms_key, new_axiom):
        """Reemplaza el axioma raíz y versiona el anterior."""
        if not hasattr(self, '_axiom_versions'):
            self._axiom_versions = {}
        old = self.ms_index.get(tuple(ms_key))
        if old:
            self._axiom_versions[tuple(ms_key)] = old
        self.ms_index[tuple(ms_key)] = new_axiom
        # También actualizar en archetypes si está
        for i, t in enumerate(self.archetypes):
            if t.nivel_3[0] == list(ms_key):
                self.archetypes[i] = new_axiom
                break

class FractalKnowledgeBase:
    def store_model(self, space_id: str, model_name: str, model_data: dict):
        return self._get_space(space_id).store_model(model_name, model_data)

    def get_model(self, space_id: str, model_name: str):
        return self._get_space(space_id).get_model(model_name)
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
    
    # ------------------------------------------------------------------
    #  API LEGACY – requerido por tests antiguos (Aurora Integral)
    # ------------------------------------------------------------------
    # ------------------------------------------------------------------
    #  API LEGACY – requerido por tests integrales antiguos
    # ------------------------------------------------------------------
    def add_entry(
        self,
        A,
        B,
        C,
        M_emergent,
        MetaM,
        R_validos=None,            # ← OPCIONAL
        transcender_id=None,
        space_id: str = "default"
    ):
        """
        Guarda una entrada completa (A, B, C, M, MetaM, R) y la indexa
        mínimamente para trazabilidad.  Todos los campos, salvo los tres
        primeros, son opcionales.
        """
        if not hasattr(self, "_entries"):
            self._entries = []

        self._entries.append(
            {
                "A": A,
                "B": B,
                "C": C,
                "M_emergent": M_emergent,
                "MetaM": MetaM,
                "R_validos": R_validos,
                "transcender_id": transcender_id,
                "timestamp": time.time(),
            }
        )
        # —– opcional: generar un arquetipo “stub” para futuras búsquedas
        Ms = M_emergent if isinstance(M_emergent, list) else [0, 0, 0]
        tensor_stub = FractalTensor(nivel_3=[Ms, [None]*3, [None]*3], Ss=Ms)
        self.add_archetype(space_id, transcender_id or f"entry_{len(self._entries)}", tensor_stub, Ss=Ms)
        return True

    


 # ===================== MÓDULO DE EVOLVER INVERSO =====================
class InverseEvolver:
    def __init__(self):
        self.trigate = Trigate()

    def infer_inputs_from_meta(self, Ms: list, MetaM: list) -> list:
        """
        Dado Ms (emergente) y MetaM, deduce M_AB, M_BC, M_CA compatibles.
        """
        M_intermediate = [TernaryLogic.ternary_xor(m, mm) for m, mm in zip(Ms, MetaM)]
        # Heurística simple: replicamos M_AB = M_BC = M_CA = M_intermediate
        return [M_intermediate, M_intermediate, M_intermediate]

    def reconstruct_vectors(self, Ms: list) -> tuple:
        """
        Deduce A y B posibles que generen Ms usando lógica inversa del Trigate.
        """
        # Heurística simple: A = [0,0,0], deduce B
        A = [0, 0, 0]
        # Deducimos B desde A y Ms usando inferencia inversa
        B = [self.trigate._LUT_DEDUCE_B.get((a, 1, m), 0) for a, m in zip(A, Ms)]
        return A, B

# ===================== NUEVO EXTENDER: CONSEJO DE EXPERTOS =====================

class Extender:
    """
    Orquestador Aurora refactorizado con expertos como métodos internos para
    simplificar el alcance y la gestión de estado.
    """
    def __init__(self, knowledge_base: "FractalKnowledgeBase"):
        self.kb = knowledge_base
        self.transcender = Transcender()  # El relator necesita un transcender

    # --- Experto Arquetipo como método ---
    def _validate_archetype(self, ss_query: list, space_id: str) -> Tuple[bool, Optional['FractalTensor']]:
        universe = self.kb._get_space(space_id)
        ss_query_fixed = tuple(int(0 if x is None else x) for x in ss_query[:3])
        # Búsqueda primaria y única, ya que ambos índices usan la misma clave
        exact_match_list = universe.ss_index.get(ss_query_fixed)
        if exact_match_list:
            return True, exact_match_list[0]
        return False, None

    # --- Experto Dinámica como método ---
    def _project_dynamics(self, ss_query: list, space_id: str) -> Tuple[bool, Optional['FractalTensor']]:
        universe = self.kb._get_space(space_id)
        best, best_sim = None, -1.0
        for archetype in universe.archetypes:
            dMs = getattr(archetype, 'dMs', None)
            if dMs and getattr(archetype, 'Ss', None):
                sim = sum(1 for a, b in zip(archetype.Ss, ss_query) if a == b) / len(ss_query)
                if sim > best_sim:
                    best_sim, best = sim, archetype
        if best and best_sim > 0.7:
            return True, best
        return False, None

    # --- Experto Relator como método ---
    def _contextualize_relations(self, ss_query: list, space_id: str) -> Tuple[bool, Optional['FractalTensor']]:
        universe = self.kb._get_space(space_id)
        if not universe.archetypes:
            return False, None
        best, best_score = None, float('-inf')
        for archetype in universe.archetypes:
            if not getattr(archetype, 'Ss', None):
                continue
            rel = self.transcender.relate_vectors(ss_query, archetype.Ss)
            score = sum(1 for bit in rel if bit == 0) 
            if score > best_score:
                best_score, best = score, archetype
        if best:
            return True, best
        return False, None

    # --- Orquestador Principal ---
    def extend_fractal(self, input_ss, contexto: dict) -> dict:
        log = [f"Extensión Aurora: espacio '{contexto.get('space_id', 'default')}'"]
        if isinstance(input_ss, FractalTensor):
            ss_query = getattr(input_ss, 'Ss', input_ss.nivel_3[0])
        else:
            ss_query = input_ss
        if not isinstance(ss_query, list) or len(ss_query) < 3:
            ss_query = [0, 0, 0]
        ss_query = [int(0 if x is None else x) for x in ss_query[:3]]
        space_id = contexto.get('space_id', 'default')

        # 1. Búsqueda de axioma
        ok_axioma, archi = self._validate_archetype(ss_query, space_id)
        if ok_axioma and archi:
            log.append(f"✅ Arquetipo encontrado: '{getattr(archi, 'metadata', {}).get('name', 'sin nombre')}'.")
            return {
                "reconstructed_tensor": copy.deepcopy(archi),
                "reconstruction_method": "reconstrucción por arquetipo (axioma)",
                "log": log
            }

        # 2. Si no hay axioma, consultar a otros expertos
        log.append("❌ No se encontró axioma. Consultando expertos...")
        ok_dyn, dyn = self._project_dynamics(ss_query, space_id)
        if ok_dyn and dyn:
            log.append("✅ Dinámica encontrada. Usando proyección.")
            dyn.nivel_3[0] = ss_query # Preservar raíz
            return {
                "reconstructed_tensor": dyn,
                "reconstruction_method": "proyección por dinámica (raíz preservada)",
                "log": log
            }

        ok_rel, rel = self._contextualize_relations(ss_query, space_id)
        if ok_rel and rel:
            log.append("✅ Relación contextual encontrada. Usando relator.")
            rel.nivel_3[0] = ss_query # Preservar raíz
            return {
                "reconstructed_tensor": rel,
                "reconstruction_method": "contextualización por relator (raíz preservada)",
                "log": log
            }
            
        # 3. Último recurso
        log.append("🤷 No se encontraron coincidencias. Devolviendo tensor neutro.")
        return {
            "reconstructed_tensor": FractalTensor.neutral(),
            "reconstruction_method": "tensor neutro (sin coincidencias)",
            "log": log
        }

    # --- LUT methods moved into Extender as proper methods ---
    def lookup_lut(self, space_id: str, ss_query: list):
        """
        Consulta la LUT para el espacio dado y la firma ss_query.
        """
        lut = getattr(self, '_lut_tables', {}).get(space_id, None)
        if lut is None:
            return None
        key = tuple(ss_query)
        return lut.get(key, None)

    def learn_lut_from_data(self, space_id: str, data: list):
        """
        Aprende una LUT auto-didacta a partir de datos [(ss_query, tensor_result)].
        Si hay conflicto, usa voto por mayoría.
        """
        lut = {}
        votes = {}
        for ss_query, tensor_result in data:
            key = tuple(ss_query)
            if key not in votes:
                votes[key] = []
            votes[key].append(tensor_result)
        # Votar por mayoría (por nivel_3[0])
        for key, tensors in votes.items():
            # Si solo hay uno, usarlo
            if len(tensors) == 1:
                lut[key] = tensors[0]
            else:
                # Votar por mayoría en nivel_3[0]
                root_votes = [t.nivel_3[0] if hasattr(t, 'nivel_3') else t for t in tensors]
                # Simple: moda por componente
                majority = []
                for i in range(3):
                    vals = [rv[i] for rv in root_votes if rv and len(rv) > i]
                    if vals:
                        count_1 = vals.count(1)
                        count_0 = vals.count(0)
                        if count_1 > count_0:
                            majority.append(1)
                        elif count_0 > count_1:
                            majority.append(0)
                        else:
                            majority.append(None)
                    else:
                        majority.append(None)
                # Crear tensor neutro y ponerle la raíz votada
                tensor_majority = FractalTensor.neutral()
                tensor_majority.nivel_3[0] = majority
                lut[key] = tensor_majority
        self.patch_lut(space_id, lut)
        return lut

    def patch_lut(self, space_id, lut):
        """Actualiza o crea la LUT para el espacio dado."""
        if not hasattr(self, '_lut_tables') or self._lut_tables is None:
            self._lut_tables = {}
        self._lut_tables[space_id] = lut

    def vote_candidates(self, candidates: list):
        """
        Vota entre varios tensores candidatos y devuelve el tensor con mayoría en la raíz.
        """
        if not candidates:
            return FractalTensor.neutral()
        root_votes = [c.nivel_3[0] if hasattr(c, 'nivel_3') else c for c in candidates]
        majority = []
        for i in range(3):
            vals = [rv[i] for rv in root_votes if rv and len(rv) > i]
            if vals:
                count_1 = vals.count(1)
                count_0 = vals.count(0)
                if count_1 > count_0:
                    majority.append(1)
                elif count_0 > count_1:
                    majority.append(0)
                else:
                    majority.append(None)
            else:
                majority.append(None)
        tensor_majority = FractalTensor.neutral()
        tensor_majority.nivel_3[0] = majority
        return tensor_majority

# Move these expert classes to top-level scope
class ExpertArquetipo:
    def __init__(self, kb):
        self.kb = kb
    def validar_axioma(self, ss_query, space_id):
        """
        Valida si existe un axioma. Es más robusto:
        1. Busca por Ss (memoria factual) en ss_index.
        2. Si falla, busca por Ms (raíz) en ms_index.
        """
        universe = self.kb._get_space(space_id)
        # --- FIX: Normalización de tipo reforzada con int() ---
        ss_query_fixed = tuple(int(0 if x is None else x) for x in ss_query[:3])
        # Búsqueda primaria por Ss/Ms en el índice (ahora ambos usan la misma clave)
        exact_match_list = universe.ss_index.get(ss_query_fixed)
        if exact_match_list:
            return True, exact_match_list[0]
        # Búsqueda de respaldo (aunque debería ser redundante si el índice es el mismo)
        exact_by_ms = universe.find_archetype_by_ms(list(ss_query_fixed))
        if exact_by_ms:
            return True, exact_by_ms
        return False, None

class ExpertDinamica:
    def __init__(self, kb):
        self.kb = kb
    def proyectar_dinamica(self, ss_query, space_id):
        # Busca tensor con dMs compatible o genera proyección neutra
        universe = self.kb._get_space(space_id)
        best, best_sim = None, 0.0
        for archetype in universe.archetypes:
            dMs = getattr(archetype, 'dMs', None)
            if dMs:
                sim = sum(1 for a, b in zip(getattr(archetype, 'Ss', []), ss_query) if a == b) / len(ss_query)
                if sim > best_sim:
                    best_sim, best = sim, archetype
        if best and best_sim > 0.7:
            return True, best
        return False, None

class ExpertRelator:
    def __init__(self, kb):
        self.kb = kb
        self.transcender = Transcender()
    def contextualizar(self, ss_query, space_id):
        # Busca relaciones semánticas entre ss_query y todos los arquetipos
        universe = self.kb._get_space(space_id)
        best, best_score = None, float('-inf')
        for archetype in universe.archetypes:
            rel = self.transcender.relate_vectors(ss_query, getattr(archetype, 'Ss', [0,0,0]))
            score = -sum(abs(x) if x is not None else 0 for x in rel)
            if score > best_score:
                best_score, best = score, archetype
        if best:
            return True, best
        return False, None


# ===================== MÓDULO DE ROTACIÓN DE TENSORES (ARC - Aurean Rotation Cycle)
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


KnowledgeBase = FractalKnowledgeBase


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
    # Initialize LUT for archetype
    extender._lut_tables = {'fisica_conceptual': {(1, 0, 1): arquetipo_movimiento}}
    extender.learn_lut_from_data('fisica_conceptual', [([1, 0, 1], arquetipo_movimiento)])

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
    
    # Construir el tensor incompleto solo con la raíz (primer vector de nivel_3)
    tensor_incompleto = FractalTensor(nivel_3=[arquetipo_movimiento.nivel_3[0][:3], [None, None, None], [None, None, None]])
    print(f"• Tensor a extender (solo con raíz): {tensor_incompleto}")

    # Extensión robusta: la función copiará todos los niveles del arquetipo encontrado
    resultado_extension = extender.extend_fractal(
        tensor_incompleto,
        contexto={'space_id': 'fisica_conceptual'}
    )

    tensor_reconstruido = resultado_extension['reconstructed_tensor']
    print(f"• Método de reconstrucción: {resultado_extension['reconstruction_method']}")
    print(f"• Tensor reconstruido: {tensor_reconstruido}")
    print("  └─ Los niveles 3, 9 y 27 se han rellenado desde la KB.")

    print("\n" + "=" * 80)
    print("🎯 DEMOSTRACIÓN FINALIZADA.")
    print("=" * 80)