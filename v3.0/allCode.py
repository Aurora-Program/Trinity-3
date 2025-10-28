# aurora_unificado.py — núcleo autosimilar compacto (Transcender + 4 objetos)
from typing import List, Optional, Tuple, Dict, Any

Trit = Optional[int]            # 0 | 1 | None
Role = Tuple[str, str, str]     # ('A','B','C')

# ---------- util ----------
def norm3(v: List[Trit]) -> List[Trit]:
    v = (list(v) + [0,0,0])[:3]
    return [None if x is None else (1 if x==1 else 0) for x in v]

def fib(n:int)->int:
    a,b=1,1
    for _ in range(n): a,b=b,a+b
    return a

BASE_ROLES: List[Role] = [('A','B','C'), ('B','C','A'), ('C','A','B')]
def rotate_roles(k:int)->List[Role]:
    s = fib(k) % 3
    return [BASE_ROLES[(i+s)%3] for i in range(3)]

def pick(name:str, A,B,C): return A if name=='A' else B if name=='B' else C

# ---------- Trigate LUTs ----------
class Trigate:
    INF, LRN, DA, DB = {},{},{},{}
    @classmethod
    def init(cls):
        vals=[0,1,None]
        for a in vals:
          for b in vals:
            for m in vals:
              cls.INF[(a,b,m)] = None if (a is None or b is None or m is None) else ((a^b) if m==1 else (1-(a^b)))
        for a in vals:
          for b in vals:
            for r in vals:
              cls.LRN[(a,b,r)] = None if (a is None or b is None or r is None) else (1 if r==(a^b) else 0)
        for b in vals:
          for m in vals:
            for r in vals:
              cls.DA[(b,m,r)] = None if (b is None or m is None or r is None) else ((b^r) if m==1 else (1-(b^r)))
        for a in vals:
          for m in vals:
            for r in vals:
              cls.DB[(a,m,r)] = None if (a is None or m is None or r is None) else ((a^r) if m==1 else (1-(a^r)))
Trigate.init()

def infer(A,B,M): A,B,M=map(norm3,(A,B,M)); return [Trigate.INF[(a,b,m)] for a,b,m in zip(A,B,M)]
def learn(A,B,R): A,B,R=map(norm3,(A,B,R)); return [Trigate.LRN[(a,b,r)] for a,b,r in zip(A,B,R)]

# ---------- Clase única autosimilar ----------
class Transcender:
    """
    Una sola clase para 4 modos:
      - 'sintetizar' → A,B,C -> M1..M3 -> Ms,Ss (coherencia top-down)   [cara subir]
      - 'extender'   → Ms -> (M1..M3) usando coherencia top-down        [cara bajar]
      - 'evolver'    → observa y registra patrones relator/emerg/dyn    [cara patrones]
      - 'armonizar'  → re-opera trigates para cerrar huecos/conflictos  [base sincroniza]
    El wiring se rota con Fibonacci (k) para preservar autosimilitud.
    La memoria compartida (mem) guarda relator/emerg/dyn.
    """
    def __init__(self, *, mode:str, k:int=0, mem:Dict[str,Any]=None):
        assert mode in ("sintetizar","extender","evolver","armonizar")
        self.mode  = mode
        self.roles = rotate_roles(k)
        self.mem   = mem if mem is not None else {"relator":[], "emerg":[], "dyn":[], "_last_ms": None}

    # --- bloques atomizados (iguales para todos) ---
    def _triplet_learn(self, A,B,C)->Tuple[List[Trit],List[Trit],List[Trit],List[Tuple]]:
        Ms=[]; trip=[]
        for (i1,i2,oz) in self.roles:
            X,Y,Z = map(norm3,(pick(i1,A,B,C), pick(i2,A,B,C), pick(oz,A,B,C)))
            M = learn(X,Y,Z); Ms.append(M); trip.append((X,Y,Z))
        return Ms[0],Ms[1],Ms[2],trip

    def _synthesize(self, M1,M2,M3)->Tuple[List[Trit],List[Trit]]:
        Ms = learn(M1,M2,M3)   # estructura (padre)
        Ss = infer(M1,M2,Ms)   # forma (shape)
        return Ms,Ss

    def _absolute_coherence(self, M1,M2,M3, Ms)->Tuple[List[Trit],List[Trit],List[Trit]]:
        # Deducciones desde el padre (permuta de faltantes)
        M1h = infer(M2,M3,Ms); M2h = infer(M3,M1,Ms); M3h = infer(M1,M2,Ms)
        # prioriza lo deducido (padre) ante None o conflicto
        def blend(obs,ded): return [ded[i] if (obs[i] is None or (obs[i] is not None and ded[i] is not None and obs[i]!=ded[i])) else obs[i] for i in range(3)]
        return blend(norm3(M1),M1h), blend(norm3(M2),M2h), blend(norm3(M3),M3h)

    # --- ejecución por modo ---
    def run(self, *, A=None,B=None,C=None, Ms=None, seeds:Tuple[List[Trit],List[Trit],List[Trit]]=None) -> Dict[str,Any]:
        if self.mode == "sintetizar":
            M1,M2,M3,_ = self._triplet_learn(A,B,C)
            Ms_,Ss     = self._synthesize(M1,M2,M3)
            M1c,M2c,M3c = self._absolute_coherence(M1,M2,M3, Ms_)
            # memoria para Evolver (relator/emerg)
            self.mem["relator"].append({"Ms":Ms_,"roles":tuple(self.roles),"Mloc":(M1c,M2c,M3c)})
            self.mem["emerg"].append({"M123":(M1c,M2c,M3c),"Ms":Ms_,"Ss":Ss})
            return {"M1":M1c,"M2":M2c,"M3":M3c,"Ms":Ms_,"Ss":Ss,"roles":self.roles}

        if self.mode == "extender":
            # seeds: (M1?,M2?,M3?) opcional
            s1,s2,s3 = seeds or ([None]*3,[None]*3,[None]*3)
            M1c,M2c,M3c = self._absolute_coherence(s1,s2,s3, norm3(Ms))
            return {"M1":M1c,"M2":M2c,"M3":M3c,"Ms":norm3(Ms)}

        if self.mode == "evolver":
            # observa usando exactamente la misma lógica que sintetizar
            obs = Transcender(mode="sintetizar", k=0, mem=self.mem).run(A=A,B=B,C=C)
            last = self.mem.get("_last_ms")
            if last is not None:
                trans = learn(last, obs["Ms"], obs["Ms"])
                self.mem["dyn"].append({"prev":last, "curr":obs["Ms"], "trans":trans})
            self.mem["_last_ms"] = obs["Ms"]
            return {"observed": obs, "memory_size": {k:len(self.mem[k]) for k in ("relator","emerg","dyn")}}

        if self.mode == "armonizar":
            s1,s2,s3 = seeds or ([None]*3,[None]*3,[None]*3)
            # cierre top-down con el Ms suministrado
            M1c,M2c,M3c = self._absolute_coherence(s1,s2,s3, norm3(Ms))
            # re-sintetiza para validar consistencia
            Ms2,Ss2 = self._synthesize(M1c,M2c,M3c)
            return {"M1":M1c,"M2":M2c,"M3":M3c,"Ms":Ms2,"Ss":Ss2}

        raise ValueError("modo desconocido")

# ---------- Fábrica de “caras” (objetos) ----------
def make_sintetizador(mem=None, k:int=0) -> Transcender: return Transcender(mode="sintetizar", k=k, mem=mem)
def make_extender(mem=None, k:int=1)     -> Transcender: return Transcender(mode="extender",   k=k, mem=mem)
def make_evolver(mem=None, k:int=2)      -> Transcender: return Transcender(mode="evolver",    k=k, mem=mem)
def make_armonizador(mem=None, k:int=0)  -> Transcender: return Transcender(mode="armonizar",  k=k, mem=mem)



MODES_TO_TENSOR = {
    "sintetizar": [1, 0, 0],
    "extender"  : [0, 1, 0],
    "evolver"   : [0, 0, 1],
    "armonizar" : [1, 1, 0],
}
TENSOR_TO_MODE = {tuple(v): k for k, v in MODES_TO_TENSOR.items()}

def tensor_to_mode(t):
    return TENSOR_TO_MODE.get(tuple(norm3(t)), "sintetizar")

def apply_mode(transcender: Transcender, mode_tensor):
    transcender.mode = tensor_to_mode(mode_tensor)

# --- 1) LEF pura: coherencia como tensor (todo Trigate) ---
def H_fractal(M1, M2, M3, Ms):
    # Deducción top-down de cada hijo
    M1h = infer(M2, M3, Ms)
    M2h = infer(M3, M1, Ms)
    M3h = infer(M1, M2, Ms)
    # Comparadores fractales (obs vs deducido)
    C1 = learn(M1, M1h, M1h)
    C2 = learn(M2, M2h, M2h)
    C3 = learn(M3, M3h, M3h)
    # Coherencia total de la triada
    return learn(C1, C2, C3)   # [1,1,1] ⇒ coherente

# --- 2) Paso autosimilar de una cara (usa tus objetos) ---
def step_autosimilar_face(face: Dict[str, Any], mem: Dict[str, Any], level_tag: str):
    # Guardar H1 del ciclo anterior (si existe) para comparaciones temporales
    old_H1 = face.get("H1", [None]*3)

    # 1) Síntesis local
    syn = make_sintetizador(mem=mem).run(A=face["A"], B=face["B"], C=face["C"])
    H0  = H_fractal(syn["M1"], syn["M2"], syn["M3"], syn["Ms"])
    face.update({"M1": syn["M1"], "M2": syn["M2"], "M3": syn["M3"], "Ms": syn["Ms"]})

    # 2) Armonización (cierre top-down)
    har = make_armonizador(mem=mem).run(Ms=face["Ms"], seeds=(face["M1"], face["M2"], face["M3"]))
    H1  = H_fractal(har["M1"], har["M2"], har["M3"], har["Ms"])
    face.update({"M1": har["M1"], "M2": har["M2"], "M3": har["M3"], "Ms": har["Ms"]})

    # 3) Dinámica (tensorial)
    # Cambio debido a la armonización local en este step
    changed = (H0 != H1)

    # Cambio temporal entre ciclos: comparamos H1 de este ciclo con H1 del ciclo anterior (old_H1)
    if old_H1 != [None]*3 and H1 != old_H1:
        face.setdefault("dyn_local", [])
        face["dyn_local"].append({
            "level": level_tag,
            "prev_H": old_H1,
            "curr_H": H1,
            "delta_H_tensor": learn(old_H1, H1, H1)
        })
        # Un cambio temporal entre H1s también cuenta como cambio general
        changed = True

    # Actualizamos H0/H1 para que el siguiente ciclo pueda usarlos
    face["H0"], face["H1"] = H0, H1

    # Mantener H_prev como el H1 del ciclo anterior (compatibilidad con propose_mode)
    # De esta forma, propose_mode puede comparar H_prev (H1 previo) con H1 actual.
    face["H_prev"] = old_H1

    return changed
# --- 3) Propuesta de modo (todo con tensores) ---
def propose_mode(face: Dict[str, Any], parent_coherence=None):
    H0, H1 = face.get("H0", [None]*3), face.get("H1", [None]*3)
    Hprev  = face.get("H_prev")
    # Estable: coherencia plena ⇒ extender
    if H1 == [1,1,1]:
        return MODES_TO_TENSOR["extender"]
    # Mejora con huecos ⇒ armonizar
    if learn(H0, H1, H1) != [1,1,1]:
        if any(None in face[k] for k in ("M1","M2","M3")):
            return MODES_TO_TENSOR["armonizar"]
    # Oscilación temporal ⇒ evolucionar
    if Hprev is not None and learn(Hprev, H1, H1) != [1,1,1]:
        return MODES_TO_TENSOR["evolver"]
    # Padre incoherente ⇒ sintetizar
    if parent_coherence is not None and parent_coherence != [1,1,1]:
        return MODES_TO_TENSOR["sintetizar"]
    # Por defecto
    return MODES_TO_TENSOR["sintetizar"]

# --- 4) Consenso tetraédrico (barrera) ---
def tetra_consensus(faces: List[Dict[str, Any]]):
    if len(faces) != 4:
        raise ValueError("Se requieren 4 caras para el tetraedro.")
    P = [f["proposal"] for f in faces]
    M12 = learn(P[0], P[1], P[2])
    MOmega = learn(M12, P[3], P[3])     # tensor de control global
    COmega = H_fractal(P[0], P[1], P[2], MOmega)  # coherencia del control
    return MOmega, COmega

def tetra_tick(faces: List[Dict[str, Any]], parent_coherence=None):
    for f in faces:
        f["proposal"] = propose_mode(f, parent_coherence)
    MOmega, COmega = tetra_consensus(faces)
    if COmega == [1,1,1]:
        # Conmutación sincrónica (todas las caras a la vez)
        for f in faces:
            apply_mode(f["transcender"], MOmega)
        return True, MOmega
    else:
        # Anti-deadlock: rotación Fibonacci en el wiring de cada cara
        for f in faces:
            # rota roles en +1 (equivalente a k+1 mod 3)
            f["transcender"].roles = rotate_roles(1)
        return False, MOmega

# --- 5) Rebase atómico descendente (cuando cambia el padre) ---
def rebase_children(parent_Ms_new, children: List[Dict[str, Any]], mem: Dict[str, Any]):
    staged = []
    for node in children:
        # Proyecta con el nuevo marco y cierra top-down
        ext = make_extender(mem=mem).run(Ms=parent_Ms_new, seeds=(node["M1"], node["M2"], node["M3"]))
        fix = make_armonizador(mem=mem).run(Ms=ext["Ms"], seeds=(ext["M1"], ext["M2"], ext["M3"]))
        # Valida coherencia en buffer
        if H_fractal(fix["M1"], fix["M2"], fix["M3"], fix["Ms"]) != [1,1,1]:
            return False  # reintentar en el próximo tick
        staged.append(fix)
    # Commit atómico
    for node, new in zip(children, staged):
        node.update(new)
    return True

# --- 6) Concretización (≠ “colapso”): tetraedro coherente → tensor ---
def concretizar_tetra(tetra: Dict[str, Any]) -> Optional[List[Trit]]:
    faces = tetra["caras"]
    # coherencia de propuestas y control
    MOmega, COmega = tetra_consensus(faces)
    if COmega != [1,1,1]:
        return None
    # Todas las caras coherentes e idéntico Ms
    Ms0 = faces[0]["Ms"]
    if H_fractal(faces[0]["M1"], faces[0]["M2"], faces[0]["M3"], Ms0) != [1,1,1]:
        return None
    for f in faces[1:]:
        if f["Ms"] != Ms0 or H_fractal(f["M1"], f["M2"], f["M3"], f["Ms"]) != [1,1,1]:
            return None
    # El tetraedro se concreta en este tensor (arquetipo)
    return Ms0

# --- 7) Construcción de tetraedros y jerarquía ---
def crear_tetraedro_from_inputs(base_tensors: List[List[Trit]], k:int=0) -> Dict[str, Any]:
    """
    Crea un tetraedro sencillo con 4 caras que comparten el mismo conjunto base (3 tensores),
    rotando roles para cada cara (autosimilitud).
    """
    if len(base_tensors) < 3:
        raise ValueError("Se requieren ≥3 tensores base.")
    mem_comp = {"relator": [], "emerg": [], "dyn": [], "_last_ms": None}
    faces = []
    A,B,C = base_tensors[0], base_tensors[1], base_tensors[2]
    for i in range(4):
        tr = Transcender(mode="sintetizar", k=(k+i)%3, mem=mem_comp)
        face = {
            "transcender": tr,
            "A": A, "B": B, "C": C,
            "M1":[None]*3, "M2":[None]*3, "M3":[None]*3, "Ms":[None]*3,
            "H0":[None]*3, "H1":[None]*3, "H_prev": None,
        }
        faces.append(face)
        # rota base A,B,C para cambiar perspectiva entre caras
        A,B,C = B,C,A
    return {"caras": faces, "mem": mem_comp}

def build_upper_level_from_concretos(concretos: List[List[Trit]]) -> List[Dict[str, Any]]:
    """
    Agrupa concretos de 3 en 3 para formar nuevos tetraedros (cada tetra usa el mismo trío rotado).
    """
    tetras = []
    for i in range(0, len(concretos), 3):
        if i+2 >= len(concretos): break
        tetras.append(crear_tetraedro_from_inputs(concretos[i:i+3], k=0))
    return tetras

# --- 8) Pipeline tetraédrico y jerárquico ---
def pipeline_tetraedrico_autosimilar(tetraedros: List[Dict[str, Any]], *,
                                     max_cycles:int=8) -> Tuple[List[Dict[str, Any]], Dict[str, Any]]:
    if not tetraedros:
        return [], {"relator": [], "emerg": [], "dyn": [], "_last_ms": None}
    mem = tetraedros[0]["mem"]  # memoria compartida (por nivel)
    for _ in range(max_cycles):
        any_change = False
        for tet in tetraedros:
            faces = tet["caras"]
            # 1) paso autosimilar por cara
            for idx, f in enumerate(faces):
                any_change |= step_autosimilar_face(f, mem, f"tet:{id(tet)}:face:{idx}")
            # 2) consenso y conmutación (barrera)
            switched, MOmega = tetra_tick(faces, parent_coherence=None)
            # 3) si cambió el marco del padre (aquí el control global puede actuar como marco),
            #    reproyecta hijos (rebase) de forma atómica
            if switched:
                # usamos Ms de la cara 0 como referencia de marco emergente
                parent_Ms = faces[0]["Ms"]
                rebase_children(parent_Ms, faces, mem)  # idempotente si ya coherente
        if not any_change:
            break
    return tetraedros, mem

def run_hierarchy_until_root(base_level_tetras: List[Dict[str, Any]],
                             *, max_levels:int=6, max_cycles:int=8):
    """
    Ejecuta el pipeline por niveles: concretiza tetraedros coherentes a tensores,
    construye el nivel superior con esos tensores, y repite hasta que quede un tensor raíz
    o ya no se pueda seguir escalando.
    """
    level = 0
    current = base_level_tetras
    shared_mem = current[0]["mem"] if current else {"relator": [], "emerg": [], "dyn": [], "_last_ms": None}
    while level < max_levels and current:
        current, shared_mem = pipeline_tetraedrico_autosimilar(current, max_cycles=max_cycles)
        # concretizar tetraedros coherentes de este nivel
        concretos = []
        for tet in current:
            t = concretizar_tetra(tet)
            if t is not None:
                concretos.append(t)
        # ¿tensor raíz?
        if len(concretos) == 1:
            return {"root_tensor": concretos[0], "mem": shared_mem, "levels": level}
        # construir nivel superior
        next_level = build_upper_level_from_concretos(concretos)
        if not next_level:
            break
        # Unificar memoria entre niveles (simbiotismo)
        for t in next_level:
            t["mem"] = shared_mem
        current = next_level
        level += 1
    return {"root_tensor": None, "mem": shared_mem, "levels": level}

# --- 9) Mini demo local (opcional) ---
if __name__ == "__main__":  # ejecuta una pequeña prueba
    base = [
        [1,0,1],[1,0,1],[1,0,1],   # patrón claro
        [0,1,0],[None,1,None],[0,1,0],  # huecos
        [1,1,0],[1,None,0],[1,1,None],  # conflictos
    ]
    # Un tetra de base con 3 tensores (se rotan entre caras)
    tet0 = crear_tetraedro_from_inputs(base[:3], k=0)
    res = run_hierarchy_until_root([tet0], max_levels=3, max_cycles=6)
    print("Tensor raíz:", res["root_tensor"])
    print("Dyn entries:", len(res["mem"]["dyn"]))