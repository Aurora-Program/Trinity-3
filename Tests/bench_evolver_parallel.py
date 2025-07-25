# tests/bench_evolver_parallel.py
import concurrent.futures, random


import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from allcode3new import Evolver, FractalTensor

evolver = Evolver()
bag = [FractalTensor.random() for _ in range(9)]

def run():
    evolver.compute_fractal_archetype(bag)

def test_threadpool(benchmark):
    def threads():
        with concurrent.futures.ThreadPoolExecutor() as ex:
            list(ex.map(lambda _: run(), range(100)))
    benchmark(threads)

def test_processpool(benchmark):
    def procs():
        with concurrent.futures.ProcessPoolExecutor() as ex:
            list(ex.map(lambda _: None, range(100)))  # warm-up
    benchmark(procs)
