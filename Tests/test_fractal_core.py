import pytest
from allcode3 import TernaryLogic, Trigate, Transcender
from allcode3 import FractalTensor, Evolver, Extender

# --- TESTS DE LÓGICA TERNARIA ---
def test_ternary_xor():
    assert TernaryLogic.ternary_xor(1, 0) == 1
    assert TernaryLogic.ternary_xor(1, 1) == 0
    assert TernaryLogic.ternary_xor(0, 0) == 0
    assert TernaryLogic.ternary_xor(None, 1) is None
    assert TernaryLogic.ternary_xor(0, None) is None

def test_ternary_xnor():
    assert TernaryLogic.ternary_xnor(1, 1) == 1
    assert TernaryLogic.ternary_xnor(0, 0) == 1
    assert TernaryLogic.ternary_xnor(1, 0) == 0
    assert TernaryLogic.ternary_xnor(None, 1) is None
    assert TernaryLogic.ternary_xnor(0, None) is None

# --- TESTS DE TRIGATE Y LUTS ---
def test_trigate_synthesize():
    trigate = Trigate()
    M, S = trigate.synthesize([1,0,1], [0,1,1])
    assert isinstance(M, list) and isinstance(S, list)
    assert len(M) == 3 and len(S) == 3

# --- TESTS DE SÍNTESIS FRACTAL 27→9→3 ---
def test_transcender_full_fractal():
    transcender = Transcender()
    t1 = FractalTensor.random()
    t2 = FractalTensor.random()
    t3 = FractalTensor.random()
    result = transcender.compute_full_fractal(t1, t2, t3)
    assert isinstance(result, FractalTensor)
    assert len(result.nivel_3) == 3
    assert len(result.nivel_9) == 9
    assert len(result.nivel_27) == 27

# --- TESTS DE POOL Y TRÍO ---
def test_tensor_pool_trio():
    from allcode3 import TensorPoolManager
    pool = TensorPoolManager()
    t1 = FractalTensor.random()
    t2 = FractalTensor.random()
    t3 = FractalTensor.random()
    pool.add_tensor(t1)
    pool.add_tensor(t2)
    pool.add_tensor(t3)
    trio = pool.get_tensor_trio()
    assert isinstance(trio, list)
    assert len(trio) == 3
    assert all(isinstance(t, FractalTensor) for t in trio)

# --- TESTS DE EXTENSIÓN Y KB ---
def test_extender_and_kb():
    from allcode3 import FractalKnowledgeBase, Extender, Evolver
    kb = FractalKnowledgeBase()
    evolver = Evolver()
    extender = Extender(kb, evolver)
    t = FractalTensor.random()
    kb.add_archetype('test_space', 'test_tensor', t)
    incomplete = FractalTensor(nivel_3=t.nivel_3)
    result = extender.extend_fractal(incomplete, {'space_id': 'test_space'})
    assert 'reconstructed_tensor' in result
    assert isinstance(result['reconstructed_tensor'], FractalTensor)

if __name__ == "__main__":
    pytest.main()
