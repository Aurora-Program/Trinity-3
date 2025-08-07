# Aurora Trinity-3: Pruebas Unitarias Fundamentales
# Ejecuta con: pytest test_aurora_unittest.py

from allcode3new import (
    FractalTensor, Trigate, Transcender, Evolver, InverseEvolver, Armonizador, FractalKnowledgeBase, Extender, impute_none
)

def test_fractaltensor_init():
    t = FractalTensor(nivel_3=[[1,0,1]])
    assert len(t.nivel_3) == 3
    assert len(t.nivel_9) == 9
    assert len(t.nivel_27) == 27
    assert t.nivel_3[0] == [1,0,1]
    assert all(len(v)==3 for v in t.nivel_9)
    assert all(len(v)==3 for v in t.nivel_27)

def test_trigate_infer_learn_symmetry():
    trigate = Trigate()
    A, B, M = [1,0,1], [0,1,1], [1,1,0]
    R = trigate.infer(A, B, M)
    M_learned = trigate.learn(A, B, R)
    assert M_learned == M

def test_trigate_synthesize_and_reconstruct():
    trigate = Trigate()
    A, B = [1,0,1], [0,1,1]
    M, S = trigate.synthesize(A, B)
    assert isinstance(M, list) and len(M) == 3
    assert isinstance(S, list) and len(S) == 3

def test_transcender_emergence():
    t = Transcender()
    A, B, C = [1,0,1], [0,1,1], [1,1,0]
    result = t.compute_vector_trio(A, B, C)
    assert "M_emergent" in result
    assert "MetaM" in result
    assert len(result["M_emergent"]) == 3

def test_evolver_archetype():
    evolver = Evolver()
    T1 = FractalTensor(nivel_3=[[1,0,1]])
    T2 = FractalTensor(nivel_3=[[1,0,1]])
    T3 = FractalTensor(nivel_3=[[1,0,1]])
    archetype = evolver.compute_fractal_archetype([T1, T2, T3])
    assert isinstance(archetype, FractalTensor)

def test_evolver_dynamics_gradient():
    evolver = Evolver()
    T1 = FractalTensor(nivel_3=[[1,0,1]])
    T2 = FractalTensor(nivel_3=[[0,1,1]])
    T3 = FractalTensor(nivel_3=[[1,1,0]])
    dynamic = evolver.analyze_fractal_dynamics([T1, T2, T3])
    assert hasattr(dynamic, "dMs")
    assert len(dynamic.dMs) == 3

def test_armonizador_harmonize():
    armo = Armonizador()
    tensor = [1, None, 0]
    archetype = [1, 1, 0]
    result = armo.harmonize(tensor, archetype=archetype)
    assert isinstance(result, dict)
    assert "output" in result
    assert result["output"][0] == 1

def test_extender_reverse_consistency():
    kb = FractalKnowledgeBase()
    extender = Extender(kb)
    tensor = FractalTensor(nivel_3=[[1,0,1]])
    kb.add_archetype("test", "arq", tensor, Ss=[1,0,1])
    result = extender.extend_fractal([1,0,1], contexto={"space_id": "test"})
    assert "reconstructed_tensor" in result
    assert isinstance(result["reconstructed_tensor"], FractalTensor)
    assert result["reconstructed_tensor"].nivel_3[0] == [1,0,1]

def test_synthesize_extend_cycle():
    evolver = Evolver()
    kb = FractalKnowledgeBase()
    extender = Extender(kb)
    T1 = FractalTensor(nivel_3=[[1,0,1]])
    T2 = FractalTensor(nivel_3=[[0,1,1]])
    T3 = FractalTensor(nivel_3=[[1,1,0]])
    archetype = evolver.compute_fractal_archetype([T1, T2, T3])
    kb.add_archetype("ciclo", "arq_cycle", archetype, Ss=archetype.nivel_3[0])
    result = extender.extend_fractal(archetype.nivel_3[0], contexto={"space_id": "ciclo"})
    assert result["reconstructed_tensor"].nivel_3[0] == archetype.nivel_3[0]

def test_total_inversion():
    evolver = Evolver()
    inverse = InverseEvolver()
    T1, T2, T3 = [FractalTensor(nivel_3=[[1,0,1]]), FractalTensor(nivel_3=[[0,1,1]]), FractalTensor(nivel_3=[[1,1,0]])]
    synthesized = evolver.compute_fractal_archetype([T1, T2, T3])
    inversed = inverse.reconstruct_fractal(synthesized)
    assert isinstance(inversed, list)
    assert all(isinstance(t, FractalTensor) for t in inversed)

def test_impute_none():
    vec = [1, None, 0]
    context = [[1,1,0],[1,0,0],[1,1,1]]
    result = impute_none(vec, context)
    assert result[1] in (0,1)
