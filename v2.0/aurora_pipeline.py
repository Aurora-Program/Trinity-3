"""
Aurora Pipeline - Coordinador central del sistema Aurora Trinity-3

Integra todos los módulos core y orquesta el ciclo completo:
  Ingesta → Síntesis → Aprendizaje → Armonización → Reconstrucción

Incluye:
  - SimpleKnowledgeBase: Almacenamiento en memoria con integración al Evolver
  - AuroraPipeline: Coordinador principal con soporte para Harmonizer
  - AuroraDemo: Ejemplos de uso para validación
"""

from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any, Tuple
import json

# Imports locales
from Trigate import Trigate, TrigateRecord, Trit
from Transcender import Transcender
from FractalTensor import FractalTensor, FractalTranscender
from Evolver import Evolver3
from Extender import Extender
from Harmonizer import Harmonizer, HarmonyResult


# ============================================================================
# KNOWLEDGE BASE SIMPLE
# ============================================================================

class SimpleKnowledgeBase:
    """
    Knowledge Base mínima en memoria.
    
    Funcionalidades:
      - Almacena resultados de síntesis fractal
      - Alimenta automáticamente al Evolver
      - Permite consultas por Ms, MetaM o tags
      - Mantiene estadísticas de aprendizaje
    """
    
    def __init__(self, evolver: Evolver3):
        self.evolver = evolver
        self.patterns = {}  # key -> data completa
        self.by_ms = {}     # Ms_str -> [keys]
        self.stats = {
            "total_stored": 0,
            "total_harmonized": 0,
            "total_escalated": 0
        }
    
    def store(self, key: str, data: Dict[str, Any], tag: str = "default"):
        """
        Almacena resultado y alimenta al Evolver.
        
        Args:
            key: Identificador único (ej: hash de inputs)
            data: Resultado completo de síntesis/armonización
            tag: Etiqueta de espacio lógico
        """
        # Almacenar
        self.patterns[key] = {
            "data": data,
            "tag": tag,
            "harmonized": data.get("harmony_applied", False),
            "escalated": data.get("harmony_escalated", False)
        }
        
        # Indexar por Ms si existe
        if "tensor_cross" in data:
            tensor = data["tensor_cross"]
            ms_key = self._ms_to_key(tensor.nivel_3)
            if ms_key not in self.by_ms:
                self.by_ms[ms_key] = []
            self.by_ms[ms_key].append(key)
        
        # Alimentar al Evolver
        if "audits" in data:
            self.evolver.observe_fractal(data, level_name=tag)
        
        # Stats
        self.stats["total_stored"] += 1
        if data.get("harmony_applied"):
            self.stats["total_harmonized"] += 1
        if data.get("harmony_escalated"):
            self.stats["total_escalated"] += 1
    
    def retrieve(self, key: str) -> Optional[Dict]:
        """Recupera por clave exacta"""
        entry = self.patterns.get(key)
        return entry["data"] if entry else None
    
    def query_by_ms(self, ms_vector: List[List[Trit]]) -> List[Dict]:
        """Recupera todos los patrones con Ms similar"""
        ms_key = self._ms_to_key(ms_vector)
        keys = self.by_ms.get(ms_key, [])
        return [self.patterns[k]["data"] for k in keys]
    
    def get_stats(self) -> Dict:
        """Retorna estadísticas de la KB"""
        return {
            **self.stats,
            "unique_ms": len(self.by_ms),
            "evolver_relators": len(self.evolver._relator),
            "evolver_emergences": len(self.evolver._emerg),
            "evolver_dynamics": len(self.evolver._dyn)
        }
    
    @staticmethod
    def _ms_to_key(ms_vector: List[List[Trit]]) -> str:
        """Convierte Ms en string para indexación"""
        return str([[x if x is not None else -1 for x in v] for v in ms_vector])


# ============================================================================
# AURORA FRACTAL EVOLVER (con Harmonizer integrado)
# ============================================================================

class AuroraFractalEvolver:
    """
    Evolucionador fractal con reparación post-síntesis.
    
    Ejecuta ciclos de síntesis fractal (27→9→3) y aplica el Harmonizer
    después de cada ronda para detectar y reparar incoherencias.
    
    Features:
      - Síntesis fractal completa (cross-level + self-synthesis)
      - Reparación automática con Harmonizer (5 niveles)
      - Auditoría completa y trazabilidad
      - Registro de arquetipos nuevos cuando se escala
    """
    
    def __init__(
        self, 
        transcender_cls, 
        trigate_cls, 
        evolver, 
        extender_cls,
        harmonizer_cls=None
    ):
        self.trigate_cls = trigate_cls
        self.transcender_core = transcender_cls(trigate_cls)
        self.fractal_tx = FractalTranscender(transcender_cls)
        self.evolver = evolver
        self.extender = extender_cls(trigate_cls, evolver)
        self.harmonizer = harmonizer_cls(trigate_cls, evolver, extender_cls) if harmonizer_cls else None
    
    def synthesize_with_harmony(
        self,
        A: FractalTensor,
        B: FractalTensor,
        C: FractalTensor,
        apply_harmony: bool = True
    ) -> Dict[str, Any]:
        """
        Síntesis fractal completa con armonización opcional.
        
        Args:
            A, B, C: FractalTensors de entrada
            apply_harmony: Si True, aplica Harmonizer post-síntesis
        
        Returns:
            Dict con:
              - tensor_cross: FractalTensor resultado
              - Ss: Síntesis factuales por nivel
              - audits: Auditorías completas
              - harmony_applied: Bool indicando si se reparó
              - harmony_audit: Auditoría del Harmonizer (si aplica)
              - harmony_escalated: Bool indicando si se escaló a arquetipo
        """
        # 1. Síntesis fractal base
        result = self.fractal_tx.synthesize(A, B, C, self.transcender_core)
        
        tensor = result["tensor_cross"]
        audits = result["audits"]
        Ss = result["Ss"]
        
        # 2. Aplicar Harmonizer si está disponible y habilitado
        harmony_applied = False
        harmony_escalated = False
        harmony_audit = []
        
        if apply_harmony and self.harmonizer:
            # Preparar datos para el Harmonizer
            Ms_triplet = (
                tensor.nivel_3[0],
                tensor.nivel_3[1],
                tensor.nivel_3[2]
            )
            
            children_observed = {
                "x": (A.nivel_3[0], A.nivel_3[1], A.nivel_3[2]),
                "y": (B.nivel_3[0], B.nivel_3[1], B.nivel_3[2]),
                "z": (C.nivel_3[0], C.nivel_3[1], C.nivel_3[2])
            }
            
            context_Ss = {}
            if Ss and "lvl3" in Ss and len(Ss["lvl3"]) >= 3:
                context_Ss = {
                    "x": Ss["lvl3"][0],
                    "y": Ss["lvl3"][1],
                    "z": Ss["lvl3"][2]
                }
            
            # Ejecutar armonización
            harmony = self.harmonizer.harmonize_from_state(
                Ms_parent_triplet=Ms_triplet,
                children_observed=children_observed,
                context_Ss=context_Ss
            )
            
            # Si hubo reparación, actualizar el tensor
            if harmony.repaired:
                harmony_applied = True
                harmony_escalated = harmony.escalated
                harmony_audit = harmony.audit
                
                # Actualizar tensor con valores reparados
                children = harmony.result["children"]
                tensor.nivel_3 = [
                    children["x"][0],  # Ms_x hijo 0
                    children["y"][0],  # Ms_y hijo 0
                    children["z"][0]   # Ms_z hijo 0
                ]
                
                # Agregar info de harmony al audit
                if "lvl3" in audits and len(audits["lvl3"]) > 0:
                    audits["lvl3"][0]["harmony"] = {
                        "applied": True,
                        "escalated": harmony_escalated,
                        "steps": len(harmony_audit)
                    }
        
        # 3. Retornar resultado completo
        return {
            "tensor_cross": tensor,
            "Ss": Ss,
            "audits": audits,
            "locals": result.get("locals", {}),
            "harmony_applied": harmony_applied,
            "harmony_audit": harmony_audit,
            "harmony_escalated": harmony_escalated
        }
    
    def evolve_batch(
        self,
        tensor_batch: List[Tuple[FractalTensor, FractalTensor, FractalTensor]],
        apply_harmony: bool = True
    ) -> List[Dict[str, Any]]:
        """
        Procesa un batch de trios de tensores.
        
        Args:
            tensor_batch: Lista de (A, B, C) tensores
            apply_harmony: Aplicar Harmonizer a cada síntesis
        
        Returns:
            Lista de resultados (uno por trio)
        """
        results = []
        for A, B, C in tensor_batch:
            result = self.synthesize_with_harmony(A, B, C, apply_harmony)
            results.append(result)
        return results


# ============================================================================
# AURORA PIPELINE (Coordinador Principal)
# ============================================================================

class AuroraPipeline:
    """
    Coordinador central del sistema Aurora.
    
    Orquesta el ciclo completo:
      1. Inicializa todos los módulos
      2. Gestiona el flujo de datos
      3. Coordina síntesis, aprendizaje y armonización
      4. Mantiene la Knowledge Base
    
    Uso básico:
        pipeline = AuroraPipeline()
        result = pipeline.run_cycle(data_A, data_B, data_C)
    """
    
    def __init__(self, enable_harmony: bool = True, verbose: bool = True):
        """
        Inicializa el pipeline completo.
        
        Args:
            enable_harmony: Habilitar Harmonizer post-síntesis
            verbose: Imprimir mensajes de progreso
        """
        self.verbose = verbose
        self.enable_harmony = enable_harmony
        
        if self.verbose:
            print("🌅 Inicializando Aurora Pipeline...")
        
        # 1. Módulos base
        self.trigate_cls = Trigate
        if self.verbose:
            print("  ✅ Trigate (LUTs ternarias)")
        
        # 2. Evolver (debe inicializarse antes que Extender y Harmonizer)
        self.evolver = Evolver3(Trigate, th_match=2, decay=0.9)
        if self.verbose:
            print("  ✅ Evolver3 (RELATOR + EMERGENCIA + DINÁMICA)")
        
        # 3. Módulos de síntesis
        self.transcender_cls = Transcender
        self.transcender = Transcender(Trigate)
        if self.verbose:
            print("  ✅ Transcender (síntesis jerárquica)")
        
        # 4. Extender (requiere Evolver)
        self.extender_cls = Extender
        self.extender = Extender(Trigate, self.evolver)
        if self.verbose:
            print("  ✅ Extender (reconstrucción top-down)")
        
        # 5. Harmonizer (requiere Trigate, Evolver y Extender)
        self.harmonizer_cls = Harmonizer if enable_harmony else None
        self.harmonizer = Harmonizer(Trigate, self.evolver, Extender) if enable_harmony else None
        if self.verbose:
            if enable_harmony:
                print("  ✅ Harmonizer (reparación 5 niveles)")
            else:
                print("  ⚠️  Harmonizer deshabilitado")
        
        # 6. Evolver Fractal (orquestador de síntesis)
        self.fractal_evolver = AuroraFractalEvolver(
            transcender_cls=Transcender,
            trigate_cls=Trigate,
            evolver=self.evolver,
            extender_cls=Extender,
            harmonizer_cls=Harmonizer if enable_harmony else None
        )
        if self.verbose:
            print("  ✅ FractalEvolver (síntesis + armonización)")
        
        # 7. Knowledge Base
        self.kb = SimpleKnowledgeBase(self.evolver)
        if self.verbose:
            print("  ✅ KnowledgeBase (almacenamiento + stats)")
        
        if self.verbose:
            print("\n✨ Aurora Pipeline listo para operar\n")
    
    def process_input(
        self,
        data_A: List[List[Trit]],
        data_B: List[List[Trit]],
        data_C: List[List[Trit]]
    ) -> Tuple[FractalTensor, FractalTensor, FractalTensor]:
        """
        Convierte datos crudos en FractalTensors.
        
        Args:
            data_A, data_B, data_C: Listas de vectores ternarios
        
        Returns:
            Tupla de (tensor_A, tensor_B, tensor_C)
        """
        def ensure_27(data: List[List[Trit]]) -> List[List[Trit]]:
            """Asegura que tengamos 27 vectores de 3 bits"""
            data = list(data)
            while len(data) < 27:
                data.append([0, 0, 0])
            return data[:27]
        
        # Asegurar longitud correcta
        data_A = ensure_27(data_A)
        data_B = ensure_27(data_B)
        data_C = ensure_27(data_C)
        
        # Crear tensores fractales
        tensor_A = FractalTensor(
            nivel_27=data_A,
            nivel_9=data_A[:9],  # Placeholder, se sintetizará
            nivel_3=data_A[:3]    # Placeholder, se sintetizará
        ).normalize()
        
        tensor_B = FractalTensor(
            nivel_27=data_B,
            nivel_9=data_B[:9],
            nivel_3=data_B[:3]
        ).normalize()
        
        tensor_C = FractalTensor(
            nivel_27=data_C,
            nivel_9=data_C[:9],
            nivel_3=data_C[:3]
        ).normalize()
        
        return tensor_A, tensor_B, tensor_C
    
    def run_cycle(
        self,
        data_A: List[List[Trit]],
        data_B: List[List[Trit]],
        data_C: List[List[Trit]],
        tag: str = "default"
    ) -> Dict[str, Any]:
        """
        Ejecuta ciclo completo: Ingesta → Síntesis → Aprendizaje → Storage.
        
        Args:
            data_A, data_B, data_C: Datos de entrada
            tag: Etiqueta de espacio lógico
        
        Returns:
            Resultado completo con tensor, auditorías, harmony info
        """
        if self.verbose:
            print(f"🔄 Ejecutando ciclo completo (tag: {tag})...")
        
        # 1. Procesar inputs
        tensor_A, tensor_B, tensor_C = self.process_input(data_A, data_B, data_C)
        if self.verbose:
            print("  ✓ Inputs procesados → FractalTensors")
        
        # 2. Síntesis con armonización
        result = self.fractal_evolver.synthesize_with_harmony(
            tensor_A, tensor_B, tensor_C,
            apply_harmony=self.enable_harmony
        )
        if self.verbose:
            print("  ✓ Síntesis fractal ejecutada")
            if result["harmony_applied"]:
                print(f"    🔧 Harmonizer aplicado ({len(result['harmony_audit'])} pasos)")
                if result["harmony_escalated"]:
                    print("    ⚠️  Escalado a arquetipo nuevo")
        
        # 3. Almacenar en KB (esto alimenta automáticamente al Evolver)
        key = f"{tag}_{hash((str(data_A), str(data_B), str(data_C)))}"
        self.kb.store(key, result, tag)
        if self.verbose:
            print("  ✓ Resultado almacenado en KB")
        
        # 4. Stats
        if self.verbose:
            stats = self.kb.get_stats()
            print(f"\n📊 Stats: {stats['total_stored']} almacenados, "
                  f"{stats['total_harmonized']} armonizados, "
                  f"{stats['total_escalated']} escalados")
        
        return result
    
    def get_stats(self) -> Dict[str, Any]:
        """Retorna estadísticas completas del sistema"""
        return {
            "kb": self.kb.get_stats(),
            "harmony_enabled": self.enable_harmony
        }


# ============================================================================
# DEMOS
# ============================================================================

class AuroraDemo:
    """Demostraciones para validar el pipeline"""
    
    def __init__(self, enable_harmony: bool = True):
        self.pipeline = AuroraPipeline(enable_harmony=enable_harmony, verbose=True)
    
    def demo_basic_synthesis(self):
        """Demo: Síntesis básica sin conflictos"""
        print("\n" + "="*70)
        print("DEMO 1: Síntesis Básica")
        print("="*70 + "\n")
        
        # Datos simples y coherentes
        data_A = [[1, 0, 1]] * 27
        data_B = [[0, 1, 0]] * 27
        data_C = [[1, 1, 0]] * 27
        
        result = self.pipeline.run_cycle(data_A, data_B, data_C, tag="demo1")
        
        print("\n📋 Resultado:")
        print(f"  Ms nivel_3: {result['tensor_cross'].nivel_3}")
        print(f"  Harmony aplicado: {result['harmony_applied']}")
        print(f"  Escalado: {result['harmony_escalated']}")
        
        return result
    
    def demo_with_conflicts(self):
        """Demo: Síntesis con incoherencias que requieren armonización"""
        print("\n" + "="*70)
        print("DEMO 2: Síntesis con Conflictos (requiere Harmonizer)")
        print("="*70 + "\n")
        
        # Datos con más variación para provocar incoherencias
        data_A = [[1, 0, 1], [0, 1, 0], [1, 1, 0]] * 9
        data_B = [[0, 1, 1], [1, 0, 1], [0, 0, 1]] * 9
        data_C = [[1, 1, 1], [0, 0, 0], [None, 1, None]] * 9
        
        result = self.pipeline.run_cycle(data_A, data_B, data_C, tag="demo2")
        
        print("\n📋 Resultado:")
        print(f"  Ms nivel_3: {result['tensor_cross'].nivel_3}")
        print(f"  Harmony aplicado: {result['harmony_applied']}")
        if result['harmony_applied']:
            print(f"  Pasos de reparación: {len(result['harmony_audit'])}")
            print(f"  Escalado: {result['harmony_escalated']}")
        
        return result
    
    def demo_batch_processing(self):
        """Demo: Procesamiento en lote"""
        print("\n" + "="*70)
        print("DEMO 3: Procesamiento en Lote (3 ciclos)")
        print("="*70 + "\n")
        
        batches = [
            ([[1, 0, 0]] * 27, [[0, 1, 0]] * 27, [[0, 0, 1]] * 27),
            ([[1, 1, 0]] * 27, [[0, 1, 1]] * 27, [[1, 0, 1]] * 27),
            ([[1, 1, 1]] * 27, [[0, 0, 0]] * 27, [[1, 0, 1]] * 27),
        ]
        
        for i, (A, B, C) in enumerate(batches, 1):
            print(f"\n--- Batch {i}/3 ---")
            result = self.pipeline.run_cycle(A, B, C, tag=f"batch_{i}")
        
        print("\n📊 Estadísticas finales:")
        stats = self.pipeline.get_stats()
        print(json.dumps(stats, indent=2))
    
    def demo_reconstruction(self):
        """Demo: Ciclo completo con reconstrucción"""
        print("\n" + "="*70)
        print("DEMO 4: Síntesis + Reconstrucción")
        print("="*70 + "\n")
        
        # 1. Síntesis
        data_A = [[1, 0, 1]] * 27
        data_B = [[0, 1, 0]] * 27
        data_C = [[1, 1, 0]] * 27
        
        result = self.pipeline.run_cycle(data_A, data_B, data_C, tag="demo4")
        
        # 2. Reconstrucción
        Ms_triplet = (
            result['tensor_cross'].nivel_3[0],
            result['tensor_cross'].nivel_3[1],
            result['tensor_cross'].nivel_3[2]
        )
        
        print("\n🔄 Reconstruyendo desde Ms_triplet...")
        reconstructed = self.pipeline.extender.extend_triplet(Ms_triplet)
        
        print("\n📋 Reconstrucción:")
        print(f"  Children x: {reconstructed['children']['x']}")
        print(f"  Children y: {reconstructed['children']['y']}")
        print(f"  Children z: {reconstructed['children']['z']}")
        print(f"  Coherencia: {reconstructed.get('coherence', {})}")


# ============================================================================
# MAIN
# ============================================================================

if __name__ == "__main__":
    print("\n" + "🌅"*35)
    print("   AURORA PIPELINE - Sistema Completo con Harmonizer")
    print("🌅"*35 + "\n")
    
    # Crear demo
    demo = AuroraDemo(enable_harmony=True)
    
    # Ejecutar demos
    try:
        demo.demo_basic_synthesis()
        demo.demo_with_conflicts()
        demo.demo_batch_processing()
        demo.demo_reconstruction()
        
        print("\n" + "="*70)
        print("✅ TODOS LOS DEMOS COMPLETADOS EXITOSAMENTE")
        print("="*70 + "\n")
        
    except Exception as e:
        print(f"\n❌ Error en demo: {e}")
        import traceback
        traceback.print_exc()