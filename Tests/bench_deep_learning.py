import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from allcode3new import Transcender

tr = Transcender()

A = [1,0,0]; B = [0,1,0]; C = [0,0,1]

def manual():
    trio = tr.compute_vector_trio(A,B,C)
    tr.trigate.infer(A,B,trio['M_emergent'])

def test_dl(benchmark):
    benchmark(lambda: tr.deep_learning(A,B,C))

def test_manual(benchmark):
    benchmark(manual)