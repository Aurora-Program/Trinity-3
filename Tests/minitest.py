import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from allcode3new import KnowledgeBase, Extender, FractalTensor, Evolver

t0 = FractalTensor(nivel_3=[[0,0,0]]*3, Ms=[0,0,0], Ss=[7,7,7])
t1 = FractalTensor(nivel_3=[[0,1,0]]*3, Ms=[0,1,0], Ss=[7,7,7])
t2 = FractalTensor(nivel_3=[[1,1,0]]*3, Ms=[1,1,0], Ss=[7,7,7])




evol = Evolver()
dyn_tensor = evol.analyze_fractal_dynamics([t0, t1, t2])  # dMs = [1,1,0]




kb = KnowledgeBase()               # sin datos anteriores
dyn_tensor.Ss = [7,7,7]
kb.add_archetype('demo','trayectoria', dyn_tensor, Ss=dyn_tensor.Ss)

ext = Extender(kb)
res = ext.extend_fractal([7,7,7], {'space_id': 'demo'})
print("Ms reconstruido:", res['reconstructed_tensor'].Ms) 