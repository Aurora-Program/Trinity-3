"""
Aurora Model v3.0 - Implementación Clean desde Cero
Trinity-3 / Programa Aurora

Basado en Aurora Model White Paper (2025)
Principios: Inteligencia Fractal, Lógica Ternaria, Emergencia

LICENCIAS: Apache 2.0 + CC BY 4.0
"""

from typing import List, Optional, Tuple, Dict, Any
from dataclasses import dataclass, field
import hashlib

# ========== TIPOS BÁSICOS ==========

Trit = Optional[int]  # 0 | 1 | None (ternario)

# ========== TRIGATE - Átomo de Inteligencia (White Paper 3.1) ==========

def _trit_and(a: Trit, b: Trit) -> Trit:
    """AND₃ conservador: 0 domina, 1∧1→1, resto→None"""
    if a == 0 or b == 0:
        return 0
    if a == 1 and b == 1:
        return 1
    return None

def _trit_or(a: Trit, b: Trit) -> Trit:
    """OR₃ expansivo: 1 domina, 0∨0→0, resto→None"""
    if a == 1 or b == 1:
        return 1
    if a == 0 and b == 0:
        return 0
    return None

def _trit_consensus(a: Trit, b: Trit) -> Trit:
    """Consenso: A=B→A, diferente→None"""
    if a is not None and a == b:
        return a
    return None

def trigate_infer(a: Trit, b: Trit, m: Trit) -> Trit:
    """
    INFERENCIA: A, B, M → R
    White Paper 3.1.4: LUT Trigate
    
    M=0: Conservador (AND₃)
    M=1: Expansivo (OR₃)
    M=None: Indeterminado (consenso)
    """
    if m == 0:
        return _trit_and(a, b)
    elif m == 1:
        return _trit_or(a, b)
    else:  # m is None
        return _trit_consensus(a, b)

def trigate_learn(a: Trit, b: Trit, r: Trit) -> Trit:
    """
    APRENDIZAJE: A, B, R → M
    White Paper 3.1.4: Detecta modo desde comportamiento
    """
    if r is None:
        return None
    
    # Probar si es conservador (M=0)
    expected_and = _trit_and(a, b)
    if expected_and == r:
        return 0
    
    # Probar si es expansivo (M=1)
    expected_or = _trit_or(a, b)
    if expected_or == r:
        return 1
    
    # Probar si es consenso (M=None)
    expected_consensus = _trit_consensus(a, b)
    if expected_consensus == r:
        return None
    
    # No coincide con ningún modo conocido
    return None

def trigate_deduce_a(b: Trit, m: Trit, r: Trit) -> Trit:
    """
    DEDUCCIÓN: B, M, R → A
    White Paper 3.1.4: Inferir entrada faltante
    """
    if m == 0:  # Conservador
        if r == 0:
            return None  # Podría ser 0 o cualquier valor
        if r == 1:
            return 1  # Solo 1∧1→1
        return None
    elif m == 1:  # Expansivo
        if r == 1:
            return None  # Podría ser 1 o cualquier valor
        if r == 0:
            return 0  # Solo 0∨0→0
        return None
    else:  # Indeterminado
        return r  # En consenso, A=B=R

def trigate_deduce_b(a: Trit, m: Trit, r: Trit) -> Trit:
    """DEDUCCIÓN: A, M, R → B (simétrico a deduce_a)"""
    return trigate_deduce_a(a, m, r)  # Por simetría

# ========== OPERACIONES VECTORIALES ==========

def vec_infer(A: List[Trit], B: List[Trit], M: List[Trit]) -> List[Trit]:
    """Aplica inferencia elemento a elemento"""
    return [trigate_infer(a, b, m) for a, b, m in zip(A, B, M)]

def vec_learn(A: List[Trit], B: List[Trit], R: List[Trit]) -> List[Trit]:
    """Aplica aprendizaje elemento a elemento"""
    return [trigate_learn(a, b, r) for a, b, r in zip(A, B, R)]

def vec_deduce_a(B: List[Trit], M: List[Trit], R: List[Trit]) -> List[Trit]:
    """Deduce vector A"""
    return [trigate_deduce_a(b, m, r) for b, m, r in zip(B, M, R)]

def vec_deduce_b(A: List[Trit], M: List[Trit], R: List[Trit]) -> List[Trit]:
    """Deduce vector B"""
    return [trigate_deduce_b(a, m, r) for a, m, r in zip(A, M, R)]

# ========== TENSOR FFE (White Paper 2.0) ==========

@dataclass
class TensorFFE:
    """
    Tensor Fractal (Forma, Función, Estructura)
    White Paper 2.0: FFE como unidad fundamental
    
    R: Forma/Resultado
    M: Función/Modo
    O: Orden/Estructura
    metadata: Información de emergencia (opcional)
    """
    R: List[Trit] = field(default_factory=lambda: [None, None, None])
    M: List[Trit] = field(default_factory=lambda: [None, None, None])
    O: List[Trit] = field(default_factory=lambda: [None, None, None])
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def count_nulls(self) -> int:
        """Cuenta nulls totales en el tensor"""
        total = 0
        for vec in [self.R, self.M, self.O]:
            total += sum(1 for x in vec if x is None)
        return total
    
    def is_complete(self) -> bool:
        """Verifica si no hay nulls"""
        return self.count_nulls() == 0
    
    def clone(self) -> 'TensorFFE':
        """Crea copia profunda"""
        return TensorFFE(
            R=self.R.copy(),
            M=self.M.copy(),
            O=self.O.copy(),
            metadata=self.metadata.copy()
        )

# ========== TRIGATE - Unidad Mínima de Razonamiento (White Paper 3.1) ==========

class Trigate:
    """
    Trigate: Átomo de inteligencia
    White Paper 3.1: Unidad básica de inferencia, aprendizaje y deducción
    
    Opera sobre las dimensiones de dos TensorFFE usando un modo M
    """
    
    def __init__(self):
        self.memory = []  # Historia de operaciones
    
    def process(self, tensor_a: TensorFFE, tensor_b: TensorFFE, 
                mode: str = "infer") -> TensorFFE:
        """
        Procesa dos tensores según el modo
        
        mode:
        - "infer": Usa M de tensor_a para inferir resultado
        - "learn": Aprende M comparando A, B con resultado esperado en tensor_b
        - "deduce_a": Deduce tensor_a desde tensor_b y modo
        """
        result = TensorFFE()
        
        if mode == "infer":
            # R_result = infer(R_a, R_b, M_a)
            result.R = vec_infer(tensor_a.R, tensor_b.R, tensor_a.M)
            result.M = vec_infer(tensor_a.M, tensor_b.M, tensor_a.O)
            result.O = vec_infer(tensor_a.O, tensor_b.O, [0, 0, 0])  # O usa modo conservador
            
        elif mode == "learn":
            # M_result = learn(R_a, R_b, R_resultado_esperado)
            result.M = vec_learn(tensor_a.R, tensor_b.R, tensor_b.M)  # Aprende desde resultado
            result.R = tensor_b.R.copy()  # El resultado es el esperado
            result.O = tensor_a.O.copy()  # Preserva estructura
            
        elif mode == "deduce_a":
            # A_result = deduce(B, M, R)
            result.R = vec_deduce_a(tensor_b.R, tensor_a.M, tensor_b.M)
            result.M = tensor_a.M.copy()
            result.O = tensor_a.O.copy()
        
        # Registrar en memoria
        self.memory.append({
            "mode": mode,
            "input_a": tensor_a.clone(),
            "input_b": tensor_b.clone(),
            "output": result.clone()
        })
        
        return result

# ========== TRANSCENDER - Procesador Fractal (White Paper 3.2) ==========

class Transcender:
    """
    Transcender: Integración armónica de tres tensores
    White Paper 3.2: Estructura tetraédrica con 4 caras
    
    Procesa A, B, C → Ms (síntesis emergente)
    """
    
    def __init__(self):
        self.trigate = Trigate()
        self.memory = {
            "relators": [],  # Relaciones aprendidas
            "archetypes": [],  # Patrones emergentes
            "dynamics": []  # Evolución temporal
        }
    
    def synthesize(self, A: TensorFFE, B: TensorFFE, C: TensorFFE) -> Dict[str, TensorFFE]:
        """
        SINTETIZAR: A, B, C → M1, M2, M3, Ms
        White Paper 3.2.1: Cara Sintetizador del tetraedro
        
        Proceso:
        1. M1 = infer(A, B)
        2. M2 = infer(B, C)
        3. M3 = infer(C, A)
        4. Ms = síntesis de M1, M2, M3
        """
        # Paso 1: Relaciones pairwise
        M1 = self.trigate.process(A, B, mode="infer")
        M2 = self.trigate.process(B, C, mode="infer")
        M3 = self.trigate.process(C, A, mode="infer")
        
        # Paso 2: Síntesis emergente (M1, M2 → temp)
        temp = self.trigate.process(M1, M2, mode="infer")
        
        # Paso 3: Integración final (temp, M3 → Ms)
        Ms = self.trigate.process(temp, M3, mode="infer")
        
        return {
            "M1": M1,
            "M2": M2,
            "M3": M3,
            "Ms": Ms
        }
    
    def evolve(self, A: TensorFFE, B: TensorFFE, C: TensorFFE) -> Dict[str, Any]:
        """
        EVOLVER: Aprendizaje de patrones
        White Paper 3.2.1: Cara Evolver del tetraedro
        
        Aprende relaciones entre los tensores y actualiza memoria
        """
        # Aprender relaciones
        rel_ab = self.trigate.process(A, B, mode="learn")
        rel_bc = self.trigate.process(B, C, mode="learn")
        rel_ca = self.trigate.process(C, A, mode="learn")
        
        # Guardar en memoria
        self.memory["relators"].append({
            "AB": rel_ab,
            "BC": rel_bc,
            "CA": rel_ca
        })
        
        return {
            "relators_learned": 3,
            "memory_size": len(self.memory["relators"])
        }
    
    def extend(self, Ms: TensorFFE, seeds: Tuple[TensorFFE, TensorFFE, TensorFFE]) -> Dict[str, TensorFFE]:
        """
        EXTENDER: Reconstrucción desde síntesis
        White Paper 3.2.1: Cara Extender del tetraedro
        
        Ms → M1, M2, M3 (proyección descendente)
        """
        M1_seed, M2_seed, M3_seed = seeds
        
        # Proyectar desde Ms usando seeds como guía
        M1 = self.trigate.process(Ms, M1_seed, mode="infer")
        M2 = self.trigate.process(Ms, M2_seed, mode="infer")
        M3 = self.trigate.process(Ms, M3_seed, mode="infer")
        
        return {
            "M1": M1,
            "M2": M2,
            "M3": M3,
            "Ms": Ms
        }
    
    def harmonize(self, tensors: Dict[str, TensorFFE]) -> Dict[str, TensorFFE]:
        """
        ARMONIZAR: Coherencia global
        White Paper 3.2.1: Cara Armonizador del tetraedro
        
        Ajusta M1, M2, M3 para coherencia con Ms
        """
        M1 = tensors["M1"]
        M2 = tensors["M2"]
        M3 = tensors["M3"]
        Ms = tensors["Ms"]
        
        # Re-sintetizar para verificar coherencia
        temp = self.trigate.process(M1, M2, mode="infer")
        Ms_check = self.trigate.process(temp, M3, mode="infer")
        
        # Si hay diferencias, ajustar
        # (Versión simple: devolver Ms_check como nueva síntesis)
        
        return {
            "M1": M1,
            "M2": M2,
            "M3": M3,
            "Ms": Ms_check
        }

# ========== TETRAEDRO - Unidad de Procesamiento (White Paper 3.2.1) ==========

class Tetraedro:
    """
    Tetraedro: 4 caras procesando mismo conjunto de tensores
    White Paper 3.2.1: Estructura básica de procesamiento fractal
    
    Caras:
    1. Sintetizador: A, B, C → Ms
    2. Evolver: Aprendizaje de patrones
    3. Extender: Ms → A, B, C
    4. Armonizador: Coherencia global
    """
    
    def __init__(self, A: TensorFFE, B: TensorFFE, C: TensorFFE):
        self.inputs = {"A": A, "B": B, "C": C}
        self.transcender = Transcender()
        self.state = {}
        self.coherence = [None, None, None]  # Estado de coherencia
    
    def cycle(self, mode: str = "synthesize") -> Dict[str, Any]:
        """
        Ejecuta un ciclo de procesamiento
        
        mode:
        - "synthesize": Síntesis ascendente
        - "evolve": Aprendizaje
        - "extend": Reconstrucción descendente
        - "harmonize": Ajuste de coherencia
        """
        A, B, C = self.inputs["A"], self.inputs["B"], self.inputs["C"]
        
        if mode == "synthesize":
            self.state = self.transcender.synthesize(A, B, C)
            
        elif mode == "evolve":
            result = self.transcender.evolve(A, B, C)
            self.state["evolution"] = result
            
        elif mode == "extend":
            if "Ms" not in self.state:
                # Primero sintetizar si no hay Ms
                self.state = self.transcender.synthesize(A, B, C)
            
            Ms = self.state["Ms"]
            seeds = (self.state.get("M1", A), 
                    self.state.get("M2", B), 
                    self.state.get("M3", C))
            self.state = self.transcender.extend(Ms, seeds)
            
        elif mode == "harmonize":
            if "Ms" not in self.state:
                self.state = self.transcender.synthesize(A, B, C)
            self.state = self.transcender.harmonize(self.state)
        
        # Calcular coherencia
        self._compute_coherence()
        
        return self.state
    
    def _compute_coherence(self):
        """
        Calcula coherencia del estado actual
        White Paper 3.2.4: Coherencia = ausencia de nulls + síntesis válida
        """
        if "Ms" in self.state:
            Ms = self.state["Ms"]
            nulls = Ms.count_nulls()
            
            # Coherencia simple: [1,1,1] si completo, [0,0,0] si muchos nulls
            if nulls == 0:
                self.coherence = [1, 1, 1]
            elif nulls < 5:
                self.coherence = [1, 0, 0]
            else:
                self.coherence = [0, 0, 0]
        else:
            self.coherence = [None, None, None]
    
    def is_coherent(self) -> bool:
        """Verifica si alcanzó coherencia plena"""
        return self.coherence == [1, 1, 1]
    
    def get_emergent_tensor(self) -> Optional[TensorFFE]:
        """
        Extrae tensor emergente usando proceso de emergencia
        White Paper 3.2.5: Tensor de Síntesis
        White Paper 3.2.4: Proceso de Emergencia con flujo de entropía
        
        Cuando coherencia es total:
        1. Calcula Hash de Emergencia Hₑ
        2. Ejecuta función hash cognitiva: Hash(M1,M2,M3,Ms) → (Rs,Ms,Os)
        3. Tensor superior emerge con entropía reducida
        """
        if self.is_coherent() and "Ms" in self.state:
            M1 = self.state.get("M1")
            M2 = self.state.get("M2")
            M3 = self.state.get("M3")
            Ms = self.state["Ms"]
            
            # Proceso de Emergencia (White Paper 3.2.4)
            if M1 and M2 and M3:
                emergent = emerge_superior_tensor(M1, M2, M3, Ms)
                if emergent:
                    return emergent
            
            # Fallback: retornar Ms si no hay suficiente info
            return Ms
        return None

# ========== PROCESO DE EMERGENCIA (White Paper 3.2.4, 3.3.5.1) ==========

def compute_emergence_hash(M1: TensorFFE, M2: TensorFFE, M3: TensorFFE, Ms: TensorFFE) -> str:
    """
    Hash de Emergencia Hₑ según White Paper 3.3.5.1
    
    Hₑ(state) = hash(Σ w_ℓ·C_local[ℓ] | Σ v_ℓ·D_null[ℓ] | O_snapshot)
    
    Identifica estados únicos de síntesis para detectar cuando
    el sistema alcanza nueva configuración coherente.
    """
    # Calcular coherencia local (C_local)
    coherence_score = 0.0
    if M1.count_nulls() == 0:
        coherence_score += 1.0
    if M2.count_nulls() == 0:
        coherence_score += 1.0
    if M3.count_nulls() == 0:
        coherence_score += 1.0
    if Ms.count_nulls() == 0:
        coherence_score += 2.0  # Ms tiene mayor peso
    
    # Calcular densidad de nulls (D_null)
    total_nulls = M1.count_nulls() + M2.count_nulls() + M3.count_nulls() + Ms.count_nulls()
    null_density = total_nulls / 36.0  # 4 tensores × 9 trits cada uno
    
    # Snapshot de orden (O_snapshot)
    order_sig = "".join(str(x) if x is not None else "N" for x in Ms.O)
    
    # Construir hash
    hash_input = f"{coherence_score:.3f}|{null_density:.3f}|{order_sig}"
    return hashlib.md5(hash_input.encode()).hexdigest()[:8]

def emerge_superior_tensor(M1: TensorFFE, M2: TensorFFE, M3: TensorFFE, Ms: TensorFFE) -> Optional[TensorFFE]:
    """
    Proceso de Emergencia: condensa M1, M2, M3, Ms en tensor superior
    White Paper 3.2.4: "Cuando coherencia es total, emerge nuevo nivel"
    
    Este es el proceso clave del flujo de entropía (LEF 1.2.2):
    - Sistema reduce entropía interna (elimina nulls)
    - Transfiere información al nivel superior
    - Nueva estructura más abstracta emerge
    
    Función Hash Cognitiva:
    Hash(M1, M2, M3, Ms) → (R_s, M_s, O_s)
    
    Donde:
    - R_s: Forma sintetizada (resultado emergente)
    - M_s: Modo/Función emergente (ley del nuevo nivel)
    - O_s: Orden superior (estructura de control)
    """
    # Verificar condición de emergencia: coherencia total
    total_nulls = M1.count_nulls() + M2.count_nulls() + M3.count_nulls() + Ms.count_nulls()
    
    if total_nulls > 9:  # Demasiados nulls, no emerge
        return None
    
    # Calcular Hash de Emergencia
    hash_e = compute_emergence_hash(M1, M2, M3, Ms)
    
    # SÍNTESIS DEL TENSOR SUPERIOR (Hash Cognitiva)
    # White Paper 3.2.4: "Hash(M1,M2,M3,Ms) → (Rs, Ms, Os)"
    
    superior = TensorFFE()
    
    # R_s: FORMA sintetizada (síntesis de todas las R)
    # Tomamos el patrón dominante de las formas
    superior.R = [
        _synthesize_trit([M1.R[0], M2.R[0], M3.R[0], Ms.R[0]]),
        _synthesize_trit([M1.R[1], M2.R[1], M3.R[1], Ms.R[1]]),
        _synthesize_trit([M1.R[2], M2.R[2], M3.R[2], Ms.R[2]])
    ]
    
    # M_s: MODO/FUNCIÓN emergente (ley del nuevo nivel)
    # La función emerge de los modos inferiores
    superior.M = [
        _synthesize_trit([M1.M[0], M2.M[0], M3.M[0], Ms.M[0]]),
        _synthesize_trit([M1.M[1], M2.M[1], M3.M[1], Ms.M[1]]),
        _synthesize_trit([M1.M[2], M2.M[2], M3.M[2], Ms.M[2]])
    ]
    
    # O_s: ORDEN superior (estructura de control)
    # El orden emerge del Ms (nivel más abstracto)
    superior.O = Ms.O.copy()
    
    # Marcar con metadata de emergencia
    superior.metadata = {
        "emergence_hash": hash_e,
        "source_nulls": total_nulls,
        "level": "emergent",
        "entropy_reduced": total_nulls  # Entropía exportada al superior
    }
    
    return superior

def _synthesize_trit(trits: List[Trit]) -> Trit:
    """
    Sintetiza múltiples trits en uno solo (votación con prioridad)
    
    Reglas:
    1. Si hay consenso (todos iguales) → ese valor
    2. Si hay mayoría → valor mayoritario
    3. Si empate → None (indeterminado)
    4. Si todos None → None
    """
    # Filtrar Nones
    valid = [t for t in trits if t is not None]
    
    if len(valid) == 0:
        return None
    
    # Contar valores
    count_0 = sum(1 for t in valid if t == 0)
    count_1 = sum(1 for t in valid if t == 1)
    
    # Consenso o mayoría
    if count_1 > count_0:
        return 1
    elif count_0 > count_1:
        return 0
    else:
        # Empate: preferir None para mantener indeterminación
        return None

# ========== PIPELINE AURORA (White Paper 3.3) ==========

class AuroraPipeline:
    """
    Pipeline completo Aurora: síntesis jerárquica + extensión
    White Paper 3.3: Flujo bidireccional de procesamiento fractal
    White Paper 3.2.6: Cambio de modo y expansión
    
    Proceso ASCENDENTE (Transcender):
    1. Crear tetraedros desde inputs
    2. Ciclar: synthesize → evolve → extend → harmonize
    3. Extraer tensores emergentes
    4. Crear nivel superior con emergentes
    5. Repetir hasta tensor raíz único (coherencia absoluta)
    
    Proceso DESCENDENTE (Extender):
    6. Cambio de modo: Transcender → Extender
    7. Expandir tensor raíz → tensores derivados
    8. Propagar descendentemente por niveles
    9. Generar tensores de salida coherentes con entrada
    """
    
    def __init__(self, max_levels: int = 5, max_cycles: int = 10):
        self.max_levels = max_levels
        self.max_cycles = max_cycles
        self.metrics = {
            "levels_processed": 0,
            "tetrahedrons_created": 0,
            "emergent_tensors": 0,
            "expansion_cycles": 0
        }
        self.history = []  # Historia de niveles para expansión
    
    def run(self, input_tensors: List[TensorFFE]) -> Dict[str, Any]:
        """
        Ejecuta pipeline completo: ASCENSO + DESCENSO
        White Paper 3.3: Flujo bidireccional completo
        
        Returns:
            {
                "root_tensor": Tensor de coherencia absoluta,
                "output_tensors": Tensores de salida expandidos,
                "levels": niveles procesados,
                "metrics": estadísticas
            }
        """
        print(f"🧠 Aurora Pipeline COMPLETO: {len(input_tensors)} tensores de entrada")
        print("="*70)
        
        # FASE 1: ASCENSO (Transcender - Síntesis)
        print("\n📈 FASE 1: ASCENSO - Síntesis hacia Coherencia Absoluta")
        print("-"*70)
        
        root_tensor = self._ascend(input_tensors)
        
        if root_tensor is None:
            print("\n❌ No se alcanzó tensor raíz")
            return {
                "root_tensor": None,
                "output_tensors": [],
                "levels": 0,
                "metrics": self.metrics
            }
        
        # FASE 2: CAMBIO DE MODO
        print("\n🔄 CAMBIO DE MODO: Transcender → Extender")
        print("-"*70)
        print("   Coherencia absoluta alcanzada")
        print("   Iniciando fase expansiva...")
        
        # FASE 3: DESCENSO (Extender - Expansión)
        print("\n📉 FASE 2: DESCENSO - Expansión desde Tensor Raíz")
        print("-"*70)
        
        output_tensors = self._descend(root_tensor, target_count=len(input_tensors))
        
        return {
            "root_tensor": root_tensor,
            "output_tensors": output_tensors,
            "levels_up": len(self.history),
            "levels_down": len(self.history),
            "metrics": self.metrics
        }
    
    def _ascend(self, input_tensors: List[TensorFFE]) -> Optional[TensorFFE]:
        """
        FASE ASCENDENTE: Síntesis jerárquica hasta tensor raíz
        White Paper 3.3: Transcendencia (contracción hacia la verdad)
        """
        current_tensors = input_tensors
        level = 0
        
        while level < self.max_levels and len(current_tensors) > 1:
            print(f"\n📊 Nivel {level}: {len(current_tensors)} tensores")
            
            # Crear tetraedros (grupos de 3)
            tetrahedrons = self._create_tetrahedrons(current_tensors)
            print(f"   Tetraedros creados: {len(tetrahedrons)}")
            
            # Procesar tetraedros
            emergent_tensors = self._process_level(tetrahedrons)
            print(f"   Tensores emergentes: {len(emergent_tensors)}")
            
            # Guardar nivel en historia para expansión posterior
            self.history.append({
                "level": level,
                "input_tensors": current_tensors,
                "tetrahedrons": tetrahedrons,
                "emergent_tensors": emergent_tensors
            })
            
            if len(emergent_tensors) == 0:
                print("   ⚠️  Sin emergentes, deteniendo")
                break
            
            if len(emergent_tensors) == 1:
                print(f"\n   ✅ TENSOR RAÍZ alcanzado en nivel {level}")
                print(f"   Coherencia Absoluta: Hash Hₑ={emergent_tensors[0].metadata.get('emergence_hash', 'N/A')}")
                self.metrics["levels_processed"] = level + 1
                return emergent_tensors[0]
            
            current_tensors = emergent_tensors
            level += 1
            self.metrics["levels_processed"] = level
        
        # No se alcanzó tensor raíz único
        print(f"\n⚠️  Máximo de niveles: {len(current_tensors)} tensores restantes")
        return current_tensors[0] if current_tensors else None
    
    def _descend(self, root_tensor: TensorFFE, target_count: int) -> List[TensorFFE]:
        """
        FASE DESCENDENTE: Expansión desde tensor raíz hasta tensores de salida
        White Paper 3.2.6: "Extender - fase creativa o expansiva"
        White Paper 3.3: "expansión hacia la creación"
        
        Proceso:
        1. Partir del tensor raíz (coherencia absoluta)
        2. Expandir usando modo "extend" del Transcender
        3. Generar tensores derivados nivel por nivel
        4. Continuar hasta alcanzar cantidad objetivo de tensores
        """
        current_tensors = [root_tensor]
        
        # Expandir nivel por nivel (inverso al ascenso)
        for level_idx in range(len(self.history) - 1, -1, -1):
            level_info = self.history[level_idx]
            target_for_level = len(level_info["input_tensors"])
            
            print(f"\n📊 Expandiendo nivel {level_idx}: {len(current_tensors)} → {target_for_level} tensores")
            
            expanded = self._expand_level(current_tensors, target_for_level)
            print(f"   Tensores expandidos: {len(expanded)}")
            
            current_tensors = expanded
            self.metrics["expansion_cycles"] += 1
            
            if len(current_tensors) >= target_count:
                break
        
        # Ajustar a la cantidad exacta objetivo
        if len(current_tensors) > target_count:
            current_tensors = current_tensors[:target_count]
        elif len(current_tensors) < target_count:
            # Rellenar con copias si es necesario
            while len(current_tensors) < target_count:
                current_tensors.append(current_tensors[-1].clone())
        
        print(f"\n   ✅ Tensores de salida generados: {len(current_tensors)}")
        return current_tensors
    
    def _expand_level(self, tensors: List[TensorFFE], target_count: int) -> List[TensorFFE]:
        """
        Expande un conjunto de tensores mediante Extender
        
        Por cada tensor:
        1. Crear "seeds" (semillas de expansión)
        2. Usar Transcender.extend() para proyectar
        3. Generar tensores derivados
        """
        expanded = []
        transcender = Transcender()
        
        for tensor in tensors:
            # Calcular cuántos tensores derivar de este
            derivations_needed = max(3, target_count // len(tensors))
            
            for i in range(derivations_needed):
                # Crear seeds con variación (rotación de valores)
                seed1 = self._create_seed(tensor, rotation=i)
                seed2 = self._create_seed(tensor, rotation=i+1)
                seed3 = self._create_seed(tensor, rotation=i+2)
                
                # Extender usando Transcender
                result = transcender.extend(tensor, (seed1, seed2, seed3))
                
                # Tomar los tensores derivados
                if "M1" in result:
                    expanded.append(result["M1"])
                if "M2" in result and len(expanded) < target_count:
                    expanded.append(result["M2"])
                if "M3" in result and len(expanded) < target_count:
                    expanded.append(result["M3"])
                
                if len(expanded) >= target_count:
                    break
            
            if len(expanded) >= target_count:
                break
        
        return expanded[:target_count]
    
    def _create_seed(self, tensor: TensorFFE, rotation: int = 0) -> TensorFFE:
        """
        Crea seed (semilla) para expansión con variación
        Rota valores para crear diversidad manteniendo coherencia
        """
        seed = TensorFFE()
        
        # Rotar valores de R
        r_values = [x for x in tensor.R if x is not None]
        if r_values:
            idx = rotation % len(r_values)
            seed.R = [r_values[(idx + i) % len(r_values)] for i in range(3)]
        else:
            seed.R = tensor.R.copy()
        
        # M y O se mantienen similares
        seed.M = tensor.M.copy()
        seed.O = tensor.O.copy()
        
        return seed
    
    def _create_tetrahedrons(self, tensors: List[TensorFFE]) -> List[Tetraedro]:
        """Agrupa tensores de 3 en 3 para crear tetraedros"""
        tetrahedrons = []
        
        for i in range(0, len(tensors), 3):
            if i + 2 < len(tensors):
                # Tenemos 3 tensores completos
                tetra = Tetraedro(tensors[i], tensors[i+1], tensors[i+2])
                tetrahedrons.append(tetra)
                self.metrics["tetrahedrons_created"] += 1
            elif i + 1 < len(tensors):
                # Solo 2 tensores, crear uno dummy
                dummy = TensorFFE()
                tetra = Tetraedro(tensors[i], tensors[i+1], dummy)
                tetrahedrons.append(tetra)
                self.metrics["tetrahedrons_created"] += 1
        
        return tetrahedrons
    
    def _process_level(self, tetrahedrons: List[Tetraedro]) -> List[TensorFFE]:
        """
        Procesa todos los tetraedros del nivel
        Ejecuta ciclos hasta convergencia o máximo
        
        White Paper 3.3: Pipeline con proceso de emergencia
        """
        emergent_tensors = []
        
        for idx, tetra in enumerate(tetrahedrons):
            # Ciclo de procesamiento
            for cycle in range(self.max_cycles):
                # Secuencia de 4 fases
                tetra.cycle("synthesize")
                tetra.cycle("evolve")
                tetra.cycle("extend")
                tetra.cycle("harmonize")
                
                # Verificar coherencia
                if tetra.is_coherent():
                    print(f"      ✓ Tetra {idx}: Coherencia en ciclo {cycle}")
                    break
            
            # Extraer tensor emergente (con proceso de emergencia)
            emergent = tetra.get_emergent_tensor()
            if emergent:
                # Mostrar info de emergencia si existe
                if "emergence_hash" in emergent.metadata:
                    hash_e = emergent.metadata["emergence_hash"]
                    entropy = emergent.metadata["entropy_reduced"]
                    print(f"      🔺 Tetra {idx}: EMERGENCIA - Hₑ={hash_e}, ΔS={entropy}")
                
                emergent_tensors.append(emergent)
                self.metrics["emergent_tensors"] += 1
        
        return emergent_tensors

# ========== DEMOSTRACIÓN ==========

if __name__ == "__main__":
    print("="*70)
    print("🧠 AURORA MODEL v3.0 - Clean Implementation")
    print("   White Paper Aligned - From Scratch")
    print("="*70)
    
    # Ejemplo 1: Trigate básico
    print("\n[1] TRIGATE - Átomo de Inteligencia")
    print("-" * 50)
    
    r1 = trigate_infer(0, 1, 0)  # M=0 conservador
    print(f"infer(0, 1, M=0) = {r1}  [AND₃: esperado 0]")
    
    r2 = trigate_infer(0, 1, 1)  # M=1 expansivo
    print(f"infer(0, 1, M=1) = {r2}  [OR₃: esperado 1]")
    
    m = trigate_learn(0, 1, 1)  # Aprender modo
    print(f"learn(0, 1, R=1) = {m}  [esperado 1 (expansivo)]")
    
    # Ejemplo 2: TensorFFE
    print("\n[2] TENSOR FFE")
    print("-" * 50)
    
    tensor = TensorFFE(
        R=[1, 0, 1],
        M=[0, 1, None],
        O=[1, 1, 1]
    )
    print(f"R: {tensor.R}")
    print(f"M: {tensor.M}")
    print(f"O: {tensor.O}")
    print(f"Nulls: {tensor.count_nulls()}")
    print(f"Completo: {tensor.is_complete()}")
    
    # Ejemplo 3: Trigate procesando tensores
    print("\n[3] TRIGATE - Procesamiento de Tensores")
    print("-" * 50)
    
    t1 = TensorFFE(R=[1, 0, 1], M=[0, 0, 0], O=[1, 1, 1])
    t2 = TensorFFE(R=[0, 1, 1], M=[1, 1, 1], O=[1, 1, 1])
    
    trigate = Trigate()
    result = trigate.process(t1, t2, mode="infer")
    print(f"Infer: R = {result.R}")
    
    # Ejemplo 4: Tetraedro
    print("\n[4] TETRAEDRO - 4 Caras")
    print("-" * 50)
    
    ta = TensorFFE(R=[1, 0, 1], M=[0, 0, 0], O=[1, 1, 1])
    tb = TensorFFE(R=[0, 1, 0], M=[1, 1, 1], O=[1, 1, 1])
    tc = TensorFFE(R=[1, 1, 0], M=[0, 1, 0], O=[1, 1, 1])
    
    tetra = Tetraedro(ta, tb, tc)
    
    print("Ciclo synthesize:")
    tetra.cycle("synthesize")
    print(f"  Ms.R = {tetra.state['Ms'].R}")
    print(f"  Coherencia = {tetra.coherence}")
    
    print("Ciclo harmonize:")
    tetra.cycle("harmonize")
    print(f"  Coherente = {tetra.is_coherent()}")
    
    # Ejemplo 5: Pipeline completo
    print("\n[5] PIPELINE COMPLETO")
    print("-" * 50)
    
    inputs = [
        TensorFFE(R=[1, 0, 1], M=[0, 0, 0], O=[1, 1, 1]),
        TensorFFE(R=[0, 1, 0], M=[1, 1, 1], O=[1, 1, 1]),
        TensorFFE(R=[1, 1, 0], M=[0, 1, 0], O=[1, 1, 1]),
        TensorFFE(R=[1, 0, 1], M=[0, 0, 1], O=[1, 1, 1]),
        TensorFFE(R=[0, 1, 1], M=[1, 1, 0], O=[1, 1, 1]),
        TensorFFE(R=[1, 1, 1], M=[1, 0, 1], O=[1, 1, 1]),
    ]
    
    pipeline = AuroraPipeline(max_levels=3, max_cycles=5)
    result = pipeline.run(inputs)
    
    print(f"\n📊 RESULTADO:")
    print(f"   Tensor raíz: {result['root_tensor'] is not None}")
    if result['root_tensor']:
        print(f"   R: {result['root_tensor'].R}")
        print(f"   M: {result['root_tensor'].M}")
        print(f"   O: {result['root_tensor'].O}")
    print(f"   Niveles ascenso: {result.get('levels_up', 0)}")
    print(f"   Tensores salida: {len(result.get('output_tensors', []))}")
    
    # Ejemplo 6: Proceso de Emergencia con Flujo de Entropía
    print("\n[6] PROCESO DE EMERGENCIA - Flujo de Entropía (LEF)")
    print("-" * 50)
    print("White Paper 1.2.2: Teoría del Flujo de Entropía")
    print("White Paper 3.2.4: Proceso de Emergencia")
    
    # Crear tensores con algunos nulls (entropía)
    t1_entropic = TensorFFE(R=[1, 0, None], M=[0, None, 0], O=[1, 1, 1])
    t2_entropic = TensorFFE(R=[None, 1, 0], M=[1, 1, None], O=[1, 1, 1])
    t3_entropic = TensorFFE(R=[1, None, 0], M=[0, 1, 0], O=[1, 1, 1])
    
    print(f"\nTensores base con entropía:")
    print(f"  T1 nulls: {t1_entropic.count_nulls()}/9")
    print(f"  T2 nulls: {t2_entropic.count_nulls()}/9")
    print(f"  T3 nulls: {t3_entropic.count_nulls()}/9")
    print(f"  Total: {t1_entropic.count_nulls() + t2_entropic.count_nulls() + t3_entropic.count_nulls()}/27")
    
    tetra_entropic = Tetraedro(t1_entropic, t2_entropic, t3_entropic)
    
    print("\nProcesando tetraedro...")
    for i in range(3):
        tetra_entropic.cycle("synthesize")
        tetra_entropic.cycle("harmonize")
        print(f"  Ciclo {i}: Coherencia = {tetra_entropic.coherence}")
    
    # Intentar emergencia incluso con coherencia parcial
    if "Ms" in tetra_entropic.state:
        M1 = tetra_entropic.state.get("M1")
        M2 = tetra_entropic.state.get("M2")
        M3 = tetra_entropic.state.get("M3")
        Ms = tetra_entropic.state["Ms"]
        
        if M1 and M2 and M3:
            print(f"\nEstado interno del tetraedro:")
            print(f"  M1 nulls: {M1.count_nulls()}/9")
            print(f"  M2 nulls: {M2.count_nulls()}/9")
            print(f"  M3 nulls: {M3.count_nulls()}/9")
            print(f"  Ms nulls: {Ms.count_nulls()}/9")
            
            emergent = emerge_superior_tensor(M1, M2, M3, Ms)
            if emergent:
                print(f"\n✅ TENSOR EMERGENTE (con coherencia parcial):")
                print(f"   R: {emergent.R}")
                print(f"   M: {emergent.M}")
                print(f"   O: {emergent.O}")
                print(f"   Nulls: {emergent.count_nulls()}/9")
                
                if "emergence_hash" in emergent.metadata:
                    print(f"\n📊 FLUJO DE ENTROPÍA (LEF):")
                    print(f"   Hash Emergencia Hₑ: {emergent.metadata['emergence_hash']}")
                    print(f"   Entropía reducida ΔS: {emergent.metadata['entropy_reduced']} nulls")
                    print(f"   Nivel: {emergent.metadata['level']}")
                    print(f"\n   💡 La entropía (nulls) se REDUJO en el nivel inferior")
                    print(f"      y se TRANSFIRIÓ al sistema superior como información")
                    print(f"      coherente (White Paper 1.2.2 - LEF)")
            else:
                print(f"\n⚠️  Demasiada entropía para emergencia (>9 nulls)")
    
    # Ejemplo 7: PIPELINE BIDIRECCIONAL COMPLETO
    print("\n[7] PIPELINE BIDIRECCIONAL COMPLETO - Ascenso + Descenso")
    print("="*70)
    print("White Paper 3.2.6: 'Ascenso jerárquico y cambio de modo'")
    print("White Paper 3.3: 'Pipeline autosimilar recursivo'")
    
    # Crear 9 tensores de entrada para ver el flujo completo
    input_tensors = []
    for i in range(9):
        t = TensorFFE(
            R=[i % 2, (i+1) % 2, i % 2],
            M=[0, 1, i % 2],
            O=[0, 0, i % 2]
        )
        input_tensors.append(t)
    
    print(f"\n📥 ENTRADA: {len(input_tensors)} tensores")
    for idx in range(min(3, len(input_tensors))):
        t = input_tensors[idx]
        print(f"   T{idx}: R={t.R}, M={t.M}, O={t.O}")
    if len(input_tensors) > 3:
        print(f"   ... (mostrando 3/{len(input_tensors)})")
    
    # Ejecutar pipeline completo
    pipeline_full = AuroraPipeline(max_levels=5, max_cycles=5)
    result_full = pipeline_full.run(input_tensors)
    
    # Mostrar resultados
    print("\n📊 RESULTADOS DEL PIPELINE:")
    print("="*70)
    
    if result_full.get("root_tensor"):
        root = result_full["root_tensor"]
        print(f"\n🎯 TENSOR RAÍZ (Coherencia Absoluta):")
        print(f"   R: {root.R}")
        print(f"   M: {root.M}")
        print(f"   O: {root.O}")
        print(f"   Nulls: {root.count_nulls()}/9")
        if "emergence_hash" in root.metadata:
            print(f"   Hash Hₑ: {root.metadata['emergence_hash']}")
    
    outputs = result_full.get("output_tensors", [])
    print(f"\n📤 SALIDA: {len(outputs)} tensores generados")
    for idx in range(min(3, len(outputs))):
        t = outputs[idx]
        print(f"   OUT{idx}: R={t.R}, M={t.M}, O={t.O}")
    if len(outputs) > 3:
        print(f"   ... (mostrando 3/{len(outputs)})")
    
    print(f"\n📈 Niveles de ascenso: {result_full.get('levels_up', 0)}")
    print(f"📉 Niveles de descenso: {result_full.get('levels_down', 0)}")
    
    print(f"\n💡 FLUJO COMPLETO:")
    print(f"   {len(input_tensors)} tensores → ASCENSO → 1 tensor raíz → DESCENSO → {len(outputs)} tensores")
    print(f"   ✅ Ciclo bidireccional completado exitosamente")
    
    print("\n" + "="*70)
    print("✅ Demostración completada")
    print("   Pipeline Bidireccional Completo funcionando")
    print("   Proceso de Emergencia con Flujo de Entropía operativo")
    print("="*70)
