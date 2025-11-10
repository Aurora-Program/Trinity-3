// pyramid_demo.c - Demo del proceso recursivo piramidal
#include "pyramid.h"
#include <stdio.h>

int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║  AURORA RECURSIVE PYRAMID - Knowledge Graph Demo      ║\n");
    printf("║  3 vectors → 1 emergent → ascend to peak → descend    ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");
    
    // === NIVEL BASE: 9 vectores ===
    // Esto creará 3 niveles: 9 → 3 → 1 (cima)
    
    Vector base_vectors[9];
    
    printf("=== Creating 9 base vectors ===\n\n");
    
    for (int i = 0; i < 9; i++) {
        for (int d = 0; d < 3; d++) {
            // Patrón variado para cada vector
            base_vectors[i].dim[d].FO = (Trit)((i + d) % 3);
            base_vectors[i].dim[d].FN = (Trit)((i * 2 + d) % 3);
            base_vectors[i].dim[d].ES = (Trit)((i + d * 2) % 3);
        }
        
        // Roles iniciales
        base_vectors[i].roles[0] = (Role)(i % 3);
        base_vectors[i].roles[1] = (Role)((i + 1) % 3);
        base_vectors[i].roles[2] = (Role)((i + 2) % 3);
        
        // Estabilidad inicial
        for (int s = 0; s < 3; s++) {
            base_vectors[i].stability[s] = 0;
        }
        
        printf("Vector %d: ", i);
        printf("FO=[%s,%s,%s] FN=[%s,%s,%s] ES=[%s,%s,%s]\n",
               trit_str(base_vectors[i].dim[0].FO),
               trit_str(base_vectors[i].dim[1].FO),
               trit_str(base_vectors[i].dim[2].FO),
               trit_str(base_vectors[i].dim[0].FN),
               trit_str(base_vectors[i].dim[1].FN),
               trit_str(base_vectors[i].dim[2].FN),
               trit_str(base_vectors[i].dim[0].ES),
               trit_str(base_vectors[i].dim[1].ES),
               trit_str(base_vectors[i].dim[2].ES));
    }
    
    // === Crear la pirámide ===
    
    Pyramid* pyramid = pyramid_create();
    
    // Inicializar con los 9 vectores base
    pyramid_init_base(pyramid, base_vectors, 9);
    
    // === CICLO ASCENDENTE ===
    
    printf("\n\n");
    pyramid_ascend_to_peak(pyramid);
    
    // Mostrar estructura
    pyramid_print(pyramid);
    
    // === Mostrar la CIMA ===
    
    if (pyramid->peak) {
        printf("\n╔════════════════════════════════════╗\n");
        printf("║         PEAK (CIMA) REACHED        ║\n");
        printf("╚════════════════════════════════════╝\n");
        pyramid_print_node(pyramid->peak);
        
        printf("\n\nPeak Emergent Dimension:\n");
        printf("  Dimension 0: FO=%s FN=%s ES=%s\n",
               trit_str(pyramid->peak->vector.dim[0].FO),
               trit_str(pyramid->peak->vector.dim[0].FN),
               trit_str(pyramid->peak->vector.dim[0].ES));
        printf("  Dimension 1: FO=%s FN=%s ES=%s\n",
               trit_str(pyramid->peak->vector.dim[1].FO),
               trit_str(pyramid->peak->vector.dim[1].FN),
               trit_str(pyramid->peak->vector.dim[1].ES));
        printf("  Dimension 2: FO=%s FN=%s ES=%s\n",
               trit_str(pyramid->peak->vector.dim[2].FO),
               trit_str(pyramid->peak->vector.dim[2].FN),
               trit_str(pyramid->peak->vector.dim[2].ES));
    }
    
    // === Validar estructura ===
    
    pyramid_validate(pyramid);
    
    // === FLUJO TERMODINÁMICO ===
    
    printf("\n\n╔════════════════════════════════════════════════════════╗\n");
    printf("║          THERMODYNAMIC FLOW PROCESSING             ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    printf("\nBEFORE thermodynamic processing:\n");
    printf("  Entropy distribution:\n");
    for (int lvl = 0; lvl < pyramid->num_levels; lvl++) {
        int total_entropy = 0;
        for (int i = 0; i < pyramid->nodes_per_level[lvl]; i++) {
            total_entropy += pyramid_entropy(&pyramid->levels[lvl][i]->vector);
        }
        printf("    Level %d: %d total nulls (avg %.2f per node)\n", 
               lvl, total_entropy, 
               (float)total_entropy / pyramid->nodes_per_level[lvl]);
    }
    
    // 1. Exportar entropía hacia arriba (FO)
    pyramid_export_entropy_upward(pyramid);
    
    printf("\nAFTER entropy export (FO nulls moved up):\n");
    printf("  Entropy distribution:\n");
    for (int lvl = 0; lvl < pyramid->num_levels; lvl++) {
        int total_entropy = 0;
        for (int i = 0; i < pyramid->nodes_per_level[lvl]; i++) {
            total_entropy += pyramid_entropy(&pyramid->levels[lvl][i]->vector);
        }
        printf("    Level %d: %d total nulls (avg %.2f per node)\n", 
               lvl, total_entropy, 
               (float)total_entropy / pyramid->nodes_per_level[lvl]);
    }
    
    // 2. Propagar función hacia abajo (FN)
    pyramid_propagate_function_downward(pyramid);
    
    // 3. Armonizar orden (ES)
    pyramid_harmonize_order(pyramid, 10);
    
    printf("\nFINAL state after thermodynamic processing:\n");
    printf("  Coherence by level:\n");
    for (int lvl = 0; lvl < pyramid->num_levels; lvl++) {
        int total_entropy = 0;
        int total_dims = pyramid->nodes_per_level[lvl] * 9; // 3 dims * 3 fields
        for (int i = 0; i < pyramid->nodes_per_level[lvl]; i++) {
            total_entropy += pyramid_entropy(&pyramid->levels[lvl][i]->vector);
        }
        float coherence = 100.0f * (1.0f - (float)total_entropy / total_dims);
        printf("    Level %d: %.1f%% coherent (%d/%d non-null)\n", 
               lvl, coherence, total_dims - total_entropy, total_dims);
    }
    
    // === CICLO DESCENDENTE ===
    
    printf("\n\n");
    pyramid_descend_from_peak(pyramid);
    
    // === Mostrar algunos nodos reconstruidos ===
    
    printf("\n\n╔════════════════════════════════════╗\n");
    printf("║    RECONSTRUCTED BASE NODES        ║\n");
    printf("╚════════════════════════════════════╝\n");
    
    for (int i = 0; i < 3; i++) {
        PyramidNode* node = pyramid_get_node(pyramid, 0, i);
        if (node) {
            printf("\nBase node %d (after descent):\n", i);
            printf("  Dimension 0: FO=%s FN=%s ES=%s\n",
                   trit_str(node->vector.dim[0].FO),
                   trit_str(node->vector.dim[0].FN),
                   trit_str(node->vector.dim[0].ES));
        }
    }
    
    // === Demostrar consultas al Knowledge Graph ===
    
    printf("\n\n╔════════════════════════════════════╗\n");
    printf("║    KNOWLEDGE GRAPH QUERIES         ║\n");
    printf("╚════════════════════════════════════╝\n");
    
    // Obtener un nodo del nivel intermedio
    PyramidNode* middle_node = pyramid_get_node(pyramid, 1, 0);
    if (middle_node) {
        printf("\nQuerying node [1][0]:\n");
        
        Dim rrr[3], mmm[3], ooo[3];
        pyramid_get_RRR(middle_node, rrr);
        pyramid_get_MMM(middle_node, mmm);
        pyramid_get_OOO(middle_node, ooo);
        
        printf("\nRRR relations (Results from 3 sources):\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%d] FO=%s FN=%s ES=%s\n", i,
                   trit_str(rrr[i].FO),
                   trit_str(rrr[i].FN),
                   trit_str(rrr[i].ES));
        }
        
        printf("\nMMM relations (Modes from 3 sources):\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%d] FO=%s FN=%s ES=%s\n", i,
                   trit_str(mmm[i].FO),
                   trit_str(mmm[i].FN),
                   trit_str(mmm[i].ES));
        }
        
        printf("\nOOO relations (Orders from 3 sources):\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%d] FO=%s FN=%s ES=%s\n", i,
                   trit_str(ooo[i].FO),
                   trit_str(ooo[i].FN),
                   trit_str(ooo[i].ES));
        }
    }
    
    // === Resumen ===
    
    printf("\n\n╔════════════════════════════════════════════════════════╗\n");
    printf("║                    SUMMARY                             ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("✓ Pyramidal structure created with %d levels\n", pyramid->num_levels);
    printf("✓ Ascent cycle: 9 → 3 → 1 (peak reached)\n");
    printf("✓ Knowledge graph stores RRR, MMM, OOO relations\n");
    printf("✓ Thermodynamic flow:\n");
    printf("    • FO (Forma): Nulls exported upward → base coherent\n");
    printf("    • FN (Función): Purpose propagated downward → peak learns\n");
    printf("    • ES (Orden): Harmonized rotation → coherence found\n");
    printf("✓ Descent cycle: reconstructed all levels from peak\n");
    printf("✓ Each node knows its sources and parent\n");
    printf("\n");
    printf("The pyramid implements:\n");
    printf("  • Recursive synthesis (3→1 pattern)\n");
    printf("  • Bidirectional flow (ascend + descend)\n");
    printf("  • Relational memory (RRR/MMM/OOO graphs)\n");
    printf("  • Fractal coherence (same rules at all levels)\n");
    printf("  • Thermodynamic intelligence:\n");
    printf("      ◦ Base = Stable/Coherent (like atoms, grammar rules)\n");
    printf("      ◦ Top = Adaptive/Flexible (like thought, society)\n");
    printf("      ◦ Entropy flows up, Function flows down, Order harmonizes\n");
    printf("\n");
    
    // Liberar memoria
    pyramid_destroy(pyramid);
    
    return 0;
}
