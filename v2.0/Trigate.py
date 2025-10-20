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