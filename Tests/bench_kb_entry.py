# tests/bench_kb_entry.py
import sys, os
import time, random, statistics, concurrent.futures as fut
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from allcode3new import  *

# run_learning_tests.py  ────────────────────────────────────────────────────


# ---------------------------------------------------------------------------
# CONFIGURACIÓN DEL EXPERIMENTO
# ---------------------------------------------------------------------------
N_TRAIN = 200        # pares (A,B) para “enseñar”
N_TEST  = 100        # consultas para verificar aprendizaje
SPACE   = "aprendizaje_demo"

rng = random.Random(42)   # repetible

# ---------------------------------------------------------------------------
# 1) GENERAMOS DATASET SINTÉTICO
#    Regla:  si A xor B == 1  ⇒  R = 1   (caso clásico)
# ---------------------------------------------------------------------------
def rand_vec():
    return [rng.randint(0,1) for _ in range(3)]

train_set, test_set = [], []
for _ in range(N_TRAIN):
    A,B = rand_vec(), rand_vec()
    R   = [a ^ b for a,b in zip(A,B)]
    train_set.append( (A,B,R) )

for _ in range(N_TEST):
    A,B = rand_vec(), rand_vec()
    R   = [a ^ b for a,b in zip(A,B)]
    test_set.append( (A,B,R) )

# ---------------------------------------------------------------------------
# 2) ENTRENAMOS – guardamos como arquetipos + entradas en la KB
# ---------------------------------------------------------------------------
kb = KnowledgeBase()
transc = Transcender()
for idx,(A,B,R) in enumerate(train_set):
    trio   = transc.compute_vector_trio(A,B,R)['M_emergent']
    kb.add_entry(A=A,B=B,C=[0,0,0],
                 M_emergent=trio,
                 MetaM=[0,0,0],
                 R_validos=R,
                 transcender_id=f"pair{idx}",
                 space_id=SPACE)

ext = Extender(kb)

# ---------------------------------------------------------------------------
# 3) TEST 1 – ¿El Extender “aprende” a reconstruir R?
# ---------------------------------------------------------------------------
aciertos = 0
t0 = time.perf_counter()
for A,B,R in test_set:
    res = ext.extend_fractal([0, 0, 0], {'space_id': 'bench'})
    # Como el KB guarda Ms = R al nivel_3[0], comparamos
    pred_R = res['reconstructed_tensor'].nivel_3[0]
    if pred_R == R:
        aciertos += 1
t_ext = time.perf_counter() - t0

# ---------------------------------------------------------------------------
# 4) TEST 2 – aprendiendo vía Trigate.learn
#    ¿Puede recuperar M exacto y luego inferir R correcto?
# ---------------------------------------------------------------------------
tri = Trigate()
t0 = time.perf_counter()
for A,B,R in test_set:
    M = tri.learn(A,B,R)             # “aprendizaje”
    R_pred = tri.infer(A,B,M)
    assert R_pred == R               # debería cumplirse siempre
t_tri = time.perf_counter() - t0

# ---------------------------------------------------------------------------
# 5) REPORTE
# ---------------------------------------------------------------------------
print("\n=== RESULTADOS DE APRENDIZAJE ===")
print(f" • Train pairs almacenados en KB:      {N_TRAIN}")
print(f" • Consultas de test:                 {N_TEST}")
print(f" • Extender reconstruye R correctamente "
      f"{aciertos}/{N_TEST}  →  {aciertos/N_TEST:.1%}")
print(f"   Tiempo total Extender: {t_ext*1000:.2f} ms")
print(f"   Promedio por consulta: {t_ext/N_TEST*1000:.3f} ms")
print(f"\n • Trigate.learn+infer assert OK en {N_TEST} rondas")
print(f"   Tiempo total Trigate:   {t_tri*1000:.2f} ms")
print("\n🎯  Fin del experimento\n")