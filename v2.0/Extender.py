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
