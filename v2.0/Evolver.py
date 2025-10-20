from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any, Tuple
from functools import lru_cache
import time

Trit = Optional[int]

def norm3(v: List[Trit]) -> List[Trit]:
    v = (list(v) + [0,0,0])[:3]
    return [None if x is None else (1 if x==1 else 0) for x in v]

def rotate3(v: List[Trit], s: int) -> List[Trit]:
    s %= 3
    return v[-s:] + v[:-s] if s else v

@lru_cache(maxsize=512)
def similarity3_cached(a_tuple: Tuple[Trit, ...], b_tuple: Tuple[Trit, ...]) -> int:
    """Versión cacheada de similarity3 para tuplas inmutables"""
    s=0
    for x,y in zip(a_tuple, b_tuple):
        if x is None or y is None: continue
        if x==y: s+=1
    return s  # 0..3

def similarity3(a: List[Trit], b: List[Trit]) -> int:
    """Wrapper para mantener compatibilidad con listas"""
    return similarity3_cached(tuple(a), tuple(b))

@dataclass
class Proto:
    key: Tuple[Any,...]
    proto: List[Trit]
    weight: float = 0.0
    count: int = 0
    last_seen: float = field(default_factory=lambda: time.time())

class Evolver3:
    """
    Evolver autosimilar (todo con Trigate):
    1) RELATOR: aprende relaciones entre dimensiones de distintos tensores condicionadas por Ms padre.
    2) EMERGENCIA: aprende patrones entre Ms (M1,M2,M3 -> Ms).
    3) DINÁMICA: aprende transición entrada→salida en rondas (Ms_prev -> Ms_next) y/o (tensor_prev -> tensor_next).
    """
    def __init__(self, trigate_cls, *, th_match:int=2, decay:float=0.9):
        self.T = trigate_cls
        self.th = th_match
        self.decay = decay
        # Bancos de patrones
        self._relator: Dict[Tuple, Proto] = {}    # key = (Ms_parent, wiring_hash, role_idx)
        self._emerg:   Dict[Tuple, Proto] = {}    # key = (role_idx,)
        self._dyn:     Dict[Tuple, Proto] = {}    # key = (level, ) o (round,)

        # memoria de contexto para dinámica
        self._last_round_ms: List[List[Trit]] = []  # Ms de la ronda anterior (acumuladas por nodo/level)

    # ---------- helpers ----------
    def _reinforce(self, bank: Dict[Tuple, Proto], key: Tuple, candidate: List[Trit]) -> None:
        cand = norm3(candidate)
        if key not in bank:
            bank[key] = Proto(key=key, proto=cand, weight=1.0, count=1)
            return
        p = bank[key]
        # refuerzo EMA + “honestidad”: solo rellenar None con nueva evidencia
        p.weight = p.weight * self.decay + similarity3(cand, p.proto)
        p.count += 1
        p.last_seen = time.time()
        for i,(a,b) in enumerate(zip(p.proto, cand)):
            if a is None and b is not None:
                p.proto[i] = b

    def _best_rot_match(self, proto: List[Trit], cand: List[Trit]) -> Tuple[int,int]:
        # devuelve (sim_max, rot_mejor)
        best=( -1, 0 )
        for r in (0,1,2):
            s = similarity3(proto, rotate3(cand,r))
            if s>best[0]: best=(s,r)
        return best

    # ---------- 1) RELATOR ----------
    def observe_relator(self, Ms_parent: List[Trit], wiring: List[Tuple[str,str,str]], M1: List[Trit], M2: List[Trit], M3: List[Trit]):
        """
        Aprende “cómo se conectan” dimensiones entre tensores dado el Ms padre.
        Representamos cada trigate (role_idx=0,1,2) por la “ley local” que vio:
           ley_i = learn(IN1_i, IN2_i, OUT_i) == M_i (redundante pero explícito)
        Guardamos un prototipo condicional por (Ms_parent, wiring, role_idx).
        """
        Ms_parent = norm3(Ms_parent)
        Mloc = [norm3(M1), norm3(M2), norm3(M3)]
        wiring_hash = tuple(wiring)  # simple
        for role_idx, M_i in enumerate(Mloc):
            key = (tuple(Ms_parent), wiring_hash, role_idx)
            self._reinforce(self._relator, key, M_i)

    # ---------- 2) EMERGENCIA ----------
    def observe_emergence(self, M1: List[Trit], M2: List[Trit], M3: List[Trit], Ms: List[Trit]):
        """
        Aprende patrón de síntesis: (M1,M2,M3) -> Ms
        Guardamos dos cosas:
         a) ley_superior = learn(M1, M2, M3)  (debería ≈ Ms ideal)
         b) “shape” = infer(M1, M2, Ms)       (Ss), puede ayudar a discriminar variantes
        """
        M1,M2,M3,Ms = map(norm3,(M1,M2,M3,Ms))
        ley_sup = self.T.learn(M1,M2,M3)   # estructura ideal
        shape   = self.T.infer(M1,M2,Ms)   # forma observada
        # patrón principal: lo que explica mejor Ms
        key_main = ("emerg",)
        self._reinforce(self._emerg, key_main, ley_sup)
        # opcional: sub-claves por “firma de hijos”
        key_sig = ("emerg_sig", tuple(M1), tuple(M2), tuple(M3))
        self._reinforce(self._emerg, key_sig, Ms)
        # opcional: forma
        key_shape = ("emerg_shape",)
        self._reinforce(self._emerg, key_shape, shape)

    # ---------- 3) DINÁMICA ----------
    def observe_dynamics_round(self, ms_list_this_round: List[List[Trit]], level_tag: str):
        """
        Aprende transición entre rondas: Ms_prev -> Ms_curr (por nodo).
        También registra un “resumen de nivel” usando learn/infer sobre agregados (promueve metapatrones).
        """
        ms_curr = [norm3(m) for m in ms_list_this_round]
        if self._last_round_ms:
            for m_prev, m_curr in zip(self._last_round_ms[:len(ms_curr)], ms_curr):
                # ley de transición local
                trans = self.T.learn(m_prev, m_curr, m_curr)  # “qué M convierte prev en curr”
                key = ("dyn_local", level_tag)
                self._reinforce(self._dyn, key, trans)
        # resumen del nivel (autosimilar): comprimimos de 3 en 3 si hay suficientes
        for i in range(0, len(ms_curr), 3):
            block = ms_curr[i:i+3]
            if len(block)==3:
                Ms_level = self.T.learn(block[0], block[1], block[2])
                keyL = ("dyn_level", level_tag)
                self._reinforce(self._dyn, keyL, Ms_level)
        # actualiza memoria
        self._last_round_ms = ms_curr

    # ---------- Ingesta de resultados ----------
    def observe_transcender(self, result: Dict[str,Any], level_tag: str = "node"):
        """
        Ingiere un Transcender.solve(...): usa
          - RELATOR: Ms (padre), wiring, M1..M3, (y coherencia si está)
          - EMERGENCIA: M1,M2,M3 -> Ms
        """
        M1,M2,M3,Ms = result["M1"], result["M2"], result["M3"], result["Ms"]
        self.observe_relator(Ms, result["wiring"], M1, M2, M3)
        self.observe_emergence(M1, M2, M3, Ms)
        # coherencia absoluta: útil para valorar “fuerza” del patrón padre
        if result.get("coherence"):
            coh = result["coherence"]
            keyC = ("coh_parent", tuple(norm3(coh["parent"])))
            # sintetizamos una “marca” de fuerza a partir de conteos
            # m≔ [filled>0, conflict>0, kept>0] como bits 0/1 para trigate
            marks = [
                1 if coh["totals"]["null_filled"]>0 else 0,
                1 if coh["totals"]["conflict_resolved"]>0 else 0,
                1 if coh["totals"]["kept_observed"]>0 else 0,
            ]
            self._reinforce(self._emerg, keyC, marks)

    def observe_fractal(self, res: Dict[str,Any], level_name: str):
        """
        Ingiere FractalTranscender.synthesize(...):
        - para cada nivel (27/9/3), recorre los nodos y llama observe_transcender(...)
        - también alimenta la DINÁMICA con las Ms cruzadas del nivel
        """
        # Ms cruzadas por nivel
        lvl_to_ms = {
            "lvl27": res["tensor_cross"].nivel_27,
            "lvl9":  res["tensor_cross"].nivel_9,
            "lvl3":  res["tensor_cross"].nivel_3,
        }
        # audits por nivel traen MetaM/wiring/score por nodo; si no están los resultados completos
        # de cada transcender individual, al menos aprendemos dinámica con las Ms del tensor resultante.
        for lvl, ms_list in lvl_to_ms.items():
            self.observe_dynamics_round(ms_list, level_tag=f"{level_name}:{lvl}")

    # ---------- Consultas ----------
    def relator_top(self, k:int=5):
        items = sorted(self._relator.values(), key=lambda p: (p.weight,p.count), reverse=True)
        return [{"key": p.key, "proto": p.proto, "w": round(p.weight,3), "n": p.count} for p in items[:k]]

    def emergence_top(self, k:int=5):
        items = sorted(self._emerg.values(), key=lambda p: (p.weight,p.count), reverse=True)
        return [{"key": p.key, "proto": p.proto, "w": round(p.weight,3), "n": p.count} for p in items[:k]]

    def dynamics_top(self, k:int=5):
        items = sorted(self._dyn.values(), key=lambda p: (p.weight,p.count), reverse=True)
        return [{"key": p.key, "proto": p.proto, "w": round(p.weight,3), "n": p.count} for p in items[:k]]

    # ---------- Métodos avanzados para Harmonizer y Extender ----------
    
    def select_relator(self, tag: str, Ms_parent: List[Trit]) -> Optional[List[Tuple[str,str,str]]]:
        """
        Selecciona el wiring más apropiado para un componente dado su Ms padre.
        Usado por Extender para reconstrucción guiada.
        
        Args:
            tag: Etiqueta del componente (ej: "x", "y", "z")
            Ms_parent: Vector Ms del padre
            
        Returns:
            Wiring sugerido o None si no hay información suficiente
        """
        Ms_parent = norm3(Ms_parent)
        best_wiring = None
        best_score = -1
        
        # Buscar en relator patrones que coincidan con Ms_parent
        for key, proto in self._relator.items():
            if len(key) >= 2:  # key = (Ms_parent_tuple, wiring_hash, role_idx)
                stored_ms = key[0]
                if similarity3(list(stored_ms), Ms_parent) >= self.th:
                    if proto.weight > best_score:
                        best_score = proto.weight
                        if len(key) >= 2:
                            best_wiring = key[1]  # wiring_hash
        
        return best_wiring if best_wiring else None
    
    def select_relator_k(self, tag: str, Ms: List[Trit], k: int = 3) -> List[Dict[str, Any]]:
        """
        Retorna los k mejores relatores candidatos para un Ms dado.
        Usado por Harmonizer para explorar alternativas de reparación.
        
        Args:
            tag: Etiqueta del componente
            Ms: Vector Ms para buscar
            k: Número de candidatos a retornar
            
        Returns:
            Lista de diccionarios con {proto, weight, count, wiring}
        """
        Ms = norm3(Ms)
        candidates = []
        
        # Buscar patrones similares en relator
        for key, proto in self._relator.items():
            if len(key) >= 2:  # (Ms_parent, wiring, role_idx)
                stored_ms = key[0]
                sim = similarity3(list(stored_ms), Ms)
                if sim >= self.th - 1:  # Umbral más permisivo para alternativas
                    candidates.append({
                        "proto": proto.proto,
                        "weight": proto.weight,
                        "count": proto.count,
                        "wiring": key[1] if len(key) >= 2 else None,
                        "similarity": sim
                    })
        
        # Ordenar por weight * similarity
        candidates.sort(key=lambda c: c["weight"] * (c["similarity"] + 1), reverse=True)
        return candidates[:k]
    
    def create_new_archetype_triplet(
        self, 
        Msx: List[Trit], 
        Msy: List[Trit], 
        Msz: List[Trit],
        *,
        tag: str = "new_archetype"
    ) -> Dict[str, Any]:
        """
        Crea un nuevo arquetipo a partir de una tripleta de Ms.
        Usado por Harmonizer cuando necesita crear patrones nuevos.
        
        Args:
            Msx, Msy, Msz: Vectores Ms de los tres componentes
            tag: Etiqueta para identificar el arquetipo
            
        Returns:
            Diccionario con el arquetipo creado y sus metadatos
        """
        Msx, Msy, Msz = map(norm3, (Msx, Msy, Msz))
        
        # Sintetizar un Ms superior desde la tripleta
        Ms_super = self.T.learn(Msx, Msy, Msz)
        
        # Calcular forma (Ss) del arquetipo
        Ss = self.T.infer(Msx, Msy, Ms_super)
        
        # Almacenar en banco de emergencia
        key_triplet = ("archetype_triplet", tuple(Msx), tuple(Msy), tuple(Msz))
        self._reinforce(self._emerg, key_triplet, Ms_super)
        
        # También almacenar la forma
        key_shape = ("archetype_shape", tag)
        self._reinforce(self._emerg, key_shape, Ss)
        
        return {
            "Ms_super": Ms_super,
            "Ss": Ss,
            "components": {"Msx": Msx, "Msy": Msy, "Msz": Msz},
            "tag": tag,
            "stored_keys": [key_triplet, key_shape]
        }
    
    def query_archetype_by_triplet(
        self, 
        Msx: List[Trit], 
        Msy: List[Trit], 
        Msz: List[Trit]
    ) -> Optional[Dict[str, Any]]:
        """
        Busca si existe un arquetipo para la tripleta dada.
        
        Returns:
            Diccionario con el arquetipo encontrado o None
        """
        Msx, Msy, Msz = map(norm3, (Msx, Msy, Msz))
        key = ("archetype_triplet", tuple(Msx), tuple(Msy), tuple(Msz))
        
        if key in self._emerg:
            proto = self._emerg[key]
            return {
                "Ms_super": proto.proto,
                "weight": proto.weight,
                "count": proto.count,
                "components": {"Msx": Msx, "Msy": Msy, "Msz": Msz}
            }
        
        return None
    
    def suggest_repair(self, Ms: List[Trit], *, context: str = "unknown") -> List[Trit]:
        """
        Sugiere una reparación para un Ms con NULLs basándose en patrones aprendidos.
        Usado por Harmonizer para rellenar valores faltantes.
        
        Args:
            Ms: Vector Ms con posibles NULLs
            context: Contexto para guiar la búsqueda
            
        Returns:
            Vector Ms reparado (mejor esfuerzo)
        """
        Ms = norm3(Ms)
        
        # Si no hay NULLs, retornar como está
        if all(x is not None for x in Ms):
            return Ms
        
        # Buscar el patrón más similar en emergencia
        best_match = None
        best_sim = -1
        
        for proto in self._emerg.values():
            # Calcular similitud solo en posiciones no-NULL
            sim = 0
            for i, (m, p) in enumerate(zip(Ms, proto.proto)):
                if m is not None and p is not None:
                    if m == p:
                        sim += 1
            
            if sim > best_sim:
                best_sim = sim
                best_match = proto.proto
        
        # Si encontramos algo, rellenar NULLs con el patrón
        if best_match:
            result = []
            for m, b in zip(Ms, best_match):
                result.append(m if m is not None else b)
            return result
        
        # Fallback: retornar [0,0,0] en NULLs
        return [m if m is not None else 0 for m in Ms]

