from typing import List, Optional, Tuple

class TernaryLogic:
    NULL = None

    @staticmethod
    def xor(a: Optional[int], b: Optional[int]) -> Optional[int]:
        if a is None or b is None:
            return None
        return (a ^ b) % 2

    @staticmethod
    def xnor(a: Optional[int], b: Optional[int]) -> Optional[int]:
        if a is None or b is None:
            return None
        return 1 if a == b else 0

class TrigateCanonical:
    _LUT_INFER = {}
    _LUT_LEARN = {}
    _LUT_DEDUCE_A = {}
    _LUT_DEDUCE_B = {}

    def __init__(self):
        if not TrigateCanonical._LUT_INFER:
            self._build_luts()

    def _build_luts(self):
        states = [0, 1, None]
        for a in states:
            for b in states:
                for r in states:
                    m = None
                    if a is not None and b is not None and r is not None:
                        if (a ^ b) == r: m = 1
                        elif (1 - (a ^ b)) == r: m = 0
                    TrigateCanonical._LUT_LEARN[(a, b, r)] = m

                for m in states:
                    r = None
                    if a is not None and b is not None and m is not None:
                        r = a ^ b if m == 1 else 1 - (a ^ b)
                    TrigateCanonical._LUT_INFER[(a, b, m)] = r

        for m in states:
            for r in states:
                for b in states:
                    a = None
                    if b is not None and m is not None and r is not None:
                        a = b ^ r if m == 1 else 1 - (b ^ r)
                    TrigateCanonical._LUT_DEDUCE_A[(b, m, r)] = a

                for a in states:
                    b = None
                    if a is not None and m is not None and r is not None:
                        b = a ^ r if m == 1 else 1 - (a ^ r)
                    TrigateCanonical._LUT_DEDUCE_B[(a, m, r)] = b

    def infer(self, A: List[int], B: List[int], M: List[int]) -> List[Optional[int]]:
        return [self._LUT_INFER.get((a, b, m)) for a, b, m in zip(A, B, M)]

    def learn(self, A: List[int], B: List[int], R: List[int]) -> List[Optional[int]]:
        return [self._LUT_LEARN.get((a, b, r)) for a, b, r in zip(A, B, R)]

    def deduce_A(self, B: List[int], M: List[int], R: List[int]) -> List[Optional[int]]:
        return [self._LUT_DEDUCE_A.get((b, m, r)) for b, m, r in zip(B, M, R)]

    def deduce_B(self, A: List[int], M: List[int], R: List[int]) -> List[Optional[int]]:
        return [self._LUT_DEDUCE_B.get((a, m, r)) for a, m, r in zip(A, M, R)]

    def synthesize(self, A: List[int], B: List[int]) -> Tuple[List[Optional[int]], List[Optional[int]]]:
        M = [TernaryLogic.xor(a, b) for a, b in zip(A, B)]
        S = [TernaryLogic.xnor(a, b) for a, b in zip(A, B)]
        return M, S
