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