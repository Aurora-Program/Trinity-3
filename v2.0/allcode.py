
# Este es el código completo de Aurora Trinity-3 para evaluacion de agentes. los modulos existen en el repostoria completa.
# trinity-3: v2.0/allcode.py
# Implementación completa de Aurora Trinity-3
from dataclasses import dataclass
from typing import List, Optional, Tuple

Trit = Optional[int]  # 0 | 1 | None

def _norm3(v: List[Trit]) -> List[Trit]:
    v = list(v)[:3] + [0,0,0]
    return [(None if x is None else (1 if x==1 else 0)) for x in v[:3]]

def _xor(a: Trit, b: Trit) -> Trit:
    return None if (a is None or b is None) else (a ^ b)

def _xnor(a: Trit, b: Trit) -> Trit:
    return None if (a is None or b is None) else (1 if a == b else 0)

@dataclass
class TrigateRecord:
    # Todos los records son SIEMPRE 3 bits
    A: List[Trit]           # entrada 1
    B: List[Trit]           # entrada 2
    M: List[Trit]           # record de aprendizaje (control)
    R: List[Trit]           # salida
    def __post_init__(self):
        self.A, self.B, self.M, self.R = _norm3(self.A), _norm3(self.B), _norm3(self.M), _norm3(self.R)

class Trigate:
    # LUTs ternarias (27 combinaciones por bit) -> O(1)
    _INF, _LRN, _DA, _DB = {}, {}, {}, {}

    @classmethod
    def init_luts(cls):
        vals = [0,1,None]
        # Infer: (a,b,m)->r
        for a in vals:
            for b in vals:
                for m in vals:
                    if None in (a,b,m):
                        r = None
                    else:
                        r = _xor(a,b) if m==1 else (1 - (_xor(a,b)))
                    cls._INF[(a,b,m)] = r
        # Learn: (a,b,r)->m
        for a in vals:
            for b in vals:
                for r in vals:
                    if None in (a,b,r):
                        m = None
                    else:
                        m = 1 if r == _xor(a,b) else 0
                    cls._LRN[(a,b,r)] = m
        # Deduce A: (b,m,r)->a
        for b in vals:
            for m in vals:
                for r in vals:
                    if None in (b,m,r):
                        a = None
                    else:
                        a = (b ^ r) if m==1 else (1 - (b ^ r))
                    cls._DA[(b,m,r)] = a
        # Deduce B: (a,m,r)->b
        for a in vals:
            for m in vals:
                for r in vals:
                    if None in (a,m,r):
                        b = None
                    else:
                        b = (a ^ r) if m==1 else (1 - (a ^ r))
                    cls._DB[(a,m,r)] = b

    # --- Operaciones núcleo (bit a bit) ---
    @staticmethod
    def infer(A: List[Trit], B: List[Trit], M: List[Trit]) -> List[Trit]:
        A,B,M = _norm3(A), _norm3(B), _norm3(M)
        return [Trigate._INF[(a,b,m)] for a,b,m in zip(A,B,M)]

    @staticmethod
    def learn(A: List[Trit], B: List[Trit], R: List[Trit]) -> List[Trit]:
        A,B,R = _norm3(A), _norm3(B), _norm3(R)
        return [Trigate._LRN[(a,b,r)] for a,b,r in zip(A,B,R)]

    @staticmethod
    def deduce_a(B: List[Trit], M: List[Trit], R: List[Trit]) -> List[Trit]:
        B,M,R = _norm3(B), _norm3(M), _norm3(R)
        return [Trigate._DA[(b,m,r)] for b,m,r in zip(B,M,R)]

    @staticmethod
    def deduce_b(A: List[Trit], M: List[Trit], R: List[Trit]) -> List[Trit]:
        A,M,R = _norm3(A), _norm3(M), _norm3(R)
        return [Trigate._DB[(a,m,r)] for a,m,r in zip(A,M,R)]

    # --- Helpers para mantener los 4 records coherentes ---
    @staticmethod
    def from_inputs(A: List[Trit], B: List[Trit], M: List[Trit]) -> TrigateRecord:
        R = Trigate.infer(A,B,M)
        return TrigateRecord(A=_norm3(A), B=_norm3(B), M=_norm3(M), R=_norm3(R))

    @staticmethod
    def from_learning(A: List[Trit], B: List[Trit], R: List[Trit]) -> TrigateRecord:
        M = Trigate.learn(A,B,R)
        return TrigateRecord(A=_norm3(A), B=_norm3(B), M=_norm3(M), R=_norm3(R))

# Inicializar LUTs al cargar el módulo
Trigate.init_luts()



#Transcender.py


from typing import List, Optional, Tuple, Dict
from functools import lru_cache

Trit = Optional[int]  # 0 | 1 | None

# ---------- utilidades ----------
@lru_cache(maxsize=1024)
def norm3_cached(v_tuple: Tuple[Trit, ...]) -> Tuple[Trit, ...]:
    """Versión cacheada de norm3 que trabaja con tuplas inmutables"""
    v_list = list(v_tuple)[:3] + [0, 0, 0]
    out = []
    for x in v_list[:3]:
        if x is None:
            out.append(None)
        else:
            out.append(1 if x == 1 else 0)
    return tuple(out)

def norm3(v: List[Trit]) -> List[Trit]:
    """Normaliza a ternario de longitud 3 (0/1/None)."""
    return list(norm3_cached(tuple(v)))

def fib(n: int) -> int:
    """Fibonacci iterativo (1,1,2,3,5,...)"""
    a, b = 1, 1
    for _ in range(n):
        a, b = b, a + b
    return a

Role = Tuple[str, str, str]  # (IN1, IN2, OUT) cada uno en {'A','B','C'}

def base_roles() -> List[Role]:
    """
    Wiring base (uno por trigate / dimensión):
      T0: A,B -> C
      T1: B,C -> A
      T2: C,A -> B
    """
    return [
        ('A', 'B', 'C'),
        ('B', 'C', 'A'),
        ('C', 'A', 'B'),
    ]

def rotate_roles(roles: List[Role], k: int) -> List[Role]:
    """
    Rotación por pasos de Fibonacci (autosimilar):
    shift_k = fib(k) % 3
    """
    s = fib(k) % 3
    return [roles[(i + s) % 3] for i in range(3)]

def pick_vector(name: str, A: List[Trit], B: List[Trit], C: List[Trit]) -> List[Trit]:
    if name == 'A':
        return A
    if name == 'B':
        return B
    return C


# ---------- Transcender ----------
class Transcender:
    """
    Transcender (núcleo Aurora):
    - Opera 3 trigates (uno por dimensión) sobre 3 vectores ternarios A,B,C (3 bits).
    - Explora 3 wirings (rotación Fibonacci mod 3) y elige el más coherente.
    - Devuelve: M1,M2,M3 (controles base), Ms (Msuperior), Ss (forma),
      MetaM=[M1,M2,M3,Ms], wiring ganador, score y bandera de reconstrucción.
    - NEW: Aplica "coherencia absoluta" top-down y devuelve un reporte para el Evolver.

    Requiere un Trigate con:
      * Trigate.infer(A,B,M) -> R
      * Trigate.learn(A,B,R) -> M
    (Todos los vectores de 3 bits ternarios)
    """

    def __init__(self, trigate_cls):
        self.T = trigate_cls

    # ---------- métricas ----------
    @staticmethod
    def _nulls(vec: List[Trit]) -> int:
        return sum(1 for x in vec if x is None)

    def _score(self, Ms_list: List[List[Trit]], Ms: List[Trit], Ss: List[Trit]) -> int:
        """
        Score simple = #NULLs totales en M1,M2,M3,Ms,Ss.
        """
        total = 0
        for m in Ms_list:
            total += self._nulls(m)
        total += self._nulls(Ms)
        total += self._nulls(Ss)
        return total

    # ---------- NEW: helpers de coherencia absoluta ----------
    def _reconcile_bit(self, obs: Trit, ded: Trit, *, prefer_ded: bool) -> Tuple[Trit, Dict[str,int]]:
        """
        Reconcilia un bit observado vs deducido.
        prefer_ded=True -> prioriza valor deducido por el padre.
        Devuelve (valor_final, contadores para el reporte).
        """
        stats = {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0}
        if obs is None and ded is not None:
            stats["null_filled"] += 1
            return ded, stats
        if obs is not None and ded is None:
            stats["kept_observed"] += 1
            return obs, stats
        if obs is not None and ded is not None and obs != ded:
            # conflicto: imponemos coherencia absoluta (padre)
            if prefer_ded:
                stats["conflict_resolved"] += 1
                return ded, stats
            else:
                stats["kept_observed"] += 1
                return obs, stats
        # iguales o ambos None
        return obs, stats

    def _enforce_absolute_coherence(
        self,
        M1: List[Trit],
        M2: List[Trit],
        M3: List[Trit],
        Ms: List[Trit]
    ) -> Tuple[List[Trit], List[Trit], List[Trit], Dict]:
        """
        Top-down: el valor superior Ms fija a sus hijas.
        Deducimos cada hija con infer(par, par, Ms) y reconciliamos bit a bit.
        Devolvemos M1', M2', M3' y un reporte agregable por el Evolver.
        """
        T = self.T
        report = {
            "child_updates": {
                "M1": {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0},
                "M2": {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0},
                "M3": {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0},
            },
            "parent": Ms,
        }

        # Deducciones por tríos, rotando el "faltante"
        M3_hat = T.infer(M1, M2, Ms)
        M1_hat = T.infer(M2, M3, Ms)
        M2_hat = T.infer(M3, M1, Ms)

        M1_new, M2_new, M3_new = [], [], []

        for i in range(3):
            # M1
            v, s = self._reconcile_bit(M1[i], M1_hat[i], prefer_ded=True)
            M1_new.append(v)
            for k in report["child_updates"]["M1"]:
                report["child_updates"]["M1"][k] += s[k]
            # M2
            v, s = self._reconcile_bit(M2[i], M2_hat[i], prefer_ded=True)
            M2_new.append(v)
            for k in report["child_updates"]["M2"]:
                report["child_updates"]["M2"][k] += s[k]
            # M3
            v, s = self._reconcile_bit(M3[i], M3_hat[i], prefer_ded=True)
            M3_new.append(v)
            for k in report["child_updates"]["M3"]:
                report["child_updates"]["M3"][k] += s[k]

        # Totales
        report["totals"] = {
            "null_filled": sum(report["child_updates"][c]["null_filled"] for c in ("M1","M2","M3")),
            "conflict_resolved": sum(report["child_updates"][c]["conflict_resolved"] for c in ("M1","M2","M3")),
            "kept_observed": sum(report["child_updates"][c]["kept_observed"] for c in ("M1","M2","M3")),
        }
        return M1_new, M2_new, M3_new, report

    # ---------- intento con un wiring concreto ----------
    def _try_wiring(
        self,
        A: List[Trit],
        B: List[Trit],
        C: List[Trit],
        roles_k: List[Role],
        check_reconstruction: bool,
        *,
        enforce_coherence: bool  # NEW: activar coherencia absoluta
    ) -> Dict:
        """
        Ejecuta los 3 trigates con el wiring dado, aprende M1..M3,
        luego sintetiza Ms (Msuperior), Ss (forma), MetaM y aplica coherencia absoluta.
        """
        # Entradas normalizadas
        A = norm3(A); B = norm3(B); C = norm3(C)

        # 1) Aprendizaje de M_i (tres trigates base)
        triplets = []
        for (in1, in2, out) in roles_k:
            X = norm3(pick_vector(in1, A, B, C))
            Y = norm3(pick_vector(in2, A, B, C))
            Z = norm3(pick_vector(out, A, B, C))
            triplets.append((X, Y, Z))

        Ms_list: List[List[Trit]] = []
        for (X, Y, Z) in triplets:
            M_i = self.T.learn(X, Y, Z)  # Aprende control bit a bit
            Ms_list.append(M_i)

        # 2) Validación opcional de reconstrucción (infer(X,Y,M) ≟ Z)
        reconstruction_ok = True
        if check_reconstruction:
            for (X, Y, Z), M_i in zip(triplets, Ms_list):
                R_hat = self.T.infer(X, Y, M_i)
                for r, z in zip(R_hat, Z):
                    if r is None or z is None:
                        continue
                    if r != z:
                        reconstruction_ok = False
                        break
                if not reconstruction_ok:
                    break

        # 3) Síntesis superior (Ms y Ss)
        #    Regla Aurora:
        #      Ms = learn(M1, M2, M3)   -> estructura emergente
        #      Ss = infer(M1, M2, Ms)   -> forma factual
        M1, M2, M3 = [norm3(m) for m in Ms_list]
        Ms = self.T.learn(M1, M2, M3)    # Msuperior (3 bits)
        Ss = self.T.infer(M1, M2, Ms)    # Forma del proceso (3 bits)

        # 4) NEW: Coherencia absoluta (top-down) y reporte para Evolver
        coherence_report = None
        if enforce_coherence:
            M1c, M2c, M3c, coh_rep = self._enforce_absolute_coherence(M1, M2, M3, Ms)
            coherence_report = coh_rep
            # opcional: re-evaluar Ss con los hijos corregidos (mantengo Ms):
            Ss = self.T.infer(M1c, M2c, Ms)
            # sustituimos hijos por su versión coherente (para auditar/score):
            M1, M2, M3 = M1c, M2c, M3c

        # 5) Traza completa
        MetaM = [M1, M2, M3, Ms]

        # 6) Scoring (ambigüedad total)
        score = self._score([M1, M2, M3], Ms, Ss)

        return {
            "M1": M1, "M2": M2, "M3": M3,
            "Ms": Ms, "Ss": Ss,
            "MetaM": MetaM,
            "wiring": roles_k,
            "score": score,
            "reconstruction_ok": reconstruction_ok,
            # NEW: reporte de coherencia absoluta para Evolver
            "coherence": coherence_report
        }

    # ---------- API principal ----------
    def solve(
        self,
        A: List[Trit],
        B: List[Trit],
        C: List[Trit],
        *,
        max_tries: int = 3,                 # NEW: solo 3 wirings únicos
        check_reconstruction: bool = True,
        enforce_coherence: bool = True      # NEW: activa coherencia absoluta
    ) -> Dict:
        """
        Explora wirings generados por rotación Fibonacci y devuelve
        el mejor resultado (mínimo score; a igualdad, prioriza reconstrucción_ok=True).
        Parada temprana si score==0 y reconstrucción_ok.
        """
        roles0 = base_roles()
        best: Dict = {}

        for k in range(max_tries):
            roles_k = rotate_roles(roles0, k)
            res = self._try_wiring(A, B, C, roles_k, check_reconstruction, enforce_coherence=enforce_coherence)

            if not best:
                best = res
            else:
                if (res["score"] < best["score"]) or (
                    res["score"] == best["score"] and res["reconstruction_ok"] and not best["reconstruction_ok"]
                ):
                    best = res

            if res["score"] == 0 and res["reconstruction_ok"]:
                break

        return best
    
# Evolver.py


from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any, Tuple
from functools import lru_cache
import time

Trit = Optional[int]

def norm3(v: List[Trit]) -> List[Trit]:
    v = (list(v) + [0,0,0])[:3]
    return [None if x is None else (1 if x==1 else 0) for x in v]

def rotate3(v: List[Trit], s: int) -> List[Trit]:
    s %= 3
    return v[-s:] + v[:-s] if s else v

@lru_cache(maxsize=512)
def similarity3_cached(a_tuple: Tuple[Trit, ...], b_tuple: Tuple[Trit, ...]) -> int:
    """Versión cacheada de similarity3 para tuplas inmutables"""
    s=0
    for x,y in zip(a_tuple, b_tuple):
        if x is None or y is None: continue
        if x==y: s+=1
    return s  # 0..3

def similarity3(a: List[Trit], b: List[Trit]) -> int:
    """Wrapper para mantener compatibilidad con listas"""
    return similarity3_cached(tuple(a), tuple(b))

@dataclass
class Proto:
    key: Tuple[Any,...]
    proto: List[Trit]
    weight: float = 0.0
    count: int = 0
    last_seen: float = field(default_factory=lambda: time.time())

class Evolver3:
    """
    Evolver autosimilar (todo con Trigate):
    1) RELATOR: aprende relaciones entre dimensiones de distintos tensores condicionadas por Ms padre.
    2) EMERGENCIA: aprende patrones entre Ms (M1,M2,M3 -> Ms).
    3) DINÁMICA: aprende transición entrada→salida en rondas (Ms_prev -> Ms_next) y/o (tensor_prev -> tensor_next).
    """
    def __init__(self, trigate_cls, *, th_match:int=2, decay:float=0.9):
        self.T = trigate_cls
        self.th = th_match
        self.decay = decay
        # Bancos de patrones
        self._relator: Dict[Tuple, Proto] = {}    # key = (Ms_parent, wiring_hash, role_idx)
        self._emerg:   Dict[Tuple, Proto] = {}    # key = (role_idx,)
        self._dyn:     Dict[Tuple, Proto] = {}    # key = (level, ) o (round,)

        # memoria de contexto para dinámica
        self._last_round_ms: List[List[Trit]] = []  # Ms de la ronda anterior (acumuladas por nodo/level)

    # ---------- helpers ----------
    def _reinforce(self, bank: Dict[Tuple, Proto], key: Tuple, candidate: List[Trit]) -> None:
        cand = norm3(candidate)
        if key not in bank:
            bank[key] = Proto(key=key, proto=cand, weight=1.0, count=1)
            return
        p = bank[key]
        # refuerzo EMA + “honestidad”: solo rellenar None con nueva evidencia
        p.weight = p.weight * self.decay + similarity3(cand, p.proto)
        p.count += 1
        p.last_seen = time.time()
        for i,(a,b) in enumerate(zip(p.proto, cand)):
            if a is None and b is not None:
                p.proto[i] = b

    def _best_rot_match(self, proto: List[Trit], cand: List[Trit]) -> Tuple[int,int]:
        # devuelve (sim_max, rot_mejor)
        best=( -1, 0 )
        for r in (0,1,2):
            s = similarity3(proto, rotate3(cand,r))
            if s>best[0]: best=(s,r)
        return best

    # ---------- 1) RELATOR ----------
    def observe_relator(self, Ms_parent: List[Trit], wiring: List[Tuple[str,str,str]], M1: List[Trit], M2: List[Trit], M3: List[Trit]):
        """
        Aprende “cómo se conectan” dimensiones entre tensores dado el Ms padre.
        Representamos cada trigate (role_idx=0,1,2) por la “ley local” que vio:
           ley_i = learn(IN1_i, IN2_i, OUT_i) == M_i (redundante pero explícito)
        Guardamos un prototipo condicional por (Ms_parent, wiring, role_idx).
        """
        Ms_parent = norm3(Ms_parent)
        Mloc = [norm3(M1), norm3(M2), norm3(M3)]
        wiring_hash = tuple(wiring)  # simple
        for role_idx, M_i in enumerate(Mloc):
            key = (tuple(Ms_parent), wiring_hash, role_idx)
            self._reinforce(self._relator, key, M_i)

    # ---------- 2) EMERGENCIA ----------
    def observe_emergence(self, M1: List[Trit], M2: List[Trit], M3: List[Trit], Ms: List[Trit]):
        """
        Aprende patrón de síntesis: (M1,M2,M3) -> Ms
        Guardamos dos cosas:
         a) ley_superior = learn(M1, M2, M3)  (debería ≈ Ms ideal)
         b) “shape” = infer(M1, M2, Ms)       (Ss), puede ayudar a discriminar variantes
        """
        M1,M2,M3,Ms = map(norm3,(M1,M2,M3,Ms))
        ley_sup = self.T.learn(M1,M2,M3)   # estructura ideal
        shape   = self.T.infer(M1,M2,Ms)   # forma observada
        # patrón principal: lo que explica mejor Ms
        key_main = ("emerg",)
        self._reinforce(self._emerg, key_main, ley_sup)
        # opcional: sub-claves por “firma de hijos”
        key_sig = ("emerg_sig", tuple(M1), tuple(M2), tuple(M3))
        self._reinforce(self._emerg, key_sig, Ms)
        # opcional: forma
        key_shape = ("emerg_shape",)
        self._reinforce(self._emerg, key_shape, shape)

    # ---------- 3) DINÁMICA ----------
    def observe_dynamics_round(self, ms_list_this_round: List[List[Trit]], level_tag: str):
        """
        Aprende transición entre rondas: Ms_prev -> Ms_curr (por nodo).
        También registra un “resumen de nivel” usando learn/infer sobre agregados (promueve metapatrones).
        """
        ms_curr = [norm3(m) for m in ms_list_this_round]
        if self._last_round_ms:
            for m_prev, m_curr in zip(self._last_round_ms[:len(ms_curr)], ms_curr):
                # ley de transición local
                trans = self.T.learn(m_prev, m_curr, m_curr)  # “qué M convierte prev en curr”
                key = ("dyn_local", level_tag)
                self._reinforce(self._dyn, key, trans)
        # resumen del nivel (autosimilar): comprimimos de 3 en 3 si hay suficientes
        for i in range(0, len(ms_curr), 3):
            block = ms_curr[i:i+3]
            if len(block)==3:
                Ms_level = self.T.learn(block[0], block[1], block[2])
                keyL = ("dyn_level", level_tag)
                self._reinforce(self._dyn, keyL, Ms_level)
        # actualiza memoria
        self._last_round_ms = ms_curr

    # ---------- Ingesta de resultados ----------
    def observe_transcender(self, result: Dict[str,Any], level_tag: str = "node"):
        """
        Ingiere un Transcender.solve(...): usa
          - RELATOR: Ms (padre), wiring, M1..M3, (y coherencia si está)
          - EMERGENCIA: M1,M2,M3 -> Ms
        """
        M1,M2,M3,Ms = result["M1"], result["M2"], result["M3"], result["Ms"]
        self.observe_relator(Ms, result["wiring"], M1, M2, M3)
        self.observe_emergence(M1, M2, M3, Ms)
        # coherencia absoluta: útil para valorar “fuerza” del patrón padre
        if result.get("coherence"):
            coh = result["coherence"]
            keyC = ("coh_parent", tuple(norm3(coh["parent"])))
            # sintetizamos una “marca” de fuerza a partir de conteos
            # m≔ [filled>0, conflict>0, kept>0] como bits 0/1 para trigate
            marks = [
                1 if coh["totals"]["null_filled"]>0 else 0,
                1 if coh["totals"]["conflict_resolved"]>0 else 0,
                1 if coh["totals"]["kept_observed"]>0 else 0,
            ]
            self._reinforce(self._emerg, keyC, marks)

    def observe_fractal(self, res: Dict[str,Any], level_name: str):
        """
        Ingiere FractalTranscender.synthesize(...):
        - para cada nivel (27/9/3), recorre los nodos y llama observe_transcender(...)
        - también alimenta la DINÁMICA con las Ms cruzadas del nivel
        """
        # Ms cruzadas por nivel
        lvl_to_ms = {
            "lvl27": res["tensor_cross"].nivel_27,
            "lvl9":  res["tensor_cross"].nivel_9,
            "lvl3":  res["tensor_cross"].nivel_3,
        }
        # audits por nivel traen MetaM/wiring/score por nodo; si no están los resultados completos
        # de cada transcender individual, al menos aprendemos dinámica con las Ms del tensor resultante.
        for lvl, ms_list in lvl_to_ms.items():
            self.observe_dynamics_round(ms_list, level_tag=f"{level_name}:{lvl}")

    # ---------- Consultas ----------
    def relator_top(self, k:int=5):
        items = sorted(self._relator.values(), key=lambda p: (p.weight,p.count), reverse=True)
        return [{"key": p.key, "proto": p.proto, "w": round(p.weight,3), "n": p.count} for p in items[:k]]

    def emergence_top(self, k:int=5):
        items = sorted(self._emerg.values(), key=lambda p: (p.weight,p.count), reverse=True)
        return [{"key": p.key, "proto": p.proto, "w": round(p.weight,3), "n": p.count} for p in items[:k]]

    def dynamics_top(self, k:int=5):
        items = sorted(self._dyn.values(), key=lambda p: (p.weight,p.count), reverse=True)
        return [{"key": p.key, "proto": p.proto, "w": round(p.weight,3), "n": p.count} for p in items[:k]]
    
     # ---------- Métodos avanzados para Harmonizer y Extender ----------
    
    def select_relator(self, tag: str, Ms_parent: List[Trit]) -> Optional[List[Tuple[str,str,str]]]:
        """
        Selecciona el wiring más apropiado para un componente dado su Ms padre.
        Usado por Extender para reconstrucción guiada.
        
        Args:
            tag: Etiqueta del componente (ej: "x", "y", "z")
            Ms_parent: Vector Ms del padre
            
        Returns:
            Wiring sugerido o None si no hay información suficiente
        """
        Ms_parent = norm3(Ms_parent)
        best_wiring = None
        best_score = -1
        
        # Buscar en relator patrones que coincidan con Ms_parent
        for key, proto in self._relator.items():
            if len(key) >= 2:  # key = (Ms_parent_tuple, wiring_hash, role_idx)
                stored_ms = key[0]
                if similarity3(list(stored_ms), Ms_parent) >= self.th:
                    if proto.weight > best_score:
                        best_score = proto.weight
                        if len(key) >= 2:
                            best_wiring = key[1]  # wiring_hash
        
        return best_wiring if best_wiring else None
    
    def select_relator_k(self, tag: str, Ms: List[Trit], k: int = 3) -> List[Dict[str, Any]]:
        """
        Retorna los k mejores relatores candidatos para un Ms dado.
        Usado por Harmonizer para explorar alternativas de reparación.
        
        Args:
            tag: Etiqueta del componente
            Ms: Vector Ms para buscar
            k: Número de candidatos a retornar
            
        Returns:
            Lista de diccionarios con {proto, weight, count, wiring}
        """
        Ms = norm3(Ms)
        candidates = []
        
        # Buscar patrones similares en relator
        for key, proto in self._relator.items():
            if len(key) >= 2:  # (Ms_parent, wiring, role_idx)
                stored_ms = key[0]
                sim = similarity3(list(stored_ms), Ms)
                if sim >= self.th - 1:  # Umbral más permisivo para alternativas
                    candidates.append({
                        "proto": proto.proto,
                        "weight": proto.weight,
                        "count": proto.count,
                        "wiring": key[1] if len(key) >= 2 else None,
                        "similarity": sim
                    })
        
        # Ordenar por weight * similarity
        candidates.sort(key=lambda c: c["weight"] * (c["similarity"] + 1), reverse=True)
        return candidates[:k]
    
    def create_new_archetype_triplet(
        self, 
        Msx: List[Trit], 
        Msy: List[Trit], 
        Msz: List[Trit],
        *,
        tag: str = "new_archetype"
    ) -> Dict[str, Any]:
        """
        Crea un nuevo arquetipo a partir de una tripleta de Ms.
        Usado por Harmonizer cuando necesita crear patrones nuevos.
        
        Args:
            Msx, Msy, Msz: Vectores Ms de los tres componentes
            tag: Etiqueta para identificar el arquetipo
            
        Returns:
            Diccionario con el arquetipo creado y sus metadatos
        """
        Msx, Msy, Msz = map(norm3, (Msx, Msy, Msz))
        
        # Sintetizar un Ms superior desde la tripleta
        Ms_super = self.T.learn(Msx, Msy, Msz)
        
        # Calcular forma (Ss) del arquetipo
        Ss = self.T.infer(Msx, Msy, Ms_super)
        
        # Almacenar en banco de emergencia
        key_triplet = ("archetype_triplet", tuple(Msx), tuple(Msy), tuple(Msz))
        self._reinforce(self._emerg, key_triplet, Ms_super)
        
        # También almacenar la forma
        key_shape = ("archetype_shape", tag)
        self._reinforce(self._emerg, key_shape, Ss)
        
        return {
            "Ms_super": Ms_super,
            "Ss": Ss,
            "components": {"Msx": Msx, "Msy": Msy, "Msz": Msz},
            "tag": tag,
            "stored_keys": [key_triplet, key_shape]
        }
    
    def query_archetype_by_triplet(
        self, 
        Msx: List[Trit], 
        Msy: List[Trit], 
        Msz: List[Trit]
    ) -> Optional[Dict[str, Any]]:
        """
        Busca si existe un arquetipo para la tripleta dada.
        
        Returns:
            Diccionario con el arquetipo encontrado o None
        """
        Msx, Msy, Msz = map(norm3, (Msx, Msy, Msz))
        key = ("archetype_triplet", tuple(Msx), tuple(Msy), tuple(Msz))
        
        if key in self._emerg:
            proto = self._emerg[key]
            return {
                "Ms_super": proto.proto,
                "weight": proto.weight,
                "count": proto.count,
                "components": {"Msx": Msx, "Msy": Msy, "Msz": Msz}
            }
        
        return None
    
    def suggest_repair(self, Ms: List[Trit], *, context: str = "unknown") -> List[Trit]:
        """
        Sugiere una reparación para un Ms con NULLs basándose en patrones aprendidos.
        Usado por Harmonizer para rellenar valores faltantes.
        
        Args:
            Ms: Vector Ms con posibles NULLs
            context: Contexto para guiar la búsqueda
            
        Returns:
            Vector Ms reparado (mejor esfuerzo)
        """
        Ms = norm3(Ms)
        
        # Si no hay NULLs, retornar como está
        if all(x is not None for x in Ms):
            return Ms
        
        # Buscar el patrón más similar en emergencia
        best_match = None
        best_sim = -1
        
        for proto in self._emerg.values():
            # Calcular similitud solo en posiciones no-NULL
            sim = 0
            for i, (m, p) in enumerate(zip(Ms, proto.proto)):
                if m is not None and p is not None:
                    if m == p:
                        sim += 1
            
            if sim > best_sim:
                best_sim = sim
                best_match = proto.proto
        
        # Si encontramos algo, rellenar NULLs con el patrón
        if best_match:
            result = []
            for m, b in zip(Ms, best_match):
                result.append(m if m is not None else b)
            return result
        
        # Fallback: retornar [0,0,0] en NULLs
        return [m if m is not None else 0 for m in Ms]




# Extendder.py

"""
Extender - Reconstrucción top-down en Aurora Trinity-3

Clase de referencia (plug-and-play)
"""
from typing import List, Optional, Tuple, Dict, Any

Trit = Optional[int]  # 0 | 1 | None

def norm3(v: List[Trit]) -> List[Trit]:
    v = (list(v) + [0,0,0])[:3]
    return [None if x is None else (1 if x==1 else 0) for x in v]

def rotate3(v: List[Trit], s: int) -> List[Trit]:
    s %= 3
    return v[-s:] + v[:-s] if s else v

def similarity3(a: List[Trit], b: List[Trit]) -> int:
    s = 0
    for x,y in zip(a,b):
        if x is None or y is None: 
            continue
        if x == y:
            s += 1
    return s  # 0..3

class Extender:
    """
    Transcender invertido:
    - Dado un Ms^sup tripleta y las políticas (relatores + dinámica),
      reconstruye Ms hijas por componente, aplicando coherencia absoluta.
    - Puede emitir vectores inferiores si hay semillas suficientes.
    Requiere Trigate con .infer/.learn y acceso a:
      evolver.select_relator(d, Ms_d_sup) -> wiring sugerido por componente
      evolver.dynamics_top/prior(d)       -> prior Ms hija si faltan datos
    """
    def __init__(self, trigate_cls, evolver):
        self.T = trigate_cls
        self.EV = evolver  # Evolver3 (o compatible)

    # --- util: reconciliación bit a bit (coherencia absoluta) ---
    def _reconcile_bit(self, obs: Trit, ded: Trit, prefer_ded: bool=True) -> Tuple[Trit, Dict[str,int]]:
        stats = {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0}
        if obs is None and ded is not None:
            stats["null_filled"] += 1;   return ded, stats
        if obs is not None and ded is None:
            stats["kept_observed"] += 1; return obs, stats
        if obs is not None and ded is not None and obs != ded:
            if prefer_ded:
                stats["conflict_resolved"] += 1; return ded, stats
            else:
                stats["kept_observed"] += 1;     return obs, stats
        return obs, stats  # iguales o ambos None

    # --- aplica coherencia absoluta para 3 hijas de un componente ---
    def _enforce_component(self, Ms_parent: List[Trit], M1: List[Trit], M2: List[Trit], M3: List[Trit]) -> Tuple[List[Trit], List[Trit], List[Trit], Dict[str,int]]:
        T = self.T
        M3_hat = T.infer(M1, M2, Ms_parent)
        M1_hat = T.infer(M2, M3, Ms_parent)
        M2_hat = T.infer(M3, M1, Ms_parent)

        rep = {"null_filled":0,"conflict_resolved":0,"kept_observed":0}
        M1n,M2n,M3n = [],[],[]
        for i in range(3):
            v,s = self._reconcile_bit(M1[i], M1_hat[i], True); M1n.append(v)
            for k in rep: rep[k] += s[k]
            v,s = self._reconcile_bit(M2[i], M2_hat[i], True); M2n.append(v)
            for k in rep: rep[k] += s[k]
            v,s = self._reconcile_bit(M3[i], M3_hat[i], True); M3n.append(v)
            for k in rep: rep[k] += s[k]
        return M1n, M2n, M3n, rep

    # --- prior simple desde la dinámica del Evolver (si existe) ---
    def _prior_child(self, d_tag: str) -> List[Trit]:
        """
        Obtiene prior dinámico del Evolver, o retorna [None,None,None].
        Maneja casos donde dynamics_top no existe o retorna vacío.
        """
        # Verificar que el Evolver tenga el método
        if not hasattr(self.EV, 'dynamics_top'):
            return [None, None, None]
        
        try:
            tops = self.EV.dynamics_top(1)  # [{'key':(...), 'proto':[...], ...}]
            if tops and len(tops) > 0:  # Si hay resultados
                proto = tops[0].get("proto")
                if isinstance(proto, list) and len(proto) >= 3:
                    return norm3(proto)
        except Exception:
            # Si falla por cualquier razón, retorna NULL honestamente
            pass
        
        return [None, None, None]

    # --- reconstruye Ms hijas para UN componente d ∈ {x,y,z} ---
    def extend_component(
        self,
        Ms_parent: List[Trit],
        *,
        seeds: List[List[Trit]] = None  # opcional: pistas/mediciones para hijas [M1?,M2?,M3?]
    ) -> Dict[str,Any]:
        Ms_parent = norm3(Ms_parent)
        seeds = seeds or [[None,None,None], [None,None,None], [None,None,None]]

        # 1) obtiene wiring sugerido por el relator (para auditoría/guía)
        wiring = None
        if hasattr(self.EV, "select_relator"):
            wiring = self.EV.select_relator("comp", Ms_parent)  # tu select por componente

        # 2) inicializa hijas con seeds o priors dinámicos
        M1, M2, M3 = [norm3(seeds[i]) for i in range(3)]
        need_prior = [all(x is None for x in M1),
                      all(x is None for x in M2),
                      all(x is None for x in M3)]
        if any(need_prior):
            prior = self._prior_child("comp")
            # asigna el prior a la hija más “vacía”
            if need_prior[0]: M1 = prior[:]
            if need_prior[1]: M2 = prior[:]
            if need_prior[2]: M3 = prior[:]

        # 3) si hay al menos dos hijas con bits definidos en alguna posición, cierra la tercera
        #    (Trigate.infer garantiza el cierre con el padre)
        #    coherencia absoluta impone el resultado final
        M1c, M2c, M3c, rep = self._enforce_component(Ms_parent, M1, M2, M3)

        return {
            "Ms_parent": Ms_parent,
            "children": {"M1": M1c, "M2": M2c, "M3": M3c},
            "wiring_hint": wiring,
            "coherence_stats": rep
        }

    # --- reconstruye tripleta de hijas (x,y,z) para un nodo ---
    def extend_triplet(
        self,
        Ms_triplet_parent: Tuple[List[Trit], List[Trit], List[Trit]],
        *,
        seeds_triplet: Tuple[List[List[Trit]], List[List[Trit]], List[List[Trit]]] = None
    ) -> Dict[str,Any]:
        (Msx, Msy, Msz) = Ms_triplet_parent
        seeds_triplet = seeds_triplet or (None, None, None)

        rx = self.extend_component(Msx, seeds=seeds_triplet[0] or None)
        ry = self.extend_component(Msy, seeds=seeds_triplet[1] or None)
        rz = self.extend_component(Msz, seeds=seeds_triplet[2] or None)

        return {
            "parent": {"Msx": norm3(Msx), "Msy": norm3(Msy), "Msz": norm3(Msz)},
            "children": {
                "x": rx["children"], "y": ry["children"], "z": rz["children"]
            },
            "coherence": {
                "x": rx["coherence_stats"],
                "y": ry["coherence_stats"],
                "z": rz["coherence_stats"]
            },
            "wiring_hints": {"x": rx["wiring_hint"], "y": ry["wiring_hint"], "z": rz["wiring_hint"]}
        }

    # --- (opcional) reconstrucción de vectores inferiores si hay evidencia ---
    def reconstruct_vectors(
        self,
        Ms_component: List[Trit],
        *,
        known_pair: Tuple[List[Trit], List[Trit]] = None  # (A?, B?) o (B?, C?)...
    ) -> Dict[str,Any]:
        """
        Si el usuario aporta dos de las tres series de bits observables (A,B,R) y el Ms del componente,
        podemos deducir la tercera con Trigate:
          - R = infer(A,B,Ms)
          - A = deducir con learn/infer si hay M y R...
        Aquí damos un esqueleto mínimo a completar según tu clase Trigate (deduce_a/deduce_b).
        """
        # Placeholder: cada stack tendrá su ‘deduce_*’ real
        out = {}
        if known_pair:
            A, B = known_pair
            R_hat = self.T.infer(norm3(A), norm3(B), norm3(Ms_component))
            out["R_hat"] = R_hat
        return out



# FractalTranscender.py
from dataclasses import dataclass
from typing import List, Optional, Dict, Any, Tuple

Trit = Optional[int]  # 0 | 1 | None

def norm3(v: List[Trit]) -> List[Trit]:
    v = list(v)[:3] + [0,0,0]
    out = []
    for x in v[:3]:
        if x is None:
            out.append(None)
        else:
            out.append(1 if x == 1 else 0)
    return out

@dataclass
class FractalTensor:
    # Cada celda es SIEMPRE un vector ternario de 3 bits
    nivel_3:  List[List[Trit]]      # len = 3
    nivel_9:  List[List[Trit]]      # len = 9
    nivel_27: List[List[Trit]]      # len = 27

    @staticmethod
    def neutral() -> "FractalTensor":
        z = [[0,0,0]]
        return FractalTensor(nivel_3=z*3, nivel_9=z*9, nivel_27=z*27)

    def normalize(self):
        self.nivel_3  = [norm3(v) for v in self.nivel_3]
        self.nivel_9  = [norm3(v) for v in self.nivel_9]
        self.nivel_27 = [norm3(v) for v in self.nivel_27]
        return self
    
    def __repr__(self) -> str:
        """Representación legible del FractalTensor"""
        def format_vec(v):
            return '[' + ','.join('·' if x is None else str(x) for x in v) + ']'
        
        lines = [
            "FractalTensor(",
            f"  nivel_3:  {[format_vec(v) for v in self.nivel_3]}",
            f"  nivel_9:  {[format_vec(v) for v in self.nivel_9[:3]]} + {len(self.nivel_9)-3} more",
            f"  nivel_27: {[format_vec(v) for v in self.nivel_27[:3]]} + {len(self.nivel_27)-3} more",
            ")"
        ]
        return '\n'.join(lines)

class FractalTranscender:
    """
    Orquesta la síntesis fractal 27→9→3 sobre (A,B,C) usando un Transcender (3 trigates)
    para COMPARAR A vs B vs C en cada nodo, y además realiza la auto-síntesis por tensor.
    Requiere un Transcender con .solve(A,B,C) que devuelva dict con claves:
      M1,M2,M3, Ms, Ss, MetaM, score, reconstruction_ok, wiring.
    """
    def __init__(self, transcender_cls):
        self.TX = transcender_cls  # clase Transcender; debe aceptar Trigate dentro

    # --- utilidades de índice hijo → padre ---
    @staticmethod
    def child_triplet_indices(parent_idx: int) -> Tuple[int,int,int]:
        base = 3 * parent_idx
        return base, base+1, base+2

    # --- paso inter: comparación cruzada a un nivel ---
    def _cross_level(self, A_vecs: List[List[Trit]], B_vecs: List[List[Trit]], C_vecs: List[List[Trit]], transcender) -> Tuple[List[List[Trit]], List[List[Trit]], List[Dict[str,Any]]]:
        Ms_list, Ss_list, audit = [], [], []
        for i in range(len(A_vecs)):
            res = transcender.solve(A_vecs[i], B_vecs[i], C_vecs[i], max_tries=3, check_reconstruction=True)
            Ms_list.append(norm3(res["Ms"]))
            Ss_list.append(norm3(res["Ss"]))
            # Guardamos auditoría completa por nodo
            audit.append({
                "MetaM": res["MetaM"],
                "wiring": res["wiring"],
                "score": res["score"],
                "reconstruction_ok": res["reconstruction_ok"]
            })
        return Ms_list, Ss_list, audit

    # --- auto-síntesis intra-tensor: niños (x3) → padre (Ms local) ---
    def _self_synthesize_up(self, child_vecs: List[List[Trit]], transcender) -> List[List[Trit]]:
        """
        child_vecs: lista de longitud múltiplo de 3; por cada bloque de 3,
        sintetiza un Ms local (estructura del propio tensor).
        """
        assert len(child_vecs) % 3 == 0
        parents = []
        for p in range(len(child_vecs)//3):
            i0,i1,i2 = self.child_triplet_indices(p)
            res = transcender.solve(child_vecs[i0], child_vecs[i1], child_vecs[i2], max_tries=3, check_reconstruction=False)
            parents.append(norm3(res["Ms"]))  # Ms local del tensor
        return parents

    # --- API principal ---
    def synthesize(self, A: FractalTensor, B: FractalTensor, C: FractalTensor, transcender) -> Dict[str, Any]:
        """
        Devuelve:
          - tensor_cross: FractalTensor con Ms cruzadas (27,9,3)
          - audits: dict con Ss y MetaM por nivel y posición
          - locals: Ms locales (auto-síntesis) de A,B,C en 9 y 3
        """
        A.normalize(); B.normalize(); C.normalize()

        audits = {"lvl27": [], "lvl9": [], "lvl3": []}
        localsynth = {"A":{}, "B":{}, "C":{}}

        # --- Nivel 27: comparación cruzada A27 vs B27 vs C27 ---
        Ms27, Ss27, audit27 = self._cross_level(A.nivel_27, B.nivel_27, C.nivel_27, transcender)
        audits["lvl27"] = audit27

        # Auto-síntesis intra-tensor (27→9) para A,B,C (dualidad jerárquica)
        A9_local = self._self_synthesize_up(A.nivel_27, transcender)
        B9_local = self._self_synthesize_up(B.nivel_27, transcender)
        C9_local = self._self_synthesize_up(C.nivel_27, transcender)
        localsynth["A"]["lvl9"] = A9_local
        localsynth["B"]["lvl9"] = B9_local
        localsynth["C"]["lvl9"] = C9_local

        # --- Nivel 9: comparación cruzada A9_local vs B9_local vs C9_local ---
        Ms9, Ss9, audit9 = self._cross_level(A9_local, B9_local, C9_local, transcender)
        audits["lvl9"] = audit9

        # Auto-síntesis intra-tensor (9→3) para A,B,C
        A3_local = self._self_synthesize_up(A9_local, transcender)
        B3_local = self._self_synthesize_up(B9_local, transcender)
        C3_local = self._self_synthesize_up(C9_local, transcender)
        localsynth["A"]["lvl3"] = A3_local
        localsynth["B"]["lvl3"] = B3_local
        localsynth["C"]["lvl3"] = C3_local

        # --- Nivel 3: comparación cruzada A3_local vs B3_local vs C3_local ---
        Ms3, Ss3, audit3 = self._cross_level(A3_local, B3_local, C3_local, transcender)
        audits["lvl3"] = audit3

        # Tensor resultado (cruzado) → Ms como valores de cada nivel
        tensor_cross = FractalTensor(
            nivel_3 = Ms3,   # len=3
            nivel_9 = Ms9,   # len=9
            nivel_27 = Ms27  # len=27
        ).normalize()

        # También devolvemos Ss por nivel si quieres reconstrucciones:
        Ss = {"lvl27": Ss27, "lvl9": Ss9, "lvl3": Ss3}

        return {
            "tensor_cross": tensor_cross,
            "Ss": Ss,
            "audits": audits,
            "locals": localsynth
        }


from typing import List, Optional, Tuple, Dict

Trit = Optional[int]  # 0 | 1 | None

# ---------- utilidades ----------
def norm3(v: List[Trit]) -> List[Trit]:
    """Normaliza a ternario de longitud 3 (0/1/None)."""
    v = list(v)[:3] + [0, 0, 0]
    out = []
    for x in v[:3]:
        if x is None:
            out.append(None)
        else:
            out.append(1 if x == 1 else 0)
    return out

def fib(n: int) -> int:
    """Fibonacci iterativo (1,1,2,3,5,...)"""
    a, b = 1, 1
    for _ in range(n):
        a, b = b, a + b
    return a

Role = Tuple[str, str, str]  # (IN1, IN2, OUT) cada uno en {'A','B','C'}

def base_roles() -> List[Role]:
    """
    Wiring base (uno por trigate / dimensión):
      T0: A,B -> C
      T1: B,C -> A
      T2: C,A -> B
    """
    return [
        ('A', 'B', 'C'),
        ('B', 'C', 'A'),
        ('C', 'A', 'B'),
    ]

def rotate_roles(roles: List[Role], k: int) -> List[Role]:
    """
    Rotación por pasos de Fibonacci (autosimilar):
    shift_k = fib(k) % 3
    """
    s = fib(k) % 3
    return [roles[(i + s) % 3] for i in range(3)]

def pick_vector(name: str, A: List[Trit], B: List[Trit], C: List[Trit]) -> List[Trit]:
    if name == 'A':
        return A
    if name == 'B':
        return B
    return C


# ---------- Transcender ----------
class Transcender:
    """
    Transcender (núcleo Aurora):
    - Opera 3 trigates (uno por dimensión) sobre 3 vectores ternarios A,B,C (3 bits).
    - Explora 3 wirings (rotación Fibonacci mod 3) y elige el más coherente.
    - Devuelve: M1,M2,M3 (controles base), Ms (Msuperior), Ss (forma),
      MetaM=[M1,M2,M3,Ms], wiring ganador, score y bandera de reconstrucción.
    - NEW: Aplica "coherencia absoluta" top-down y devuelve un reporte para el Evolver.

    Requiere un Trigate con:
      * Trigate.infer(A,B,M) -> R
      * Trigate.learn(A,B,R) -> M
    (Todos los vectores de 3 bits ternarios)
    """

    def __init__(self, trigate_cls):
        self.T = trigate_cls

    # ---------- métricas ----------
    @staticmethod
    def _nulls(vec: List[Trit]) -> int:
        return sum(1 for x in vec if x is None)

    def _score(self, Ms_list: List[List[Trit]], Ms: List[Trit], Ss: List[Trit]) -> int:
        """
        Score simple = #NULLs totales en M1,M2,M3,Ms,Ss.
        """
        total = 0
        for m in Ms_list:
            total += self._nulls(m)
        total += self._nulls(Ms)
        total += self._nulls(Ss)
        return total

    # ---------- NEW: helpers de coherencia absoluta ----------
    def _reconcile_bit(self, obs: Trit, ded: Trit, *, prefer_ded: bool) -> Tuple[Trit, Dict[str,int]]:
        """
        Reconcilia un bit observado vs deducido.
        prefer_ded=True -> prioriza valor deducido por el padre.
        Devuelve (valor_final, contadores para el reporte).
        """
        stats = {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0}
        if obs is None and ded is not None:
            stats["null_filled"] += 1
            return ded, stats
        if obs is not None and ded is None:
            stats["kept_observed"] += 1
            return obs, stats
        if obs is not None and ded is not None and obs != ded:
            # conflicto: imponemos coherencia absoluta (padre)
            if prefer_ded:
                stats["conflict_resolved"] += 1
                return ded, stats
            else:
                stats["kept_observed"] += 1
                return obs, stats
        # iguales o ambos None
        return obs, stats

    def _enforce_absolute_coherence(
        self,
        M1: List[Trit],
        M2: List[Trit],
        M3: List[Trit],
        Ms: List[Trit]
    ) -> Tuple[List[Trit], List[Trit], List[Trit], Dict]:
        """
        Top-down: el valor superior Ms fija a sus hijas.
        Deducimos cada hija con infer(par, par, Ms) y reconciliamos bit a bit.
        Devolvemos M1', M2', M3' y un reporte agregable por el Evolver.
        """
        T = self.T
        report = {
            "child_updates": {
                "M1": {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0},
                "M2": {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0},
                "M3": {"null_filled": 0, "conflict_resolved": 0, "kept_observed": 0},
            },
            "parent": Ms,
        }

        # Deducciones por tríos, rotando el "faltante"
        M3_hat = T.infer(M1, M2, Ms)
        M1_hat = T.infer(M2, M3, Ms)
        M2_hat = T.infer(M3, M1, Ms)

        M1_new, M2_new, M3_new = [], [], []

        for i in range(3):
            # M1
            v, s = self._reconcile_bit(M1[i], M1_hat[i], prefer_ded=True)
            M1_new.append(v)
            for k in report["child_updates"]["M1"]:
                report["child_updates"]["M1"][k] += s[k]
            # M2
            v, s = self._reconcile_bit(M2[i], M2_hat[i], prefer_ded=True)
            M2_new.append(v)
            for k in report["child_updates"]["M2"]:
                report["child_updates"]["M2"][k] += s[k]
            # M3
            v, s = self._reconcile_bit(M3[i], M3_hat[i], prefer_ded=True)
            M3_new.append(v)
            for k in report["child_updates"]["M3"]:
                report["child_updates"]["M3"][k] += s[k]

        # Totales
        report["totals"] = {
            "null_filled": sum(report["child_updates"][c]["null_filled"] for c in ("M1","M2","M3")),
            "conflict_resolved": sum(report["child_updates"][c]["conflict_resolved"] for c in ("M1","M2","M3")),
            "kept_observed": sum(report["child_updates"][c]["kept_observed"] for c in ("M1","M2","M3")),
        }
        return M1_new, M2_new, M3_new, report

    # ---------- intento con un wiring concreto ----------
    def _try_wiring(
        self,
        A: List[Trit],
        B: List[Trit],
        C: List[Trit],
        roles_k: List[Role],
        check_reconstruction: bool,
        *,
        enforce_coherence: bool  # NEW: activar coherencia absoluta
    ) -> Dict:
        """
        Ejecuta los 3 trigates con el wiring dado, aprende M1..M3,
        luego sintetiza Ms (Msuperior), Ss (forma), MetaM y aplica coherencia absoluta.
        """
        # Entradas normalizadas
        A = norm3(A); B = norm3(B); C = norm3(C)

        # 1) Aprendizaje de M_i (tres trigates base)
        triplets = []
        for (in1, in2, out) in roles_k:
            X = norm3(pick_vector(in1, A, B, C))
            Y = norm3(pick_vector(in2, A, B, C))
            Z = norm3(pick_vector(out, A, B, C))
            triplets.append((X, Y, Z))

        Ms_list: List[List[Trit]] = []
        for (X, Y, Z) in triplets:
            M_i = self.T.learn(X, Y, Z)  # Aprende control bit a bit
            Ms_list.append(M_i)

        # 2) Validación opcional de reconstrucción (infer(X,Y,M) ≟ Z)
        reconstruction_ok = True
        if check_reconstruction:
            for (X, Y, Z), M_i in zip(triplets, Ms_list):
                R_hat = self.T.infer(X, Y, M_i)
                for r, z in zip(R_hat, Z):
                    if r is None or z is None:
                        continue
                    if r != z:
                        reconstruction_ok = False
                        break
                if not reconstruction_ok:
                    break

        # 3) Síntesis superior (Ms y Ss)
        #    Regla Aurora:
        #      Ms = learn(M1, M2, M3)   -> estructura emergente
        #      Ss = infer(M1, M2, Ms)   -> forma factual
        M1, M2, M3 = [norm3(m) for m in Ms_list]
        Ms = self.T.learn(M1, M2, M3)    # Msuperior (3 bits)
        Ss = self.T.infer(M1, M2, Ms)    # Forma del proceso (3 bits)

        # 4) NEW: Coherencia absoluta (top-down) y reporte para Evolver
        coherence_report = None
        if enforce_coherence:
            M1c, M2c, M3c, coh_rep = self._enforce_absolute_coherence(M1, M2, M3, Ms)
            coherence_report = coh_rep
            # opcional: re-evaluar Ss con los hijos corregidos (mantengo Ms):
            Ss = self.T.infer(M1c, M2c, Ms)
            # sustituimos hijos por su versión coherente (para auditar/score):
            M1, M2, M3 = M1c, M2c, M3c

        # 5) Traza completa
        MetaM = [M1, M2, M3, Ms]

        # 6) Scoring (ambigüedad total)
        score = self._score([M1, M2, M3], Ms, Ss)

        return {
            "M1": M1, "M2": M2, "M3": M3,
            "Ms": Ms, "Ss": Ss,
            "MetaM": MetaM,
            "wiring": roles_k,
            "score": score,
            "reconstruction_ok": reconstruction_ok,
            # NEW: reporte de coherencia absoluta para Evolver
            "coherence": coherence_report
        }

    # ---------- API principal ----------
    def solve(
        self,
        A: List[Trit],
        B: List[Trit],
        C: List[Trit],
        *,
        max_tries: int = 3,                 # NEW: solo 3 wirings únicos
        check_reconstruction: bool = True,
        enforce_coherence: bool = True      # NEW: activa coherencia absoluta
    ) -> Dict:
        """
        Explora wirings generados por rotación Fibonacci y devuelve
        el mejor resultado (mínimo score; a igualdad, prioriza reconstrucción_ok=True).
        Parada temprana si score==0 y reconstrucción_ok.
        """
        roles0 = base_roles()
        best: Dict = {}

        for k in range(max_tries):
            roles_k = rotate_roles(roles0, k)
            res = self._try_wiring(A, B, C, roles_k, check_reconstruction, enforce_coherence=enforce_coherence)

            if not best:
                best = res
            else:
                if (res["score"] < best["score"]) or (
                    res["score"] == best["score"] and res["reconstruction_ok"] and not best["reconstruction_ok"]
                ):
                    best = res

            if res["score"] == 0 and res["reconstruction_ok"]:
                break

        return best


# Harmonizer.py
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Tuple

Trit = Optional[int]  # 0 | 1 | None
Tripleta = Tuple[List[Trit], List[Trit], List[Trit]]  # (x,y,z) cada uno 3 bits


def norm3(v: List[Trit]) -> List[Trit]:
    v = (list(v) + [0, 0, 0])[:3]
    return [None if x is None else (1 if x == 1 else 0) for x in v]


def similarity3(a: List[Trit], b: List[Trit]) -> int:
    s = 0
    for x, y in zip(a, b):
        if x is None or y is None:
            continue
        if x == y:
            s += 1
    return s


def rotate3(v: List[Trit], s: int) -> List[Trit]:
    s %= 3
    return v[-s:] + v[:-s] if s else v


@dataclass
class HarmonyResult:
    result: Dict[str, Any]  # estado final reparado del nodo (Ms padre + hijas)
    audit: List[Dict[str, Any]]
    repaired: bool
    escalated: bool = False


class Harmonizer:
    """
    Sistema de reparación por escalones (solo sobre estructuras Ms/Ss).

    Núcleo: Trigate (infer/learn) aplicado a Ms (no A,B,C).

    Escalado:
      1) Soft: re-rotación mínima de hijas (semillas) por componente (Fibonacci mod 3)
      2) Contextual: Extender (top-down) con Ss (memoria) + dinámica (priors)
      3) Local: micro-ajuste bit a bit con Trigate (cierre con el padre)
      4) Estructural: segundo mejor relator del Evolver (wiring alternativo)
      5) Arquetípico: registrar arquetipo nuevo si persiste incoherencia
    """

    def __init__(
        self, 
        trigate_cls, 
        evolver, 
        extender_cls,
        max_conflicts: int = 0,
        max_null_fills: int = 3,
        min_child_sim: int = 6
        ):
        """
        Inicializa el Harmonizer con umbrales configurables.
        
        Args:
            trigate_cls: Clase Trigate para operaciones lógicas
            evolver: Instancia del Evolver para acceso a conocimiento
            extender_cls: Clase Extender para reconstrucción top-down
            max_conflicts: Máximo de conflictos permitidos (default: 0)
            max_null_fills: Máximo de NULLs permitidos (default: 3)
            min_child_sim: Similitud mínima entre hijas (default: 6)
        """
        self.T = trigate_cls
        self.EV = evolver
        self.EXT = extender_cls(trigate_cls, evolver)

        # Umbrales configurables
        self.max_conflicts = max_conflicts
        self.max_null_fills = max_null_fills
        self.min_child_sim = min_child_sim

    # ---------- utilidades ----------
    def _coh_stats_ok(self, coh: Dict[str, int]) -> bool:
        return (
            coh.get("conflict_resolved", 999) <= self.max_conflicts
            and coh.get("null_filled", 999) <= self.max_null_fills
        )

    def _score_tripleta(self, trip: Tripleta) -> int:
        return sum(0 if b is None else 1 for comp in trip for b in comp)

    # ---------- Paso 1: Soft (re-rotación de semillas) ----------
    def _soft_rerotate_seeds(
        self,
        seeds_trip: Tuple[Tripleta, Tripleta, Tripleta],
        Ss_trip: Dict[str, List[Trit]] = None,  # memoria contextual por componente {'x':Ssx,'y':Ssy,'z':Ssz}
    ) -> Tuple[Tripleta, Tripleta, Tripleta]:
        """
        Selecciona la rotación (s∈{0,1,2}) para cada componente (x,y,z).
        Si hay Ss_d, desempata por máxima similitud a Ss_d (memory contextual).
        """

        def score_defined_bits(trio: Tripleta) -> int:
            return sum(0 if b is None else 1 for comp in trio for b in comp)

        def best_rot_of(trio: Tripleta, Ss_d: List[Trit] = None) -> Tripleta:
            M1, M2, M3 = trio
            cands = [(rotate3(M1, s), rotate3(M2, s), rotate3(M3, s)) for s in (0, 1, 2)]
            if Ss_d is None:
                return max(cands, key=score_defined_bits)
            return max(cands, key=lambda t: sum(similarity3(t[i], Ss_d) for i in range(3)))

        tags = ["x", "y", "z"]
        out = tuple(
            best_rot_of(trio, (Ss_trip or {}).get(tag))
            for tag, trio in zip(tags, seeds_trip)
        )
        return out

    # ---------- Paso 3: Local (micro-ajuste con Trigate + stats) ----------
    def _micro_adjust(
        self,
        Ms_parent: List[Trit],
        trio: Tripleta,
    ) -> Tuple[Tripleta, Dict[str, int]]:
        """
        Cierre local con Trigate.infer y reconciliación priorizando el padre (deducido).
        Devuelve tripleta ajustada + stats: null_filled, conflict_resolved.
        """
        M1, M2, M3 = [norm3(x) for x in trio]
        R3 = self.T.infer(M1, M2, norm3(Ms_parent))
        R1 = self.T.infer(M2, M3, norm3(Ms_parent))
        R2 = self.T.infer(M3, M1, norm3(Ms_parent))

        # Función de mezcla: prioriza deducido sobre observado
        def blend(obs: List[Trit], ded: List[Trit]) -> List[Trit]:
            """Reconcilia observado vs deducido, priorizando deducido en conflictos"""
            return [ded[i] if obs[i] is None else obs[i] if ded[i] is None else ded[i] 
                    for i in range(len(obs))]

        M1n, M2n, M3n = blend(M1, R1), blend(M2, R2), blend(M3, R3)

        # estadísticas locales
        stats = {"null_filled": 0, "conflict_resolved": 0}
        for (orig, ded) in ((M1, R1), (M2, R2), (M3, R3)):
            for o, d in zip(orig, ded):
                if o is None and d is not None:
                    stats["null_filled"] += 1
                elif o is not None and d is not None and o != d:
                    stats["conflict_resolved"] += 1

        return (M1n, M2n, M3n), stats

    # ---------- armonización principal ----------
    def harmonize_from_state(
        self,
        *,
        Ms_parent_triplet: Tripleta,
        children_observed: Dict[str, Tripleta],
        context_Ss: Dict[str, List[Trit]] = None,
    ) -> HarmonyResult:
        audit: List[Dict[str, Any]] = []
        Msx, Msy, Msz = [norm3(c) for c in Ms_parent_triplet]
        seeds_x, seeds_y, seeds_z = (
            children_observed["x"],
            children_observed["y"],
            children_observed["z"],
        )

        # === 1) SOFT: re-rotación mínima de semillas (con Ss si existen) ===
        seeds_rot = self._soft_rerotate_seeds(
            (seeds_x, seeds_y, seeds_z), Ss_trip=context_Ss
        )
        audit.append(
            {
                "step": "1_soft_rerotate",
                "defined_bits": sum(self._score_tripleta(s) for s in seeds_rot),
                "ctx_present": bool(context_Ss),
            }
        )

        # === 2) CONTEXTUAL: Extender (top-down) con Ss + dinámica ===
        res_ext = self.EXT.extend_triplet((Msx, Msy, Msz), seeds_triplet=seeds_rot)
        coh = res_ext.get("coherence", {})
        audit.append(
            {
                "step": "2_contextual_extend",
                "coherence": coh,
                "wiring_hints": res_ext.get("wiring_hints"),
            }
        )

        children_ctx = res_ext["children"]
        trip_x = (
            children_ctx["x"]["M1"],
            children_ctx["x"]["M2"],
            children_ctx["x"]["M3"],
        )
        trip_y = (
            children_ctx["y"]["M1"],
            children_ctx["y"]["M2"],
            children_ctx["y"]["M3"],
        )
        trip_z = (
            children_ctx["z"]["M1"],
            children_ctx["z"]["M2"],
            children_ctx["z"]["M3"],
        )

        if coh and all(self._coh_stats_ok(coh[d]) for d in coh):
            return HarmonyResult(
                result={
                    "parent": {"Msx": Msx, "Msy": Msy, "Msz": Msz},
                    "children": {"x": trip_x, "y": trip_y, "z": trip_z},
                },
                audit=audit,
                repaired=True,
            )

        # === 3) LOCAL: micro-ajuste bit a bit (Trigate) ===
        (trip_x_adj, stats_x) = self._micro_adjust(Msx, trip_x)
        (trip_y_adj, stats_y) = self._micro_adjust(Msy, trip_y)
        (trip_z_adj, stats_z) = self._micro_adjust(Msz, trip_z)

        sim_gain = (
            sum(similarity3(a, b) for a, b in zip(trip_x, trip_x_adj))
            + sum(similarity3(a, b) for a, b in zip(trip_y, trip_y_adj))
            + sum(similarity3(a, b) for a, b in zip(trip_z, trip_z_adj))
        )
        audit.append(
            {
                "step": "3_local_micro_adjust",
                "similarity_gain_bits": sim_gain,
                "local_stats": {"x": stats_x, "y": stats_y, "z": stats_z},
            }
        )

        if sim_gain >= self.min_child_sim:
            return HarmonyResult(
                result={
                    "parent": {"Msx": Msx, "Msy": Msy, "Msz": Msz},
                    "children": {"x": trip_x_adj, "y": trip_y_adj, "z": trip_z_adj},
                },
                audit=audit,
                repaired=True,
            )

        # === 4) ESTRUCTURAL: cambiar relator y re-extender ===
        alt_hints = {}
        if hasattr(self.EV, "select_relator_k"):
            for tag, Ms in (("x", Msx), ("y", Msy), ("z", Msz)):
                alts = self.EV.select_relator_k(tag, Ms, k=2)
                alt_hints[tag] = alts[1] if len(alts) > 1 else None
        audit.append({"step": "4_structural_alt_relator", "alt_hints": alt_hints})

        res_ext2 = self.EXT.extend_triplet(
            (Msx, Msy, Msz),
            seeds_triplet=(trip_x_adj, trip_y_adj, trip_z_adj),
        )
        coh2 = res_ext2.get("coherence", {})
        audit.append({"step": "4_structural_reextend", "coherence": coh2})

        if coh2 and all(self._coh_stats_ok(coh2[d]) for d in coh2):
            ch2 = res_ext2["children"]
            return HarmonyResult(
                result={
                    "parent": {"Msx": Msx, "Msy": Msy, "Msz": Msz},
                    "children": {
                        "x": (ch2["x"]["M1"], ch2["x"]["M2"], ch2["x"]["M3"]),
                        "y": (ch2["y"]["M1"], ch2["y"]["M2"], ch2["y"]["M3"]),
                        "z": (ch2["z"]["M1"], ch2["z"]["M2"], ch2["z"]["M3"]),
                    },
                },
                audit=audit,
                repaired=True,
            )

        # === 5) ARQUETÍPICO: escalar (nuevo arquetipo) ===
        incoherent = {
            "parent": {"Msx": Msx, "Msy": Msy, "Msz": Msz},
            "children": {"x": trip_x_adj, "y": trip_y_adj, "z": trip_z_adj},
        }
        audit.append({"step": "5_archetypic_escalate"})
        if hasattr(self.EV, "create_new_archetype_triplet"):
            self.EV.create_new_archetype_triplet(
                Ms_parent_triplet=(Msx, Msy, Msz),
                children_triplets=incoherent["children"],
            )
            audit.append({"archetype_created": True})

        return HarmonyResult(result=incoherent, audit=audit, repaired=True, escalated=True)



# AuroraPipline.py

"""
Aurora Pipeline - Coordinador central del sistema Aurora Trinity-3

Integra todos los módulos core y orquesta el ciclo completo:
  Ingesta → Síntesis → Aprendizaje → Armonización → Reconstrucción

Incluye:
  - SimpleKnowledgeBase: Almacenamiento en memoria con integración al Evolver
  - AuroraPipeline: Coordinador principal con soporte para Harmonizer
  - AuroraDemo: Ejemplos de uso para validación
"""

from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any, Tuple
import json

# Imports locales
from Trigate import Trigate, TrigateRecord, Trit
from Transcender import Transcender
from FractalTensor import FractalTensor, FractalTranscender
from Evolver import Evolver3
from Extender import Extender
from Harmonizer import Harmonizer, HarmonyResult


# ============================================================================
# KNOWLEDGE BASE SIMPLE
# ============================================================================

class SimpleKnowledgeBase:
    """
    Knowledge Base mínima en memoria.
    
    Funcionalidades:
      - Almacena resultados de síntesis fractal
      - Alimenta automáticamente al Evolver
      - Permite consultas por Ms, MetaM o tags
      - Mantiene estadísticas de aprendizaje
    """
    
    def __init__(self, evolver: Evolver3):
        self.evolver = evolver
        self.patterns = {}  # key -> data completa
        self.by_ms = {}     # Ms_str -> [keys]
        self.stats = {
            "total_stored": 0,
            "total_harmonized": 0,
            "total_escalated": 0
        }
    
    def store(self, key: str, data: Dict[str, Any], tag: str = "default"):
        """
        Almacena resultado y alimenta al Evolver.
        
        Args:
            key: Identificador único (ej: hash de inputs)
            data: Resultado completo de síntesis/armonización
            tag: Etiqueta de espacio lógico
        """
        # Almacenar
        self.patterns[key] = {
            "data": data,
            "tag": tag,
            "harmonized": data.get("harmony_applied", False),
            "escalated": data.get("harmony_escalated", False)
        }
        
        # Indexar por Ms si existe
        if "tensor_cross" in data:
            tensor = data["tensor_cross"]
            ms_key = self._ms_to_key(tensor.nivel_3)
            if ms_key not in self.by_ms:
                self.by_ms[ms_key] = []
            self.by_ms[ms_key].append(key)
        
        # Alimentar al Evolver
        if "audits" in data:
            self.evolver.observe_fractal(data, level_tag=tag)
        
        # Stats
        self.stats["total_stored"] += 1
        if data.get("harmony_applied"):
            self.stats["total_harmonized"] += 1
        if data.get("harmony_escalated"):
            self.stats["total_escalated"] += 1
    
    def retrieve(self, key: str) -> Optional[Dict]:
        """Recupera por clave exacta"""
        entry = self.patterns.get(key)
        return entry["data"] if entry else None
    
    def query_by_ms(self, ms_vector: List[List[Trit]]) -> List[Dict]:
        """Recupera todos los patrones con Ms similar"""
        ms_key = self._ms_to_key(ms_vector)
        keys = self.by_ms.get(ms_key, [])
        return [self.patterns[k]["data"] for k in keys]
    
    def get_stats(self) -> Dict:
        """Retorna estadísticas de la KB"""
        return {
            **self.stats,
            "unique_ms": len(self.by_ms),
            "evolver_relators": len(self.evolver._relator),
            "evolver_emergences": len(self.evolver._emerg),
            "evolver_dynamics": len(self.evolver._dyn)
        }
    
    @staticmethod
    def _ms_to_key(ms_vector: List[List[Trit]]) -> str:
        """Convierte Ms en string para indexación"""
        return str([[x if x is not None else -1 for x in v] for v in ms_vector])


# ============================================================================
# AURORA FRACTAL EVOLVER (con Harmonizer integrado)
# ============================================================================

class AuroraFractalEvolver:
    """
    Evolucionador fractal con reparación post-síntesis.
    
    Ejecuta ciclos de síntesis fractal (27→9→3) y aplica el Harmonizer
    después de cada ronda para detectar y reparar incoherencias.
    
    Features:
      - Síntesis fractal completa (cross-level + self-synthesis)
      - Reparación automática con Harmonizer (5 niveles)
      - Auditoría completa y trazabilidad
      - Registro de arquetipos nuevos cuando se escala
    """
    
    def __init__(
        self, 
        transcender_cls, 
        trigate_cls, 
        evolver, 
        extender_cls,
        harmonizer_cls=None
    ):
        self.trigate_cls = trigate_cls
        self.transcender_core = transcender_cls(trigate_cls)
        self.fractal_tx = FractalTranscender(transcender_cls)
        self.evolver = evolver
        self.extender = extender_cls(trigate_cls, evolver)
        self.harmonizer = harmonizer_cls(trigate_cls, evolver, extender_cls) if harmonizer_cls else None
    
    def synthesize_with_harmony(
        self,
        A: FractalTensor,
        B: FractalTensor,
        C: FractalTensor,
        apply_harmony: bool = True
    ) -> Dict[str, Any]:
        """
        Síntesis fractal completa con armonización opcional.
        
        Args:
            A, B, C: FractalTensors de entrada
            apply_harmony: Si True, aplica Harmonizer post-síntesis
        
        Returns:
            Dict con:
              - tensor_cross: FractalTensor resultado
              - Ss: Síntesis factuales por nivel
              - audits: Auditorías completas
              - harmony_applied: Bool indicando si se reparó
              - harmony_audit: Auditoría del Harmonizer (si aplica)
              - harmony_escalated: Bool indicando si se escaló a arquetipo
        """
        # 1. Síntesis fractal base
        result = self.fractal_tx.synthesize(A, B, C, self.transcender_core)
        
        tensor = result["tensor_cross"]
        audits = result["audits"]
        Ss = result["Ss"]
        
        # 2. Aplicar Harmonizer si está disponible y habilitado
        harmony_applied = False
        harmony_escalated = False
        harmony_audit = []
        
        if apply_harmony and self.harmonizer:
            # Preparar datos para el Harmonizer
            Ms_triplet = (
                tensor.nivel_3[0],
                tensor.nivel_3[1],
                tensor.nivel_3[2]
            )
            
            children_observed = {
                "x": (A.nivel_3[0], A.nivel_3[1], A.nivel_3[2]),
                "y": (B.nivel_3[0], B.nivel_3[1], B.nivel_3[2]),
                "z": (C.nivel_3[0], C.nivel_3[1], C.nivel_3[2])
            }
            
            context_Ss = {}
            if Ss and "lvl3" in Ss and len(Ss["lvl3"]) >= 3:
                context_Ss = {
                    "x": Ss["lvl3"][0],
                    "y": Ss["lvl3"][1],
                    "z": Ss["lvl3"][2]
                }
            
            # Ejecutar armonización
            harmony = self.harmonizer.harmonize_from_state(
                Ms_parent_triplet=Ms_triplet,
                children_observed=children_observed,
                context_Ss=context_Ss
            )
            
            # Si hubo reparación, actualizar el tensor
            if harmony.repaired:
                harmony_applied = True
                harmony_escalated = harmony.escalated
                harmony_audit = harmony.audit
                
                # Actualizar tensor con valores reparados
                children = harmony.result["children"]
                tensor.nivel_3 = [
                    children["x"][0],  # Ms_x hijo 0
                    children["y"][0],  # Ms_y hijo 0
                    children["z"][0]   # Ms_z hijo 0
                ]
                
                # Agregar info de harmony al audit
                if "lvl3" in audits and len(audits["lvl3"]) > 0:
                    audits["lvl3"][0]["harmony"] = {
                        "applied": True,
                        "escalated": harmony_escalated,
                        "steps": len(harmony_audit)
                    }
        
        # 3. Retornar resultado completo
        return {
            "tensor_cross": tensor,
            "Ss": Ss,
            "audits": audits,
            "locals": result.get("locals", {}),
            "harmony_applied": harmony_applied,
            "harmony_audit": harmony_audit,
            "harmony_escalated": harmony_escalated
        }
    
    def evolve_batch(
        self,
        tensor_batch: List[Tuple[FractalTensor, FractalTensor, FractalTensor]],
        apply_harmony: bool = True
    ) -> List[Dict[str, Any]]:
        """
        Procesa un batch de trios de tensores.
        
        Args:
            tensor_batch: Lista de (A, B, C) tensores
            apply_harmony: Aplicar Harmonizer a cada síntesis
        
        Returns:
            Lista de resultados (uno por trio)
        """
        results = []
        for A, B, C in tensor_batch:
            result = self.synthesize_with_harmony(A, B, C, apply_harmony)
            results.append(result)
        return results


# ============================================================================
# AURORA PIPELINE (Coordinador Principal)
# ============================================================================

class AuroraPipeline:
    """
    Coordinador central del sistema Aurora.
    
    Orquesta el ciclo completo:
      1. Inicializa todos los módulos
      2. Gestiona el flujo de datos
      3. Coordina síntesis, aprendizaje y armonización
      4. Mantiene la Knowledge Base
    
    Uso básico:
        pipeline = AuroraPipeline()
        result = pipeline.run_cycle(data_A, data_B, data_C)
    """
    
    def __init__(self, enable_harmony: bool = True, verbose: bool = True):
        """
        Inicializa el pipeline completo.
        
        Args:
            enable_harmony: Habilitar Harmonizer post-síntesis
            verbose: Imprimir mensajes de progreso
        """
        self.verbose = verbose
        self.enable_harmony = enable_harmony
        
        if self.verbose:
            print("🌅 Inicializando Aurora Pipeline...")
        
        # 1. Módulos base
        self.trigate_cls = Trigate
        if self.verbose:
            print("  ✅ Trigate (LUTs ternarias)")
        
        # 2. Evolver (debe inicializarse antes que Extender y Harmonizer)
        self.evolver = Evolver3(Trigate, th_match=2, decay=0.9)
        if self.verbose:
            print("  ✅ Evolver3 (RELATOR + EMERGENCIA + DINÁMICA)")
        
        # 3. Módulos de síntesis
        self.transcender_cls = Transcender
        self.transcender = Transcender(Trigate)
        if self.verbose:
            print("  ✅ Transcender (síntesis jerárquica)")
        
        # 4. Extender (requiere Evolver)
        self.extender_cls = Extender
        self.extender = Extender(Trigate, self.evolver)
        if self.verbose:
            print("  ✅ Extender (reconstrucción top-down)")
        
        # 5. Harmonizer (requiere Trigate, Evolver y Extender)
        self.harmonizer_cls = Harmonizer if enable_harmony else None
        self.harmonizer = Harmonizer(Trigate, self.evolver, Extender) if enable_harmony else None
        if self.verbose:
            if enable_harmony:
                print("  ✅ Harmonizer (reparación 5 niveles)")
            else:
                print("  ⚠️  Harmonizer deshabilitado")
        
        # 6. Evolver Fractal (orquestador de síntesis)
        self.fractal_evolver = AuroraFractalEvolver(
            transcender_cls=Transcender,
            trigate_cls=Trigate,
            evolver=self.evolver,
            extender_cls=Extender,
            harmonizer_cls=Harmonizer if enable_harmony else None
        )
        if self.verbose:
            print("  ✅ FractalEvolver (síntesis + armonización)")
        
        # 7. Knowledge Base
        self.kb = SimpleKnowledgeBase(self.evolver)
        if self.verbose:
            print("  ✅ KnowledgeBase (almacenamiento + stats)")
        
        if self.verbose:
            print("\n✨ Aurora Pipeline listo para operar\n")
    
    def process_input(
        self,
        data_A: List[List[Trit]],
        data_B: List[List[Trit]],
        data_C: List[List[Trit]]
    ) -> Tuple[FractalTensor, FractalTensor, FractalTensor]:
        """
        Convierte datos crudos en FractalTensors.
        
        Args:
            data_A, data_B, data_C: Listas de vectores ternarios
        
        Returns:
            Tupla de (tensor_A, tensor_B, tensor_C)
        """
        def ensure_27(data: List[List[Trit]]) -> List[List[Trit]]:
            """Asegura que tengamos 27 vectores de 3 bits"""
            data = list(data)
            while len(data) < 27:
                data.append([0, 0, 0])
            return data[:27]
        
        # Asegurar longitud correcta
        data_A = ensure_27(data_A)
        data_B = ensure_27(data_B)
        data_C = ensure_27(data_C)
        
        # Crear tensores fractales
        tensor_A = FractalTensor(
            nivel_27=data_A,
            nivel_9=data_A[:9],  # Placeholder, se sintetizará
            nivel_3=data_A[:3]    # Placeholder, se sintetizará
        ).normalize()
        
        tensor_B = FractalTensor(
            nivel_27=data_B,
            nivel_9=data_B[:9],
            nivel_3=data_B[:3]
        ).normalize()
        
        tensor_C = FractalTensor(
            nivel_27=data_C,
            nivel_9=data_C[:9],
            nivel_3=data_C[:3]
        ).normalize()
        
        return tensor_A, tensor_B, tensor_C
    
    def run_cycle(
        self,
        data_A: List[List[Trit]],
        data_B: List[List[Trit]],
        data_C: List[List[Trit]],
        tag: str = "default"
    ) -> Dict[str, Any]:
        """
        Ejecuta ciclo completo: Ingesta → Síntesis → Aprendizaje → Storage.
        
        Args:
            data_A, data_B, data_C: Datos de entrada
            tag: Etiqueta de espacio lógico
        
        Returns:
            Resultado completo con tensor, auditorías, harmony info
        """
        if self.verbose:
            print(f"🔄 Ejecutando ciclo completo (tag: {tag})...")
        
        # 1. Procesar inputs
        tensor_A, tensor_B, tensor_C = self.process_input(data_A, data_B, data_C)
        if self.verbose:
            print("  ✓ Inputs procesados → FractalTensors")
        
        # 2. Síntesis con armonización
        result = self.fractal_evolver.synthesize_with_harmony(
            tensor_A, tensor_B, tensor_C,
            apply_harmony=self.enable_harmony
        )
        if self.verbose:
            print("  ✓ Síntesis fractal ejecutada")
            if result["harmony_applied"]:
                print(f"    🔧 Harmonizer aplicado ({len(result['harmony_audit'])} pasos)")
                if result["harmony_escalated"]:
                    print("    ⚠️  Escalado a arquetipo nuevo")
        
        # 3. Almacenar en KB (esto alimenta automáticamente al Evolver)
        key = f"{tag}_{hash((str(data_A), str(data_B), str(data_C)))}"
        self.kb.store(key, result, tag)
        if self.verbose:
            print("  ✓ Resultado almacenado en KB")
        
        # 4. Stats
        if self.verbose:
            stats = self.kb.get_stats()
            print(f"\n📊 Stats: {stats['total_stored']} almacenados, "
                  f"{stats['total_harmonized']} armonizados, "
                  f"{stats['total_escalated']} escalados")
        
        return result
    
    def get_stats(self) -> Dict[str, Any]:
        """Retorna estadísticas completas del sistema"""
        return {
            "kb": self.kb.get_stats(),
            "harmony_enabled": self.enable_harmony
        }


# ============================================================================
# DEMOS
# ============================================================================

class AuroraDemo:
    """Demostraciones para validar el pipeline"""
    
    def __init__(self, enable_harmony: bool = True):
        self.pipeline = AuroraPipeline(enable_harmony=enable_harmony, verbose=True)
    
    def demo_basic_synthesis(self):
        """Demo: Síntesis básica sin conflictos"""
        print("\n" + "="*70)
        print("DEMO 1: Síntesis Básica")
        print("="*70 + "\n")
        
        # Datos simples y coherentes
        data_A = [[1, 0, 1]] * 27
        data_B = [[0, 1, 0]] * 27
        data_C = [[1, 1, 0]] * 27
        
        result = self.pipeline.run_cycle(data_A, data_B, data_C, tag="demo1")
        
        print("\n📋 Resultado:")
        print(f"  Ms nivel_3: {result['tensor_cross'].nivel_3}")
        print(f"  Harmony aplicado: {result['harmony_applied']}")
        print(f"  Escalado: {result['harmony_escalated']}")
        
        return result
    
    def demo_with_conflicts(self):
        """Demo: Síntesis con incoherencias que requieren armonización"""
        print("\n" + "="*70)
        print("DEMO 2: Síntesis con Conflictos (requiere Harmonizer)")
        print("="*70 + "\n")
        
        # Datos con más variación para provocar incoherencias
        data_A = [[1, 0, 1], [0, 1, 0], [1, 1, 0]] * 9
        data_B = [[0, 1, 1], [1, 0, 1], [0, 0, 1]] * 9
        data_C = [[1, 1, 1], [0, 0, 0], [None, 1, None]] * 9
        
        result = self.pipeline.run_cycle(data_A, data_B, data_C, tag="demo2")
        
        print("\n📋 Resultado:")
        print(f"  Ms nivel_3: {result['tensor_cross'].nivel_3}")
        print(f"  Harmony aplicado: {result['harmony_applied']}")
        if result['harmony_applied']:
            print(f"  Pasos de reparación: {len(result['harmony_audit'])}")
            print(f"  Escalado: {result['harmony_escalated']}")
        
        return result
    
    def demo_batch_processing(self):
        """Demo: Procesamiento en lote"""
        print("\n" + "="*70)
        print("DEMO 3: Procesamiento en Lote (3 ciclos)")
        print("="*70 + "\n")
        
        batches = [
            ([[1, 0, 0]] * 27, [[0, 1, 0]] * 27, [[0, 0, 1]] * 27),
            ([[1, 1, 0]] * 27, [[0, 1, 1]] * 27, [[1, 0, 1]] * 27),
            ([[1, 1, 1]] * 27, [[0, 0, 0]] * 27, [[1, 0, 1]] * 27),
        ]
        
        for i, (A, B, C) in enumerate(batches, 1):
            print(f"\n--- Batch {i}/3 ---")
            result = self.pipeline.run_cycle(A, B, C, tag=f"batch_{i}")
        
        print("\n📊 Estadísticas finales:")
        stats = self.pipeline.get_stats()
        print(json.dumps(stats, indent=2))
    
    def demo_reconstruction(self):
        """Demo: Ciclo completo con reconstrucción"""
        print("\n" + "="*70)
        print("DEMO 4: Síntesis + Reconstrucción")
        print("="*70 + "\n")
        
        # 1. Síntesis
        data_A = [[1, 0, 1]] * 27
        data_B = [[0, 1, 0]] * 27
        data_C = [[1, 1, 0]] * 27
        
        result = self.pipeline.run_cycle(data_A, data_B, data_C, tag="demo4")
        
        # 2. Reconstrucción
        Ms_triplet = (
            result['tensor_cross'].nivel_3[0],
            result['tensor_cross'].nivel_3[1],
            result['tensor_cross'].nivel_3[2]
        )
        
        print("\n🔄 Reconstruyendo desde Ms_triplet...")
        reconstructed = self.pipeline.extender.extend_triplet(Ms_triplet)
        
        print("\n📋 Reconstrucción:")
        print(f"  Children x: {reconstructed['children']['x']}")
        print(f"  Children y: {reconstructed['children']['y']}")
        print(f"  Children z: {reconstructed['children']['z']}")
        print(f"  Coherencia: {reconstructed.get('coherence', {})}")


# ============================================================================
# MAIN
# ============================================================================

if __name__ == "__main__":
    print("\n" + "🌅"*35)
    print("   AURORA PIPELINE - Sistema Completo con Harmonizer")
    print("🌅"*35 + "\n")
    
    # Crear demo
    demo = AuroraDemo(enable_harmony=True)
    
    # Ejecutar demos
    try:
        demo.demo_basic_synthesis()
        demo.demo_with_conflicts()
        demo.demo_batch_processing()
        demo.demo_reconstruction()
        
        print("\n" + "="*70)
        print("✅ TODOS LOS DEMOS COMPLETADOS EXITOSAMENTE")
        print("="*70 + "\n")
        
    except Exception as e:
        print(f"\n❌ Error en demo: {e}")
        import traceback
        traceback.print_exc()
