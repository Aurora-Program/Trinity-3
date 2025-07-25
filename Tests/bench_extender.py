# tests/bench_extender.py
import pytest, random

import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from allcode3new import Extender, KnowledgeBase, FractalTensor

kb = KnowledgeBase()
extender = Extender(knowledge_base=kb)

# Pre-calienta la KB con 100 arquetipos
for i in range(100):
    seed = [random.randint(0, 1) for _ in range(3)]
    t = FractalTensor(nivel_3=[seed, seed, seed], Ss=seed)
    kb.add_archetype('bench', f'id{i}', t, Ss=seed)

query_hit   = kb.universes['bench'].archetypes[0].Ss
query_miss  = [1, 1, 1]  # improbable que exista

def test_extender_hit(benchmark):
    benchmark(lambda: extender.extend_fractal(query_hit, {'space_id': 'bench'}))

def test_extender_miss(benchmark):
    benchmark(lambda: extender.extend_fractal(query_miss, {'space_id': 'bench'}))
