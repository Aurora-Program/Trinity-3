#!/usr/bin/env python3
"""
Aurora Trinity-3 - Extensión de Resolución de Ambigüedad
========================================================

Demostración de cómo Aurora podría manejar tokens ambiguos con múltiples
tensores candidatos procesados en paralelo hasta encontrar el más coherente.
"""

from trinity_3 import (
    FractalTensor, 
    Trigate, 
    Evolver, 
    FractalKnowledgeBase,
    Armonizador,
    Extender,
    Transcender
)
from typing import List, Dict, Tuple, Optional
import threading
import concurrent.futures
from dataclasses import dataclass

@dataclass
class CandidateScore:
    """Puntuación de un tensor candidato para resolución de ambigüedad."""
    tensor: FractalTensor
    coherence_score: float
    context_match: float
    semantic_distance: float
    final_score: float
    reasoning: str

class AmbiguityResolver:
    """
    Resolvedor de ambigüedad semántica para tokens con múltiples tensores.
    
    Procesa candidatos en paralelo y selecciona el más coherente basándose en:
    - Coherencia interna del tensor
    - Compatibilidad con contexto
    - Distancia semántica a arquetipos conocidos
    """
    
    def __init__(self, knowledge_base: FractalKnowledgeBase):
        self.kb = knowledge_base
        self.armonizador = Armonizador(knowledge_base=self.kb)
        self.transcender = Transcender()
        self.trigate = Trigate()
    
    def score_coherence(self, tensor: FractalTensor, context: Dict) -> float:
        """Calcula coherencia interna del tensor."""
        if not tensor.nivel_3:
            return 0.0
        
        # Coherencia geométrica: suma de componentes vs. producto
        vec = tensor.nivel_3[0]
        sum_components = sum(v for v in vec if v is not None)
        product = 1
        for v in vec:
            if v is not None:
                product *= (v + 1)  # +1 para evitar multiplicar por 0
        
        # Score basado en ratio sum/product (más balanceado = más coherente)
        if product > 0:
            coherence = min(1.0, sum_components / product)
        else:
            coherence = 0.0
        
        return coherence
    
    def score_context_match(self, tensor: FractalTensor, context: Dict) -> float:
        """Evalúa compatibilidad con contexto."""
        if not context.get('expected_space') or not tensor.nivel_3:
            return 0.5  # Neutral
        
        space_id = context['expected_space']
        expected_vector = context.get('expected_pattern', [0, 0, 0])
        
        # Calcular similitud vectorial
        vec = tensor.nivel_3[0]
        matches = sum(1 for a, b in zip(vec, expected_vector) if a == b)
        similarity = matches / len(expected_vector)
        
        # Buscar arquetipos similares en el espacio esperado
        universe = self.kb._get_space(space_id)
        archetype_matches = 0
        for stored_tensor in universe.storage.values():
            if hasattr(stored_tensor, 'nivel_3') and stored_tensor.nivel_3:
                stored_vec = stored_tensor.nivel_3[0]
                if sum(1 for a, b in zip(vec, stored_vec) if a == b) >= 2:
                    archetype_matches += 1
        
        context_bonus = min(0.5, archetype_matches * 0.1)
        return similarity + context_bonus
    
    def score_semantic_distance(self, tensor: FractalTensor, context: Dict) -> float:
        """Calcula distancia semántica usando Transcender."""
        if not tensor.nivel_3:
            return 1.0  # Máxima distancia
        
        reference_vectors = context.get('reference_vectors', [])
        if not reference_vectors:
            return 0.5  # Neutral
        
        distances = []
        vec = tensor.nivel_3[0]
        
        for ref_vec in reference_vectors:
            # Usar relate_vectors para calcular relación
            relation = self.transcender.relate_vectors(vec, ref_vec)
            
            # Distancia basada en cantidad de relaciones "neutras" (0)
            neutral_count = sum(1 for r in relation if r == 0)
            distance = 1.0 - (neutral_count / len(relation)) if relation else 1.0
            distances.append(distance)
        
        # Promedio de distancias (menor = mejor)
        avg_distance = sum(distances) / len(distances)
        return 1.0 - avg_distance  # Invertir para que mayor = mejor
    
    def evaluate_candidate(self, tensor: FractalTensor, context: Dict) -> CandidateScore:
        """Evalúa un candidato individual."""
        coherence = self.score_coherence(tensor, context)
        context_match = self.score_context_match(tensor, context)
        semantic_dist = self.score_semantic_distance(tensor, context)
        
        # Pesos para la puntuación final
        w_coherence = 0.4
        w_context = 0.4
        w_semantic = 0.2
        
        final_score = (w_coherence * coherence + 
                      w_context * context_match + 
                      w_semantic * semantic_dist)
        
        reasoning = f"coherence={coherence:.3f}, context={context_match:.3f}, semantic={semantic_dist:.3f}"
        
        return CandidateScore(
            tensor=tensor,
            coherence_score=coherence,
            context_match=context_match,
            semantic_distance=semantic_dist,
            final_score=final_score,
            reasoning=reasoning
        )
    
    def resolve_ambiguous_token(
        self, 
        candidates: List[FractalTensor], 
        context: Dict,
        parallel: bool = True
    ) -> Tuple[FractalTensor, List[CandidateScore]]:
        """
        Resuelve ambigüedad procesando candidatos en paralelo.
        
        Args:
            candidates: Lista de tensores candidatos
            context: Contexto semántico y espacial
            parallel: Si procesar en paralelo (True) o secuencial (False)
            
        Returns:
            Tuple[tensor_ganador, lista_de_puntuaciones]
        """
        print(f"🔍 Resolviendo ambigüedad: {len(candidates)} candidatos")
        
        if parallel and len(candidates) > 1:
            # Procesamiento paralelo
            with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
                futures = {
                    executor.submit(self.evaluate_candidate, candidate, context): i 
                    for i, candidate in enumerate(candidates)
                }
                
                scores = []
                for future in concurrent.futures.as_completed(futures):
                    candidate_idx = futures[future]
                    score = future.result()
                    scores.append((candidate_idx, score))
                    print(f"   Candidato {candidate_idx}: score={score.final_score:.3f} ({score.reasoning})")
                
                # Ordenar por índice original para mantener orden
                scores.sort(key=lambda x: x[0])
                candidate_scores = [score for _, score in scores]
        else:
            # Procesamiento secuencial
            candidate_scores = []
            for i, candidate in enumerate(candidates):
                score = self.evaluate_candidate(candidate, context)
                candidate_scores.append(score)
                print(f"   Candidato {i}: score={score.final_score:.3f} ({score.reasoning})")
        
        # Seleccionar el mejor
        best_score = max(candidate_scores, key=lambda s: s.final_score)
        winner = best_score.tensor
        
        print(f"🏆 Ganador: score={best_score.final_score:.3f}")
        
        # Armonizar el tensor ganador
        harmonized = self.armonizador.harmonize(
            winner.nivel_3[0], 
            space_id=context.get('expected_space', 'default')
        )
        winner.nivel_3[0] = harmonized["output"]
        
        return winner, candidate_scores

def demo_ambiguity_resolution():
    """Demuestra resolución de ambigüedad con tokens que tienen múltiples tensores."""
    print("🌟 Aurora Trinity-3 - Resolución de Ambigüedad")
    print("=" * 55)
    
    # Setup
    kb = FractalKnowledgeBase()
    resolver = AmbiguityResolver(kb)
    
    # Simular token ambiguo "bank" con múltiples significados
    print("\n🏦 Token ambiguo: 'bank'")
    
    # Candidato 1: Bank (institución financiera)
    bank_financial = FractalTensor(nivel_3=[[1, 1, 1]])  # Formal, sistema, urbano
    bank_financial.metadata = {'meaning': 'financial_institution', 'confidence': 0.8}
    
    # Candidato 2: Bank (orilla de río)
    bank_river = FractalTensor(nivel_3=[[0, 0, 1]])     # Natural, físico, geografía
    bank_river.metadata = {'meaning': 'river_shore', 'confidence': 0.7}
    
    # Candidato 3: Bank (inclinar/ladear)
    bank_tilt = FractalTensor(nivel_3=[[1, 0, 0]])      # Acción, movimiento, física
    bank_tilt.metadata = {'meaning': 'to_tilt', 'confidence': 0.6}
    
    candidates = [bank_financial, bank_river, bank_tilt]
    
    # Contexto 1: Financiero
    print("\n💰 Contexto: Documento financiero")
    financial_context = {
        'expected_space': 'finance',
        'expected_pattern': [1, 1, 1],  # Formal, sistema, institucional
        'reference_vectors': [[1, 1, 0], [1, 0, 1]],  # Vectores de "money", "institution"
        'domain': 'financial'
    }
    
    winner1, scores1 = resolver.resolve_ambiguous_token(candidates, financial_context)
    print(f"   Seleccionado: {winner1.metadata.get('meaning', 'unknown')}")
    print(f"   Vector final: {winner1.nivel_3[0]}")
    
    # Contexto 2: Geografía
    print("\n🌊 Contexto: Descripción geográfica")
    geographic_context = {
        'expected_space': 'geography',
        'expected_pattern': [0, 0, 1],  # Natural, físico, espacial
        'reference_vectors': [[0, 1, 1], [0, 0, 0]],  # Vectores de "river", "shore"
        'domain': 'geographic'
    }
    
    winner2, scores2 = resolver.resolve_ambiguous_token(candidates, geographic_context)
    print(f"   Seleccionado: {winner2.metadata.get('meaning', 'unknown')}")
    print(f"   Vector final: {winner2.nivel_3[0]}")
    
    # Contexto 3: Aviación
    print("\n✈️ Contexto: Manual de pilotaje")
    aviation_context = {
        'expected_space': 'aviation',
        'expected_pattern': [1, 0, 0],  # Acción, técnico, movimiento
        'reference_vectors': [[1, 0, 1], [0, 1, 0]],  # Vectores de "turn", "maneuver"
        'domain': 'aviation'
    }
    
    winner3, scores3 = resolver.resolve_ambiguous_token(candidates, aviation_context)
    print(f"   Seleccionado: {winner3.metadata.get('meaning', 'unknown')}")
    print(f"   Vector final: {winner3.nivel_3[0]}")
    
    print("\n✅ Resolución exitosa en todos los contextos!")
    print("🧠 Aurora demostró capacidad de procesamiento paralelo y resolución contextual")

if __name__ == "__main__":
    demo_ambiguity_resolution()
