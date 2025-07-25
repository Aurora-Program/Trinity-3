

import random, pytest
from allcode3new import *
import tracemalloc
import threading


def random_tensor(seed=None):
    rng = random.Random(seed)
    bit = lambda: rng.choice([0, 1, None])
    vec = lambda: [bit(), bit(), bit()]
    return FractalTensor(
        nivel_3=[vec() for _ in range(3)],
        nivel_9=[vec() for _ in range(9)],
        nivel_27=[vec() for _ in range(27)],
    )


# c:\Users\p_m_a\Aurora\Trinity-3\Test\test_trigate.py


trigate = Trigate()

def rand_trit():
    return random.choice([0, 1, None])

@pytest.mark.parametrize("a,b,m", [(rand_trit(), rand_trit(), rand_trit()) for _ in range(2000)])




def test_infer_learn_roundtrip(a, b, m):
    r = trigate.infer([a], [b], [m])[0]
    m2 = trigate.learn([a], [b], [r])[0]
    if None not in (a, b, m, r):
        assert m2 == m

trigate = Trigate()
dataset = [([random.randint(0,1)], [random.randint(0,1)], [random.randint(0,1)]) for _ in range(10_000)]

def test_infer_speed(benchmark):
    benchmark(lambda: [trigate.infer(A, B, M) for A, B, M in dataset])


evolver = Evolver()
tensor_bag = [random_tensor(i) for i in range(300)]

def test_archetype_speed(benchmark):
    benchmark(lambda: evolver.compute_fractal_archetype(tensor_bag[:9]))


def test_memory_usage():
    kb = FractalKnowledgeBase()
    tracemalloc.start()
    for i in range(10_000):
        t = FractalTensor.random()
        kb.add_archetype('stress', f'id{i}', t, Ss=t.nivel_3[0])
    _, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    assert peak < 80 * 1024 ** 2   # < 80 MiB



def test_thread_safety():
    kb = FractalKnowledgeBase()
    def writer(idx):
        t = FractalTensor.random()
        kb.add_archetype('threads', f'id{idx}', t, Ss=t.nivel_3[0])
    threads = [threading.Thread(target=writer, args=(i,)) for i in range(200)]
    for th in threads: th.start()
    for th in threads: th.join()
    universe = kb._get_space('threads')
    assert universe.coherence_violations == 0




if __name__ == "__main__":
    # Ejecuta todos los tests y benchmarks
    pytest.main(["-q", "Test"])