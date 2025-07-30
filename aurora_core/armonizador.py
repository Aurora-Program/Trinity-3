from __future__ import annotations
"""
Armonizador
===========
Complemento autosimilar para Aurora Trinity‑3 que afina 
coherencia y corrige ambigüedades a tres escalones:

1. *Vector*  – Micro‑ajusta las coordenadas Ss/Ms/MetaM.
2. *Regla*   – Re‑encamina entradas en LUT / Knowledge‑Base.
3. *Valor*   – Sintoniza parámetros globales (umbral, pesos…).

El módulo está pensado como *post‑hook* del `Extender`;
llámese después de cada reconstrucción para garantizar
consonancia.
"""
from typing import List, Tuple, Dict, Any, Optional
import itertools
import warnings

Vector = List[Optional[int]]  # Ternary value: 0 | 1 | None


# (eliminada importación duplicada)
from math import prod, pow
from functools import reduce
from dataclasses import dataclass
import copy

@dataclass(frozen=True)
class AmbiguityScore:
    score: int
    path: str = ""
    def __int__(self):
        return self.score

def _h_vec(v, a):
    """Ambigüedad de dos vectores ternarios (≈ la antigua función)."""
    return sum(
        1 if x is None or y is None or x != y else 0
        for x, y in zip(v, a)
    )

def ambiguity_fractal(tensor, arche, *, op='xor') -> float:
    """ℋ fractal ∈ [0,3] donde 0 = idéntico, 3 ≈ totalmente ambiguo."""
    # Nivel 3
    H3 = _h_vec(tensor.nivel_3[0], arche.nivel_3[0])
    # Nivel 9
    h9 = [_h_vec(v, a) for v, a in zip(tensor.nivel_9, arche.nivel_9)]
    H9 = pow(prod(max(1, h) for h in h9), 1/9) - 1   # geom. mean
    # Nivel 27
    h27 = [_h_vec(v, a) for v, a in zip(tensor.nivel_27, arche.nivel_27)]
    H27 = pow(prod(max(1, h) for h in h27), 1/27) - 1
    # Combinar (XOR ternario o suma ponderada)
    if op == 'xor':
        ℋ = int(H3) ^ int(round(H9)) ^ int(round(H27))
    else:             # weighted sum, weights ↓ con la escala
        ℋ = (H3 * 0.6) + (H9 * 0.3) + (H27 * 0.1)
    return ℋ, H3, H9, H27


class Armonizador:
    """Afinador jerárquico autosimilar con ambigüedad fractal."""
    def __init__(self, knowledge_base=None, *,
                 tau_1: float = 1.0, tau_2: float = 1.5, tau_3: float = 2.0):
        self.kb = knowledge_base
        self.tau_1, self.tau_2, self.tau_3 = tau_1, tau_2, tau_3

    def _microshift(self, tensor, archetype):
        # Solo ajusta la raíz (nivel_3[0])
        vec = tensor.nivel_3[0]
        arch = archetype.nivel_3[0]
        neighbors = []
        for deltas in itertools.product([-1, 0, 1], repeat=len(vec)):
            cand = []
            for v, d in zip(vec, deltas):
                if v is None:
                    cand.append(v)
                else:
                    nv = v + d
                    if nv < 0:
                        nv = 0
                    elif nv > 1:
                        nv = 1
                    cand.append(nv)
            neighbors.append(cand)
        best = vec
        best_score = _h_vec(vec, arch)
        for n in neighbors:
            sc = _h_vec(n, arch)
            if sc < best_score:
                best, best_score = n, sc
        # Devuelve un nuevo tensor con la raíz ajustada
        t2 = copy.deepcopy(tensor)
        t2.nivel_3[0] = best
        return t2

    def _regrewire(self, tensor, archetype, space_id="default"):
        # Busca en la KB un Ms cercano y ajusta la raíz
        if self.kb is None:
            return tensor
        ms_query = tensor.nivel_3[0]
        # Suponemos que la KB tiene método find_archetype_by_ms
        match = self.kb.find_archetype_by_ms(space_id, ms_query)
        if match:
            t2 = copy.deepcopy(tensor)
            t2.nivel_3[0] = match.nivel_3[0]
            return t2
        return tensor

    def _metatune(self, tensor, archetype):
        # Suavizado ternario: None→None, 0→0, 1→1 (identidad, pero aquí puedes poner otra lógica)
        vec = tensor.nivel_3[0]
        tuned = [v if v in (0,1) else None for v in vec]
        t2 = copy.deepcopy(tensor)
        t2.nivel_3[0] = tuned
        return t2

    def harmonize(self, tensor, *, archetype=None, space_id="default") -> dict:
        """
        Afinado fractal: opera sobre FractalTensor y compara todos los niveles.
        Devuelve dict con output, stage, ambiguity fractal y métricas parciales.
        """
        # Import FractalTensor here to avoid circular imports
        try:
            from allcode import FractalTensor
        except ImportError:
            from ..allcode import FractalTensor

        # Wrap tensor and archetype as FractalTensor if needed
        def ensure_tensor(obj):
            if hasattr(obj, 'nivel_3') and hasattr(obj, 'nivel_9') and hasattr(obj, 'nivel_27'):
                return obj
            # Assume it's a vector (list of ints/None)
            return FractalTensor(nivel_3=[obj], nivel_9=[[None, None, None] for _ in range(9)], nivel_27=[[None, None, None] for _ in range(27)])

        tensor_ft = ensure_tensor(tensor)
        if archetype is None:
            archetype_ft = tensor_ft
        else:
            archetype_ft = ensure_tensor(archetype)

        ℋ, H3, H9, H27 = ambiguity_fractal(tensor_ft, archetype_ft)
        if ℋ <= self.tau_1:
            return {"output": tensor, "stage": "vector", "ambiguity": ℋ, "H3": H3, "H9": H9, "H27": H27}
        t2 = self._microshift(tensor, archetype)
        t2_ft = ensure_tensor(t2)
        ℋ2, H3_2, H9_2, H27_2 = ambiguity_fractal(t2_ft, archetype_ft)
        if ℋ2 <= self.tau_2:
            return {"output": t2, "stage": "microshift", "ambiguity": ℋ2, "H3": H3_2, "H9": H9_2, "H27": H27_2}
        t3 = self._regrewire(t2, archetype, space_id=space_id)
        t3_ft = ensure_tensor(t3)
        ℋ3, H3_3, H9_3, H27_3 = ambiguity_fractal(t3_ft, archetype_ft)
        if ℋ3 <= self.tau_3:
            return {"output": t3, "stage": "regrewire", "ambiguity": ℋ3, "H3": H3_3, "H9": H9_3, "H27": H27_3}
        t4 = self._metatune(t3, archetype)
        t4_ft = ensure_tensor(t4)
        ℋ4, H3_4, H9_4, H27_4 = ambiguity_fractal(t4_ft, archetype_ft)
        stage = "metatune" if ℋ4 <= self.tau_3 else "falla_critica"
        if stage == "falla_critica":
            warnings.warn("Armonizador: falla crítica – no se pudo reducir ambigüedad fractal")
        return {"output": t4, "stage": stage, "ambiguity": ℋ4, "H3": H3_4, "H9": H9_4, "H27": H27_4}
