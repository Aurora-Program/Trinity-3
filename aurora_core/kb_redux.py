from dataclasses import dataclass, replace
from typing import Tuple, FrozenSet
from immutables import Map

# Vector y arquetipo
Vector3 = Tuple[int, int, int]  # Ejemplo: (1,0,1)
Archetype = object  # Reemplaza por FractalTensor real si lo deseas

@dataclass(frozen=True)
class UniverseState:
    ms_index: Map  # Map[Vector3, Archetype]
    name_index: Map  # Map[str, Vector3]
    ss_index: Map  # Map[Vector3, FrozenSet[Vector3]]
    models: Map  # Map[str, dict]

@dataclass(frozen=True)
class KBState:
    universes: Map  # Map[str, UniverseState]

@dataclass(frozen=True)
class AddArchetype:
    space: str
    name: str
    ms: Vector3
    ss: Vector3
    tensor: Archetype

def add_archetype_reducer(state: KBState, act: AddArchetype) -> KBState:
    u = state.universes.get(act.space) or UniverseState(
        ms_index=Map(), name_index=Map(), ss_index=Map(), models=Map())
    if act.ms in u.ms_index:
        return state  # No sobrescribe
    new_u = replace(
        u,
        ms_index=u.ms_index.set(act.ms, act.tensor),
        name_index=u.name_index.set(act.name, act.ms),
        ss_index=u.ss_index.set(act.ms, (u.ss_index.get(act.ms) or frozenset()) | {act.ss})
    )
    return replace(state, universes=state.universes.set(act.space, new_u))

class KBStore:
    def __init__(self, initial: KBState = None):
        self._state = initial or KBState(universes=Map())
    @property
    def state(self):
        return self._state
    def dispatch(self, action):
        if isinstance(action, AddArchetype):
            self._state = add_archetype_reducer(self._state, action)
        # Aquí puedes agregar más acciones

# Ejemplo de uso:
# store = KBStore()
# store.dispatch(AddArchetype("silabas", "PA", (1,0,1), (1,0,1), tensor))
# print(store.state)
